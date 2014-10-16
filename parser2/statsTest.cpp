
#include "Stats.hpp"
#include <iostream>

using namespace std;

int main(){
	Mean<double> test;
	for(int i = 0; i < 100; i++) test.push_back(i);

	cout << "Mean:        " << test.mean() << endl;
	cout << "Variance:    " << test.variance() << endl;
	cout << "Std Dev:     " << test.stdDev() << endl;
	cout << "Uncertainty: " << test.uncertainty() << endl;
	cout << "measurement: " << test.measurement() << endl;
}
