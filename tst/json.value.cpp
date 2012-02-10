//          Copyright Lepesme "Jiboo" Jean-Baptiste 2012
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include "json.hpp"

TEST(json_value, nulls)
{
	json::value v;
	EXPECT_TRUE(v.is_null());
}

TEST(json_value, bools)
{
	json::value v = false;
	EXPECT_TRUE(v.is_bool());
	EXPECT_FALSE(v.to_bool());
}

TEST(json_value, ints)
{
	json::value v = 1234;
	EXPECT_TRUE(v.is_integer());
	EXPECT_TRUE(v.is_number());
	EXPECT_EQ(v.to_integer(), 1234);
}

TEST(json_value, reals)
{
	json::value v = 12.34;
	EXPECT_TRUE(v.is_real());
	EXPECT_TRUE(v.is_number());
	EXPECT_EQ(v.to_real(), 12.34);
}

TEST(json_value, strings)
{
	json::value v = "foo";
	EXPECT_TRUE(v.is_string());
	EXPECT_EQ(v.to_string(), std::string("foo"));
}

TEST(json_value, arrays)
{
	std::string bar("bar");

	json::value arr = json::array();
	
	EXPECT_TRUE(arr.is_array());
	EXPECT_NO_THROW(arr.to_array());
	EXPECT_EQ(arr.size(), 0);
	
	arr.add(json::value());
	arr.add(true);
	arr.add(false);
	arr.add(0ll);
	arr.add(1234ll);
	arr.add(0.0);
	arr.add(12.34);
	arr.add("foo");
	arr.add(bar);
	arr.add(arr);

	EXPECT_EQ(arr.size(), 10);
	EXPECT_EQ(arr.get(9).size(), 9); // If not pass, it's a reference and not a copy, should it be a ref?
	
	EXPECT_TRUE(arr.get(0).is_null());
	EXPECT_TRUE(arr.get(1).to_bool());
}

TEST(json_value, objects)
{
	std::string bar("bar");

	json::value obj = json::object();
	
	EXPECT_TRUE(obj.is_object());
	EXPECT_NO_THROW(obj.to_object());
	EXPECT_EQ(obj.size(), 0);
	
	obj.add("null", json::value());
	obj.add("true", true);
	obj.add("false", false);
	obj.add("int1", 0ll);
	obj.add("int2", 1234ll);
	obj.add("dbl1", 0.0);
	obj.add("dbl2", 12.34);
	obj.add("str1", "foo");
	obj.add("str2", bar);
	obj.add("obj", obj);

	EXPECT_EQ(obj.size(), 10);
	EXPECT_EQ(obj.get("obj").size(), 9); // If not pass, it's a reference and not a copy, should it be a ref?
	
	EXPECT_TRUE(obj.get("null").is_null());
	EXPECT_TRUE(obj.get("true").to_bool());
}

