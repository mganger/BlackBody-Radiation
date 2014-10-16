//this header is supposed to simplify standard deviations
#ifndef STATS_HPP
#define STATS_HPP

#include "Measurement-Type/Measurement.hpp"
#include <vector>

template<class T>
class Mean : public std::vector<T> {

	public:
		T mean(){
			int vectorSize = this->size();
			T sum = 0;
			for(int i = 0; i < vectorSize; i++){
				sum += (*this)[i];
			}
			return sum / vectorSize;
		}
		T variance(){
			int vectorSize = this->size();
			T sum = 0;
			T mean = this->mean();
			for(int i = 0; i < vectorSize; i++){
				T inner = (*this)[i] - mean;
				sum += inner * inner;
			}
			return sum / vectorSize;
		}
		T stdDev(){return sqrt(this->variance());}
		T uncertainty(){return sqrt(this->variance() / this->size());};

		Measurement<T> measurement(){
			return Measurement<T>(this->mean(), this->uncertainty());
		}
};

#endif
