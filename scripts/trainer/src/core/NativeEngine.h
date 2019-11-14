#pragma once

#include <cstdint>
#include <unordered_map>
#include <type_traits>

typedef uintptr_t(*GetNativeAddressFunc)(uint64_t hash);

struct Vector3f
{
	float x;
	float y;
	float z;
};

namespace Native
{
	class Context
	{
		// Internal RAGE stuff
		uint64_t* retVal = stack;
		uint64_t argCount = 0;
		uint64_t* stackPtr = stack;
		uint64_t dataCount = 0;
		uint64_t spaceForResults[24];

		// Our stack
		uint64_t stack[24]{ 0 };

	public:
		template<class T>
		T& At(uint32_t idx)
		{
			static_assert(sizeof(T) <= 8, "Argument is too big");

			return *reinterpret_cast<T*>(stack + idx);
		}

		uint32_t GetArgsCount()
		{
			return argCount;
		}

		void SetArgsCount(uint32_t idx)
		{
			argCount = idx;
		}

		template<class T, class... Args>
		void Push(T arg, Args... args)
		{
			static_assert(sizeof(T) <= 8, "Argument is too big");

			*(T*)(stack + argCount++) = arg;

			if constexpr (sizeof...(Args) > 0)
				Push(args...);
		}

		template<class T>
		T Result()
		{
			return *reinterpret_cast<T*>(retVal);
		}
		template<>
		void Result<void>() { }

		template<>
		Vector3f Result<Vector3f>()
		{
			Vector3f vec;
			vec.x = *(float*)((uintptr_t)retVal + 0);
			vec.y = *(float*)((uintptr_t)retVal + 8);
			vec.z = *(float*)((uintptr_t)retVal + 16);
			return vec;
		}

		void Reset()
		{
			argCount = 0;
			dataCount = 0;
		}

		void CopyResults()
		{
			uint64_t a1 = (uint64_t)this;

			uint64_t result;

			for (; *(uint32_t *)(a1 + 24); *(uint32_t*)(*(uint64_t *)(a1 + 8i64 * *(signed int *)(a1 + 24) + 32) + 16i64) = result)
			{
				--*(uint32_t*)(a1 + 24);
				**(uint32_t **)(a1 + 8i64 * *(signed int *)(a1 + 24) + 32) = *(uint32_t*)(a1 + 16 * (*(signed int *)(a1 + 24) + 4i64));
				*(uint32_t*)(*(uint64_t *)(a1 + 8i64 * *(signed int *)(a1 + 24) + 32) + 8i64) = *(uint32_t*)(a1
					+ 16i64
					* *(signed int *)(a1 + 24)
					+ 68);
				result = *(unsigned int *)(a1 + 16i64 * *(signed int *)(a1 + 24) + 72);
			}
			--*(uint32_t*)(a1 + 24);
		}
	};

	typedef void(__cdecl *Handler)(Context * context);

	inline GetNativeAddressFunc& GetFunc()
	{
		static GetNativeAddressFunc _nativeAddressGetFunc;
		return _nativeAddressGetFunc;
	}

	inline void SetEssentialFunction(GetNativeAddressFunc func)
	{
		GetFunc() = func;
	}

	inline Handler GetHandler(uint64_t hashName)
	{
		return (Handler)GetFunc()(hashName);
	}

	void Invoke(Handler fn, Context& ctx);

	template<class Retn = uint64_t, class... Args>
	Retn Invoke(Handler fn, Args... args)
	{
		static Context ctx;

		if (!fn) return Retn();

		ctx.Reset();

		if constexpr (sizeof...(Args) > 0)
			ctx.Push(args...);

		Invoke(fn, ctx);
		return ctx.Result<Retn>();
	}

	template<class Retn = uint64_t, class... Args>
	Retn Invoke(uint64_t hashName, Args... args)
	{
		return Invoke<Retn>(GetHandler(hashName), args...);
	}

	inline void Invoke(uint64_t hashName, Context& ctx)
	{
		Invoke(GetHandler(hashName), ctx);
	}
};