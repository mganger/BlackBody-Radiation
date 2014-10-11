//this program converts the table of values to wavelength

#include <math.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

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

//this class handles the specific calibration of temperature data
class TempCalibration {
	private:
		double maximum;
		double baseline;
		double maxPower;

	public:
		TempCalibration(vector<DataPoint>& input, int sampleSize, double timeRange, double targetPower){
			maximum = maxDataPoint(input, sampleSize);
			baseline = findBaseline(input, timeRange);
			maxPower = targetPower;
		}

		//needs to get the baseline for the measurement, and then find the max temperature
		double maxDataPoint(vector<DataPoint>& input, int sampleSize){
			int size = input.size();
			double max = input[0].getPotential();
			for(int i = 1; i < size; i++){
				double average = 0;
				//sum the numbers
				for(int j = i - sampleSize && j >= 0; j < i + sampleSize && j < size; j++){
					average += input[j].getPotential();
				}
				average /= (sampleSize * 2.0);
				if(average > max) max = average;
			}
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



vector<string> toStringArray(int argc, char ** argv){
	vector<string> output;
	for(int i = 1; i < argc; i++){
		output.emplace_back(argv[i]);
	}
	return output;
}


int main(int argc, char ** argv){
	vector<string> args = toStringArray(argc, argv);
	if(args.size() != 5){
		cout << "Wrong number of arguments" << endl;
		cout << "input output samples timeRange targetPower" << endl;
		exit(1);
	}

	Calibration::setSlope(-1.2);
	Wavelength::setA(13900);
	Wavelength::setB(1.689);

	//take the input file
	InputFile rawFile(args[0]);
	ofstream outputFile(args[1], ios::trunc);
	vector<DataPoint> data = rawFile.getPointArray();

	//calibrate points
	int sampleSize = atoi(args[2].c_str());
	double timeRange = atof(args[3].c_str());
	double targetPower = atof(args[4].c_str());
	TempCalibration tempCal(data, 10, 10, 100);
	tempCal.calibrateDataPoints(data);

	//output the file
	int size = data.size();
	for(int j = 0; j < size; j++){
		outputFile << data[j].getTime() << ';';
		outputFile << data[j].getPotential() << ';';
		outputFile << data[j].getAngle() << ';';
		outputFile << data[j].getWavelength() << ';';
		outputFile << data[j].getPower() << endl;
	}
}
