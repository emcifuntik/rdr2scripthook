param(
    [string[]]$Backend = @(
        'dotnet-nativeaot',
        'dotnet-wasmtime',
        'lua-native',
        'lua-wasmtime'
    ),
    [ValidateRange(0, 100)]
    [int]$Warmup = 3,
    [ValidateRange(1, 1000)]
    [int]$Samples = 15,
    [ValidateSet('on', 'off')]
    [string]$Fuel = 'on',
    [Alias('Workload')]
    [string[]]$WorkloadFilter = @(),
    [string[]]$Category = @(),
    [switch]$NoBuild,
    [string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'

$benchmarkRoot = $PSScriptRoot
$repoRoot = Split-Path -Parent $benchmarkRoot
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $benchmarkRoot 'results'
}

$manifests = @{}
Get-ChildItem -LiteralPath (Join-Path $benchmarkRoot 'backends') -Directory |
    ForEach-Object {
        $manifestPath = Join-Path $_.FullName 'backend.json'
        if (Test-Path -LiteralPath $manifestPath) {
            $manifest = Get-Content -LiteralPath $manifestPath -Raw |
                ConvertFrom-Json
            $manifest | Add-Member -NotePropertyName root `
                -NotePropertyValue $_.FullName
            $manifests[$manifest.id] = $manifest
        }
    }

$selected = foreach ($id in $Backend) {
    if (-not $manifests.ContainsKey($id)) {
        throw "Unknown backend '$id'. Available: $($manifests.Keys -join ', ')"
    }
    $manifests[$id]
}

if (-not $NoBuild) {
    foreach ($item in $selected) {
        $buildScript = Join-Path $item.root 'build.ps1'
        if (-not (Test-Path -LiteralPath $buildScript)) {
            throw "Backend '$($item.id)' has no build.ps1"
        }
        & $buildScript -Configuration Release
        if ($LASTEXITCODE -ne 0) {
            throw "Backend '$($item.id)' build failed: $LASTEXITCODE"
        }
    }
}

$wasmRunner = Join-Path $repoRoot 'BIN\Release\wasm_bench_runner.exe'
if ($selected.mode -contains 'wasmtime') {
    if (-not $NoBuild) {
        & cmake --preset windows-release
        if ($LASTEXITCODE -ne 0) { throw "CMake configure failed: $LASTEXITCODE" }
        & cmake --build --preset windows-release --target wasm_bench_runner
        if ($LASTEXITCODE -ne 0) { throw "Benchmark runner build failed: $LASTEXITCODE" }
    }
    if (-not (Test-Path -LiteralPath $wasmRunner)) {
        throw "Wasmtime benchmark runner is missing: $wasmRunner"
    }
}

$workloadsBySuite = @{}
foreach ($item in $selected) {
    if ([string]::IsNullOrWhiteSpace($item.suite) -or
        [string]::IsNullOrWhiteSpace($item.workloads)) {
        throw "Backend '$($item.id)' does not declare a suite and workloads file"
    }
    if (-not $workloadsBySuite.ContainsKey($item.suite)) {
        $workloadPath = Join-Path $benchmarkRoot $item.workloads
        if (-not (Test-Path -LiteralPath $workloadPath -PathType Leaf)) {
            throw "Workload suite '$($item.suite)' is missing: $workloadPath"
        }
        $suiteWorkloads = @(Import-Csv -LiteralPath $workloadPath)
        if ($Category.Count -gt 0) {
            $suiteWorkloads = @($suiteWorkloads | Where-Object {
                $candidate = $_
                @($Category | Where-Object {
                    $candidate.category -like $_
                }).Count -gt 0
            })
        }
        if ($WorkloadFilter.Count -gt 0) {
            $suiteWorkloads = @($suiteWorkloads | Where-Object {
                $candidate = $_
                @($WorkloadFilter | Where-Object {
                    $candidate.name -like $_ -or [string]$candidate.id -eq $_
                }).Count -gt 0
            })
        }
        if ($suiteWorkloads.Count -eq 0) {
            throw "Suite '$($item.suite)' has no workloads matching the filters"
        }
        $workloadsBySuite[$item.suite] = $suiteWorkloads
    }
}
$allRows = [System.Collections.Generic.List[object]]::new()
$unsupportedRows = [System.Collections.Generic.List[object]]::new()

foreach ($item in $selected) {
    $artifact = Join-Path $benchmarkRoot $item.artifact
    if (-not (Test-Path -LiteralPath $artifact -PathType Leaf)) {
        throw "Backend artifact is missing: $artifact"
    }

    foreach ($benchmarkCase in $workloadsBySuite[$item.suite]) {
        if (@($item.unsupported_workloads) -contains $benchmarkCase.name) {
            $unsupportedRows.Add([pscustomobject]@{
                suite = $item.suite
                backend = $item.id
                workload = $benchmarkCase.name
                reason = 'declared unsupported by backend manifest'
            })
            Write-Warning "Skipping unsupported workload '$($benchmarkCase.name)' on '$($item.id)'"
            continue
        }
        $arguments = @(
            '--backend', $item.id,
            '--workload', $benchmarkCase.name,
            '--workload-id', $benchmarkCase.id,
            '--iterations', $benchmarkCase.iterations,
            '--seed', $benchmarkCase.seed,
            '--warmup', $Warmup,
            '--samples', $Samples
        )
        if ($item.mode -eq 'native') {
            $nativeArguments = [System.Collections.Generic.List[string]]::new()
            if ($item.entrypoint) {
                $nativeArguments.Add((Join-Path $benchmarkRoot $item.entrypoint))
            }
            if ($item.source) {
                $nativeArguments.Add('--source')
                $nativeArguments.Add((Join-Path $benchmarkRoot $item.source))
            }
            foreach ($argument in $arguments) {
                $nativeArguments.Add([string]$argument)
            }
            $lines = & $artifact @nativeArguments
        } elseif ($item.mode -eq 'wasmtime') {
            $lines = & $wasmRunner '--module' $artifact @arguments '--fuel' $Fuel
        } else {
            throw "Unsupported backend mode '$($item.mode)'"
        }
        if ($LASTEXITCODE -ne 0) {
            throw "Backend '$($item.id)' failed on '$($benchmarkCase.name)'"
        }
        $rows = @($lines | ConvertFrom-Csv)
        if ($rows.Count -ne $Samples) {
            throw "Backend '$($item.id)' returned $($rows.Count) samples; expected $Samples"
        }
        foreach ($row in $rows) {
            $row | Add-Member -NotePropertyName suite `
                -NotePropertyValue $item.suite
            $row | Add-Member -NotePropertyName category `
                -NotePropertyValue $benchmarkCase.category
            $allRows.Add($row)
        }
    }
}

foreach ($suite in $workloadsBySuite.Keys) {
    foreach ($benchmarkCase in $workloadsBySuite[$suite]) {
        $checksums = @($allRows | Where-Object {
            $_.suite -eq $suite -and $_.workload -eq $benchmarkCase.name
        } | Select-Object -ExpandProperty checksum -Unique)
        if ($checksums.Count -eq 0) {
            $suiteBackendCount = @($selected | Where-Object suite -eq $suite).Count
            $unsupportedCount = @($unsupportedRows | Where-Object {
                $_.suite -eq $suite -and $_.workload -eq $benchmarkCase.name
            }).Count
            if ($unsupportedCount -eq $suiteBackendCount) { continue }
        }
        if ($checksums.Count -ne 1) {
            throw "Checksum mismatch for '$suite/$($benchmarkCase.name)': $($checksums -join ', ')"
        }
    }
}

function Get-Percentile([double[]]$Values, [double]$Percentile) {
    $ordered = @($Values | Sort-Object)
    $index = [Math]::Max(0, [Math]::Ceiling($ordered.Count * $Percentile) - 1)
    return $ordered[$index]
}

$summaries = [System.Collections.Generic.List[object]]::new()
foreach ($suite in @($workloadsBySuite.Keys | Sort-Object)) {
    $suiteBackends = @($selected | Where-Object suite -eq $suite)
    $baseline = @($suiteBackends | Where-Object baseline -eq $true)
    if ($baseline.Count -gt 1) {
        throw "Suite '$suite' declares more than one baseline backend"
    }
    foreach ($benchmarkCase in $workloadsBySuite[$suite]) {
        $baselineRows = if ($baseline.Count -eq 1) {
            @($allRows | Where-Object {
                $_.backend -eq $baseline[0].id -and
                $_.suite -eq $suite -and
                $_.workload -eq $benchmarkCase.name
            })
        } else { @() }
        $baselineMedian = $null
        if ($baselineRows.Count -gt 0) {
            $baselineMedian = Get-Percentile `
                @($baselineRows | ForEach-Object { [double]$_.elapsed_ns }) 0.5
        }

        foreach ($item in $suiteBackends) {
            $rows = @($allRows | Where-Object {
                $_.backend -eq $item.id -and $_.suite -eq $suite -and
                $_.workload -eq $benchmarkCase.name
            })
            if ($rows.Count -eq 0) { continue }
            $values = @($rows | ForEach-Object { [double]$_.elapsed_ns })
            $median = Get-Percentile $values 0.5
            $p95 = Get-Percentile $values 0.95
            $ratio = if ($null -ne $baselineMedian) {
                $median / $baselineMedian
            } else { [double]::NaN }
            $medianFuel = Get-Percentile `
                @($rows | ForEach-Object { [double]$_.fuel_consumed }) 0.5
            $summaries.Add([pscustomobject]@{
                suite = $suite
                category = $benchmarkCase.category
                backend = $item.id
                workload = $benchmarkCase.name
                median_ms = [Math]::Round($median / 1e6, 3)
                p95_ms = [Math]::Round($p95 / 1e6, 3)
                throughput_mops = [Math]::Round(
                    ([double]$benchmarkCase.iterations * 1000.0) / $median, 3)
                relative_to_native = if ([double]::IsNaN($ratio)) {
                    '-'
                } else { '{0:N2}x' -f $ratio }
                ratio_value = if ([double]::IsNaN($ratio)) { $null } else {
                    [Math]::Round($ratio, 4)
                }
                median_fuel = if ($medianFuel -eq 0) { '-' } else {
                    [Math]::Round($medianFuel)
                }
                median_fuel_value = $medianFuel
                production_budget = if ($medianFuel -eq 0) { '-' } else {
                    '{0:N1}%' -f ($medianFuel / 100000000.0 * 100.0)
                }
                checksum = $rows[0].checksum
            })
        }
    }
}

New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$timestamp = (Get-Date).ToUniversalTime().ToString('yyyyMMdd-HHmmss')
$csvPath = Join-Path $OutputDirectory "$timestamp.csv"
$reportPath = Join-Path $OutputDirectory "$timestamp.md"
$allRows | Export-Csv -LiteralPath $csvPath -NoTypeInformation -Encoding utf8

$cpu = try { (Get-CimInstance Win32_Processor | Select-Object -First 1).Name.Trim() }
       catch { 'Unknown CPU' }
$commit = (& git -C $repoRoot rev-parse HEAD).Trim()
$worktreeState = if (& git -C $repoRoot status --porcelain) { 'dirty' } else { 'clean' }
$externalSources = [System.Collections.Generic.List[string]]::new()
@($selected | Where-Object source_repository |
    Select-Object -ExpandProperty source_repository -Unique) |
    ForEach-Object {
        $sourcePath = Join-Path (Split-Path -Parent $repoRoot) $_
        if (Test-Path -LiteralPath (Join-Path $sourcePath '.git')) {
            $sourceCommit = (& git -C $sourcePath rev-parse HEAD).Trim()
            $sourceState = if (& git -C $sourcePath status --porcelain) {
                'dirty'
            } else { 'clean' }
            $externalSources.Add("- External source ${_}: ``$sourceCommit`` ($sourceState)")
        } else {
            $externalSources.Add("- External source ${_}: unavailable")
        }
    }
$markdown = [System.Collections.Generic.List[string]]::new()
$markdown.Add('# Runtime benchmark results')
$markdown.Add('')
$markdown.Add("- UTC: $((Get-Date).ToUniversalTime().ToString('u'))")
$markdown.Add("- CPU: $cpu")
$markdown.Add("- Commit: ``$commit`` ($worktreeState)")
foreach ($source in $externalSources) { $markdown.Add($source) }
$markdown.Add("- Warmup/sample count: $Warmup / $Samples")
$markdown.Add("- Wasmtime fuel accounting: $Fuel (measurement budget is raised to avoid traps)")
$markdown.Add('')
$baselineIds = @($selected | Where-Object baseline -eq $true |
    Select-Object -ExpandProperty id -Unique)
$comparable = @($summaries | Where-Object {
    $null -ne $_.ratio_value -and $_.backend -notin $baselineIds
})
if ($comparable.Count -gt 0) {
    $markdown.Add('## Performance overview')
    $markdown.Add('')
    $markdown.Add('| Suite | Backend | Workloads | At/faster than native | Within 1.25x | Worst ratio |')
    $markdown.Add('| --- | --- | ---: | ---: | ---: | ---: |')
    foreach ($group in @($comparable | Group-Object suite, backend)) {
        $items = @($group.Group)
        $atNative = @($items | Where-Object { $_.ratio_value -le 1.0 }).Count
        $within = @($items | Where-Object { $_.ratio_value -le 1.25 }).Count
        $worst = ($items | Sort-Object ratio_value -Descending | Select-Object -First 1).ratio_value
        $markdown.Add("| $($items[0].suite) | $($items[0].backend) | $($items.Count) | $atNative | $within | $('{0:N2}x' -f $worst) |")
    }
    $markdown.Add('')
    $markdown.Add('### Top remaining regressions')
    $markdown.Add('')
    $markdown.Add('| Suite | Category | Backend | Workload | Median (ms) | vs native |')
    $markdown.Add('| --- | --- | --- | --- | ---: | ---: |')
    foreach ($item in @($comparable | Sort-Object ratio_value -Descending | Select-Object -First 10)) {
        $markdown.Add("| $($item.suite) | $($item.category) | $($item.backend) | $($item.workload) | $($item.median_ms) | $($item.relative_to_native) |")
    }
    $markdown.Add('')
}
if ($Fuel -eq 'on') {
    $fuelRows = @($summaries | Where-Object { $_.median_fuel_value -gt 0 })
    if ($fuelRows.Count -gt 0) {
        $markdown.Add('## Highest production fuel consumers')
        $markdown.Add('')
        $markdown.Add('| Suite | Backend | Workload | Median fuel | 100M budget |')
        $markdown.Add('| --- | --- | --- | ---: | ---: |')
        foreach ($item in @($fuelRows | Sort-Object median_fuel_value -Descending | Select-Object -First 10)) {
            $markdown.Add("| $($item.suite) | $($item.backend) | $($item.workload) | $($item.median_fuel) | $($item.production_budget) |")
        }
        $markdown.Add('')
    }
}
$markdown.Add('## Full results')
$markdown.Add('')
$markdown.Add('| Suite | Category | Backend | Workload | Median (ms) | P95 (ms) | M iterations/s | vs native | Median fuel | 100M budget |')
$markdown.Add('| --- | --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |')
foreach ($summary in $summaries) {
    $markdown.Add("| $($summary.suite) | $($summary.category) | $($summary.backend) | $($summary.workload) | $($summary.median_ms) | $($summary.p95_ms) | $($summary.throughput_mops) | $($summary.relative_to_native) | $($summary.median_fuel) | $($summary.production_budget) |")
}
$markdown.Add('')
$markdown.Add('Lower latency and a ratio closer to 1.00x are better.')
if ($unsupportedRows.Count -gt 0) {
    $markdown.Add('')
    $markdown.Add('## Unsupported capabilities')
    $markdown.Add('')
    $markdown.Add('| Suite | Backend | Workload | Reason |')
    $markdown.Add('| --- | --- | --- | --- |')
    foreach ($unsupported in $unsupportedRows) {
        $markdown.Add("| $($unsupported.suite) | $($unsupported.backend) | $($unsupported.workload) | $($unsupported.reason) |")
    }
}
$markdown | Set-Content -LiteralPath $reportPath -Encoding utf8
Copy-Item -LiteralPath $csvPath -Destination (Join-Path $OutputDirectory 'latest.csv') -Force
Copy-Item -LiteralPath $reportPath -Destination (Join-Path $OutputDirectory 'latest.md') -Force

$summaries | Format-Table suite, category, backend, workload, median_ms, p95_ms,
    throughput_mops, relative_to_native, median_fuel, production_budget -AutoSize
Write-Host "Raw samples: $csvPath"
Write-Host "Report:      $reportPath"
