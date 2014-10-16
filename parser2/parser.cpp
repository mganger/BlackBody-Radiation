//this program converts the table of values to wavelength

#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "Measurement-Type/Measurement.hpp"
#include "Stats.hpp"
#include "DataPoint.hpp"
#include "FileIO.hpp"
#include "DataManip.hpp"

using namespace std;
using namespace boost::multiprecision;

typedef cpp_dec_float_50 BigFloat;
typedef Measurement< cpp_dec_float_50 > BigMeasure;

//parse the input arguments to a string array
vector<string> toStringArray(int argc, char ** argv){
	vector<string> output;
	for(int i = 1; i < argc; i++){
		output.emplace_back(argv[i]);
	}
	return output;
}



int main(int argc, char ** argv){
	vector<string> args = toStringArray(argc, argv);
	if(args.size() != 9){
		cout << "Wrong number of arguments" << endl;
		cout << "input output samples timeRange voltage current resistance temperature slope" << endl;
		exit(1);
	}
	cout.precision(3);

	string inputName, outputName;
	BigFloat samples(args[2]), timeRange(args[3]), voltage(args[4]), current(args[5]), resistance(args[6]), temperature(args[7]), slope(args[8]);
	BigFloat a(13900.0), b(1.689);

	AngleCalibration<BigMeasure>::setSlope(slope);
	Wavelength<BigMeasure>::setA(a);
	Wavelength<BigMeasure>::setB(b);

	//take the input file
	InputFile<BigMeasure> rawFile(args[0]);
	ofstream outputFile(args[1], ios::trunc);
	vector<DataPoint<BigMeasure> > data = rawFile.getPointArray();

	//find the WiensLaw stuff
	BigMeasure voltageM(voltage);
	BigMeasure currentM(current);
	BigMeasure resistanceM(resistance);
	BigMeasure temperatureM(temperature);
	PowerOutput<BigMeasure> power(resistanceM, temperatureM, voltageM, currentM);

	cout << "Temp:       " << power.getTemp() << endl;
	cout << "Temp0:      " << power.getTemp0() << endl;
	cout << "HotResist:  " << power.getHotResist() << endl;
	cout << "ColdResist: " << power.getColdResist() << endl;
	cout << "Voltage:    " << power.getVoltage() << endl;
	cout << "Current:    " << power.getCurrent() << endl;
	cout << "Wavelength: " << power.getWavelength() << endl;
	cout << "Intensity:  " << power.getIntensity() << endl << endl;

	//calibrate points
	int sampleSize = static_cast<int>(samples);
	if(sampleSize <=0) sampleSize = 1;

	BigMeasure timeRangeM(timeRange);
	TempCalibration<BigMeasure> tempCal(data, sampleSize, timeRange, temperatureM);
	tempCal.calibrateDataPoints(data);

	//output the file
	cout << endl << "Outputting to file in form:" << endl;
	cout << "Time(s); Voltage(V); CalibratedAngle(°); Wavelength(nm); Intensity(W/m^2 / nm)" << endl;
	int size = data.size();
	outputFile.precision(10);
	for(int j = 0; j < size; j++){
		outputFile << data[j].getTime() << ';';
		outputFile << data[j].getPotential() << ';';
		outputFile << data[j].getAngle() << ';';
		outputFile << data[j].getWavelength() << ';';
		outputFile << data[j].getPower() << endl;
	}
}
