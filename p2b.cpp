// Code to read graph instances from a file.  Uses the Boost Graph Library (BGL).

#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <fstream>
#include <time.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/integer.hpp>

#define LargeValue 99999999

using namespace std;
using namespace boost;

int const NONE = -1;  // Used to represent a node that does not exist

struct VertexProperties;
struct EdgeProperties;

typedef adjacency_list<vecS, vecS, undirectedS, VertexProperties, EdgeProperties> Graph;

struct VertexProperties
{
	pair<int, int> cell; // maze cell (x,y) value
	Graph::vertex_descriptor pred;
	bool visited;
	bool marked;
	int weight;
	int color;
	int degree;
};

// Create a struct to hold properties for each edge
struct EdgeProperties
{
	int weight;
	bool visited;
	bool marked;
};

void setNodeDegrees(Graph &g, int w)
// Set all node degree to w.
{
	pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);

	for (Graph::vertex_iterator vItr = vItrRange.first; vItr != vItrRange.second; ++vItr)
	{
		g[*vItr].degree = w;
	}
}

void setNodeColorss(Graph &g, int w)
// Set all node color to w.
{
	pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);

	for (Graph::vertex_iterator vItr = vItrRange.first; vItr != vItrRange.second; ++vItr)
	{
		g[*vItr].color = w;
	}
}

void initializeGraph(Graph &g, ifstream &fin)
// Initialize g using data from fin.  
{
	int n, e;
	int j, k;

	fin >> n >> e;
	Graph::vertex_descriptor v;

	// Add nodes.
	for (int i = 0; i < n; i++)
		v = add_vertex(g);

	for (int i = 0; i < e; i++)
	{
		fin >> j >> k;
		add_edge(j, k, g);  // Assumes vertex list is type vecS
	}

	setNodeDegrees(g, 0);
	setNodeColorss(g, -1);
}

//produces total number of conflicts in a given graph
int findNumberConflicts(Graph &g, int numColors)
{
	//keeps track of number of conflict
	int conflicts = 0;

	//loop through all edges
	//if the source and target has the same color then add 1 to conflict

	//get pair of iterators pointing to the beginning and end of list of edges
	pair<Graph::edge_iterator, Graph::edge_iterator> eItrRange = edges(g);

	//go through all edges
	for (Graph::edge_iterator eItr = eItrRange.first; eItr != eItrRange.second; ++eItr)
	{
		//find the source and target for each edge
		Graph::vertex_descriptor vertex1, vertex2;
		vertex1 = target(*eItr, g);
		vertex2 = source(*eItr, g);
		if (g[vertex1].color == g[vertex2].color)
			conflicts++;
	}
	return conflicts;
}

int findAvaliableColor(Graph &g, int i, int numColors)
{
	int n = num_vertices(g);
	//int *array = new int[n];
	vector<int> array(n, 0);
	//check which color have been taken by the neighbors
	pair<Graph::adjacency_iterator, Graph::adjacency_iterator>
		vItrRange3 = adjacent_vertices(i, g);
	for (Graph::adjacency_iterator z = vItrRange3.first; z != vItrRange3.second; ++z)
	{
		cout << "*z: " << *z << endl;
		cout << "g[*z].color " << g[*z].color << endl;
		if (g[*z].color != -1)
		{
			array[g[*z].color]++;
			cout << "hi: " << array[g[*z].color] << endl;
		}
	}

	cout << "Printing array[]: \n";
	for (int i = 0; i < numColors; i++)
		cout << array[i] << endl;

	//if not all color are taken by its neighbor, return the lowest order color
	for (int i = 0; i < numColors; i++)
	{
		if (array[i] == 0)
			return i;
	}

	//if all color are taken, return the color that is used least
	int leastUsed = 0;
	for (int i = 0; i < numColors; i++)
	{
		if (array[i] < array[leastUsed])
		{
			leastUsed = i;
		}
	}

	//delete[]array;

	return leastUsed;
}

int greedyColoring(Graph &g, int numColors, int t)
{
	//keep track of start time
	clock_t startTime = clock();

	//keeps track of the graph with minimal conflicts
	Graph minConflictGraph;

	//keeps track of the minimum conflict, set to inifinite as default
	int minConflict = LargeValue;

	//vector to store vertex pointer and degree based on decreasing degree number
	vector< vector <int>> order(num_vertices(g), vector<int>(2));

	int iterator = 0;
	//find out number of degrees for each node and save it in the properties
	pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
	for (Graph::vertex_iterator i = vItrRange.first; i != vItrRange.second; ++i)
	{
		int numberOfNeighbor = 0;

		//loop through all adjacent nodes
		pair<Graph::adjacency_iterator, Graph::adjacency_iterator> 
			vItrRange1 = adjacent_vertices(*i, g);
		for (Graph::adjacency_iterator j = vItrRange1.first; j != vItrRange1.second; ++j)
		{
			numberOfNeighbor++;
		}

		//g[*i].degree = numberOfNeighbor;

		
		//cout << "*i: " << *i << " ";
		//cout << "neighbors: " << numberOfNeighbor << endl;
		order[iterator][0] = *i;
		order[iterator][1] = numberOfNeighbor;
		iterator++;
	}

	//sort the vector
	sort(order.begin(), order.end(),
		[](const vector<int> &a, const vector<int> &b)
		{
		return (a[1] > b[1]);
	});

	/*
	//print out sorted vector 
	cout << "Sorted Vector: \n";
	for (uint16_t i = 0; i < num_vertices(g); i++)
	{
		for (int j = 0; j < 2; j++)
		{
			cout << order[i][j] << " ";
		}
		cout << endl;
	}
	*/

	//assign colors
	for (uint16_t j = 0; j < num_vertices(g); ++j)
	{
		//cout << order[j][0] << endl;
		int avaliableColor = findAvaliableColor(g, order[j][0], numColors);
		//cout << "avaliable color: " << avaliableColor << endl;
		g[order[j][0]].color = avaliableColor;
	}

	return findNumberConflicts(g, numColors);
}


int main()
{
	char x;
	ifstream fin;
	ofstream fout;
	string fileName;

	// Read the name of the graph from the keyboard or
	// hard code it here for testing.

	fileName = "color24-4.input";

	//   cout << "Enter filename" << endl;
	//   cin >> fileName;

	fin.open(fileName.c_str());
	if (!fin)
	{
		cerr << "Cannot open " << fileName << endl;
		exit(1);
	}

	fout.open("color24-4.output");
	if (!fout)
	{
		cerr << "Cannot open output file \n";
		exit(1);
	}

	try
	{
		cout << "Reading graph" << endl;
		Graph g;

		//read the number of colors
		int numColors;
		fin >> numColors;

		cout << "numColors = " << numColors << endl;
		initializeGraph(g, fin);

		int answer = greedyColoring(g, numColors, 5);

		cout << "Num nodes: " << num_vertices(g) << endl;
		cout << "Num edges: " << num_edges(g) << endl;
		cout << "Answer: " << answer << endl;
		cout << endl;
		fout << "Num nodes: " << num_vertices(g) << endl;
		fout << "Num edges: " << num_edges(g) << endl;
		fout << "Answer: " << answer << endl;
		fout << endl;

		fout << "Final Config: ";
		cout << "Final Config: ";
		pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
		for (Graph::vertex_iterator vItr = vItrRange.first; vItr != vItrRange.second; ++vItr)
		{
			cout << g[*vItr].color;
			fout << g[*vItr].color;
		}
		cout << endl;

		fout << endl;
	}
	catch (char*) {}
}