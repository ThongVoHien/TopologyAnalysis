#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <limits>
#include <chrono>

using namespace std;

/* structs */

struct node {
	int label;
	int r, c;
	vector<node*> children;
	int depth;
	vector< pair<int,int> > negative_bigrades;
	vector< pair<int,int> > bigrades;

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
	double init_mem;
	double final_mem;

	void print(ostream& out=cout) {
		out << "Image Size: " << image_columns << "x" << image_rows << endl;
		out << "Time to read in image: " << image_read_time/1000 << "s" << endl;
		out << "Number of distinct function values: " << distinct_fn_vals << endl;
		out << "Number of thickness levels: " << num_thick_levels << endl;
		out << "Maximum bigrades on a pixel: " << max_num_bigrades << endl;
		out << "Average bigrades on a pixel: " << avg_num_bigrades << endl;
		out << "Maximum time to thicken: " << max_thick_time/1000 << "s" << endl;
		out << "Average time to thicken: " << avg_thick_time/1000 << "s" << endl;
		out << "Total time to build bifiltration: " << total_time/1000 << "s" << endl;
		out << "Time to write bifiltration to output file: " << output_time/1000 << "s" << endl;
	}

} Statistics;

/* global variables */
queue<node*> que;
int num_que_items;
vector<node*> graph;
map<int, vector<int> > value_list;
int rows, columns;

void print_bigrades(vector< pair<int,int> > bg, ostream& out=cout) {
	for (int i = 0; i < bg.size(); i++)
		out << "(" << bg[i].first << "," << bg[i].second << ")" << " ";
	out << endl;
}

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

void get_white_bigrades(int value, bool log=false, ostream& out=cout) {
	if (log) out << "New call to get_white_bigrades()" << endl;
	
	while (!que.empty()){
		node* n = que.front();
		if (log) out << "Popping element: Pixel-" << n->label << endl;
		que.pop();
		num_que_items--;
		if (n->depth > Statistics.num_thick_levels)
			Statistics.num_thick_levels = n->depth;
		if (n->bigrades.size() > Statistics.max_num_bigrades)
			Statistics.max_num_bigrades = n->bigrades.size();

		for (int i = 0; i < n->children.size(); i++) {
			if (value == 0) continue;
			if (n->children[i]->negative_bigrades.empty()) {
				if (log) out << "Setting depth of Pixel-" << n->children[i]->label << " to " << n->depth + 1 << endl;
				n->children[i]->depth = n->depth + 1;
				if (log) out << "Empty negative_bigrades for Pixel-" << n->children[i]->label << endl;
				if (log) out << "Adding bigrade: (" << value << "," << n->children[i]->depth << ")" << endl;
				n->children[i]->negative_bigrades.push_back(pair<int,int>(value-1, n->children[i]->depth));
				Statistics.avg_num_bigrades++;
				if (log) out << "Pushing into queue: Pixel-" << n->children[i]->label << endl;
				que.push(n->children[i]);
				num_que_items++;
			}
			else if (get_min_y(n->children[i]->negative_bigrades) > n->depth + 1) {
				if (log) out << "Setting depth of Pixel-" << n->children[i]->label << " to " << n->depth + 1 << endl;
				n->children[i]->depth = n->depth + 1;
				if (log) out << "Bigrades for Pixel-" << n->children[i]->label << ": ";
				if (log) print_bigrades(n->children[i]->negative_bigrades, out);
				if (log) out << "Adding bigrade: (" << value << "," << n->children[i]->depth << ")" << endl;
				n->children[i]->negative_bigrades.push_back(pair<int,int>(value-1, n->children[i]->depth));
				Statistics.avg_num_bigrades++;
				if (log) out << "Pushing into queue: Pixel-" << n->children[i]->label << endl;
				que.push(n->children[i]);
				num_que_items++;
			}
		}
	}
}

void convert_white_to_negative_bigrades(bool log=false, ostream& out=cout) {
	if (log) out << "New call to convert_white_to_negative_bigrades()" << endl;
	
	for (int i = 0; i < graph.size(); i++) {
		vector< pair<int, int>> bg = graph[i]->negative_bigrades;
		graph[i]->negative_bigrades.clear();
		for (int j = bg.size()-2; j >= 0; j--){
			pair<int, int> negative_bigrade = {min(bg[j].first, bg[j+1].first)+1, min(-bg[j].second, -bg[j+1].second)+1};
			if (negative_bigrade.second == 0) continue;
			graph[i]->negative_bigrades.push_back(negative_bigrade);
			while ((graph[i]->bigrades.size() > 0) && 
					(graph[i]->bigrades[graph[i]->bigrades.size() - 1].first >= negative_bigrade.first)){
				graph[i]->bigrades.pop_back();
			}
			graph[i]->bigrades.push_back(negative_bigrade);
		}
	}
}

void get_all_negative_bigrades(bool log=false, ostream& out=cout) {
	for (map<int, vector<int>>::reverse_iterator it = value_list.rbegin(); it != value_list.rend(); it++) {
		auto thick_start = chrono::high_resolution_clock::now();
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
		get_white_bigrades(it->first, log, out);

		auto thick_stop = chrono::high_resolution_clock::now();
		double elapsted_time = chrono::duration_cast<chrono::milliseconds>(thick_stop - thick_start).count();
		Statistics.avg_thick_time += elapsted_time;
		if (elapsted_time > Statistics.max_thick_time)
			Statistics.max_thick_time = elapsted_time;
	}
	convert_white_to_negative_bigrades(log, out);
}

void get_positive_bigrades(int value, bool log=false, ostream& out=cout) {
	if (log) out << "New call to get_bigrades()" << endl;
	
	while (!que.empty()){
		node* n = que.front();
		if (log) out << "Popping element: Pixel-" << n->label << endl;
		que.pop();
		num_que_items--;
		if (n->depth > Statistics.num_thick_levels)
			Statistics.num_thick_levels = n->depth;
		if (n->bigrades.size() > Statistics.max_num_bigrades)
			Statistics.max_num_bigrades = n->bigrades.size();

		for (int i = 0; i < n->children.size(); i++) {
			if (n->children[i]->bigrades.empty()) {
				if (log) out << "Setting depth of Pixel-" << n->children[i]->label << " to " << n->depth + 1 << endl;
				n->children[i]->depth = n->depth + 1;
				if (log) out << "Empty bigrades for Pixel-" << n->children[i]->label << endl;
				if (log) out << "Adding bigrade: (" << value << "," << n->children[i]->depth << ")" << endl;
				n->children[i]->bigrades.push_back(pair<int,int>(value, n->children[i]->depth));
				Statistics.avg_num_bigrades++;
				if (log) out << "Pushing into queue: Pixel-" << n->children[i]->label << endl;
				que.push(n->children[i]);
				num_que_items++;
			}
			else if (get_min_y(n->children[i]->bigrades) > n->depth + 1) {
				if (log) out << "Setting depth of Pixel-" << n->children[i]->label << " to " << n->depth + 1 << endl;
				n->children[i]->depth = n->depth + 1;
				if (log) out << "Bigrades for Pixel-" << n->children[i]->label << ": ";
				if (log) print_bigrades(n->children[i]->bigrades, out);
				if (log) out << "Adding bigrade: (" << value << "," << n->children[i]->depth << ")" << endl;
				n->children[i]->bigrades.push_back(pair<int,int>(value, n->children[i]->depth));
				Statistics.avg_num_bigrades++;
				if (log) out << "Pushing into queue: Pixel-" << n->children[i]->label << endl;
				que.push(n->children[i]);
				num_que_items++;
			}
		}
	}
}

void get_all_bigrades(bool log=false, ostream& out=cout) {
	for (auto it = value_list.begin(); it != prev(value_list.end()); it++) {
		auto thick_start = chrono::high_resolution_clock::now();
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
		get_positive_bigrades(it->first, log, out);
		auto thick_stop = chrono::high_resolution_clock::now();
		double elapsted_time = chrono::duration_cast<chrono::milliseconds>(thick_stop - thick_start).count();
		Statistics.avg_thick_time += elapsted_time;
		if (elapsted_time > Statistics.max_thick_time)
			Statistics.max_thick_time = elapsted_time;
	}

	get_all_negative_bigrades(log, out);

}

void print_all_bigrades(ostream& out=cout) {
	out << "(" << columns << "," << rows << ")" << endl;
	for (int i = 0; i < graph.size(); i++) {
		out << graph[i]->label << ": ";
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

	build_graph();

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

	// print_all_square_edge();
	// print_all_edge_vertex(num_edges);
	// print_graph();
	// print_all_values();

	return 0;
}