using System;
using System.Runtime.CompilerServices;

namespace Rdr2.Benchmarks;

public static class BenchmarkKernels
{
    public const int AbiVersion = 1;

    public static long Run(int workloadId, int iterations, long seed) =>
        workloadId switch
        {
            0 => IntegerMix(iterations, unchecked((uint)seed)),
            1 => Branching(iterations, unchecked((uint)seed)),
            2 => MemoryTransform(iterations, unchecked((uint)seed)),
            3 => CallChain(iterations, unchecked((uint)seed)),
            _ => throw new ArgumentOutOfRangeException(nameof(workloadId)),
        };

    [MethodImpl(MethodImplOptions.AggressiveOptimization)]
    private static long IntegerMix(int iterations, uint seed)
    {
        unchecked
        {
            uint value = seed | 1u;
            uint checksum = 0x9e3779b9u;
            for (int index = 0; index < iterations; ++index)
            {
                value = value * 1664525u + 1013904223u;
                value ^= value >> 13;
                value *= 2246822519u;
                checksum += value ^ (uint)index;
            }
            return (long)(((ulong)value << 32) | checksum);
        }
    }

    [MethodImpl(MethodImplOptions.AggressiveOptimization)]
    private static long Branching(int iterations, uint seed)
    {
        unchecked
        {
            uint value = seed | 1u;
            uint checksum = 0;
            for (int index = 0; index < iterations; ++index)
            {
                if ((value & 1u) != 0)
                    value = value * 3u + 1u;
                else
                    value >>= 1;

                if (value < 16u)
                    value += seed ^ (uint)index;
                checksum ^= value + (uint)index * 2654435761u;
            }
            return (long)(((ulong)value << 32) | checksum);
        }
    }

    [MethodImpl(MethodImplOptions.AggressiveOptimization)]
    private static long MemoryTransform(int iterations, uint seed)
    {
        unchecked
        {
            var values = new uint[4096];
            uint state = seed;
            for (int index = 0; index < values.Length; ++index)
            {
                state = state * 1664525u + 1013904223u;
                values[index] = state;
            }

            for (int round = 0; round < iterations; ++round)
            {
                uint carry = (uint)round + seed;
                for (int index = 0; index < values.Length; ++index)
                {
                    uint value = values[index];
                    value = (value ^ carry) * 2246822519u;
                    carry = (carry << 5) | (carry >> 27);
                    values[index] = value + carry;
                }
            }

            uint checksum = 0;
            for (int index = 0; index < values.Length; ++index)
                checksum = (checksum * 16777619u) ^ values[index];
            return (long)(((ulong)state << 32) | checksum);
        }
    }

    [MethodImpl(MethodImplOptions.AggressiveOptimization)]
    private static long CallChain(int iterations, uint seed)
    {
        unchecked
        {
            uint value = seed;
            uint checksum = 0;
            for (int index = 0; index < iterations; ++index)
            {
                value = Step0(value, (uint)index);
                checksum += value;
            }
            return (long)(((ulong)value << 32) | checksum);
        }
    }

    [MethodImpl(MethodImplOptions.NoInlining)]
    private static uint Step0(uint value, uint salt) => Step1(value + salt);

    [MethodImpl(MethodImplOptions.NoInlining)]
    private static uint Step1(uint value) => Step2(value ^ 0x85ebca6bu);

    [MethodImpl(MethodImplOptions.NoInlining)]
    private static uint Step2(uint value) => Step3(value * 3266489917u);

    [MethodImpl(MethodImplOptions.NoInlining)]
    private static uint Step3(uint value) => (value << 7) | (value >> 25);
}
