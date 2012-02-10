//          Copyright Lepesme "Jiboo" Jean-Baptiste 2012
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace boost {
namespace network {
namespace hashs {

namespace byteorder
{
	inline uint8_t flip(uint8_t i)
	{
		return i;
	}

	inline uint16_t flip(uint16_t i)
	{
		return ((i << 8) & 0xFF00) | ((i >> 8) & 0x00FF);
	}
	
	inline uint32_t flip(uint32_t i)
	{
		return 
			  ((i << 24) & 0xFF000000)
			| ((i >> 24) & 0x000000FF)
			| ((i << 8)  & 0x00FF0000)
			| ((i >> 8)  & 0x0000FF00);
	}

	inline uint64_t flip(uint64_t i)
	{
		uint32_t lo = static_cast<uint32_t>(i);
		uint32_t hi = static_cast<uint32_t>(i >> 32);
		
		return (static_cast<uint64_t>(hi) << 32) | lo;
	}

	
	template<typename T>
	inline T little(T i)
	{
		#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			return i;
		#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			return flip(i);
		#else
			#error Unsupported __BYTE_ORDER__ value
		#endif
	}
	
	template<typename T>
	inline T big(T i)
	{
		#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			return flip(i);
		#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			return i;
		#else
			#error Unsupported __BYTE_ORDER__ value
		#endif
	}
} // namespace byteorder

} // namespace hashs
} // namespace network
} // namespace boost

