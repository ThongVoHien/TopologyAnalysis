#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include <vector>
// #include <assert.h>

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

	double scale = 2*(value-1)*1.0/(columns+rows-2);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++){
			
			int taxicab_distance_from_center = abs(i - (rows-1)/2) + abs(j - (columns-1)/2);

			int val = value-1 - int(taxicab_distance_from_center * scale);

			assert(val >= 0);
			file << val << " ";
		}
		file << endl;
	}

	file.close();

	return 0;
}