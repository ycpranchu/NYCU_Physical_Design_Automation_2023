#pragma GCC optimize(3, "Ofast", "inline")
#ifndef CHANNEL_ROUTNIG_H_
#define CHANNEL_ROUTNIG_H_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <unordered_map>
#include <map>
#include <set>

#include <string.h>

using namespace std;

struct Terminal
{
    int id;
    int from;
    int to;

    int from_position;
    int to_position;

    int channel = 0;
    int channel_top = 0;
    int channel_bottom = 0;

    int prev_count = 0;
    vector<Terminal *> next_terminals;

    bool arranged = false;
};

class Channel_Routing
{
private:
    int Terminal_Count;
    int Max_boundary = 0;
    int Max_id = 0;

    vector<vector<int>> Boundary_top;
    vector<vector<int>> Boundary_bottom;
    int Terminal_top[50];
    int Terminal_bottom[50];

    int current_pin[50];
    int record_times[50];

    array<vector<Terminal>, 50> Terminals_map;          // key = terminal id
    array<vector<Terminal *>, 50> Terminals_top_map;    // key = position
    array<vector<Terminal *>, 50> Terminals_bottom_map; // key = position
    list<Terminal *> Terminals_list;
    list<Terminal *>::iterator iter;

    int Channel_Count = 1;

public:
    Channel_Routing(vector<vector<int>> &a, vector<vector<int>> &b, int c[], int d[], int e);
    ~Channel_Routing();

    void Building_Graph();
    void Routing();
    void Output(string filename);
};

#endif