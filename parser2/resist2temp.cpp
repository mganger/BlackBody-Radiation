#include "DataManip.hpp"
#include "Laws.hpp"

#include <iostream>

using namespace std;

int main(int argc, char** argv){
	if(argc != 5){
		cout << "resist2temp resistance temp voltage current" << endl;
		exit(1);
	}
	double resistance(atof(argv[1])), temp(atof(argv[2])), voltage(atof(argv[3])), current(atof(argv[4]));
	PowerOutput<double> power(resistance, temp, voltage, current);
	cout << power.getTemp() << endl;
}
