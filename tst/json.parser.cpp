//          Copyright Lepesme "Jiboo" Jean-Baptiste 2012
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <chrono>
#include <sstream>
#include <fstream>
#include <gtest/gtest.h>
#include "json.hpp"

TEST(json_parser, complex)
{
	std::string json("{\"lol\":\"test\",\n\t\n        \"real\"\n:\t   0.21, \"int\": 12393, \"true\": true, \"false\": false, \"null\": null,"
		"\"array\": [1,2,3], \"complex\":{\"sub1\":[1,2,3], \"sub2\":[1,2,3]}, \"escape\":\"tab\\ttab\\u2018\"}");

	std::stringstream ss(json);
	json::value test;
	ss >> test;
	
	EXPECT_EQ(test.get("complex").get("sub1").get(0).to_integer(), 1);
}

TEST(json_parser, unicode)
{
	std::string json("{\"uni\":\"\\u2018\"}");

	std::stringstream ss(json);
	json::value test;
	ss >> test;
	
	EXPECT_EQ(test.get("uni").to_string(), "\u2018");
}

TEST(json_parser, empty)
{
	std::string json("{\"array\":[], \"object\":{}}");

	std::stringstream ss(json);
	json::value test;
	ss >> test;
	
	EXPECT_EQ(test.get("array").to_array().size(), 0);
	EXPECT_EQ(test.get("object").to_object().size(), 0);
}

TEST(json_parser, json_test_suite_sample)
{
	std::ifstream sample("tst/sample.json");

	json::value test;
	sample >> test;
}

