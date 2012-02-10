//          Copyright Lepesme "Jiboo" Jean-Baptiste 2012
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdint>
#include <array>
#include <istream>

#include "byteorder.hpp"

namespace boost {
namespace network {
namespace hashs {

class md5
{
public:
	typedef std::array<uint32_t, 4> digest;

private:
	std::istream &source;
	size_t pos = 0, size = 0;
	size_t nbwords;

	uint32_t A = byteorder::little(0x67452301);
	uint32_t B = byteorder::little(0xefcdab89);
	uint32_t C = byteorder::little(0x98badcfe);
	uint32_t D = byteorder::little(0x10325476);

	template<typename T>
	inline T leftrotate(T i, int shift)
	{
		return (i << shift) | (i >> ((sizeof(T)*8) - shift));
	}

	inline void fstep(
		uint32_t &a, uint32_t b, uint32_t c, uint32_t d,
		int x, int s, int t)
	{
		a = b + leftrotate((a + ((b & c) | (~(b) & d)) + x + t), s);
	}
	
	inline void gstep(
		uint32_t &a, uint32_t b, uint32_t c, uint32_t d,
		int x, int s, int t)
	{
		a = b + leftrotate((a + ((b & d) | (c & ~(d))) + x + t), s);
	}
	
	inline void hstep(
		uint32_t &a, uint32_t b, uint32_t c, uint32_t d,
		int x, int s, int t)
	{
		a = b + leftrotate((a + (b ^ c ^ d) + x + t), s);
	}
	
	inline void istep(
		uint32_t &a, uint32_t b, uint32_t c, uint32_t d,
		int x, int s, int t)
	{
		a = b + leftrotate((a + (c ^ (b | ~(d))) + x + t), s);
	}
	
	inline uint32_t next_word()
	{
		int read = 0;
		unsigned char c[4];
		
		if(!source.eof())
		{
			for(read = 0; !source.eof() && (read < 4); read++)
				c[read] = source.get();
			
			if(!source.eof())
			{
				pos += 4;
			
				return byteorder::little(
					  (static_cast<uint32_t>(c[0]) <<  0)
					| (static_cast<uint32_t>(c[1]) <<  8)
					| (static_cast<uint32_t>(c[2]) << 16)
					| (static_cast<uint32_t>(c[3]) << 24));
			}
			else
			{
				this->size = this->pos + (--read);

				size_t chunks = ((this->size) / 64) + 1;
				this->nbwords = chunks * 16;
				
				this->pos += 4;

				switch(read)
				{
					case 0:
						return byteorder::little(0x00000080);
					case 1:
						return byteorder::little(
							  (static_cast<uint32_t>(c[0]) <<  0)
							| 0x00008000);
					case 2:
						return byteorder::little(
							  (static_cast<uint32_t>(c[0]) <<  0)
							| (static_cast<uint32_t>(c[1]) <<  8)
							| 0x00800000);
					case 3:
						return byteorder::little(
							  (static_cast<uint32_t>(c[0]) <<  0)
							| (static_cast<uint32_t>(c[1]) <<  8)
							| (static_cast<uint32_t>(c[2]) << 16)
							| 0x80000000);
				}
			}
		}
		else
		{
			this->pos += 4;
		
			if((this->pos / 4) >= this->nbwords - 2)
			{
				
				if((pos / 4) == this->nbwords - 1)
					return byteorder::little(static_cast<uint32_t>((this->size * 8) >> 0));
				else if((pos / 4) == this->nbwords - 0)
					return byteorder::little(static_cast<uint32_t>((this->size * 8) >> 32));
			}
			else
				return 0x00000000;
		}
		return 0x00000000;
	}

	inline void step(const uint32_t X[16])
	{
		/* Round 1 */
		fstep(A, B, C, D, X[ 0],  7, 0xd76aa478); //FIXME: Should be little endian?
		fstep(D, A, B, C, X[ 1], 12, 0xe8c7b756);
		fstep(C, D, A, B, X[ 2], 17, 0x242070db);
		fstep(B, C, D, A, X[ 3], 22, 0xc1bdceee);
	
		fstep(A, B, C, D, X[ 4],  7, 0xf57c0faf);
		fstep(D, A, B, C, X[ 5], 12, 0x4787c62a);
		fstep(C, D, A, B, X[ 6], 17, 0xa8304613);
		fstep(B, C, D, A, X[ 7], 22, 0xfd469501);
	
		fstep(A, B, C, D, X[ 8],  7, 0x698098d8);
		fstep(D, A, B, C, X[ 9], 12, 0x8b44f7af);
		fstep(C, D, A, B, X[10], 17, 0xffff5bb1);
		fstep(B, C, D, A, X[11], 22, 0x895cd7be);
	
		fstep(A, B, C, D, X[12],  7, 0x6b901122);
		fstep(D, A, B, C, X[13], 12, 0xfd987193);
		fstep(C, D, A, B, X[14], 17, 0xa679438e);
		fstep(B, C, D, A, X[15], 22, 0x49b40821);
	
		/* Round 2 */
		gstep(A, B, C, D, X[ 1],  5, 0xf61e2562);
		gstep(D, A, B, C, X[ 6],  9, 0xc040b340);
		gstep(C, D, A, B, X[11], 14, 0x265e5a51);
		gstep(B, C, D, A, X[ 0], 20, 0xe9b6c7aa);
	
		gstep(A, B, C, D, X[ 5],  5, 0xd62f105d);
		gstep(D, A, B, C, X[10],  9, 0x02441453);
		gstep(C, D, A, B, X[15], 14, 0xd8a1e681);
		gstep(B, C, D, A, X[ 4], 20, 0xe7d3fbc8);
	
		gstep(A, B, C, D, X[ 9],  5, 0x21e1cde6);
		gstep(D, A, B, C, X[14],  9, 0xc33707d6);
		gstep(C, D, A, B, X[ 3], 14, 0xf4d50d87);
		gstep(B, C, D, A, X[ 8], 20, 0x455a14ed);
	
		gstep(A, B, C, D, X[13],  5, 0xa9e3e905);
		gstep(D, A, B, C, X[ 2],  9, 0xfcefa3f8);
		gstep(C, D, A, B, X[ 7], 14, 0x676f02d9);
		gstep(B, C, D, A, X[12], 20, 0x8d2a4c8a);
	
		/* Round 3 */
		hstep(A, B, C, D, X[ 5],  4, 0xfffa3942);
		hstep(D, A, B, C, X[ 8], 11, 0x8771f681);
		hstep(C, D, A, B, X[11], 16, 0x6d9d6122);
		hstep(B, C, D, A, X[14], 23, 0xfde5380c);
	
		hstep(A, B, C, D, X[ 1],  4, 0xa4beea44);
		hstep(D, A, B, C, X[ 4], 11, 0x4bdecfa9);
		hstep(C, D, A, B, X[ 7], 16, 0xf6bb4b60);
		hstep(B, C, D, A, X[10], 23, 0xbebfbc70);
	
		hstep(A, B, C, D, X[13],  4, 0x289b7ec6);
		hstep(D, A, B, C, X[ 0], 11, 0xeaa127fa);
		hstep(C, D, A, B, X[ 3], 16, 0xd4ef3085);
		hstep(B, C, D, A, X[ 6], 23, 0x04881d05);
	
		hstep(A, B, C, D, X[ 9],  4, 0xd9d4d039);
		hstep(D, A, B, C, X[12], 11, 0xe6db99e5);
		hstep(C, D, A, B, X[15], 16, 0x1fa27cf8);
		hstep(B, C, D, A, X[ 2], 23, 0xc4ac5665);
	
		/* Round 4 */
		istep(A, B, C, D, X[ 0],  6, 0xf4292244);
		istep(D, A, B, C, X[ 7], 10, 0x432aff97);
		istep(C, D, A, B, X[14], 15, 0xab9423a7);
		istep(B, C, D, A, X[ 5], 21, 0xfc93a039);
	
		istep(A, B, C, D, X[12],  6, 0x655b59c3);
		istep(D, A, B, C, X[ 3], 10, 0x8f0ccc92);
		istep(C, D, A, B, X[10], 15, 0xffeff47d);
		istep(B, C, D, A, X[ 1], 21, 0x85845dd1);
	
		istep(A, B, C, D, X[ 8],  6, 0x6fa87e4f);
		istep(D, A, B, C, X[15], 10, 0xfe2ce6e0);
		istep(C, D, A, B, X[ 6], 15, 0xa3014314);
		istep(B, C, D, A, X[13], 21, 0x4e0811a1);
	
		istep(A, B, C, D, X[ 4],  6, 0xf7537e82);
		istep(D, A, B, C, X[11], 10, 0xbd3af235);
		istep(C, D, A, B, X[ 2], 15, 0x2ad7d2bb);
		istep(B, C, D, A, X[ 9], 21, 0xeb86d391);
	}

public:
	md5(std::istream &target) : source(target)
	{
	}

	digest hash()
	{
		uint32_t AA, BB, CC, DD;
		uint32_t X[16];

		for(size_t i = 0; !source.eof() || (i < this->nbwords); i+=16)
		{
			AA = A;
			BB = B;
			CC = C;
			DD = D;

			for(size_t j = 0; j < 16; j++)
				X[j] = this->next_word();

			this->step(X);

			A += AA;
			B += BB;
			C += CC;
			D += DD;
		}
	
		return {{
			byteorder::big(A),
			byteorder::big(B),
			byteorder::big(C),
			byteorder::big(D)}};
	}
};

} // namespace hashs
} // namespace network
} // namespace boost

