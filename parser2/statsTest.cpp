
#include "Stats.hpp"
#include "Measurement-Type/Measurement.hpp"
#include <iostream>
#include <vector>

using namespace std;

class Integrateable {
	private:
		Measurement<double> xvalue, yvalue;
	public:
		Integrateable(Measurement<double> x, Measurement<double> y){
			xvalue = x, yvalue = y;
		}
		Measurement<double> independent(){ return xvalue; }
		Measurement<double> dependent(){ return yvalue; }
};

int main(){
	//test the stats library
	Mean<double> test;
	for(int i = 0; i < 100; i++) test.push_back(i);

	cout << "Mean:        " << test.mean() << endl;
	cout << "Variance:    " << test.variance() << endl;
	cout << "Std Dev:     " << test.stdDev() << endl;
	cout << "Uncertainty: " << test.uncertainty() << endl;
	cout << "measurement: " << test.measurement() << endl;


	//test the trapeziod library
	Trap<Measurement<double> > testTrap(10,20,4);
	cout << endl << "Trapezoid Area (b1 = 10, b2 = 20, h = 4): " << testTrap.getArea() << endl;

	//test the integration library
	vector<Integrateable> testVect;
	for(int i = 0; i < 40; i++) testVect.emplace_back(i,i*i);
	Integrate<Integrateable,Measurement<double> > testInt(testVect, Measurement<double>(0));
	testInt.integration();

	cout << endl << "Integration" << endl;
	auto intVect = testInt.getOutput();
	auto ot = testVect.begin();
	for(auto it = intVect.begin(); it != intVect.end(), ot != testVect.end(); it++, ot++){
		cout << ot->dependent() << " -> " << *it << endl;
	}
	
}
