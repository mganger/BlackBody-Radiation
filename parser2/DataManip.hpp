#ifndef DATAMANIP_HPP
#define DATAMANIP_HPP

#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include "Measurement-Type/Measurement.hpp"
#include "Stats.hpp"
#include "DataPoint.hpp"

using std::cout;
using std::endl;


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



//note that the template class must implement independent() and dependent()
template <class T>
bool integrationCompare(T lhs, T rhs){return lhs.independent() < rhs.independent();}

//I = input, T = output
template <class I, class T>
class Integrate {
	private:
		std::vector<T> integrated;
		std::vector<I> * inputData;
		T offset;
		T max;

	public:
		Integrate(std::vector<I>& input, T inputOffset){
			std::sort(input.begin(), input.end(), integrationCompare<I>);
			inputData = &input;
			integrated = std::vector<T>(input.size(), 0);
			offset = inputOffset;
		}

		void integration(){
			typename std::vector<T>::iterator ot = integrated.begin();
			ot++;
			for(typename std::vector<I>::iterator it = inputData->begin(); it != inputData->end(); it++){
				T diff = (it+1)->independent() - it->independent();

				if(boost::math::isnan(diff.getNumber()) || diff == 0) continue;
				if(boost::math::isinf(it->independent().getNumber())) continue;
				if(boost::math::isinf((it+1)->independent().getNumber())) continue;

				cout << std::setw(15) << it->independent() << ';';
				cout << std::setw(15) << ot[-1] << " + ";
				cout << std::setw(15) << diff << " * (";
				cout << it->dependent() << " - ";
				cout << offset << ")";

				max = ot[-1] + diff * (it->dependent() - offset);
				ot[0] = max;
				cout << " = " << std::setw(15) << ot[0] << endl;
				ot++;
			}
		}

		std::vector<T>& getOutput(){return integrated;}
		T getLast(){return max;}
};


//this class handles the specific calibration of temperature data
template <class T>
class TempCalibration {
	private:
		T baseline;

		T maxWavelength;
		T maxPower;
		T normalization;
		T powerPerArea;

	public:
		TempCalibration(std::vector<DataPoint<T> >& input, int sampleSize, T timeRange, T temperature){
			maxDataPoint(input, sampleSize);
			findBaseline(input, timeRange);

			//perform integration
			Integrate< DataPoint<T>, T > normInt(input, baseline);
			normInt.integration();

			//perform normalization based on temperature
			powerPerArea = StefanBoltzmann<T>::intensity(temperature);
			normalization = powerPerArea / normInt.getLast();
			maxPower = normalization * (maxPower - baseline);

			cout << "Integration max: " << normInt.getLast() << endl;
			cout << "Calibration:     " << normalization << "W / m^2 / V" << endl;

			cout << sampleSize * 2 << " point moving average" << endl;
			cout << "Max: " << maxPower << " W/m^2 at " << maxWavelength << " nm"<< endl;
		}

		//needs to get the baseline for the measurement, and then find the max temperature
		void maxDataPoint(std::vector<DataPoint<T> >& input, int sampleSize){
			typename std::vector<DataPoint<T> >::iterator it,jt,maxt;
			T max(0); 

			for(it = input.begin(); it != input.end(); it++){
				//sum the numbers
				Mean<T> meanObj;
				for(jt = it - sampleSize; jt >= input.begin() && jt < it + sampleSize && jt < input.end(); jt++) meanObj.push_back(jt->getPotential());
				if(meanObj.mean() > max) maxt = it;
			}

			//get the wavelength at that point
			Mean<T> waveMean, voltMean;
			for(jt = maxt - sampleSize; jt >= input.begin() && jt < maxt + sampleSize && jt < input.end(); jt++){
				waveMean.push_back(jt->getWavelength());
				voltMean.push_back(jt->getPotential());
			}
			maxWavelength = T(waveMean.mean().getNumber(), waveMean.uncertainty().getNumber());
			maxPower = T(voltMean.mean().getNumber(), voltMean.uncertainty().getNumber());
		}

		//find the average of the first n seconds
		void findBaseline(std::vector<DataPoint<T> >& input, T timeRange){
			Mean<T> mean;
			if(input.empty());
			for(auto it = input.begin(); it->getTime() < timeRange && it != input.end(); it++) mean.push_back(it->getPotential());
			baseline = T(mean.mean().getNumber(), mean.uncertainty().getNumber());
		}

		//linear calibration, p=0 @ baseline and p = maxPower @ maximum
		void calibrateDataPoints(std::vector<DataPoint<T> >& input){
			for(typename std::vector<DataPoint<T> >::iterator it = input.begin(); it != input.end(); it++){
				it->setPower((it->getPotential() - baseline) * normalization);
			}
		}
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
		T a0;
		T sigma;

		T temp0;
		T temperature;
		T intensity;

		T tempCalc(){
			T resistRatio = hotResistance / coldResistance;
			return temp0 + (resistRatio - 1) / a0;
		}

	public:
		PowerOutput(T resistance, T startingTemp, T inputVoltage, T inputCurrent){
			a0 = 4.5e-3;
			sigma = 5.67e-8;
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


#endif
