//this program converts the table of values to wavelength

#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <quadmath.h>


using namespace std;

class Calibration {
	public:
		static void setSlope(double inputSlope){slope = inputSlope;}
		double calibrate(double input){
			return input*slope;
		}
	protected:
		static double slope;
};
double Calibration::slope = 1;




class Wavelength {
	private:
		static double A;
		static double B;

	protected:
		//methods
		double n(double theta){
			double squared = 2.0/sqrt(3) * sin(toRadians(theta)) + 1.0/2.0;
			return sqrt(squared*squared + 3.0/4.0);
		}
		double toRadians(double degrees){return degrees * atan(1)*4 / 180.0;}
		double toWavelength(double theta){
			return sqrt(A / (n(theta) - B));
		}
	public:
		static void setA(double inputA){A = inputA;}
		static void setB(double inputB){B = inputB;}
};
double Wavelength::A = 1;
double Wavelength::B = 1;




class DataPoint : protected Calibration, public Wavelength {
	private:
		double time;
		double potential;
		double rawAngle;
		double calAngle;
		double wavelength;
		double power;


	public:
		//constructor
		DataPoint(double inputTime, double inputPotential, double inputAngle){
			time = inputTime;
			potential = inputPotential;
			rawAngle = inputAngle;
			calAngle = calibrate(inputAngle);
			wavelength = toWavelength(calAngle);
		}

		double getTime(){return time;}
		double getPotential(){return potential;}
		double getAngle(){return calAngle;}
		double getWavelength(){return wavelength;}
		double getPower(){return power;}
		double setPower(double input){power = input;}
};

class StefanBoltzmann {
	public:
		static constexpr double sigma = 5.67e-8;
		static double intensity(double temp){return sigma * pow(temp, 4);}
};

class WiensLaw {
	private:
		double wConst = 2.89776829;
		double temperature;
		double wavelength;

	public:
		WiensLaw(){};
		WiensLaw(double temp){
			temperature = temp;
			wavelength = wConst / temp;
		}
		double getTemp(){return temperature;}
		double getWavelength(){return wavelength;}

};

class PowerDensity {
	private:
		__float128 pi = M_PI;
		__float128 h = 6.626e-34L;
		__float128 k = 5.67e-8L;
		__float128 c = 3e8L;

		long double wavelength;
		long double temperature;
		long double density;

		long double densityCalc(){
			__float128 num, denum;
			num = 2 * pi * h * c*c;
			denum = powq(wavelength, 5) * (expq((h*c) / (wavelength * k * temperature)) - 1);
			cout << "num:   " << static_cast<long double>(num) << endl;
			cout << "denum: " << static_cast<long double>(denum) << endl;
			return num / denum;
		}
	public:
		PowerDensity(long double inputWavelength, long double inputTemperature){
			wavelength = inputWavelength * 1e-9;
			temperature= inputTemperature;
			density = densityCalc();
		}

		long double getWavelength(){return wavelength * 1e9;}
		double getTemp(){return temperature;}
		long double getDensity(){return density * 1e9;}
};




//this class handles the specific calibration of temperature data
class TempCalibration {
	private:
		double maximum;
		double baseline;
		long double maxPower;
		double maxWavelength;

	public:
		TempCalibration(vector<DataPoint>& input, int sampleSize, double timeRange, double temperature){
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
		double maxDataPoint(vector<DataPoint>& input, int sampleSize){
			int size = input.size();
			double max = input[0].getPotential();
			double peak = 0;
			for(int i = 1; i < size; i++){
				double average = 0;
				int count = 0;
				//sum the numbers
				for(int j = i - sampleSize; j >= 0 && j < i + sampleSize && j < size; j++){
					average += input[j].getPotential();
					count++;
				}
				average /= (count);
				if(average > max){
					max = average;
					peak = input[i].getWavelength();
				}
			}
			maxWavelength = peak;
			cout << "Peak Wavelength: " << peak << " nm" << endl;
			return max;
		}
		double findBaseline(vector<DataPoint>& input, double timeRange){
			double sum = 0;
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
			double slope = maxPower / (maximum - baseline);
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

				pointArray.emplace_back(toDouble(time), toDouble(voltage), toDouble(angle));
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
		double voltage;
		double current;
		double hotResistance;
		double coldResistance;
		double a0 = 4.5e-3;
		double sigma = 5.67e-8;

		double temp0;
		double temperature;
		double intensity;

		double tempCalc(){
			double resistRatio = hotResistance / coldResistance;
			return temp0 + (resistRatio - 1) / a0;
		}

	public:
		PowerOutput(double resistance, double startingTemp, double inputVoltage, double inputCurrent){
			voltage = inputVoltage;
			current = inputCurrent;
			coldResistance = resistance;
			hotResistance = voltage / current;
			temp0 = startingTemp;

			temperature = tempCalc();
			prediction = WiensLaw(temperature);
			intensity = StefanBoltzmann::intensity(temperature);
		}

		double getTemp(){return temperature;}
		double getTemp0(){return temp0;}
		double getHotResist(){return hotResistance;}
		double getColdResist(){return coldResistance;}
		double getVoltage(){return voltage;}
		double getCurrent(){return current;}
		double getWavelength(){return prediction.getWavelength();}
		double getIntensity(){return intensity;}
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
	if(args.size() != 8){
		cout << "Wrong number of arguments" << endl;
		cout << "input output samples timeRange voltage current resistance temperature" << endl;
		exit(1);
	}

	Calibration::setSlope(-1.2);
	Wavelength::setA(13900);
	Wavelength::setB(1.689);

	//take the input file
	InputFile rawFile(args[0]);
	ofstream outputFile(args[1], ios::trunc);
	vector<DataPoint> data = rawFile.getPointArray();

	//find the WiensLaw stuff
	double voltage = atof(args[4].c_str());
	double current = atof(args[5].c_str());
	double resistance = atof(args[6].c_str());
	double temperature = atof(args[7].c_str());
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
	double timeRange = atof(args[3].c_str());
	double targetPower = power.getIntensity();
	TempCalibration tempCal(data, sampleSize, timeRange, targetPower);
	tempCal.calibrateDataPoints(data);

	//output the file
	cout << endl << "Outputting to file in form:" << endl;
	cout << "Time(s); Voltage(V); CalibratedAngle(°); Wavelength(nm); Intensity(W/m^2 / nm)" << endl;
	int size = data.size();
	for(int j = 0; j < size; j++){
		outputFile << data[j].getTime() << ';';
		outputFile << data[j].getPotential() << ';';
		outputFile << data[j].getAngle() << ';';
		outputFile << data[j].getWavelength() << ';';
		outputFile << data[j].getPower() << endl;
	}

	//test
	cout << "=================" << endl;
	PowerDensity test(330, 2500);
}
