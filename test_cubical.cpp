#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <limits>
#include <sstream>

using namespace std;

/* structs */

struct node {
	int label;
	int r, c;
	vector<node*> children;
	int depth;
	// vector<int> x_grades;
	// vector<int> y_grades;
	vector< pair<int,int> > bigrades;
	int pixel;

	node() { label = -1; }
};

/* global variables */
queue<node*> que;
int num_que_items, num_x_grades = 0, num_y_grades = 0;
vector<node*> graph;
map<int, vector<int> > value_list;
int rows, columns;

// void print_bigrades(vector<int> x, vector<int> y, ostream& out=cout) {
// 	for (int i = 0; i < x.size(); i++)
// 		out << "(" << x[i] << "," << y[i] << ")" << " ";
// 	out << endl;
// }

void print_bigrades(vector< pair<int,int> > bg, ostream& out=cout) {
	for (int i = 0; i < bg.size(); i++)
		out << "(" << bg[i].first << "," << bg[i].second << ")" << " ";
	out << endl;
}

// int get_min_y(vector<int> &grades) {
// 	int min_y = grades[0];
// 	for (int i = 1; i < grades.size(); i++) {
// 		if (grades[i] < min_y)
// 			min_y = grades[i];
// 	}
// 	return min_y;
// }

int get_min_y(vector< pair<int, int> >& grades) {
	int min_y = grades[0].second;
	for (int i = 1; i < grades.size(); i++) {
		if (grades[i].second < min_y)
			min_y = grades[i].second;
	}
	return min_y;
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

void get_bigrades(int value, bool log=false, ostream& out=cout) {
	if (log) out << "New call to get_bigrades()" << endl;
	
	while (!que.empty()){
		node* n = que.front();
		if (log) out << "Popping element: Pixel-" << n->label << endl;
		que.pop();
		num_que_items--;

		for (int i = 0; i < n->children.size(); i++) {
			// if (n->children[i]->x_grades.empty()) {
			if (n->children[i]->bigrades.empty()) {
				if (log) out << "Setting depth of Pixel-" << n->children[i]->label << " to " << n->depth + 1 << endl;
				n->children[i]->depth = n->depth + 1;
				if (log) out << "Empty bigrades for Pixel-" << n->children[i]->label << endl;
				if (log) out << "Adding bigrade: (" << value << "," << n->children[i]->depth << ")" << endl;
				n->children[i]->bigrades.push_back(pair<int,int>(value, n->children[i]->depth));
				// num_x_grades = n->children[i]->x_grades.size();
				// n->children[i]->x_grades.push_back(value);
				// num_y_grades = n->children[i]->y_grades.size();
				// n->children[i]->y_grades.push_back(n->children[i]->depth);
				if (log) out << "Pushing into queue: Pixel-" << n->children[i]->label << endl;
				que.push(n->children[i]);
				num_que_items++;
			}
			// else if (get_min_y(n->children[i]->y_grades) > n->depth + 1) {
			else if (get_min_y(n->children[i]->bigrades) > n->depth + 1) {
				if (log) out << "Setting depth of Pixel-" << n->children[i]->label << " to " << n->depth + 1 << endl;
				n->children[i]->depth = n->depth + 1;
				if (log) out << "Bigrades for Pixel-" << n->children[i]->label << ": ";
				if (log) print_bigrades(n->children[i]->bigrades, out);
				// if (log) print_bigrades(n->children[i]->x_grades, n->children[i]->y_grades);
				if (log) out << "Adding bigrade: (" << value << "," << n->children[i]->depth << ")" << endl;
				n->children[i]->bigrades.push_back(pair<int,int>(value, n->children[i]->depth));
				// num_x_grades = n->children[i]->x_grades.size();
				// n->children[i]->x_grades.push_back(value);
				// num_y_grades = n->children[i]->y_grades.size();
				// n->children[i]->y_grades.push_back(n->children[i]->depth);
				if (log) out << "Pushing into queue: Pixel-" << n->children[i]->label << endl;
				que.push(n->children[i]);
				num_que_items++;
			}
		}
	}
	// if (! que.empty())
	// 	get_bigrades(value, log, out);
}

bool test_bigrades(){
	for (int label = 0; label < rows * columns; label++){
		// cout << "label " << label << " pixel " << graph[label]->pixel << endl;
		for (auto it = graph[label]->bigrades.begin(); it != graph[label]->bigrades.end(); it++){
			// cout << "test bigrade: (" << it->first << "," << it->second << ")" << endl;

			// two ways to obtain that bigrade value:
			//  - (1) its label is = the current threshold on x_axis, then y_axis = 0		
			//  - (2) it is reached through BFS => the min of y_value of (all neighbors with same x_value) + 1 = its y_value

			

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

				int min_surround_y = it->second;

				for (int i = 0; i < graph[label]->children.size(); i++) { // for each child (or neighbor)
					node* child = graph[label]->children[i];
					for (auto sub_it = child->bigrades.begin(); sub_it != child->bigrades.end(); sub_it++){ // for each bigrade of that child
						if (sub_it->first == it->first){ // only consider neighbors with same x_value
							min_surround_y = min(min_surround_y, sub_it->second);
						}
					}
				}

				if (it->second != min_surround_y + 1){
					cout << "Error at node " << label << " at (" << 
						graph[label]->r << "," << graph[label]->c << ") with bigrades (" << 
						it->first << "," << it->second << ")\n";
					return false; 
				}
			}
		}
	}
	return true;
}

void get_all_bigrades(bool log=false, ostream& out=cout) {
	for (auto it = value_list.begin(); it != value_list.end(); it++) {
		if (log) out << "Working with value: " << it->first << endl;
		node* root = new node();
		root->depth = -1;
		if (log) out << "Pixels: ";
		for (int i = 0; i < it->second.size(); i++) {
			root->children.push_back(graph[it->second[i]]);
			if (log) out << "Pixel-" << it->second[i] << " ";
		}
		if (log) out << endl;
		que.push(root);
		num_que_items = 1;
		get_bigrades(it->first, log, out);
	}
}

void print_all_bigrades(ostream& out=cout) {
	for (int i = 0; i < graph.size(); i++) {
		out << graph[i]->label << ": ";
		// print_bigrades(graph[i]->x_grades, graph[i]->y_grades);
		print_bigrades(graph[i]->bigrades, out);
	}
}

void print_graph(ostream& out=cout) {
	for (int i = 0; i < graph.size(); i++) {
		out << graph[i]->label << ": ";
		for (auto it = graph[i]->children.begin(); it != graph[i]->children.end(); it++)
			out << (*it)->label << " ";
		out << endl;
	}
}

void print_all_edge_vertex(int total, ostream& out=cout) {
	for (int e = 0; e < total; e++) {
		out << "Edge " << e << ":" << endl;
		out << "Vertices: ";
		if (e/((columns+1)*rows) == 0)
			out << e << " "  << e+(columns+1) << endl;
		else {
			int j = e-(columns+1)*rows;
			int k = j/(rows+1) + (j%(rows+1))*(columns+1);
			out << k << " " << k+1 << endl; 
		}
	}
}

void print_all_square_edge(ostream& out=cout) {
	for (auto it = graph.begin(); it != graph.end(); it++) {
		out << "Pixel " << (*it)->label << ":" << endl;
		out << "Edges: " << ((*it)->c)+((*it)->r)*(columns+1) << " " 
						 << (((*it)->c)+1)+((*it)->r)*(columns+1) << " " 
						 << (columns+1)*rows+((*it)->r)+((*it)->c)*(rows+1) << " " 
						 << (columns+1)*rows+(((*it)->r)+1)+((*it)->c)*(rows+1) << endl;	
	}
}

void print_all_values(ostream& out=cout) {
	for (auto it = value_list.begin(); it != value_list.end(); it++) {
		out << it->first << ": ";
		for (int i = 0; i < it->second.size(); i++)
			out << it->second[i] << " ";
		out << endl;
	}
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
		bigrades_file = "bigrades.out";
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
		// filename = "data_files/pixels.img";
		filename = "data_files/280_280_10.txt";
		bigrades_file = "data_files/280_280_10_bigrades.txt";
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

		c++;
		if (c == columns) {
			r++;
			c = 0;
		}
		if (r == rows)
			break;
	}

	build_graph();


	// read bigrades file
	string line;
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


	// if (logging) {
	// 	ofstream logger(log_file);
	// 	get_all_bigrades(logging, logger);
	// 	logger.close();
	// }
	// else
	// 	get_all_bigrades(logging);

	// print_all_bigrades(output);
	// print_all_square_edge();
	// cout << "\n\n";
	// print_all_edge_vertex(num_edges);

	// print_graph();
	// print_all_values();

	return 0;
}