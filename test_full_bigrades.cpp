#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <limits>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

using namespace std;

/* structs */

struct node {
	int label;
	int r, c;
	vector<node*> children;
	int depth;
	vector< pair<int,int> > bigrades;
	int pixel;

	node() { label = -1; }
};

/* global variables */
queue<node*> que;
vector<node*> graph;
map<int, vector<int> > value_list;
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

// if isEqual == true
//   return (point <label> is at depth <depth>)
// else
//   return (point <label> is at depth <= <depth>)
bool check_point_depth_via_bfs(int label, int depth, bool isEqual){
	while (!que.empty()){
		node* n = que.front();
		que.pop();
		// if (log) out << "Popping element: Pixel-" << n->label << endl;
		if (n->depth >= depth){
			// if (log) out << "Reach desired depth"<< endl;
			return false;
		}

		for (int i = 0; i < n->children.size(); i++) {
			node* child = n->children[i];
			if (child->depth == -1){
				child->depth = n->depth + 1;
				if (child->label == label){
					// check 1st condition
					if (isEqual) return (child->depth == depth);
					else return (child->depth <= depth);
				}
				// cout << child->label << " " << child->depth << endl;
				que.push(child);
			}
		}
	}
	return false;
}
bool is_point_at_depth(int label, int depth){
	return check_point_depth_via_bfs(label, depth, true);
}
bool is_point_at_max_depth(int label, int depth){
	return check_point_depth_via_bfs(label, depth, false);
}


bool verify_bigrade(int x, int y, int label){
	// verify bigrade (x,y) (y < 0): meet all these 3 conditions:
	// - verify at (x, y) is black
	// - verify at (x, y-1) is white
	//   BFS (all points with values > x) 
	//			with depth |y|: not reach this point
	//   		with depth |y-1|: reach this point 
	//		Other words: that label is at depth |y-1|
	// - verify at (x-1, y) is white
	//   BFS (all points with values >= x) 
	//			with depth of |y|: reach this point
	//		Other words: that label is at depth |y|

	// cout << "Test point " << label << " bigrade (" << x << "," << y << ")\n";
	/* first two conditions */
	// obtain all points with value >= x
	while (!que.empty()){
		node* n = que.front();
		que.pop();
	}
	node* root = new node();
	root->depth = -1;
	for (auto value_it = prev(value_list.end()); value_it->first > x; value_it--) {
		// cout << "Value " << value_it->first << endl;
		for (int i = 0; i < value_it->second.size(); i++) {
			root->children.push_back(graph[value_it->second[i]]);
			// cout << graph[value_it->second[i]]->label << endl;
			// if (log) out << "Pixel-" << value_it->second[i] << " ";
		}
	}
	que.push(root);
	
	// reset the depth
	for (int i = 0; i < graph.size(); i++){
		graph[i]->depth = -1;
	}
	// bfs depth y
	// cout << "Test point " << label << " bigrade (" << x << "," << y << 
	// 	") at depth " << abs(y-1) << endl;
	if (!is_point_at_depth(label, abs(y-1))) return false;
	
	// reset & prepare for next bfs
	while (!que.empty()) que.pop();
	for (int i = 0; i < graph.size(); i++)
		graph[i]->depth = -1;

	/* 3rd condition */
	// cout << "Test point " << label << " bigrade (" << x << "," << y << 
	// 	") at depth " << abs(y) << endl;
	for (int i = 0; i < value_list[x].size(); i++) { // Need checking
		root->children.push_back(graph[value_list[x][i]]);
		// cout << graph[value_list[x][i]]->label << endl;
		// if (log) out << "Pixel-" << value_it->second[i] << " ";
	}
	root->depth = -1;
	que.push(root);
	if (!is_point_at_max_depth(label, abs(y))) return false;
	// bfs depth y
	return true;
}

bool test_random_bigrades(int num_points=200){
	while (num_points > 0){
		int label = rand()%(rows*columns);
		int bigrade_id = rand()%(graph[label]->bigrades.size());
		int x = graph[label]->bigrades[bigrade_id].first;
		int y = graph[label]->bigrades[bigrade_id].second;
		if (y >= 0) continue;
		if (verify_bigrade(x, y, label) == false) return false;
		num_points -- ;
	}
	// for (int label = 0; label < rows * columns; label++){
	// 	// cout << "label " << label << " pixel " << graph[label]->pixel << endl;
	// 	for (auto it = graph[label]->bigrades.begin(); it != graph[label]->bigrades.end(); it++){
	// 		// cout << "test bigrade: (" << it->first << "," << it->second << ")" << endl;

	// 		// test negative bigrades
	// 		int x = it->first;
	// 		int y = it->second;
	// 		if (y >= 0) continue;
	// 		if (verify_bigrade(x, y, label) == false) return false;
	// 	}
	// }
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
    ios_base::sync_with_stdio(false);
    srand((unsigned) time(0));

	string filename, bigrades_file;
	bool logging;
	string log_file;

	if (argc == 2) {
		filename = argv[1];
		bigrades_file = argv[1];
		bigrades_file += "_bigrades";
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
		bigrades_file = "data_files/test_image_bigrades";
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
			int y_grade = atoi( bigrade.substr(comma+1).c_str());
			// printf("x_grade = %s y_grade = %s\n", x_grade.c_str(), y_grade.c_str());

			graph[atoi(label.c_str())]->bigrades.push_back( make_pair(x_grade, y_grade));
		}
		
	}

	if (test_random_bigrades()){
		cout << "Correct!\n";
	} else{
		cout << "Incorrect!\n";
	}

	return 0;
}