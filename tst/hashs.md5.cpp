//          Copyright Lepesme "Jiboo" Jean-Baptiste 2012
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/io/ios_state.hpp>
#include <gtest/gtest.h>

#include "md5.hpp"

using namespace boost::network::hashs;

std::ostream& operator<<(std::ostream &os, const boost::network::hashs::md5::digest &digest)
{
	boost::io::ios_all_saver guard(os);
	
	os << std::hex << std::noshowbase << std::setfill('0');
	for(int i = 0; i < 4; i++)
		os << std::setw(8) << digest[i] << ' ';
	
	return os;
}

bool tests(const std::string &s, const boost::network::hashs::md5::digest &expect)
{
	std::stringstream is(s);
	auto digest = boost::network::hashs::md5(is).hash();
	
	if(digest != expect)
	{
		std::cout << "Digest of " << s << std::endl;
		std::cout << "\tExpected :\t" << expect << std::endl;
		std::cout << "\tGot :\t\t" << digest << std::endl;
		return false;
	}
	return true;
}

bool testf(const std::string &f, const boost::network::hashs::md5::digest &expect)
{
	std::ifstream is(f, std::ios::in | std::ios::binary);
	if(!is.is_open())
	{
		return false;
	}
	auto digest = boost::network::hashs::md5(is).hash();
	is.close();

	if(digest != expect)
	{
		std::cout << "Digest of " << f << std::endl;
		std::cout << "\tExpected :\t" << expect << std::endl;
		std::cout << "\tGot :\t\t" << digest << std::endl;
		return false;
	}
	return true;
}

TEST(hashs_md5, empty)
{
	EXPECT_TRUE(tests("", {{0xd41d8cd9, 0x8f00b204, 0xe9800998, 0xecf8427e}}));
}

TEST(hashs_md5, a)
{
	EXPECT_TRUE(tests("a", {{0x0cc175b9, 0xc0f1b6a8, 0x31c399e2, 0x69772661}}));
}

TEST(hashs_md5, abc)
{
	EXPECT_TRUE(tests("abc", {{0x90015098, 0x3cd24fb0, 0xd6963f7d, 0x28e17f72}}));
}

TEST(hashs_md5, message_digest)
{
	EXPECT_TRUE(tests("message digest",	{{0xf96b697d, 0x7cb7938d, 0x525a2f31, 0xaaf161d0}}));
}

TEST(hashs_md5, _long)
{
	EXPECT_TRUE(tests(
		"12345678901234567890123456789012345678901234567890123456789012345678901234567890",	
		{{0x57edf4a2, 0x2be3c955, 0xac49da2e, 0x2107b67a}}));
}

TEST(hashs_md5, tst_main_cpp)
{
	EXPECT_TRUE(testf("tst/main.cpp", {{0x8ad398d4, 0x02ddb697, 0x69542bc3, 0xebacc822}}));
}

TEST(hashs_md5, tst_sample_cpp)
{
	EXPECT_TRUE(testf("tst/sample.json", {{0x2b2cf1f7, 0x2f00fea6, 0xdcad8585, 0xa9f8f4e5}}));
}

