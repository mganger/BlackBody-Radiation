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

	Calibration::setSlope(1.2);
	Wavelength::setA(13900);
	Wavelength::setB(1.689);

	InputFile testFile(args[0]);

	vector<DataPoint> data = testFile.getPointArray();

	cout << data.size() << endl;
	cout << data[0].getTime() << endl;
	cout << data[0].getPotential() << endl;
	cout << data[0].getAngle() << endl;
	cout << data[0].getWavelength() << endl;
}
