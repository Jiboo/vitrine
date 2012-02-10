#          Copyright Lepesme "Jiboo" Jean-Baptiste 2012
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

CXX=clang++
CXXFLAGS=-std=c++0x -Wall -Wextra -I. -g

TST_SRC=$(wildcard tst/*.cpp)
TST_OBJ=$(TST_SRC:.cpp=.cpp.o)
TST_LIB=-lpthread -lgtest

all: test

clean:
	rm tst/*.o

test: $(TST_OBJ)
	$(CXX) $(CXXFLAGS) $^ $(TST_LIB) -o $@

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

