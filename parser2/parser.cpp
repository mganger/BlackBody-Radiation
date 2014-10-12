//this program converts the table of values to wavelength

#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>

using namespace std;
using namespace boost::multiprecision;

typedef cpp_dec_float_100 BigFloat;

class Calibration {
	public:
		static void setSlope(BigFloat inputSlope){slope = inputSlope;}
		BigFloat calibrate(BigFloat input){
			return input*slope;
		}
	protected:
		static BigFloat slope;
};
BigFloat Calibration::slope = 1;




class Wavelength {
	private:
		static BigFloat A;
		static BigFloat B;

	protected:
		//methods
		BigFloat n(BigFloat theta){
			BigFloat squared = 2.0/sqrt(3) * sin(toRadians(theta)) + 1.0/2.0;
			return sqrt(squared*squared + 3.0/4.0);
		}
		BigFloat toRadians(BigFloat degrees){return degrees * atan(1)*4 / 180.0;}
		BigFloat toWavelength(BigFloat theta){
			return sqrt(A / (n(theta) - B));
		}
	public:
		static void setA(BigFloat inputA){A = inputA;}
		static void setB(BigFloat inputB){B = inputB;}
};
BigFloat Wavelength::A = 1;
BigFloat Wavelength::B = 1;




class DataPoint : protected Calibration, public Wavelength {
	private:
		BigFloat time;
		BigFloat potential;
		BigFloat rawAngle;
		BigFloat calAngle;
		BigFloat wavelength;
		BigFloat power;


	public:
		//constructor
		DataPoint(BigFloat inputTime, BigFloat inputPotential, BigFloat inputAngle){
			time = inputTime;
			potential = inputPotential;
			rawAngle = inputAngle;
			calAngle = calibrate(inputAngle);
			wavelength = toWavelength(calAngle);
		}

		BigFloat getTime(){return time;}
		BigFloat getPotential(){return potential;}
		BigFloat getAngle(){return calAngle;}
		BigFloat getWavelength(){return wavelength;}
		BigFloat getPower(){return power;}
		BigFloat setPower(BigFloat input){power = input;}
};

class StefanBoltzmann {
	public:
		static BigFloat sigma;
		static BigFloat intensity(BigFloat temp){return sigma * pow(temp, 4);}
};

BigFloat StefanBoltzmann::sigma(5.67e-8);

class WiensLaw {
	private:
		BigFloat wConst = 2.89776829;
		BigFloat temperature;
		BigFloat wavelength;

	public:
		WiensLaw(){};
		WiensLaw(BigFloat temp){
			temperature = temp;
			wavelength = wConst / temp;
		}
		BigFloat getTemp(){return temperature;}
		BigFloat getWavelength(){return wavelength;}

};

class PowerDensity {
	private:
		BigFloat pi = M_PI;
		BigFloat h = 6.626e-34L;
		BigFloat k = 5.67e-8L;
		BigFloat c = 3e8L;

		BigFloat wavelength;
		BigFloat temperature;
		BigFloat density;

		BigFloat densityCalc(){
			BigFloat num, denum;
			num = 2 * pi * h * c*c;
			denum = pow(wavelength, 5) * (exp((h*c) / (wavelength * k * temperature)) - 1);
			cout << "num:   " <<  num << endl;
			cout << "denum: " << denum << endl;
			return num / denum;
		}
	public:
		PowerDensity(BigFloat inputWavelength, BigFloat inputTemperature){
			wavelength = inputWavelength * 1e-9;
			temperature= inputTemperature;
			density = densityCalc();
		}

		BigFloat getWavelength(){return wavelength * 1e9;}
		BigFloat getTemp(){return temperature;}
		BigFloat getDensity(){return density / 1e9;}
};




//this class handles the specific calibration of temperature data
class TempCalibration {
	private:
		BigFloat maximum;
		BigFloat baseline;
		BigFloat maxPower;
		BigFloat maxWavelength;

	public:
		TempCalibration(vector<DataPoint>& input, int sampleSize, BigFloat timeRange, BigFloat temperature){
			maximum = maxDataPoint(input, sampleSize);
			baseline = findBaseline(input, timeRange);
			PowerDensity maxDensity(maxWavelength, temperature);
			maxPower = maxDensity.getDensity();

			cout << sampleSize * 2 << " point moving average" << endl;
			cout << "Max:    " << maximum << " V" << endl;
			cout << "base:   " << baseline << " V" << endl;
			cout << "target: " << maxPower << " W/m^2 / nm" << endl;
		}

		//needs to get the baseline for the measurement, and then find the max temperature
		BigFloat maxDataPoint(vector<DataPoint>& input, int sampleSize){
			int size = input.size();
			BigFloat max = input[0].getPotential();
			BigFloat peak = 0;
			for(int i = 1; i < size; i++){
				BigFloat average = 0;
				int count = 0;
				//sum the numbers
				for(int j = i - sampleSize; j >= 0 && j < i + sampleSize && j < size; j++){
					average += input[j].getPotential();
					count++;
				}
				average /= BigFloat(count);
				if(average > max){
					max = average;
					peak = input[i].getWavelength();
				}
			}
			maxWavelength = peak;
			cout << "Peak Wavelength: " << peak << " nm" << endl;
			return max;
		}
		BigFloat findBaseline(vector<DataPoint>& input, BigFloat timeRange){
			BigFloat sum = 0;
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
		void calibrateDataPoints(vector<DataPoint>& input){
			int size = input.size();
			BigFloat slope = maxPower / (maximum - baseline);
			for(int i = 0; i < size; i++){
				input[i].setPower((input[i].getPotential() - baseline) * slope);
			}
		}
};


class InputFile {
	private:
		ifstream file;
		vector<DataPoint> pointArray;
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

				pointArray.emplace_back(BigFloat(time), BigFloat(voltage), BigFloat(angle));
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

		vector<DataPoint>& getPointArray(){return pointArray;}
};



//uses information about the light source to obtain the right blackbody spectrum
class PowerOutput {
	private:
		WiensLaw prediction;
		BigFloat voltage;
		BigFloat current;
		BigFloat hotResistance;
		BigFloat coldResistance;
		BigFloat a0 = 4.5e-3;
		BigFloat sigma = 5.67e-8;

		BigFloat temp0;
		BigFloat temperature;
		BigFloat intensity;

		BigFloat tempCalc(){
			BigFloat resistRatio = hotResistance / coldResistance;
			return temp0 + (resistRatio - 1) / a0;
		}

	public:
		PowerOutput(BigFloat resistance, BigFloat startingTemp, BigFloat inputVoltage, BigFloat inputCurrent){
			voltage = inputVoltage;
			current = inputCurrent;
			coldResistance = resistance;
			hotResistance = voltage / current;
			temp0 = startingTemp;

			temperature = tempCalc();
			prediction = WiensLaw(temperature);
			intensity = StefanBoltzmann::intensity(temperature);
		}

		BigFloat getTemp(){return temperature;}
		BigFloat getTemp0(){return temp0;}
		BigFloat getHotResist(){return hotResistance;}
		BigFloat getColdResist(){return coldResistance;}
		BigFloat getVoltage(){return voltage;}
		BigFloat getCurrent(){return current;}
		BigFloat getWavelength(){return prediction.getWavelength();}
		BigFloat getIntensity(){return intensity;}
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

	Calibration::setSlope(atof(args[8].c_str()));
	Wavelength::setA(13900.0L);
	Wavelength::setB(1.689L);

	//take the input file
	InputFile rawFile(args[0]);
	ofstream outputFile(args[1], ios::trunc);
	vector<DataPoint> data = rawFile.getPointArray();

	//find the WiensLaw stuff
	BigFloat voltage(args[4]);
	BigFloat current(args[5]);
	BigFloat resistance(args[6]);
	BigFloat temperature(args[7]);
	PowerOutput power(resistance, temperature, voltage, current);

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
	TempCalibration tempCal(data, sampleSize, timeRange, targetPower);
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
