//this program converts the table of values to wavelength

#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "Measurement-Type/Measurement.hpp"
#include "Stats.hpp"

using namespace std;
using namespace boost::multiprecision;

typedef cpp_dec_float_50 BigFloat;
typedef Measurement< BigFloat > BigMeasure;

template <class T>
class AngleCalibration {
	public:
		static void setSlope(T inputSlope){slope = inputSlope;}
		static T calibrate(T input){
			return input*slope;
		}
	protected:
		static T slope;
};
template <class T> T AngleCalibration<T>::slope = 1;




template <class T>
class Wavelength {
	private:
		static T A;
		static T B;

	protected:
		//methods
		static T n(T theta){
			T squared = 2.0/sqrt(3) * sin(toRadians(theta)) + 1.0/2.0;
			return sqrt(squared*squared + 3.0/4.0);
		}
		static T toRadians(T degrees){return degrees * atan(1)*4 / 180.0;}
		static T toWavelength(T theta){
			return sqrt(A / (n(theta) - B));
		}
	public:
		static void setA(T inputA){A = inputA;}
		static void setB(T inputB){B = inputB;}
};
template <class T> T Wavelength<T>::A = 1;
template <class T> T Wavelength<T>::B = 1;




template <class T>
class DataPoint : protected AngleCalibration<T>, public Wavelength<T> {
	private:
		T time;
		T potential;
		T rawAngle;
		T calAngle;
		T wavelength;
		T power;


	public:
		//constructor
		DataPoint(T inputTime, T inputPotential, T inputAngle){
			time = inputTime;
			potential = inputPotential;
			rawAngle = inputAngle;
			calAngle = AngleCalibration<T>::calibrate(inputAngle);
			wavelength = Wavelength<T>::toWavelength(calAngle);
		}

		T getTime(){return time;}
		T getPotential(){return potential;}
		T getAngle(){return calAngle;}
		T getWavelength(){return wavelength;}
		T getPower(){return power;}
		void setPower(T input){power = input;}
};

template <class T>
class StefanBoltzmann {
	public:
		static T sigma;
		static T intensity(T temp){return sigma * pow(temp, 4);}
};

template <class T> T StefanBoltzmann<T>::sigma(5.67e-8);




template <class T>
class WiensLaw {
	private:
		T wConst = 2.89776829;
		T temperature;
		T wavelength;

	public:
		WiensLaw(){};
		WiensLaw(T temp){
			temperature = temp;
			wavelength = wConst / temp;
		}
		T getTemp(){return temperature;}
		T getWavelength(){return wavelength;}
};




template <class T>
class PowerDensity {
	private:
		T pi = M_PI;
		T h = 6.626e-34L;
		T k = 5.67e-8L;
		T c = 3e8L;

		T wavelength;
		T temperature;
		T density;

		T densityCalc(){
			T num, denum;
			num = 2 * pi * h * c*c;
			denum = pow(wavelength, 5) * (exp((h*c) / (wavelength * k * temperature)) - 1);
			cout << "num:   " <<  num << endl;
			cout << "denum: " << denum << endl;
			return num / denum;
		}
	public:
		PowerDensity(T inputWavelength, T inputTemperature){
			wavelength = inputWavelength * 1e-9;
			temperature= inputTemperature;
			density = densityCalc();
		}

		T getWavelength(){return wavelength * 1e9;}
		T getTemp(){return temperature;}
		T getDensity(){return density / 1e9;}
};




//this class handles the specific calibration of temperature data
template <class T>
class TempCalibration {
	private:
		T maximum;
		T baseline;
		T maxPower;
		T maxWavelength;

	public:
		TempCalibration(vector<DataPoint<T> >& input, int sampleSize, T timeRange, T temperature){
			maximum = maxDataPoint(input, sampleSize);
			baseline = findBaseline(input, timeRange);
			PowerDensity<T> maxDensity(maxWavelength, temperature);
			maxPower = maxDensity.getDensity();

			cout << sampleSize * 2 << " point moving average" << endl;
			cout << "Max:    " << maximum << " V" << endl;
			cout << "base:   " << baseline << " V" << endl;
			cout << "target: " << maxPower << " W/m^2 / nm" << endl;
		}

		//needs to get the baseline for the measurement, and then find the max temperature
		T maxDataPoint(vector<DataPoint<T> >& input, int sampleSize){
			int size = input.size();
			T max = input[0].getPotential();
			T peak = 0;
			for(int i = 1; i < size; i++){
				//sum the numbers
				Mean<T> meanObj;
				for(int j = i - sampleSize; j >= 0 && j < i + sampleSize && j < size; j++) meanObj.push_back(input[j].getPotential());
				T average(meanObj.mean());

				if(average > max){
					max = average;
					peak = input[i].getWavelength();
				}
			}
			maxWavelength = peak;
			cout << "Peak Wavelength: " << peak << " nm" << endl;
			return max;
		}
		T findBaseline(vector<DataPoint<T> >& input, T timeRange){
			T sum = 0;
			unsigned long int count = 0;
			int size = input.size();
			for(int i = 0; i < size; i++){
				if(input[i].getTime() > timeRange) break;
				sum += input[i].getPotential();
				count++;
			}
			if(count == 0) return 0;
			return sum / count;
		}

		//linear calibration, p=0 @ baseline and p = maxPower @ maximum
		void calibrateDataPoints(vector<DataPoint<T> >& input){
			int size = input.size();
			T slope = maxPower / (maximum - baseline);
			for(int i = 0; i < size; i++){
				input[i].setPower((input[i].getPotential() - baseline) * slope);
			}
		}
};


template <class T>
class InputFile {
	private:
		ifstream file;
		vector<DataPoint<T> > pointArray;
	protected:
		void fileToArray(){
			//clear the array, go to the beginning of the file
			pointArray.clear();
			file.seekg(0, file.beg);

			//remove the first line
			string tmp;
			getline(file, tmp);

			//put the rest of the lines into strings, and construct new objects
			for(;;){
				string time,voltage,angle,junk;
				getline(file, time, ',');
				getline(file, voltage, ',');
				getline(file, angle, ',');
				getline(file, junk);

				//break if any of the strings are empty (end of file)
				if(isEmpty(time) || isEmpty(voltage) || isEmpty(angle)) break;

				pointArray.emplace_back(T(time), T(voltage), T(angle));
			}
		}

		//helper methods
		bool isEmpty(string& input){
			return input.length() == 0;
		}
		double toDouble(string& input){
			return atof(input.c_str());
		}
	public:
		InputFile(char * filename){
			file.open(filename);
			fileToArray();
		}
		InputFile(string filename){
			file.open(filename.c_str());
			fileToArray();
		}
		~InputFile(){
			file.close();
		}

		vector<DataPoint<T> >& getPointArray(){return pointArray;}
};



//uses information about the light source to obtain the right blackbody spectrum
template <class T>
class PowerOutput {
	private:
		WiensLaw<T> prediction;
		T voltage;
		T current;
		T hotResistance;
		T coldResistance;
		T a0 = 4.5e-3;
		T sigma = 5.67e-8;

		T temp0;
		T temperature;
		T intensity;

		T tempCalc(){
			T resistRatio = hotResistance / coldResistance;
			return temp0 + (resistRatio - 1) / a0;
		}

	public:
		PowerOutput(T resistance, T startingTemp, T inputVoltage, T inputCurrent){
			voltage = inputVoltage;
			current = inputCurrent;
			coldResistance = resistance;
			hotResistance = voltage / current;
			temp0 = startingTemp;

			temperature = tempCalc();
			prediction = WiensLaw<T>(temperature);
			intensity = StefanBoltzmann<T>::intensity(temperature);
		}

		T getTemp(){return temperature;}
		T getTemp0(){return temp0;}
		T getHotResist(){return hotResistance;}
		T getColdResist(){return coldResistance;}
		T getVoltage(){return voltage;}
		T getCurrent(){return current;}
		T getWavelength(){return prediction.getWavelength();}
		T getIntensity(){return intensity;}
};



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
	cout.precision(10);

	AngleCalibration<BigFloat>::setSlope(BigFloat(args[8]));
	Wavelength<BigFloat>::setA(13900.0L);
	Wavelength<BigFloat>::setB(1.689L);

	//take the input file
	InputFile<BigFloat> rawFile(args[0]);
	ofstream outputFile(args[1], ios::trunc);
	vector<DataPoint<BigFloat> > data = rawFile.getPointArray();

	//find the WiensLaw stuff
	BigFloat voltage(args[4]);
	BigFloat current(args[5]);
	BigFloat resistance(args[6]);
	BigFloat temperature(args[7]);
	PowerOutput<BigFloat> power(resistance, temperature, voltage, current);

	cout << "Temp:       " << power.getTemp() << endl;
	cout << "Temp0:      " << power.getTemp0() << endl;
	cout << "HotResist:  " << power.getHotResist() << endl;
	cout << "ColdResist: " << power.getColdResist() << endl;
	cout << "Voltage:    " << power.getVoltage() << endl;
	cout << "Current:    " << power.getCurrent() << endl;
	cout << "Wavelength: " << power.getWavelength() << endl;
	cout << "Intensity:  " << power.getIntensity() << endl << endl;

	//calibrate points
	int sampleSize = atoi(args[2].c_str());
	if(sampleSize <=0) sampleSize = 1;
	BigFloat timeRange(args[3]);
	BigFloat targetPower = power.getIntensity();
	TempCalibration<BigFloat> tempCal(data, sampleSize, timeRange, targetPower);
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
