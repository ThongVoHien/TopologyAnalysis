#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <limits>
#include <chrono>
#include <math.h>

using namespace std;

typedef long long int ull;
/* structs */

struct node {
	int label;
	int r, c;
	int r_diff, c_diff;
	vector<node*> children;
	int value;
	vector< pair<int, ull> > negative_bigrades;
	vector< pair<int, ull> > bigrades;

	node() { label = -1; }
};

struct stats {
	int image_rows;
	int image_columns;
	double image_read_time;
	int distinct_fn_vals;
	int num_thick_levels;
	int max_num_bigrades;
	double avg_num_bigrades;
	double max_thick_time;
	double avg_thick_time;
	double total_time;
	double output_time;

	void print(ostream& out=cout) {
		out << "Image Size: " << image_columns << "x" << image_rows << endl;
		out << "Time to read in image: " << image_read_time/1000 << "s" << endl;
		// out << "Number of distinct function values: " << distinct_fn_vals << endl;
		out << "Number of thickness levels: " << num_thick_levels << endl;
		out << "Maximum bigrades on a pixel: " << max_num_bigrades << endl;
		out << "Average bigrades on a pixel: " << avg_num_bigrades << endl;
		out << "Maximum time to thicken: " << max_thick_time/1000 << "s" << endl;
		out << "Average time to thicken: " << avg_thick_time/1000 << "s" << endl;
		out << "Total time to build bifiltration: " << total_time/1000 << "s" << endl;
		out << "Time to write bifiltration to output file: " << output_time/1000 << "s" << endl;
	}

} Statistics;

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


/* global variables */
int num_que_items;
// contain structure, connection of all nodes in the square
vector<node*> graph;
// mapping value to all node labels with that value
map<int, vector<int> > value_list;
int rows, columns;
set<ull> distance_list;

void print_bigrades(vector< pair<int, ull> > bg, ostream& out=cout) {
	for (int i = 0; i < bg.size(); i++)
		out << "(" << bg[i].first << "," << bg[i].second << ")" << " ";
	out << endl;
}


void print_all_bigrades(ostream& out=cout) {
	out << "(" << columns << "," << rows << ")" << endl;
	for (int i = 0; i < graph.size(); i++) {
		out << graph[i]->label << ": ";
		print_bigrades(graph[i]->bigrades, out);
	}
}

int get_max_x(vector< pair<int, ull> >& grades) {
	int max_x = grades[0].first;
	for (int i = 1; i < grades.size(); i++) {
		if (grades[i].first > max_x)
			max_x = grades[i].first;
	}
	return max_x;
}

int get_min_x(vector< pair<int, ull> >& grades) {
	int min_x = grades[0].first;
	for (int i = 1; i < grades.size(); i++) {
		if (grades[i].first < min_x)
			min_x = grades[i].first;
	}
	return min_x;
}
// void build_graph() {

// 	for (int label = 0; label < graph.size(); label++) {
// 		if (label % columns != 0)
// 			graph[label]->children.push_back(graph[label-1]);
// 		if ((label+1) % columns != 0)
// 			graph[label]->children.push_back(graph[label+1]);
// 		if (label >= columns)
// 			graph[label]->children.push_back(graph[label-columns]);
// 		if (label + columns < rows*columns)
// 			graph[label]->children.push_back(graph[label+columns]);
// 	}
// }
int Count = 0;

priority_queue< pair<node*, pair<int, int>>, vector<pair<node*, pair<int, int> > >, compareNode > que;

void get_white_bigrades(int value, bool log=false, ostream& out=cout) {
	if (log) out << "New call to get_positive_bigrades()" << endl;
	if (value == 0) return; // TODO: WTH??

	while (!que.empty()){ // O(n)
		// Count++;
		// if (log) out << "Popping element: Pixel-" << n->label << endl;
		pair<node*, pair<int, int>> next_node = que.top(); // O(log(n))
		node* n = next_node.first;
		pair<int, int> coordinate_diff = next_node.second;
		que.pop();
		num_que_items--;

		pair<int, ull> potential_bigrade = { value-1, sum_square(coordinate_diff) };
		// Skip if this negative_bigrades ~ last bigrade
		if (n->negative_bigrades.size() != 0) {
			pair<int, ull> last_bigrade = n->negative_bigrades[ n->negative_bigrades.size() - 1];
			// cout << "Size " << n->negative_bigrades.size() << endl;
			// cout << "previous: " << last_bigrade.first << " " << last_bigrade.second << endl;
			if ( (last_bigrade.first == potential_bigrade.first) || (last_bigrade.second  <= potential_bigrade.second) ) 
				continue;
		}

		// cout << "Push " << potential_bigrade.first << " " << potential_bigrade.second << " to label " << n->label << endl;
		// each point gets here once
		n->negative_bigrades.push_back(potential_bigrade);
		// Statistics.avg_num_negative_bigrades++;
		if (graph[n->label]->negative_bigrades.size() > Statistics.max_num_bigrades)
				Statistics.max_num_bigrades = graph[n->label]->negative_bigrades.size();

		int label = n->label;
		// cout << "TEST\n";
		if (label % columns != 0){ // left
			// if (!graph[label-1]->negative_bigrades.empty())
			// cout << "\t" << label -1 << " " << get_min_x(graph[label-1]->negative_bigrades) << " " << value << endl;
			if ((graph[label-1]->negative_bigrades.empty()) || (get_min_x(graph[label-1]->negative_bigrades) >= value -1)) {
				que.push( {graph[label-1], {coordinate_diff.first, coordinate_diff.second-1} });
				num_que_items++;
			}
		}
		if ((label+1) % columns != 0){ // right
			// if (!graph[label+1]->negative_bigrades.empty())
			// 	cout << "\t" << label +1 << " " << get_min_x(graph[label+1]->negative_bigrades) << " " << value << endl;
			if ((graph[label+1]->negative_bigrades.empty()) || (get_min_x(graph[label+1]->negative_bigrades) >= value -1)) {
				que.push( {graph[label+1], {coordinate_diff.first, coordinate_diff.second+1} });
				num_que_items++;
			}
		}
		if (label >= columns){ // up
			// if (!graph[label-columns]->negative_bigrades.empty())
			// 	cout << "\t" << label-columns << " " << get_min_x(graph[label-columns]->negative_bigrades) << " " << value << endl;
			if ((graph[label-columns]->negative_bigrades.empty()) || (get_min_x(graph[label-columns]->negative_bigrades) >= value -1)) {
				que.push( {graph[label-columns], {coordinate_diff.first-1, coordinate_diff.second} });
				num_que_items++;
			}
		}
		if (label + columns < rows*columns){ // down
			// if (!graph[label+columns]->negative_bigrades.empty())
			// 	cout << "\t" << label+columns << " " << get_min_x(graph[label+columns]->negative_bigrades) << " " << value << endl;
			if ((graph[label+columns]->negative_bigrades.empty()) || (get_min_x(graph[label+columns]->negative_bigrades) >= value -1)) {
				que.push( {graph[label+columns], {coordinate_diff.first+1, coordinate_diff.second} });
				num_que_items++;
			}
		}

		// 4 corners
		if (label % columns != 0){ // left
			// bottom
			int new_label = label - 1 + columns;
			if ( (new_label >= 0) && (new_label < rows*columns) ){
				if ((graph[new_label]->negative_bigrades.empty()) || (get_min_x(graph[new_label]->negative_bigrades) >= value -1)) {
					que.push( {graph[new_label], {coordinate_diff.first+1, coordinate_diff.second-1} });
					num_que_items++;
				}
			}
			// top
			new_label = label - 1 - columns;
			if (new_label >= 0){
				if ((graph[new_label]->negative_bigrades.empty()) || (get_min_x(graph[new_label]->negative_bigrades) >= value -1)) {
					que.push( {graph[new_label], {coordinate_diff.first-1, coordinate_diff.second-1} });
					num_que_items++;
				}
			}
		}

		if ((label+1) % columns != 0){ // right
			// top
			int new_label = label + 1 - columns;
			if ( (new_label >= 0) && (new_label < rows*columns) ){
				if ((graph[new_label]->negative_bigrades.empty()) || (get_min_x(graph[new_label]->negative_bigrades) >= value -1)) {
					que.push( {graph[new_label], {coordinate_diff.first-1, coordinate_diff.second+1} });
					num_que_items++;
				}
			}
			// bottom
			new_label = label + 1 + columns;
			if (new_label < rows*columns){
				if ((graph[new_label]->negative_bigrades.empty()) || (get_min_x(graph[new_label]->negative_bigrades) >= value -1)) {
					que.push( {graph[new_label], {coordinate_diff.first+1, coordinate_diff.second+1} });
					num_que_items++;
				}
			}
		}
		// cout << "DONE TEST" << endl;
	}
}

void convert_white_to_negative_bigrades(bool log=false, ostream& out=cout) {
	if (log) out << "New call to convert_white_to_negative_bigrades()" << endl;
	// cout << "white bigrades\n";
	for (int i = 0; i < graph.size(); i++) {
		vector< pair<int, ull>> bg = graph[i]->negative_bigrades;

		// cout << "Label " << i << endl;
		// for (int j = bg.size()-1; j >= 0; j--){
		// 	cout << bg[j].first << " " << bg[j].second << endl;
		// }
	}
	// cout << "Start converting\n";
	for (int i = 0; i < graph.size(); i++) {
		vector< pair<int, ull>> bg = graph[i]->negative_bigrades;
		// cout << "label: " << graph[i]->label << endl;
		graph[i]->negative_bigrades.clear();
		for (int j = bg.size()-2; j >= 0; j--){
			// cout << bg[j].first << " " << bg[j].second << "    " << bg[j+1].first << " " << bg[j+1].second << endl;
			
			set<ull>::iterator y = distance_list.lower_bound( max(bg[j].second, bg[j+1].second));
			// cout << "\t" << *y << endl;
			if (y != distance_list.begin()) y = prev(y);
			// int y = value_list[0];
			// for (int val = 0; val < value_list.size(); val++){
			// 	if (value_list[val] >= min(-bg[j].second, -bg[j+1].second)) break;
			// 	y = value_list[val];
			// }
			

			pair<int, int> negative_bigrade = {min(bg[j].first, bg[j+1].first)+1, -(*y)};
			// pair<int, int> negative_bigrade = {min(bg[j].first, bg[j+1].first)+1, min(-bg[j].second, -bg[j+1].second)+1}; // need fix for +1
			// cout << negative_bigrade.first << " " << negative_bigrade.second << endl;

			if (negative_bigrade.second == 0) continue;
			graph[i]->negative_bigrades.push_back(negative_bigrade);
			while ((graph[i]->bigrades.size() > 0) && 
					(graph[i]->bigrades[graph[i]->bigrades.size() - 1].first >= negative_bigrade.first)){
				Statistics.avg_num_bigrades--;
				graph[i]->bigrades.pop_back();
			}
			Statistics.avg_num_bigrades++;
			graph[i]->bigrades.push_back(negative_bigrade);
		}
		if (graph[i]->bigrades.size() > Statistics.max_num_bigrades)
			Statistics.max_num_bigrades = graph[i]->bigrades.size();
	}
}

void get_all_negative_bigrades(bool log=false, ostream& out=cout) {
	// scan all possible values (smallest to largest)
	for (map<int, vector<int>>::reverse_iterator it = value_list.rbegin(); it != value_list.rend(); it++) { // O(value)
		auto thick_start = chrono::high_resolution_clock::now();

		if (log) out << "Working with value: " << it->first << endl;
		// cout << "Working with value: " << it->first << endl;
		// root->depth = -1;
		// if (log) out << "Pixels: ";

		// obtain all points value = current value = it->first
		for (int i = 0; i < it->second.size(); i++) { // O(n)
			que.push({ graph[it->second[i]], {0,0} });
			// if (log) out << "Pixel-" << it->second[i] << " ";
			// cout << "Pixel-" << it->second[i] << " ";
		}
		// if (log) out << endl;
		// cout << endl;
		get_white_bigrades(it->first, log, out); // O(n)

		auto thick_stop = chrono::high_resolution_clock::now();
		double elapsted_time = chrono::duration_cast<chrono::milliseconds>(thick_stop - thick_start).count();
		Statistics.avg_thick_time += elapsted_time;
		if (elapsted_time > Statistics.max_thick_time)
			Statistics.max_thick_time = elapsted_time;
	}
	convert_white_to_negative_bigrades(log, out);
}

void get_positive_bigrades(int value, bool log=false, ostream& out=cout) {
	if (log) out << "New call to get_positive_bigrades()" << endl;
	
	while (!que.empty()){ // O(n)
		Count++;
		// cout << "Que size " << que.size() << endl;
		// if (log) out << "Popping element: Pixel-" << n->label << endl;
		pair<node*, pair<int, int>> next_node = que.top(); // O(log(n))
		node* n = next_node.first;
		pair<int, int> coordinate_diff = next_node.second;
		que.pop();
		num_que_items--;

		// Skip if this bigrades ~ last bigrade
		if (n->bigrades.size() != 0) {
			pair<int, ull> last_bigrade = n->bigrades[ n->bigrades.size() - 1];
			if ( (last_bigrade.first == value) || (last_bigrade.second  <= sum_square(coordinate_diff)) ) 
				continue;
		}

		// each point gets here once
		n->bigrades.push_back({ value, sum_square(coordinate_diff) });
		Statistics.avg_num_bigrades++;
		if (graph[n->label]->bigrades.size() > Statistics.max_num_bigrades)
				Statistics.max_num_bigrades = graph[n->label]->bigrades.size();

		int label = n->label;
		if (label % columns != 0){ // left
			if ((graph[label-1]->bigrades.empty()) || (get_max_x(graph[label-1]->bigrades) < value)) {
				que.push( {graph[label-1], {coordinate_diff.first, coordinate_diff.second-1} });
				num_que_items++;
			}
		}
		if ((label+1) % columns != 0){ // right
			if ((graph[label+1]->bigrades.empty()) || (get_max_x(graph[label+1]->bigrades) < value)) {
				que.push( {graph[label+1], {coordinate_diff.first, coordinate_diff.second+1} });
				num_que_items++;
			}
		}
		if (label >= columns){ // up
			if ((graph[label-columns]->bigrades.empty()) || (get_max_x(graph[label-columns]->bigrades) < value)) {
				que.push( {graph[label-columns], {coordinate_diff.first-1, coordinate_diff.second} });
				num_que_items++;
			}
		}
		if (label + columns < rows*columns){ // down
			if ((graph[label+columns]->bigrades.empty()) || (get_max_x(graph[label+columns]->bigrades) < value)) {
				que.push( {graph[label+columns], {coordinate_diff.first+1, coordinate_diff.second} });
				num_que_items++;
			}
		}

		// 4 corners
		if (label % columns != 0){ // left
			// bottom
			int new_label = label - 1 + columns;
			if ( (new_label >= 0) && (new_label < rows*columns) ){
				if ((graph[new_label]->bigrades.empty()) || (get_max_x(graph[new_label]->bigrades) < value)) {
					que.push( {graph[new_label], {coordinate_diff.first+1, coordinate_diff.second-1} });
					num_que_items++;
				}
			}
			// top
			new_label = label - 1 - columns;
			if (new_label >= 0){
				if ((graph[new_label]->bigrades.empty()) || (get_max_x(graph[new_label]->bigrades) < value)) {
					que.push( {graph[new_label], {coordinate_diff.first-1, coordinate_diff.second-1} });
					num_que_items++;
				}
			}
		}

		if ((label+1) % columns != 0){ // right
			// top
			int new_label = label + 1 - columns;
			if ( (new_label >= 0) && (new_label < rows*columns) ){
				if ((graph[new_label]->bigrades.empty()) || (get_max_x(graph[new_label]->bigrades) < value)) {
					que.push( {graph[new_label], {coordinate_diff.first-1, coordinate_diff.second+1} });
					num_que_items++;
				}
			}
			// bottom
			new_label = label + 1 + columns;
			if (new_label < rows*columns){
				if ((graph[new_label]->bigrades.empty()) || (get_max_x(graph[new_label]->bigrades) < value)) {
					que.push( {graph[new_label], {coordinate_diff.first+1, coordinate_diff.second+1} });
					num_que_items++;
				}
			}
		}
	}
}

void get_all_bigrades(bool log=false, ostream& out=cout) { // O( nvlog(n))
	// scan all possible values (smallest to largest)
	for (auto it = value_list.begin(); it != value_list.end(); it++) { // O(value)
		auto thick_start = chrono::high_resolution_clock::now();

		if (log) out << "Working with value: " << it->first << endl;
		// root->depth = -1;
		// if (log) out << "Pixels: ";

		// obtain all points value = current value = it->first
		for (int i = 0; i < it->second.size(); i++) { // O(n)
			que.push({ graph[it->second[i]], {0,0} });
			// if (log) out << "Pixel-" << it->second[i] << " ";
			// cout << "Pixel-" << it->second[i] << " ";
		}
		// if (log) out << endl;
		// cout << endl;
		get_positive_bigrades(it->first, log, out); // O(n)

		auto thick_stop = chrono::high_resolution_clock::now();
		double elapsted_time = chrono::duration_cast<chrono::milliseconds>(thick_stop - thick_start).count();
		Statistics.avg_thick_time += elapsted_time;
		if (elapsted_time > Statistics.max_thick_time)
			Statistics.max_thick_time = elapsted_time;
	}

	get_all_negative_bigrades(log, out);
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

	string filename, outputfile;
	bool logging;
	string log_file;

	if (argc == 2) {
		filename = argv[1];
		outputfile = argv[1];
		outputfile += "_bigrades";
		logging = false;
	}
	else if (argc == 3) {
		filename = argv[1];
		outputfile = argv[2];
		logging = false;
	}
	else if (argc == 4) {
		filename = argv[1];
		outputfile = argv[2];
		log_file = argv[3];
		logging = true;
	}
	else {
		filename = "data_files/test_image";
		outputfile = "data_files/test_image_bigrades";
		logging = false;
	}

	ifstream data(filename);
	ofstream output(outputfile);

	data >> columns >> rows;
	Statistics.image_rows = rows;
	Statistics.image_columns = columns;

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

	auto read_start = chrono::high_resolution_clock::now();
	while (data >> pixel) {

		node* n = new node();
		n->value = pixel;
		n->label = c+r*columns;
		n->r = r;
		n->c = c;
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
	auto read_stop = chrono::high_resolution_clock::now();

	Statistics.image_read_time = chrono::duration_cast<chrono::milliseconds>(read_stop - read_start).count();
	Statistics.distinct_fn_vals = value_list.size();
	Statistics.num_thick_levels = 0;
	Statistics.max_num_bigrades = 0;
	Statistics.avg_num_bigrades = 0;
	Statistics.max_thick_time = 0;
	Statistics.avg_thick_time = 0;
	// Statistics.distinct_dc_vals = 0;


	for (ull R = 0; R < rows; R++){
		for (ull C = 0; C < columns; C++){
	// 		cout << R*R + C*C << " ";
			distance_list.insert(R*R + C*C);
		}
	}
	// cout << endl;
	// build_graph();

	if (logging) {
		ofstream logger(log_file);
		get_all_bigrades(logging, logger);
		logger.close();
	}
	else {
		auto bigrade_start = chrono::high_resolution_clock::now();
		get_all_bigrades(logging);
		auto bigrade_stop = chrono::high_resolution_clock::now();
		Statistics.total_time = chrono::duration_cast<chrono::milliseconds>(bigrade_stop - bigrade_start).count();
	}

	Statistics.avg_num_bigrades /= num_squares;
	Statistics.avg_thick_time /= Statistics.distinct_fn_vals;

	auto out_start = chrono::high_resolution_clock::now();
	print_all_bigrades(output);
	auto out_stop = chrono::high_resolution_clock::now();
	Statistics.output_time = chrono::duration_cast<chrono::milliseconds>(out_stop - out_start).count();

	Statistics.print();

	cout << Count << endl;

	// print_all_square_edge();
	// print_all_edge_vertex(num_edges);
	// print_graph();
	// print_all_values();

	return 0;
}