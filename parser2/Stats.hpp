//this header is supposed to simplify standard deviations
#ifndef STATS_HPP
#define STATS_HPP

#include "Measurement-Type/Measurement.hpp"
#include <vector>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

using std::cout;
using std::endl;

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

template <class T>
class Trap {
		T area;
		T error;
	public:
		T getArea(){return area.merge(area, error);}
		Trap(T b1, T b2, T h){
			area = h * 0.5 * (b1 + b2);
			T diff = b1 - b2;
			error = h * sqrt(diff * diff);
		}
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
		T max; //V*nm

	public:
		Integrate(std::vector<I>& input, T inputOffset){
			std::sort(input.begin(), input.end(), integrationCompare<I>);
			inputData = &input;
			integrated = std::vector<T>(input.size(), 0);
			offset = inputOffset;
		}

		void integration(){
			auto ot = integrated.begin();
			ot++;

			for(auto it = inputData->begin(); it != (inputData->end() - 1); it++){
				T h = it[1].independent() - it[0].independent();

				if(boost::math::isnan(h.getNumber()) || h == 0) continue;
				if(boost::math::isinf(it->independent().getNumber())) continue;
				if(boost::math::isinf((it+1)->independent().getNumber())) continue;

				T b1 = it[0].dependent();
				T b2 = it[1].dependent();

				Trap<T> trap(b1 - offset,b2 - offset,h);
				max = ot[-1] + trap.getArea();
				ot[0] = max;
				cout << max << endl;
				ot++;
			}
		}

		std::vector<T>& getOutput(){return integrated;}
		//V*nm
		T getLast(){return max;}
};

#endif
