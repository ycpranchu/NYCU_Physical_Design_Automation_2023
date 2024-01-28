#pragma GCC optimize(3, "Ofast", "inline")
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>

#include "Channel_Routing.h"

using namespace std;

string input_filename;
string output_filename;

int Terminal_Count = 0;

vector<vector<int>> Boundary_top(50);
vector<vector<int>> Boundary_bottom(50);
int Terminal_top[50] = {0};
int Terminal_bottom[50] = {0};

void Input_File(string filename);
