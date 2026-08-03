using System.Diagnostics;
using System.Globalization;
using Rdr2.Benchmarks;

static string Required(IReadOnlyDictionary<string, string> arguments,
                       string name)
{
    if (!arguments.TryGetValue(name, out string? value))
        throw new ArgumentException($"Missing {name}");
    return value;
}

static string Csv(string value) => $"\"{value.Replace("\"", "\"\"")}\"";

var parsed = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
for (int index = 0; index < args.Length; index += 2)
{
    if (index + 1 >= args.Length || !args[index].StartsWith("--"))
        throw new ArgumentException("Arguments must be --name value pairs");
    parsed[args[index]] = args[index + 1];
}

string backend = Required(parsed, "--backend");
string workload = Required(parsed, "--workload");
int workloadId = int.Parse(Required(parsed, "--workload-id"),
                           CultureInfo.InvariantCulture);
int iterations = int.Parse(Required(parsed, "--iterations"),
                            CultureInfo.InvariantCulture);
long seed = long.Parse(Required(parsed, "--seed"),
                       CultureInfo.InvariantCulture);
int warmup = int.Parse(Required(parsed, "--warmup"),
                       CultureInfo.InvariantCulture);
int samples = int.Parse(Required(parsed, "--samples"),
                        CultureInfo.InvariantCulture);

long checksum = 0;
for (int index = 0; index < warmup; ++index)
    checksum = BenchmarkKernels.Run(workloadId, iterations, seed);

GC.Collect();
GC.WaitForPendingFinalizers();
GC.Collect();

Console.WriteLine("backend,engine,workload,iterations,sample,elapsed_ns,checksum,fuel_consumed");
for (int sample = 0; sample < samples; ++sample)
{
    long started = Stopwatch.GetTimestamp();
    checksum = BenchmarkKernels.Run(workloadId, iterations, seed);
    long stopped = Stopwatch.GetTimestamp();
    long elapsedNanoseconds = (long)Math.Round(
        (stopped - started) * (1_000_000_000.0 / Stopwatch.Frequency));
    Console.WriteLine(string.Join(',',
        Csv(backend), Csv("nativeaot"), Csv(workload),
        iterations.ToString(CultureInfo.InvariantCulture),
        sample.ToString(CultureInfo.InvariantCulture),
        elapsedNanoseconds.ToString(CultureInfo.InvariantCulture),
        checksum.ToString(CultureInfo.InvariantCulture), "0"));
}
