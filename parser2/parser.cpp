//this program converts the table of values to wavelength

#include <math.h>
#include <iostream>
#include <vector>

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




using namespace std;

int main(int argc, char ** argv){
	Calibration::setSlope(1.2);
	Wavelength::setA(13900);
	Wavelength::setB(1.689);

	DataPoint testPoint(0.1234, 0.948, 50);

	cout << "Time:       " << testPoint.getTime() << endl;
	cout << "Voltage:    " << testPoint.getPotential() << endl;
	cout << "Angle:      " << testPoint.getAngle() << endl;
	cout << "Wavelength: " << testPoint.getWavelength() << endl;
}
