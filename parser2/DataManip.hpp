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
#include "Laws.hpp"
#include "DataPoint.hpp"

using std::cout;
using std::endl;

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
		T getWavelength(){return prediction.getWavelength() * 1e9;}
		T getIntensity(){return intensity;}
		T getPeakDensity(){return PowerDensity<T>(prediction.getWavelength(),temperature).getDensity() / 1e9;}
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
		TempCalibration(std::vector<DataPoint<T> >& input, int sampleSize, T timeRange, PowerOutput<T> power){
			//find the baseline and peak (based on time)
			findBaseline(input, timeRange);
			maxDataPoint(input, sampleSize);

			//perform integration
			Integrate< DataPoint<T>, T > normInt(input, baseline);
			normInt.integration();

			//perform normalization based on temperature
			powerPerArea = StefanBoltzmann<T>::intensity(power.getTemp());
			//W/m^2 /nm /V  =  W/m^2       /     V*nm
			T theoryPeak = power.getPeakDensity();
			normalization = theoryPeak / (maxPower - baseline);
			//maxPower = normalization * (maxPower - baseline);

			//output information to the shell
			cout << "Calibration: " << normalization << " W/m^2 / V" << endl;
			cout << sampleSize * 2 << " point moving average" << endl;
			cout << "Max: " << maxPower << " W/m^2 / nm at " << maxWavelength << " nm"<< endl;
		}

		//needs to get the baseline for the measurement, and then find the max temperature
		void maxDataPoint(std::vector<DataPoint<T> >& input, int sampleSize){
			T max(0); 

			auto maxt = input.begin();
			for(auto it = input.begin(); it != input.end(); it++){
				//sum the numbers
				Mean<T> meanObj;
				for(auto jt = it - sampleSize; jt >= input.begin() && jt < it + sampleSize && jt < input.end(); jt++) meanObj.push_back(jt->getPotential());
				if(meanObj.mean() > max){
					maxt = it;
					max = meanObj.mean();
				}
			}

			//get the wavelength at that point
			Mean<T> waveMean, voltMean;
			for(auto jt = maxt - sampleSize; jt >= input.begin() && jt < maxt + sampleSize && jt < input.end(); jt++){
				waveMean.push_back(jt->getWavelength());
				voltMean.push_back(jt->getPotential());
			}
			maxWavelength = T().merge(waveMean.mean(), waveMean.uncertainty());
			maxPower = T().merge(voltMean.mean(), voltMean.uncertainty());
		}

		//find the average of the first n seconds
		void findBaseline(std::vector<DataPoint<T> >& input, T timeRange){
			Mean<T> mean;
			if(input.empty());
			for(auto it = input.begin(); it->getTime() < timeRange && it != input.end(); it++) mean.push_back(it->getPotential());
			baseline = T().merge(mean.mean(), mean.stdDev());
		}

		//linear calibration, p=0 @ baseline and p = maxPower @ maximum
		void calibrateDataPoints(std::vector<DataPoint<T> >& input){
			for(auto it = input.begin(); it != input.end(); it++){
				it->setPower((it->getPotential() - baseline) * normalization);
			}
		}
};



#endif
