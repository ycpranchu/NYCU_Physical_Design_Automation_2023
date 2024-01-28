#pragma GCC optimize(3, "Ofast", "inline")
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <deque>
using namespace std;

struct Parameter
{
    vector<string> id_edge_map;
    vector<string> gate_map;
    vector<string> mm_map;
    int mm_p[100], mm_n[100];
    int gate_p_edge1[100], gate_p_edge2[100];
    int gate_n_edge1[100], gate_n_edge2[100];
    int width_p = 0, width_n = 0;
    int count_gate;
};

inline int Input_File(string filename, Parameter *parameter)
{
    ifstream ifs;
    ifs.open(filename);

    if (!ifs.is_open())
    {
        cout << "Failed to open the input file.\n";
        return 1; // EXIT_FAILURE
    }

    string line, token;
    int count_id = 0, count_gate = 0, count_mm = 0;

    parameter->id_edge_map.resize(200);
    parameter->gate_map.resize(100);
    parameter->mm_map.resize(200);

    unordered_map<string, int> edge_id_map, gate_map, mm_map;
    unordered_map<string, int> gate_record_p, gate_record_n;
    unordered_map<string, int> mm_record_p, mm_record_n;

    memset(parameter->gate_p_edge1, -1, sizeof(parameter->gate_p_edge1));
    memset(parameter->gate_p_edge2, -1, sizeof(parameter->gate_p_edge2));
    memset(parameter->gate_n_edge1, -1, sizeof(parameter->gate_n_edge1));
    memset(parameter->gate_n_edge2, -1, sizeof(parameter->gate_n_edge2));

    while (getline(ifs, line)) // Process each line of the file
    {
        istringstream iss(line);
        iss >> token;
        if (token.substr(0, 2) != "MM")
            continue;

        string mm = token.substr(1);
        string drain, gate, source;
        iss >> drain >> gate >> source;
        iss >> token; // skip

        iss >> token;
        int mos_type = (token == "pmos_rvt") ? 0 : 1;

        if ((mos_type == 0 && parameter->width_p == 0) || (mos_type == 1 && parameter->width_n == 0))
        {
            string width_str;
            iss >> width_str;
            int width = stoi(width_str.substr(2, width_str.find_first_not_of(".0123456789", 2)));

            if (mos_type == 0)
                parameter->width_p = width;
            else
                parameter->width_n = width;
        }

        auto add_edge = [&](const string &edge)
        {
            if (edge_id_map.count(edge) == 0)
            {
                edge_id_map[edge] = count_id;
                parameter->id_edge_map[count_id] = edge;
                count_id += 1;
            }
        };

        add_edge(drain);
        add_edge(source);
        int drain_id = edge_id_map[drain];
        int source_id = edge_id_map[source];

        string gate_name = gate;
        gate += "_" + to_string(mos_type == 0 ? gate_record_p[gate]++ : gate_record_n[gate]++) + "_ycpin";
        string mm_name = mm;
        mm += "_" + to_string(mos_type == 0 ? mm_record_p[mm]++ : mm_record_n[mm]++) + "_ycpin";

        if (gate_map.count(gate) == 0)
        {
            gate_map[gate] = count_gate;
            parameter->gate_map[count_gate] = gate_name;
            count_gate += 1;
        }

        if (mm_map.count(mm) == 0)
        {
            mm_map[mm] = count_mm;
            parameter->mm_map[count_mm] = mm_name;
            count_mm += 1;
        }

        int gate_id = gate_map[gate];
        int mm_id = mm_map[mm];

        if (mos_type == 0) // Save data into pmos_type data structure
        {
            parameter->mm_p[gate_id] = mm_id;
            parameter->gate_p_edge1[gate_id] = drain_id;
            parameter->gate_p_edge2[gate_id] = source_id;
        }
        else // Save data into nmos_type data structure
        {
            parameter->mm_n[gate_id] = mm_id;
            parameter->gate_n_edge1[gate_id] = drain_id;
            parameter->gate_n_edge2[gate_id] = source_id;
        }
    }

    parameter->count_gate = count_gate;
    ifs.close();
    return 0;
}

inline int Output_File(string filename,
                       vector<string> &finfet_p, vector<string> &finfet_n,
                       vector<string> &path_p, vector<string> &path_n,
                       double *hpwl)
{
    ofstream ofs;
    ofs.open(filename);

    if (!ofs.is_open())
    {
        cout << "Failed to open the output file.\n";
        return 1; // EXIT_FAILURE
    }

    ofs << *hpwl << endl;

    auto write_vector = [&](const vector<string> &vec)
    {
        for (size_t i = 0; i < vec.size(); ++i)
        {
            ofs << vec[i];
            if (i < vec.size() - 1)
            {
                ofs << ' ';
            }
        }
    };

    write_vector(finfet_p);
    ofs << '\n';
    write_vector(path_p);
    ofs << '\n';
    write_vector(finfet_n);
    ofs << '\n';
    write_vector(path_n);

    return 0;
}

inline void restructure_path(int *finfet,
                             vector<string> &id_edge_map, vector<string> &gate_map, vector<string> &mm_map,
                             int *mm_p, int *mm_n,
                             int *gate_p_edge1, int *gate_p_edge2,
                             int *gate_n_edge1, int *gate_n_edge2,
                             vector<string> &path_p, vector<string> &path_n,
                             vector<string> &finfet_p, vector<string> &finfet_n,
                             int count_gate)
{
    int prev_edge2_p = gate_p_edge2[finfet[0]];
    int prev_edge2_n = gate_n_edge2[finfet[0]];

    finfet_p.emplace_back(mm_map[mm_p[finfet[0]]]);
    finfet_n.emplace_back(mm_map[mm_n[finfet[0]]]);

    path_p.emplace_back(id_edge_map[gate_p_edge1[finfet[0]]]);
    path_p.emplace_back(gate_map[finfet[0]]);
    path_p.emplace_back(id_edge_map[prev_edge2_p]);

    path_n.emplace_back(id_edge_map[gate_n_edge1[finfet[0]]]);
    path_n.emplace_back(gate_map[finfet[0]]);
    path_n.emplace_back(id_edge_map[prev_edge2_n]);

    for (size_t i = 1; i < count_gate; ++i)
    {
        int edge1_p = gate_p_edge1[finfet[i]];
        int edge2_p = gate_p_edge2[finfet[i]];
        int edge1_n = gate_n_edge1[finfet[i]];
        int edge2_n = gate_n_edge2[finfet[i]];

        if (edge1_p != prev_edge2_p || edge1_n != prev_edge2_n)
        {
            finfet_p.emplace_back("Dummy");
            finfet_n.emplace_back("Dummy");

            path_p.emplace_back("Dummy");
            path_n.emplace_back("Dummy");

            path_p.emplace_back(id_edge_map[edge1_p]);
            path_n.emplace_back(id_edge_map[edge1_n]);
        }

        finfet_p.emplace_back(mm_map[mm_p[finfet[i]]]);
        finfet_n.emplace_back(mm_map[mm_n[finfet[i]]]);

        path_p.emplace_back(gate_map[finfet[i]]);
        path_n.emplace_back(gate_map[finfet[i]]);

        path_p.emplace_back(id_edge_map[edge2_p]);
        path_n.emplace_back(id_edge_map[edge2_n]);

        prev_edge2_p = edge2_p;
        prev_edge2_n = edge2_n;
    }
}

void Fast_Simulated_Annealing(vector<string> &id_edge_map,
                              vector<string> &gate_map,
                              int *mm_p, int *mm_n,
                              int *gate_p_edge1, int *gate_p_edge2,
                              int *gate_n_edge1, int *gate_n_edge2,
                              int width_p, int width_n, int count_gate,
                              int *finfet, double *hpwl);

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    string input_filename = argv[1];
    string output_filename = argv[2];

    // cout << "----------------------------------------------------------------" << endl;
    // cout << "[Initialize]" << endl;

    Parameter parameter;
    vector<string> id_edge_map;
    vector<string> gate_map;
    vector<string> mm_map;
    int *mm_p, *mm_n;
    int *gate_p_edge1, *gate_p_edge2;
    int *gate_n_edge1, *gate_n_edge2;
    int width_p = 0, width_n = 0;
    int count_gate = 0;

    Input_File(input_filename, &parameter);

    id_edge_map = parameter.id_edge_map;
    mm_p = parameter.mm_p;
    mm_n = parameter.mm_n;
    gate_map = parameter.gate_map;
    mm_map = parameter.mm_map;
    gate_p_edge1 = parameter.gate_p_edge1;
    gate_p_edge2 = parameter.gate_p_edge2;
    gate_n_edge1 = parameter.gate_n_edge1;
    gate_n_edge2 = parameter.gate_n_edge2;
    width_p = parameter.width_p;
    width_n = parameter.width_n;
    count_gate = parameter.count_gate;

    // cout << "----------------------------------------------------------------" << endl;
    // cout << "[Initialize Finfet Path]" << endl;

    int finfet[count_gate];
    for (int i = 0; i < count_gate; ++i)
        finfet[i] = i;
    random_shuffle(&finfet[0], &finfet[count_gate]);

    // cout << "----------------------------------------------------------------" << endl;
    // cout << "[Simulated Annealing]" << endl;

    double hpwl = 0.0;
    Fast_Simulated_Annealing(id_edge_map, gate_map,
                             mm_p, mm_n,
                             gate_p_edge1, gate_p_edge2,
                             gate_n_edge1, gate_n_edge2,
                             width_p, width_n, count_gate,
                             finfet, &hpwl);

    vector<string> path_p, path_n, finfet_p, finfet_n;
    restructure_path(finfet, id_edge_map, gate_map, mm_map,
                     mm_p, mm_n,
                     gate_p_edge1, gate_p_edge2,
                     gate_n_edge1, gate_n_edge2,
                     path_p, path_n,
                     finfet_p, finfet_n,
                     count_gate);

    Output_File(output_filename, finfet_p, finfet_n, path_p, path_n, &hpwl);
    return 0;
}