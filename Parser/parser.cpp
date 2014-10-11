//this program parses the data into tables

template <class T>
class Table {
	vector<T> data;
	int rows, columns;

	Table(int inputRows, int inputColumns){
		data = vector(inputRows * intputColumns);
		rows = inputRows;
		columns = inputColumns;
	}

	T getAt(int row, int column){return data[row*rows + column];}

	void appendRow(vector<T> inputData){
		if(inputData.size() != columns) return;
		for(int i = 0; i < columns; i++) data.push_back(inputData[i]);
	}

};

int main(int argc, char ** argv){
	
}
