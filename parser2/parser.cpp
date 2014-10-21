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

template<>
const char * Measurement<BigFloat>::printChar = ";";

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

	//output the pertinent information
	cout << "Temp:       " << power.getTemp() << " K" << endl;
	cout << "Temp0:      " << power.getTemp0() << " K" << endl;
	cout << "HotResist:  " << power.getHotResist() << " Ω" << endl;
	cout << "ColdResist: " << power.getColdResist() << " Ω" << endl;
	cout << "Voltage:    " << power.getVoltage() << " V" << endl;
	cout << "Current:    " << power.getCurrent() << " A" << endl;
	cout << "Wavelength: " << power.getWavelength() << " nm" << endl;
	cout << "Intensity:  " << power.getIntensity() << " W/m^2" << endl << endl;
	cout << "Density:    " << power.getPeakDensity() << " W/m^2 / nm at "<< power.getWavelength() << endl << endl;

	//calibrate data according to the temperature of the bulb, and the expected flux
	int sampleSize = static_cast<int>(samples);
	if(sampleSize <=0) sampleSize = 1;
	BigMeasure timeRangeM(timeRange);
	TempCalibration<BigMeasure> tempCal(data, sampleSize, timeRange, power.getTemp());
	tempCal.calibrateDataPoints(data);

	//output the file
	cout << endl << "Outputting to file in form:" << endl;
	cout << left << setw(30) << "Time(s);" << setw(30) << "Voltage(V);" << setw(30) << "CalibratedAngle(°);" << setw(30) << "Wavelength(nm);" << setw(30) << "Intensity(W/m^2 / nm)" << endl;
	int size = data.size();
	outputFile.precision(10);
	outputFile << left;
	for(int j = 0; j < size; j++){
		outputFile << setw(30) << data[j].getTime() << ';';
		outputFile << setw(30) << data[j].getPotential() << ';';
		outputFile << setw(30) << data[j].getAngle() << ';';
		outputFile << setw(30) << data[j].getWavelength() << ';';
		outputFile << setw(30) << data[j].getPower() << endl;
	}
}
