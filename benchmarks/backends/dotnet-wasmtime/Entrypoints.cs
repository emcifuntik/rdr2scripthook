using System.Runtime.InteropServices;
using Rdr2.Benchmarks;

namespace Rdr2.Benchmarks.DotNetWasmtime;

public static class Entrypoints
{
    [UnmanagedCallersOnly(EntryPoint = "bench_abi_version")]
    public static int AbiVersion() => BenchmarkKernels.AbiVersion;

    [UnmanagedCallersOnly(EntryPoint = "bench_run")]
    public static long Run(int workloadId, int iterations, long seed) =>
        BenchmarkKernels.Run(workloadId, iterations, seed);
}
