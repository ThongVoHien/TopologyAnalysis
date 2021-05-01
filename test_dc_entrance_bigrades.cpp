#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <limits>
#include <set>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

using namespace std;

typedef long long int ull;

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
// queue<node*> que;
vector<node*> graph;
map<int, vector<int> > value_list;
int rows, columns;
set<ull> distance_list;

int sum_square(pair<int, int> x){
	return x.first* x.first + x.second * x.second;
}
struct compareNode{
	bool operator()(const pair<node*, pair<int, int>>& x, const pair<node*, pair<int, int>>& y){
		// if (sum_square(x.second) != sum_square(y.second))
			return sum_square(x.second) > sum_square(y.second);
		// else return x.first->label < y.first->label;
	}
};

priority_queue< pair<node*, pair<int, int>>, vector<pair<node*, pair<int, int> > >, compareNode > que;


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
	// cout << "check_point_depth_via_bfs point " << label <<
	// 	" at depth " << depth << endl;
	while (!que.empty()){
		// obtain the closest node
		pair<node*, pair<int, int>> next_node = que.top(); // O(log(n))
		node* n = next_node.first;
		pair<int, int> coordinate_diff = next_node.second;
		que.pop();

		// num_que_items--;
		if (n->depth != -1) continue;
		n->depth = sum_square(coordinate_diff);
		// if (log) out << "Popping element: Pixel-" << n->label << endl;
		// cout << "Popping element: Pixel-" << n->label << " with depth " << n->depth << endl;
		// cout << label << " " << n-
		if (n->label == label){
			if (isEqual) return (n->depth == depth);
			else return (n->depth <= depth);
		}

		if (n->depth > depth){
			// if (log) out << "Reach desired depth"<< endl;
			return false;
		}
		

		// for (int i = 0; i < n->children.size(); i++) {
		// 	node* child = n->children[i];
		// 	if (child->depth == -1){
		// 		child->depth = n->depth + 1;
		// 		if (child->label == label){
		// 			// check 1st condition
		// 			if (isEqual) return (child->depth == depth);
		// 			else return (child->depth <= depth);
		// 		}
		// 		// cout << child->label << " " << child->depth << endl;
		// 		que.push(child);
		// 	}
		// }

		int label = n->label;
		if (label % columns != 0){ // left
			if (graph[label-1]->depth == -1) {
				que.push( {graph[label-1], {coordinate_diff.first, coordinate_diff.second-1} });
				// num_que_items++;
			}
		}
		if ((label+1) % columns != 0){ // right
			if (graph[label+1]->depth == -1) {
				que.push( {graph[label+1], {coordinate_diff.first, coordinate_diff.second+1} });
				// num_que_items++;
			}
		}
		if (label >= columns){ // up
			if (graph[label-columns]->depth == -1) {
				que.push( {graph[label-columns], {coordinate_diff.first-1, coordinate_diff.second} });
				// num_que_items++;
			}
		}
		if (label + columns < rows*columns){ // down
			if (graph[label+columns]->depth == -1) {
				que.push( {graph[label+columns], {coordinate_diff.first+1, coordinate_diff.second} });
				// num_que_items++;
			}
		}

		// 4 corners
		if (label % columns != 0){ // left
			// bottom
			int new_label = label - 1 + columns;
			if ( (new_label >= 0) && (new_label < rows*columns) ){
				if (graph[new_label]->depth == -1) {
					que.push( {graph[new_label], {coordinate_diff.first+1, coordinate_diff.second-1} });
					// num_que_items++;
				}
			}
			// top
			new_label = label - 1 - columns;
			if (new_label >= 0){
				if (graph[new_label]->depth == -1) {
					que.push( {graph[new_label], {coordinate_diff.first-1, coordinate_diff.second-1} });
					// num_que_items++;
				}
			}
		}

		if ((label+1) % columns != 0){ // right
			// top
			int new_label = label + 1 - columns;
			if ( (new_label >= 0) && (new_label < rows*columns) ){
				if (graph[new_label]->depth == -1) {
					que.push( {graph[new_label], {coordinate_diff.first-1, coordinate_diff.second+1} });
					// num_que_items++;
				}
			}
			// bottom
			new_label = label + 1 + columns;
			if (new_label < rows*columns){
				if (graph[new_label]->depth == -1) {
					que.push( {graph[new_label], {coordinate_diff.first+1, coordinate_diff.second+1} });
					// num_que_items++;
				}
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
	// verify positive bigrade (x,y) (y < 0): meet all these 3 conditions:
	// - verify at (x, y) is black
	// - verify at (x, y+1) is white
	//   		with depth |y-1|: reach this point 
	//		Other words: that label is at depth y
	// - verify at (x+1, y) is white
	//   BFS (all points with values >= x) 
	//			with depth of |y|: reach this point
	//		Other words: that label is at least depth > y

	// verify negative bigrade (x,y) (y < 0): meet all these 3 conditions:
	// - verify at (x, y) is black
	// - verify at (x, -next(|y|)) is white
	//   BFS (all points with values > x) 
	//			with depth |y|: not reach this point
	//   		with depth |y-1|: reach this point 
	//		Other words: that label is at depth next(|y|)
	// - verify at (x-1, y) is white
	//   BFS (all points with values >= x) 
	//			with depth of |y|: reach this point
	//		Other words: that label is at depth |y|

	// // empty queue
	// while (!que.empty()){
	// 	// node* n = que.top();
	// 	que.pop();
	// }

	// reset the depth
	while (!que.empty()) que.pop();
	for (int i = 0; i < graph.size(); i++){
		graph[i]->depth = -1;
	}
	/* first two conditions */
	// obtain all points with value >= x
	for (auto value_it = prev(value_list.end()); value_it->first > x; value_it--) {
		// cout << "Value " << value_it->first << endl;
		for (int i = 0; i < value_it->second.size(); i++) {
			que.push({ graph[value_it->second[i]], {0,0} });
			// cout << graph[value_it->second[i]]->label << endl;
			// cout << value_it->second[i] << " ";
			// if (log) out << "Pixel-" << value_it->second[i] << " ";
		}
	}
	
	// cout << endl;
	
	// bfs depth y
	set<ull>::iterator depth = next(distance_list.lower_bound(-y));
	cout << "Test point " << label << " bigrade (" << x << "," << y << 
		") at depth " << *depth << endl;
	if (!is_point_at_depth(label, *depth) ) return false;
	
	// reset & prepare for next bfs
	while (!que.empty()) que.pop();
	for (int i = 0; i < graph.size(); i++)
		graph[i]->depth = -1;

	/* 3rd condition */
	cout << "Test point " << label << " bigrade (" << x << "," << y << 
		") at max depth " << abs(y) << endl;
	for (int i = 0; i < value_list[x].size(); i++) { // Need checking
		que.push({ graph[value_list[x][i]], {0,0} });
		// cout << graph[value_list[x][i]]->label << endl;
		// if (log) out << "Pixel-" << value_it->second[i] << " ";
	}
	// root->depth = -1;
	// que.push({ root, {0,0} });
	if (!is_point_at_max_depth(label, abs(y))) return false;
	// bfs depth y
	return true;
}

bool test_random_bigrades(int num_points=200){
	// while (num_points > 0){
	// 	int label = rand()%(rows*columns);
	// 	int bigrade_id = rand()%(graph[label]->bigrades.size());
	// 	int x = graph[label]->bigrades[bigrade_id].first;
	// 	int y = graph[label]->bigrades[bigrade_id].second;
	// 	if (y >= 0) continue;
	// 	if (verify_bigrade(x, y, label) == false) return false;
	// 	num_points -- ;
	// }

	for (int label = 0; label < rows * columns; label++){
		// cout << "label " << label << " pixel " << graph[label]->pixel << endl;
		for (auto it = graph[label]->bigrades.begin(); it != graph[label]->bigrades.end(); it++){
			// cout << "test bigrade: (" << it->first << "," << it->second << ")" << endl;

			// test negative bigrades
			int x = it->first;
			int y = it->second;
			if (y >= 0) continue;
			if (verify_bigrade(x, y, label) == false) return false;
			num_points --;
			if (num_points == 0) return true;
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
    ios_base::sync_with_stdio(false);
    srand((unsigned) time(0));

	string filename, bigrades_file;
	bool logging;
	string log_file;
	int num_points_check = 2000;

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
	else if (argc == 5) {
		filename = argv[1];
		bigrades_file = argv[2];
		log_file = argv[3];
		num_points_check = atoi(argv[4]);
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

	// build_graph();

	for (ull R = 0; R < rows; R++){
		for (ull C = 0; C < columns; C++){
			// cout << R*R + C*C << " ";
			distance_list.insert(R*R + C*C);
		}
	}

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

	if (test_random_bigrades(num_points_check)){
		cout << "Correct!\n";
	} else{
		cout << "Incorrect!\n";
	}

	return 0;
}