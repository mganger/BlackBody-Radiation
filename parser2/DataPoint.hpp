#ifndef DATAPOINT_HPP
#define DATAPOINT_HPP

#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "Measurement-Type/Measurement.hpp"
#include "Stats.hpp"
#include "DataPoint.hpp"

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
template <class T> T AngleCalibration<T>::slope(1);




template <class T>
class Wavelength {
	private:
		static T A;
		static T B;

	protected:
		//methods
		static T n(T theta){
			T squared =  sin(toRadians(theta)) * 2.0/sqrt(3) + 1.0/2.0;
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
template <class T> T Wavelength<T>::A(1);
template <class T> T Wavelength<T>::B(1);




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

		//for integration
		T independent(){return boost::math::isnan(wavelength) ? std::numeric_limits<double>::infinity() : wavelength;}
		T dependent(){return potential;}
};

#endif
