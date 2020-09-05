#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <limits>
#include <chrono>
#include <iomanip>
#include <cmath>

using namespace std;

/* structs */

struct node {
	int label;
	int r, c;
	vector< pair<int,double> > dc_bigrades;

	node() { label = -1; }
};

struct stats {
	int image_rows;
	int image_columns;
	double image_read_time;
	int distinct_fn_vals;
	int distinct_dc_vals;
	int max_num_bigrades;
	double avg_num_bigrades;
	double total_time;
	double output_time;

	void print(ostream& out=cout) {
		out << "Image Size: " << image_columns << "x" << image_rows << endl;
		out << "Time to read in image: " << image_read_time/1000 << "s" << endl;
		out << "Number of distinct function values: " << distinct_fn_vals << endl;
		out << "Number of distinct distance values: " << distinct_dc_vals << endl;
		out << "Maximum bigrades on a pixel: " << max_num_bigrades << endl;
		out << "Average bigrades on a pixel: " << avg_num_bigrades << endl;
		out << "Total time to build bifiltration: " << total_time/1000 << "s" << endl;
		out << "Time to write bifiltration to output file: " << output_time/1000 << "s" << endl;
	}

} Statistics;

/* global variables */
vector<node*> graph;
map<int, vector<int> > value_list;
map<double, vector< pair<int,int> > > distance_list;
int rows, columns, count;

void print_bigrades(vector< pair<int,double> > bg, ostream& out=cout) {
	for (int i = 0; i < bg.size(); i++)
		out << "(" << bg[i].first << "," << setprecision(2) << bg[i].second << ")" << " ";
	out << endl;
}

double get_min_y(vector< pair<int, double> >& grades) {
	double min_y = grades[0].second;
	for (int i = 1; i < grades.size(); i++) {
		if (grades[i].second < min_y)
			min_y = grades[i].second;
	}
	return min_y;
}

void get_dc_bigrades(bool log=false, ostream& out=cout) {
	count = 0;
	// go through all values
	for (auto it = value_list.begin(); it != value_list.end(); it++) {
		// go through all possible distances
		long num_set = 0;
		for (auto d = distance_list.begin(); d != distance_list.end(); d++) {
			// go through all pixels that appear for that value
			for (int i = 0; i < it->second.size(); i++) {
				node* pixel = graph[it->second[i]];
				// go through all pixels in that distance
				// make sure within bounds
				int plabel = -1;
				for (auto distance = d->second.begin(); distance != d->second.end(); distance++) {
					if (pixel->c + distance->first < columns && pixel->r + distance->second < rows) {
						plabel = (pixel->c+distance->first)+(pixel->r+distance->second)*columns;
						if (graph[plabel]->dc_bigrades.empty()) {
							graph[plabel]->dc_bigrades.push_back(pair<int,double>(it->first, d->first));
							num_set++;
							Statistics.avg_num_bigrades++;
							if (graph[plabel]->bigrades.size() > Statistics.max_num_bigrades)
								Statistics.max_num_bigrades = graph[plabel]->bigrades.size();
						} else if (get_min_y(graph[plabel]->dc_bigrades) > d->first) {
							graph[plabel]->dc_bigrades.push_back(pair<int,double>(it->first, d->first));
							num_set++;
							Statistics.avg_num_bigrades++;
							if (graph[plabel]->bigrades.size() > Statistics.max_num_bigrades)
								Statistics.max_num_bigrades = graph[plabel]->bigrades.size();
						}
					}
					if (pixel->c - distance->first > -1 && pixel->r + distance->second < rows) {
						plabel = (pixel->c-distance->first)+(pixel->r+distance->second)*columns;
						if (graph[plabel]->dc_bigrades.empty()) {
							graph[plabel]->dc_bigrades.push_back(pair<int,double>(it->first, d->first));
							num_set++;
							Statistics.avg_num_bigrades++;
							if (graph[plabel]->bigrades.size() > Statistics.max_num_bigrades)
								Statistics.max_num_bigrades = graph[plabel]->bigrades.size();
						} else if (get_min_y(graph[plabel]->dc_bigrades) > d->first) {
							graph[plabel]->dc_bigrades.push_back(pair<int,double>(it->first, d->first));
							num_set++;
							Statistics.avg_num_bigrades++;
							if (graph[plabel]->bigrades.size() > Statistics.max_num_bigrades)
								Statistics.max_num_bigrades = graph[plabel]->bigrades.size();
						}
					}
					if (pixel->c + distance->first < columns && pixel->r - distance->second > -1) {
						plabel = (pixel->c+distance->first)+(pixel->r-distance->second)*columns;
						if (graph[plabel]->dc_bigrades.empty()) {
							graph[plabel]->dc_bigrades.push_back(pair<int,double>(it->first, d->first));
							num_set++;
							Statistics.avg_num_bigrades++;
							if (graph[plabel]->bigrades.size() > Statistics.max_num_bigrades)
								Statistics.max_num_bigrades = graph[plabel]->bigrades.size();
						} else if (get_min_y(graph[plabel]->dc_bigrades) > d->first) {
							graph[plabel]->dc_bigrades.push_back(pair<int,double>(it->first, d->first));
							num_set++;
							Statistics.avg_num_bigrades++;
							if (graph[plabel]->bigrades.size() > Statistics.max_num_bigrades)
								Statistics.max_num_bigrades = graph[plabel]->bigrades.size();
						}
					}
					if (pixel->c - distance->first > -1 && pixel->r - distance->second > -1) {
						plabel = (pixel->c-distance->first)+(pixel->r-distance->second)*columns;
						if (graph[plabel]->dc_bigrades.empty()) {
							graph[plabel]->dc_bigrades.push_back(pair<int,double>(it->first, d->first));
							num_set++;
							Statistics.avg_num_bigrades++;
							if (graph[plabel]->bigrades.size() > Statistics.max_num_bigrades)
								Statistics.max_num_bigrades = graph[plabel]->bigrades.size();
						} else if (get_min_y(graph[plabel]->dc_bigrades) > d->first) {
							graph[plabel]->dc_bigrades.push_back(pair<int,double>(it->first, d->first));
							num_set++;
							Statistics.avg_num_bigrades++;
							if (graph[plabel]->bigrades.size() > Statistics.max_num_bigrades)
								Statistics.max_num_bigrades = graph[plabel]->bigrades.size();
						}
					}
					if (num_set == graph.size())
						break;
				}
				if (num_set == graph.size())
					break;
			}
			if (num_set == graph.size()) {
				count++;
				break;
			}
		}
	}
}

void print_all_bigrades(ostream& out=cout) {
	out << "(" << columns << "," << rows << ")" << endl;
	for (int i = 0; i < graph.size(); i++) {
		out << graph[i]->label << ": ";
		print_bigrades(graph[i]->dc_bigrades, out);
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

void print_all_distances(ostream& out=cout) {
	for (auto it = distance_list.begin(); it != distance_list.end(); it++) {
		out << setprecision(2) << it->first << ": ";
		for (int i = 0; i < it->second.size(); i++)
			out << "(" << it->second[i].first << "," << it->second[i].second << ") ";
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
		outputfile += "_dc_bigrades";
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
		outputfile = "data_files/test_image_dc_bigrades";
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
		distance_list[sqrt(r*r+c*c)].push_back(pair<int,int>(c,r));

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
	Statistics.distinct_dc_vals = distance_list.size();
	Statistics.max_num_bigrades = 0;
	Statistics.avg_num_bigrades = 0;

	if (logging) {
		ofstream logger(log_file);
		get_dc_bigrades(logging, logger);
		logger.close();
	}
	else {
		auto bigrade_start = chrono::high_resolution_clock::now();
		// get_all_bigrades(logging);
		get_dc_bigrades(logging);
		auto bigrade_stop = chrono::high_resolution_clock::now();
		Statistics.total_time = chrono::duration_cast<chrono::milliseconds>(bigrade_stop - bigrade_start).count();
	}

	Statistics.avg_num_bigrades /= num_squares;

	auto out_start = chrono::high_resolution_clock::now();
	print_all_bigrades(output);
	auto out_stop = chrono::high_resolution_clock::now();
	Statistics.output_time = chrono::duration_cast<chrono::milliseconds>(out_stop - out_start).count();

	Statistics.print();
	cout << "Break out: " << count << " times." << endl;

	// print_all_distances();
	// print_all_square_edge();
	// print_all_edge_vertex(num_edges);
	// print_graph();
	// print_all_values();

	return 0;
}