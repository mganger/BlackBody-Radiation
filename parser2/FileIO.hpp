#ifndef FILEIO_HPP
#define FILEIO_HPP

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
class InputFile {
	private:
		std::ifstream file;
		std::vector<DataPoint<T> > pointArray;
	protected:
		void fileToArray(){
			//clear the array, go to the beginning of the file
			pointArray.clear();
			file.seekg(0, file.beg);

			//remove the first line
			std::string tmp;
			std::getline(file, tmp);

			//put the rest of the lines into strings, and construct new objects
			for(;;){
				std::string time,voltage,angle,junk;
				std::getline(file, time, ',');
				std::getline(file, voltage, ',');
				std::getline(file, angle, ',');
				std::getline(file, junk);

				//break if any of the strings are empty (end of file)
				if(isEmpty(time) || isEmpty(voltage) || isEmpty(angle)) break;

				pointArray.emplace_back(atof(time.c_str()), T(atof(voltage.c_str()),0.05), atof(angle.c_str()));
			}
		}

		//helper methods
		bool isEmpty(std::string& input){
			return input.length() == 0;
		}
	public:
		InputFile(char * filename){
			file.open(filename);
			fileToArray();
		}
		InputFile(std::string filename){
			file.open(filename.c_str());
			fileToArray();
		}
		~InputFile(){
			file.close();
		}

		std::vector<DataPoint<T> >& getPointArray(){return pointArray;}
};

#endif
