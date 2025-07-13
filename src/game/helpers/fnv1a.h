#pragma once
#include <cstdint>
#include <cstddef>
#include <string.h>

namespace fnv1a
{
	inline constexpr uint32_t uHash32 = 0x811C9DC5;
	inline constexpr uint32_t uPrime32 = 0x1000193;
	inline constexpr uint64_t uHash64 = 0xcbf29ce484222325;
	inline constexpr uint64_t uPrime64 = 0x100000001b3;

	// compile-time hashes
	constexpr uint32_t hash32Const(const char* szString, const uint32_t uValue = uHash32) noexcept
	{
		return (szString[0] == '\0') ? uValue : hash32Const(&szString[1], (uValue ^ uint32_t(szString[0])) * uPrime32);
	}
	constexpr uint64_t hash64Const(const char* szString, const uint64_t uValue = uHash64) noexcept
	{
		return (szString[0] == '\0') ? uValue : hash64Const(&szString[1], (uValue ^ uint64_t(szString[0])) * uPrime64);
	}

	// runtime hashes
	inline uint32_t hash32(const char* szString)
	{
		uint32_t uHashed = uHash32;

		for (std::size_t i = 0U; i < strlen(szString); ++i)
		{
			uHashed ^= szString[i];
			uHashed *= uPrime32;
		}

		return uHashed;
	}
	inline uint64_t hash64(const char* szString)
	{
		uint64_t uHashed = uHash64;

		for (std::size_t i = 0U; i < strlen(szString); ++i)
		{
			uHashed ^= szString[i];
			uHashed *= uPrime64;
		}

		return uHashed;
	}
}