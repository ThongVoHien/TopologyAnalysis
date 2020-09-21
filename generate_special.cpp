#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include <vector>

using namespace std;

#define ROWS 5
#define COLUMNS 5
#define VALUE 10

int main(int argc, char** argv) {

	string filename = "data_files/test_image";
	int rows = ROWS;
	int columns = COLUMNS;
	int value = VALUE;

	switch (argc) {
		case 1:
			break;
		case 2:
			filename = argv[1];
			break;
		case 3:
			filename = argv[1];
			value = stoi(argv[2]);
			break;
		case 5:
			filename = argv[1];
			value = stoi(argv[2]);
			columns = stoi(argv[3]);
			rows = stoi(argv[4]);
			break;
		default:
			exit(1);
	}

	srand(time(0));

	ofstream file(filename);
	file << columns << " " << rows << endl;

	// int i = 0;
	vector<int> no_occ;
	for (int i = 0; i < value; i++){
		no_occ.push_back(1);
	}
	no_occ[value-1] = columns * rows - value+1;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++){
			
			int val;
			while (true){
				val = rand()%value;
				if (no_occ[val] != 0){
					no_occ[val] --;
					break;
				} 
			}
			file << val << " ";
		}
		file << endl;
	}

	file.close();

	return 0;
}