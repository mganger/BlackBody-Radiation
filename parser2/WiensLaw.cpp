#include "DataManip.hpp"
#include "Measurement-Type/Measurement.hpp"

#include <iostream>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <vector>
#include <fstream>

#include <iostream>
//#include <string>
//#include "FileIO.hpp"

using namespace std;
using namespace boost::multiprecision;

typedef cpp_dec_float_50 BigFloat;
typedef Measurement< cpp_dec_float_50 > BigMeasure;


template<>
const char * Measurement<BigFloat>::printChar = ";";

vector<string> toStringArray(int argc, char ** argv){
	vector<string> output;
	for(int i = 1; i < argc; i++){
		output.emplace_back(argv[i]);
	}
	return output;
}


int main(int argc, char ** argv){
	vector<string> args = toStringArray(argc,argv);

	if(args.size() != 4){cout << "Wrong number of arguments" << endl << "resistance, temperature, voltage, current"<< endl;exit(1);}

	vector<string> data;
	vector<string> uncertainty;
	string dataString;

	ifstream inputFile("../Adjusted/MaxWavelength");
	getline(inputFile,dataString);
	cout << dataString << endl;

	ofstream outputFile;
	outputFile.open("Wien.dat",ios::app);

//	TempCalibration<BigMeasure> calibration;

	BigFloat resistance(args[0]),temperature(args[1]),voltage(args[2]),current(args[3]);
	cout << "Voltage:	"<< voltage << endl;
	PowerOutput<BigMeasure> power(resistance,temperature,voltage,current);
	cout << "Temperature: " << power.getTemp() << " K" << endl;
	cout << "Wavelength: " << power.getWavelength() << " K" << endl;
	cout << "-------------"<<endl;

	outputFile << voltage << ";	" << power.getWavelength() << ";	" << endl;
//										  << calibration.getMaxWavelength() << endl;
	outputFile.close();

}
