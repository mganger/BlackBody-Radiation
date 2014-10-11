//this program parses the data into tables

#include <vector>
#include <math.h>
#include <iostream>
#include <string>

using namespace std;

template <class T>
class Table {
		vector<T> data;
		int rows, columns;

	public:
		Table(int inputRows, int inputColumns){
			data = vector<T>(inputRows * inputColumns);
			rows = inputRows;
			columns = inputColumns;
		}

		T getAt(int row, int column){return data[row*rows + column];}

		void appendRow(vector<T> inputData){
			if(inputData.size() != columns) return;
			for(int i = 0; i < columns; i++) data.push_back(inputData[i]);
		}
};

class Scientific {
		long double number = 0;
		long int power = 0;

		void check(){
			while(number >= 10){
				number /=10;
				power += 1;
			}
			while(number < 1){
				number *= 10;
				power -= 1;
			}
		}

		long double toBase(Scientific& input, int targetBase){
			long double inputNumber = input.getNumber();
			long int baseDifference = targetBase - input.getPower();
			return inputNumber * pow(10, baseDifference);
		}
	public:
		//constructors
		Scientific(){};

		template <class T>
		Scientific(T input){
			*this = input;
		}

		//operations
		Scientific& multiply(Scientific& input){
			number *= input.getNumber();
			power += input.getPower();
			check();
			return *this;
		}
		Scientific& divide(Scientific& input){
			number /= input.getNumber();
			power -= input.getPower();
			check();
			return *this;
		}

		Scientific& add(Scientific& input){
			number += toBase(input, power);
			check();
			return *this;
		}

		Scientific& subtract(Scientific& input){
			number -= toBase(input, power);
			check();
			return *this;
		}

		//printing
		string toString(){return to_string(number) + "E" + to_string(power);}

		//operator overloading
		Scientific& operator+(Scientific& rhs){return add(rhs);}
		Scientific& operator-(Scientific& rhs){return subtract(rhs);}
		Scientific& operator/(Scientific& rhs){return divide(rhs);}
		Scientific& operator*(Scientific& rhs){return multiply(rhs);}

		Scientific& operator+=(Scientific& rhs){return add(rhs);}
		Scientific& operator-=(Scientific& rhs){return subtract(rhs);}
		Scientific& operator/=(Scientific& rhs){return divide(rhs);}
		Scientific& operator*=(Scientific& rhs){return multiply(rhs);}

		//assignment operators
		template <class T>
		Scientific& operator=(T input){
			number = input;
			check();
			return *this;
		}
		Scientific& operator=(Scientific& input){
			number = input.getNumber();
			power = input.getPower();
		}
		
		
		//getters
		long getNumber(){return number;}
		long int getPower(){return power;}
};

int main(int argc, char ** argv){
	Scientific number;
	number = 10;
	number *= number(10);
	cout << number.toString() << endl;
}
