// Project 1a: Solving knapsack using exhaustive search
//

#include <iostream>
#include <limits.h>
#include <list>
#include <fstream>
#include <queue>
#include <vector>
#include <time.h>
#include <cmath>
#include <stack>

using namespace std;

#include "d_except.h"
#include "d_matrix.h"
#include "knapsack.h"

void greedyKnapsack(knapsack &k, int time)
{
	//keep track of start time
	clock_t startTime = clock();

	//get the number of objects avaliable
	int size = k.getNumObjects();

	//get the cost limit of knapsack
	int limit = k.getCostLimit();

	//vector to keep track of object value/cost ratio
	vector<vector<double>> ratio(size, vector<double>(2));

	//calculate the value/cost ratio of each item and place them in decreasing order
	for (int i = 0; i < size; i++)
	{
		double ra = (double)k.getValue(i) / (double)k.getCost(i);
		ratio[i][0] = ra;
		ratio[i][1] = i;
	}

	//sort ratio based on decreasing order
	sort(ratio.begin(), ratio.end(), 
		[](const vector<double>&a, const vector<double>&b)
		{
			return a[0] > b[0];
		});
	
	/*
	//debug: print out sorted ratio vector
	cout << "Sorted: \n";
	for (int x=0; x<ratio.size(); x++)
	{
		for (int j = 0; j < 2; j++)
		{
			cout << ratio[x][j] << " ";
		}
		cout << endl;
		
	}
	*/

	int currentCost = 0;
	clock_t timeElapsed = 0;
	for(int i = 0; i < size; i++)
	{
		//check if current item exceeds weight limit
		if ((currentCost + k.getCost(ratio[i][1])) == limit)
		{
			k.select(ratio[i][1]);
			currentCost = currentCost + k.getCost(ratio[i][1]);
			break;
		}

		else if ((currentCost + k.getCost(ratio[i][1])) < limit)
		{
			k.select(ratio[i][1]);
			currentCost = currentCost + k.getCost(ratio[i][1]);
		}

		//check time
		timeElapsed = (clock() - startTime) / CLOCKS_PER_SEC;
		if (timeElapsed >= time)
			break;
	}

}

int main()
{
	char x;
	ifstream fin;
	ofstream fout;
	stack <int> moves;
	string fileName;

	// Read the name of the file from the keyboard or
	// hard code it here for testing.

	 fileName = "knapsack1024.input";

	//cout << "Enter filename" << endl;
	//cin >> fileName;

	fin.open(fileName.c_str());
	if (!fin)
	{
		cerr << "Cannot open " << fileName << endl;
		exit(1);
	}

	fout.open("knapsack1024.output");
	if (!fout)
	{
		cerr << "Cannot open " << fileName << endl;
		exit(1);
	}

	try
	{
		cout << "Reading knapsack instance" << endl;
		knapsack k(fin);

		greedyKnapsack(k, 600);

		cout << endl << "Best solution" << endl;
		cout << "Cost limit: " << k.getCostLimit() << endl;
		k.printSolution();
		fout << "------------------------------------------------" << endl;
		fout << "Num objects: " << k.getNumObjects() << " Cost Limit: " << k.getCostLimit() << endl;

		fout << "Total value: " << k.getValue() << endl;
		fout << "Total cost: " << k.getCost() << endl << endl;

		for (int i = 0; i < k.getNumObjects(); i++)
			if (k.isSelected(i))
				fout << i << "  " << k.getValue(i) << " " << k.getCost(i) << endl;
		fout << endl;
	}

	catch (indexRangeError &ex)
	{
		cout << ex.what() << endl; exit(1);
	}
	catch (rangeError &ex)
	{
		cout << ex.what() << endl; exit(1);
	}
}


