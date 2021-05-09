#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>

using namespace std;

#define ROWS 5
#define COLUMNS 5
#define VALUE 10

int main(int argc, char** argv) {

	string location = "data_files/generated/";
	string filename = location;
	int rows = ROWS;
	int columns = COLUMNS;
	int value = VALUE;

	switch (argc) {
		case 1:
			break;
		case 2:
			filename = location + argv[1];
			break;
		case 3:
			filename = location + argv[1];
			value = stoi(argv[2]);
			break;
		case 5:
			filename = location + argv[1];
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

	// for (int i = 0; i < rows; i++) {
	// 	for (int j = 0; j < columns; j++)
	// 		file << (i+j)%value << " ";
	// 	file << endl;
	// }

	for (int i = 0; i < rows; i++) { // (0 -> v-1 -> 0)
		for (int j = 0; j < columns; j++){
			int val = (i+j)%(2*value-2); // (0 -> 2v-1)
			if (val >= value) val = (2*value-2) - val; // (v, 2v-1) => (2v-1, 0)
			file << val << " ";
		}
		file << endl;
	}

	// for (int i = 0; i < rows; i++) {
	// 	for (int j = 0; j < columns; j++){
	// 		int val = (i+j)%(2*value);
	// 		if (val >= value) val =2*value-val;
	// 		file << value - val << " ";
	// 	}
	// 	file << endl;
	// }


	file.close();

	return 0;
}