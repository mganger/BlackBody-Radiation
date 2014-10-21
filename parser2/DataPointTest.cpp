#include "DataPoint.hpp"

int main(){
	double a(90),b(180),c(46);

	AngleCalibration<double>::setSlope(3);
	std::cout << a << "," << b << "," << c << std::endl;
	std::cout << AngleCalibration<double>::calibrate(a) << std::endl;
	std::cout << AngleCalibration<double>::calibrate(b) << std::endl;
	std::cout << AngleCalibration<double>::calibrate(c) << std::endl;
	
	Wavelength<double>::setA(13900);
	Wavelength<double>::setB(1.689);

	for(int i = 0;i<180;i++){
		std::cout << Wavelength<double>::toWavelength(i) << "," << i << endl;
	}
}
