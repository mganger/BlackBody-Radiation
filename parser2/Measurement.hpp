#ifndef MEASUREMENT_HPP
#define MEASUREMENT_HPP

#include <iostream>
#include <string>
#include <cmath>


template <class T>
class Measurement {
	private:
		T number, uncert, relative;
	public:
		//constructors
		Measurement(T inputNumber, T inputUncert){
			number = inputNumber;
			uncert = std::abs(inputUncert);
			relative = uncert/number;
		}
		Measurement(T inputNumber){
			number = inputNumber;
			uncert = 0;
			relative = 0;
		}

		T getNumber(){return number;}
		T getUncert(){return uncert;}
		T getRelUnc(){return relative;}

		//promotion
		Measurement<T> operator=(T rhs){
			number = rhs;
			uncert = 0;
			relative = 0;
			return *this;
		}

		//addition, subtraction
		Measurement<T> operator+(Measurement<T> input){
			T outN = number + input.getNumber();
			T inputUncert = input.getUncert();
			T outU = sqrt(uncert*uncert + inputUncert * inputUncert);

			Measurement<T> output(outN,outU);
			return output;
		}
		Measurement<T> operator-(Measurement<T> input){
			T outN = number - input.getNumber();
			T inputUncert = input.getUncert();
			T outU = sqrt(uncert*uncert + inputUncert * inputUncert);

			Measurement<T> output(outN,outU);
			return output;
		}

		//multiplication, division
		Measurement<T> operator*(Measurement<T> input){
			T outN = number * input.getNumber();
			T inputUncert = input.getRelUnc();

			T outU = outN * sqrt(relative*relative + inputUncert*inputUncert);
			Measurement<T> output(outN, outU);
			return output;
		}
		Measurement<T> operator/(Measurement<T> input){
			T outN = number / input.getNumber();
			T inputUncert = input.getRelUnc();

			T outU = outN * sqrt(relative*relative + inputUncert*inputUncert);
			Measurement<T> output(outN, outU);
			return output;
		}




		//addition, subtraction for +=
		Measurement<T>& operator+=(Measurement<T> input){
			*this = *this + input;
			return *this;
		}
		Measurement<T>& operator-=(Measurement<T> input){
			*this = *this - input;
			return *this;
		}

		//multiplication, division
		Measurement<T>& operator*=(Measurement<T> input){
			*this = *this * input;
			return *this;
		}
		Measurement<T>& operator/=(Measurement<T> input){
			*this = *this / input;
			return *this;
		}
};

//=======================================================================================
//function overloads

template <class T>
Measurement<T> pow(Measurement<T> input, Measurement<T> power){
	T x = input.getNumber();
	T y = power.getNumber();
	T dx = input.getUncert();
	T dy = power.getUncert();

	T outN = pow(x, y);

	T outU = sqrt( pow((y * pow(x, y - 1)), 2) * dx*dx + pow((pow(x,y)*log(std::abs(x))),2)*dy*dy);
	Measurement<T> output(outN, outU);
	return output;
}

template <class T>
Measurement<T> sin(Measurement<T> input){
	T inN = input.getNumber();
	T outU = cos(inN) * input.getUncert();
	T outN = sin(inN);

	return Measurement<T>(outN, outU);
}

template <class T>
Measurement<T> cos(Measurement<T> input){
	T inN = input.getNumber();
	T outU = sin(inN) * input.getUncert();
	T outN = cos(inN);

	return Measurement<T>(outN, outU);
}

template <class T>
Measurement<T> log(Measurement<T> input){
	return Measurement<T>(log(input.getNumber()), input.getRelUnc());
}

template <class T>
Measurement<T> log10(Measurement<T> input){
	return log(input) / Measurement<T>(log(T(10)));
}

template <class T>
Measurement<T> tan(Measurement<T> input){
	T inN = input.getNumber();
	T cosN = cos(inN);
	T outU = input.getUncert() / (cosN * cosN);

	return Measurement<T>(tan(inN), outU);
}

template <class T>
Measurement<T> exp(Measurement<T> input){
	T expN = exp(input.getNumber());
	return Measurement<T>(expN, input.getUncert() * expN);
}

template <class T>
Measurement<T> atan(Measurement<T> input){
	T x = input.getNumber();

	return Measurement<T>(atan(x), input.getUncert() / (1 + x * x));
}

template <class T>
Measurement<T> asin(Measurement<T> input){
	T x = input.getNumber();

	return Measurement<T>(asin(x), input.getUncert() / sqrt(1 - x*x));
}

template <class T>
Measurement<T> acos(Measurement<T> input){
	T x = input.getNumber();

	return Measurement<T>(acos(x), input.getUncert() / sqrt(1 - x*x));
}

template <class T>
Measurement<T> atan2(Measurement<T> input1, Measurement<T> input2){
	T x = input1.getNumber();
	T y = input2.getNumber();

	Measurement<T> combo = input1 / input2;
	

	return Measurement<T>(atan2(x,y), combo.getUncert() / (1 + pow(combo.getNumber(), 2)));
}

template <class T>
Measurement<T> sqrt(Measurement<T> input){
	T sqrtX = sqrt(input.getNumber());

	return Measurement<T>(sqrtX, input.getUncert() / (2 * sqrtX));
}

template <class T>
Measurement<T> sinh(Measurement<T> input){
	return (exp(input) - exp(Measurement<T>(-1) * input))/Measurement<T>(2);
}

template <class T>
Measurement<T> cosh(Measurement<T> input){
	return (exp(input) + exp(Measurement<T>(-1) * input))/Measurement<T>(2);
}

template <class T>
Measurement<T> tanh(Measurement<T> input){
	return sinh(input) / cosh(input);
}

template <class T>
Measurement<T> asinh(Measurement<T> input){
	return log(input + sqrt(input*input + Measurement<T>(1)));
}

template <class T>
Measurement<T> acosh(Measurement<T> input){
	return log(input + sqrt(input*input - Measurement<T>(1)));
}

template <class T>
Measurement<T> atanh(Measurement<T> input){
	Measurement<T> one = Measurement<T>(1);
	return Measurement<T>(0.5) * log((one+input)/(one-input));
}

//===============================================================================
//print overload

template <class T>
std::ostream& operator<<(std::ostream& os, Measurement<T> ms){
	os << ms.getNumber() << " ± " << ms.getUncert();
	return os;
}

#endif
