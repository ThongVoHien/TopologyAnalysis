#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <limits>
#include <sstream>
#include <cmath>

using namespace std;

typedef long long int ull;

/* structs */

struct node {
	int label;
	int r, c;
	vector<node*> children;
	int depth;
	vector< pair<int,ull> > bigrades;
	int pixel;

	node() { label = -1; }
};

/* global variables */
vector<node*> graph;
map<int, vector<int> > value_list;
map<ull, vector< pair<int,int> > > distance_list;
int rows, columns;

void print_bigrades(vector< pair<int,int> > bg, ostream& out=cout) {
	for (int i = 0; i < bg.size(); i++)
		out << "(" << bg[i].first << "," << bg[i].second << ")" << " ";
	out << endl;
}

void build_graph() {

	for (int i = 0; i < graph.size(); i++) {
		if (i % columns != 0)
			graph[i]->children.push_back(graph[i-1]);
		if ((i+1) % columns != 0)
			graph[i]->children.push_back(graph[i+1]);
		if (i >= columns)
			graph[i]->children.push_back(graph[i-columns]);
		if (i + columns < rows*columns)
			graph[i]->children.push_back(graph[i+columns]);
	}
}

bool isPerfectSquare(int x) 
{   
  double sr = sqrt(x); 
  return ((sr - floor(sr)) == 0); 
} 

bool test_bigrades(){
	for (int label = 0; label < rows * columns; label++){
		// cout << "label " << label << " pixel " << graph[label]->pixel << endl;
		for (auto it = graph[label]->bigrades.begin(); it != graph[label]->bigrades.end(); it++){
			// cout << "test bigrade: (" << it->first << "," << it->second << ")" << endl;

			// two ways to obtain that bigrade value:
			//  - (1) its label is = the current threshold on x_axis, then y_axis = 0		
			//  - (2) it is reached through a point with same x and y=0

			if (it->second == 0){ // (1)
				if (it->first != graph[label]->pixel) {
					cout << "Error at node " << label << " at (" << 
						graph[label]->r << "," << graph[label]->c << ") with bigrades (" << 
						it->first << "," << it->second << ")\n";
					return false;
				}
			} 
			else{ // (2)
				// cout << it->first << " " << graph[lable]->pixel << endl;
				if (it->first >= graph[label]->pixel){
					cout << "Error at node " << label << " at (" << 
						graph[label]->r << "," << graph[label]->c << ") with bigrades (" << 
						it->first << "," << it->second << ")\n";
					return false; // obvious
				}

				// go through all possible ways to reach that distance
				// for each (c,r) value, check 4 corner pixels to find a bigrade with x=same and y=0
				bool ok = false;
				for (auto d = distance_list[it->second].begin(); d != distance_list[it->second].end(); d++) {
					// bottom right
					if (graph[label]->c + d->first < columns && graph[label]->r + d->second < rows) {
						int plabel = (graph[label]->c+d->first)+(graph[label]->r+d->second)*columns;
						for (auto b = graph[plabel]->bigrades.begin(); b != graph[plabel]->bigrades.end(); b++) {
							if (b->first == it->first && b->second == 0) {
								ok = true;
								break;
							}
						}
					}
					if (ok)
						break;
					// bottom left
					if (graph[label]->c - d->first > -1 && graph[label]->r + d->second < rows) {
						int plabel = (graph[label]->c-d->first)+(graph[label]->r+d->second)*columns;
						for (auto b = graph[plabel]->bigrades.begin(); b != graph[plabel]->bigrades.end(); b++) {
							if (b->first == it->first && b->second == 0) {
								ok = true;
								break;
							}
						}
					}
					if (ok)
						break;
					// top right
					if (graph[label]->c + d->first < columns && graph[label]->r - d->second > -1) {
						int plabel = (graph[label]->c+d->first)+(graph[label]->r-d->second)*columns;
						for (auto b = graph[plabel]->bigrades.begin(); b != graph[plabel]->bigrades.end(); b++) {
							if (b->first == it->first && b->second == 0) {
								ok = true;
								break;
							}
						}
					}
					if (ok)
						break;
					// top left
					if (graph[label]->c - d->first > -1 && graph[label]->r - d->second > -1) {
						int plabel = (graph[label]->c-d->first)+(graph[label]->r-d->second)*columns;
						for (auto b = graph[plabel]->bigrades.begin(); b != graph[plabel]->bigrades.end(); b++) {
							if (b->first == it->first && b->second == 0) {
								ok = true;
								break;
							}
						}
					}
					if (ok)
						break;
				}

				if (ok)
					continue;
				else
					return false;
			}
		}
	}
	return true;
}


/* Edges: Total - rows*(columns+1) + columns*(rows+1)
For pixel in row r and column c:
	c+r*(columns+1)
	(c+1)+r*(columns+1)
	(columns+1)*rows+r+c*(rows+1)
	(columns+1)*rows+(r+1)+c*(rows+1)
*/

/* Vertices: Total - (rows+1)*(columns+1)
For edge e:
	if e/((columns+1)*rows) == 0:
		e, e+(columns+1)
	else:
		k = e-(columns+1)*rows
		k/(rows+1) + (k%(rows+1))*(columns+1), <- +1
*/

int main(int argc, char** argv) {

	string filename, bigrades_file;
	bool logging;
	string log_file;

	if (argc == 2) {
		filename = argv[1];
		bigrades_file = argv[1];
		bigrades_file += "_dc_bigrades_tom";
		logging = false;
	}
	else if (argc == 3) {
		filename = argv[1];
		bigrades_file = argv[2];
		logging = false;
	}
	else if (argc == 4) {
		filename = argv[1];
		bigrades_file = argv[2];
		log_file = argv[3];
		logging = true;
	}
	else {
		filename = "data_files/test_image";
		bigrades_file = "data_files/test_image_dc_bigrades_tom";
		logging = false;
	}

	ifstream data(filename);
	ifstream bigrades_file_if(bigrades_file);

	data >> columns >> rows;

	int num_squares = rows*columns;
	int num_edges = rows*(columns+1) + columns*(rows+1);
	int num_vertices = (rows+1)*(columns+1);

	graph.resize(num_squares);

	// cout << "Total squares: " << num_squares << endl;
	// cout << "Total edges: " << num_edges << endl;
	// cout << "Total Vertices: " << num_vertices << endl;

	// read in image data
	int pixel;
	int r = 0, c = 0;

	while (data >> pixel) {

		node* n = new node();
		n->label = c+r*columns;
		n->r = r;
		n->c = c;
		n->pixel = pixel;
		graph[n->label] = n;

		value_list[pixel].push_back(n->label);
		distance_list[r*r+c*c].push_back(pair<int,int>(c,r));

		c++;
		if (c == columns) {
			r++;
			c = 0;
		}
		if (r == rows)
			break;
	}

	// build_graph();

	// read bigrades file
	string line;
	// skip first line
	getline(bigrades_file_if, line);
	while (getline(bigrades_file_if, line)){
		string bigrade, label;
		istringstream iss(line);
		iss >> label;
		// cout << line << endl;
		label = label.substr(0, label.size() - 1);

		// printf("label = %s\n", label.c_str());
		// printf("line = %s\n", line.c_str());

		while (iss >> bigrade){
			// printf("bigrade: %s\n", bigrade.c_str());
			size_t comma = bigrade.find(",");
			int x_grade = atoi( bigrade.substr(1, comma).c_str());
			// change this to ull
			int y_grade = atoi( bigrade.substr(comma+1).c_str());
			// printf("x_grade = %s y_grade = %s\n", x_grade.c_str(), y_grade.c_str());

			graph[atoi(label.c_str())]->bigrades.push_back( make_pair(x_grade, y_grade));
		}
		
	}

	if (test_bigrades()){
		cout << "Correct!\n";
	} else{
		cout << "Incorrect!\n";
	}

	return 0;
}