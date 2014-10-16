cppFiles = parser.cpp statsTest.cpp
hppFiles = $(wildcard *.hpp)

all: $(cppFiles:.cpp=) 

test: parser 
	./parser ../7Vtrial6.csv test.csv 2 10 6 0.61 .84 273 -1.2
stats: statsTest
	./statsTest

%: %.cpp $(hppFiles)
	g++ -O3 -std=c++11 -Wall -Werror $< -o $@
