#ifndef LAWS_HPP
#define LAWS_HPP
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

using std::cout;
using std::endl;

template <class T>
class StefanBoltzmann {
	public:
		static T sigma;
		static T intensity(T temp){return sigma * pow(temp, 4);}
};

template <class T> T StefanBoltzmann<T>::sigma(5.670373e-8);



template <class T>
class WiensLaw {
	private:
		T wConst = 2.89776829e-3; //m * K
		T temperature;
		T wavelength;

	public:
		WiensLaw(){};
		WiensLaw(T temp){
			temperature = temp;
			wavelength = wConst / temp;
		}
		T getTemp(){return temperature;}
		T getWavelength(){return wavelength;} //in m
};


template <class T>
class PowerDensity {
	private:
		T pi = M_PI;
		T h = 6.62606957e-34L; // J*s
		T k = 1.3806488e-23L;  // J/K
		T c = 299792458.0L;    // m/s

		//in meters
		T wavelength;
		T temperature;
		T density;

		T densityCalc(){
			T num, denum;
			num = pi * h * c*c * 2;
			denum = pow(wavelength, 5) * (exp((h*c) / (wavelength * k * temperature)) - 1);
			return num / denum;
		}
	public:
		//in nanometers
		PowerDensity(T inputWavelength, T inputTemperature){
			wavelength = inputWavelength;
			cout << "Density Wavelength: " << wavelength << " m" << endl;
			temperature= inputTemperature;
			density = densityCalc();
		}

		//in nanometers
		T getWavelength(){return wavelength;}
		T getTemp(){return temperature;}
		T getDensity(){return density;}
};

#endif
