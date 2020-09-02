#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <limits>

using namespace std;

/* structs */

struct node {
	int label;
	int r, c;
	vector<node*> children;
	int depth;
	bool in_que, explored;
	vector< pair<int,int> > bigrades;

	node() { label = -1; }
};

/* global variables */
vector<node*> que;
int num_que_items, num_explored;
vector<node*> graph;
map<int, vector<int> > value_list;
int rows, columns;
int num_squares, num_edges, num_vertices;

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

void get_bigrades(int value, int start, bool log=false, ostream& out=cout) {
	
	if (num_explored >= num_squares) {
		if (log) out << "Finished exploring all pixels" << endl;
		return;
	}

	int i, size = que.size();
	if (log) out << "Starting iteration over valid pixels" << endl;
	for (i = start; i < size; i++) {
		if (log) out << "Working on Pixel - " << que[i]->label << endl;
		if (! que[i]->explored) {
			if (log) out << "Unexplored pixel" << endl;
			if (que[i]->bigrades.empty() || get_min_y(que[i]->bigrades) > que[i]->depth) {
				que[i]->bigrades.push_back(pair<int,int>(value, que[i]->depth));\
				if (log) out << "Added bigrades" << endl;
			}
			que[i]->explored = true;
			num_explored++;
		}
		for (int j = 0; j < que[i]->children.size(); j++) {
			if (log) out << "Scanning through children" << endl;
			if (! que[i]->children[j]->in_que) {
				if (log) out << "Pixel not in que - " << que[i]->children[j]->label << endl;
				que.push_back(que[i]->children[j]);
				num_que_items++;
				que[i]->children[j]->in_que = true;
				que[i]->children[j]->depth = que[i]->depth + 1;
				if (log) out << "Added pixel to que" << endl;
			}
		}
	}

	get_bigrades(value, i, log, out);
}

void clear_all(bool log=false, ostream& out=cout) {
	if (log) out << "Clearing everything" << endl;
	for (int i = 0; i < graph.size(); i++) {
		graph[i]->in_que = false;
		graph[i]->explored = false;
	}
	if (log) out << "Cleared booleans" << endl;
	num_que_items = 0;
	num_explored = 0;
	que.clear();
	if (log) out << "Cleared que" << endl;
}

void get_all_bigrades(bool log=false, ostream& out=cout) {
	for (auto it = value_list.begin(); it != value_list.end(); it++) {
		if (log) out << "Working with value - " << it->first << endl;
		clear_all(log, out);
		for (int i = 0; i < it->second.size(); i++) {
			que.push_back(graph[it->second[i]]);
			num_que_items++;
			graph[it->second[i]]->in_que = true;
			graph[it->second[i]]->depth = 0;
		}
		if (log) out << "Added all points to que, starting bigrades calculation" << endl;
		get_bigrades(it->first, 0, log, out);
		if (log) out << "Finished bigrade calculation" << endl;
	}
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
		outputfile += "_alt_bigrades";
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
		outputfile = "data_files/test_image_alt_bigrades";
		logging = false;
	}

	ifstream data(filename);
	ofstream output(outputfile);

	data >> columns >> rows;

	num_squares = rows*columns;
	num_edges = rows*(columns+1) + columns*(rows+1);
	num_vertices = (rows+1)*(columns+1);

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

	if (logging) {
		ofstream logger(log_file);
		get_all_bigrades(logging, logger);
		logger.close();
	}
	else
		get_all_bigrades(logging);

	print_all_bigrades(output);

	return 0;
}