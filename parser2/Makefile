cppFiles = parser.cpp statsTest.cpp DataPointTest.cpp resist2temp.cpp
hppFiles = $(wildcard *.hpp)

all: $(cppFiles:.cpp=) 

test: parser 
	./parser ../7Vtrial6.csv test.csv 2 10 6 0.61 1.62 293 -1.2
stats: statsTest
	./statsTest

resist: resist2temp

wien: WiensLaw
	touch Wien.dat
	rm Wien.dat
	./WiensLaw .81 295 5 .51
	./WiensLaw .81 295 6 .56
	./WiensLaw .81 295 7 .61
	./WiensLaw .81 295 8 .66
	./WiensLaw .81 295 9 .70
	./WiensLaw .81 295 10 .74

%: %.cpp $(hppFiles)
	g++ -O3 -std=c++11 -Wall -Werror $< -o $@
