#pragma GCC optimize(3, "Ofast", "inline")
#include <iostream>
#include <algorithm>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <deque>
#include <random>
using namespace std;
#define E 2.718281

inline double hpwl_calculate(vector<int> &path_p, vector<int> &path_n,
                             int width_p, int width_n,
                             int count_p, int count_n);

inline void Disturb(int *finfet, int count_gate, int count_p, int count_n,
                    int *gate_p_edge1, int *gate_p_edge2,
                    int *gate_n_edge1, int *gate_n_edge2,
                    int *mm_p, int *mm_n, vector<string> &gate_map,
                    int *seed_0, int *seed_1)
{
    *seed_0 = rand_r((unsigned int *)seed_0);
    *seed_1 = rand_r((unsigned int *)seed_1);

    int check = 0;

    if (*seed_0 % 3 == 0)
    {
        int node_1 = *seed_1 % count_gate;
        int node_2 = count_gate;

        for (int i = node_1 + 1; i < count_gate; ++i)
        {
            if (gate_map[finfet[node_1]] == gate_map[finfet[i]])
            {
                node_2 = i;
                break;
            }
        }

        if (node_2 != count_gate)
        {
            if (*seed_0 > RAND_MAX / 2)
            {
                int temp = gate_p_edge1[finfet[node_1]];
                gate_p_edge1[finfet[node_1]] = gate_p_edge1[finfet[node_2]];
                gate_p_edge1[finfet[node_2]] = temp;

                temp = gate_p_edge2[finfet[node_1]];
                gate_p_edge2[finfet[node_1]] = gate_p_edge2[finfet[node_2]];
                gate_p_edge2[finfet[node_2]] = temp;

                temp = mm_p[finfet[node_1]];
                mm_p[finfet[node_1]] = mm_p[finfet[node_2]];
                mm_p[finfet[node_2]] = temp;
            }
            else
            {
                int temp = gate_n_edge1[finfet[node_1]];
                gate_n_edge1[finfet[node_1]] = gate_n_edge1[finfet[node_2]];
                gate_n_edge1[finfet[node_2]] = temp;

                temp = gate_n_edge2[finfet[node_1]];
                gate_n_edge2[finfet[node_1]] = gate_n_edge2[finfet[node_2]];
                gate_n_edge2[finfet[node_2]] = temp;

                temp = mm_n[finfet[node_1]];
                mm_n[finfet[node_1]] = mm_n[finfet[node_2]];
                mm_n[finfet[node_2]] = temp;
            }

            check = 1;
        }
    }

    if (count_gate > 1)
    {
        if (*seed_0 % 3 == 1 || check == 0 && *seed_0 % 2 == 0) // swap gate
        {
            int node_1 = *seed_1 % count_gate;
            int node_2 = node_1;

            while (node_2 == node_1)
            {
                *seed_1 = rand_r((unsigned int *)seed_1);
                node_2 = *seed_1 % count_gate;
            }

            int temp = finfet[node_1];
            finfet[node_1] = finfet[node_2];
            finfet[node_2] = temp;
        }
    }

    if (*seed_0 % 3 == 2 || check == 0 && *seed_0 % 2 == 1) // swap gate p & n edge
    {
        int node_1 = *seed_1 % count_gate;

        if (*seed_0 > RAND_MAX / 2)
        {
            int temp = gate_p_edge1[node_1];
            gate_p_edge1[node_1] = gate_p_edge2[node_1];
            gate_p_edge2[node_1] = temp;
        }
        else
        {
            int temp = gate_n_edge1[node_1];
            gate_n_edge1[node_1] = gate_n_edge2[node_1];
            gate_n_edge2[node_1] = temp;
        }
    }
}

inline void restructure_edge(int *finfet,
                             int *gate_p_edge1, int *gate_p_edge2,
                             int *gate_n_edge1, int *gate_n_edge2,
                             vector<int> &path_p, vector<int> &path_n,
                             int count_gate)
{
    path_p.clear(), path_n.clear();

    int prev_edge2_p = gate_p_edge2[finfet[0]];
    int prev_edge2_n = gate_n_edge2[finfet[0]];

    path_p.emplace_back(gate_p_edge1[finfet[0]]);
    path_p.emplace_back(prev_edge2_p);
    path_n.emplace_back(gate_n_edge1[finfet[0]]);
    path_n.emplace_back(prev_edge2_n);

    for (size_t i = 1; i < count_gate; ++i)
    {
        int edge1_p = gate_p_edge1[finfet[i]];
        int edge2_p = gate_p_edge2[finfet[i]];
        int edge1_n = gate_n_edge1[finfet[i]];
        int edge2_n = gate_n_edge2[finfet[i]];

        if (edge1_p != prev_edge2_p || edge1_n != prev_edge2_n)
        {
            path_p.emplace_back(-1);
            path_n.emplace_back(-1);

            path_p.emplace_back(edge1_p);
            path_n.emplace_back(edge1_n);
        }

        path_p.emplace_back(edge2_p);
        path_n.emplace_back(edge2_n);

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
                              int *finfet, double *hpwl)
{
    vector<int> path_p, path_n;
    int seed_0 = time(NULL), seed_1 = time(NULL) + 1, seed_2 = time(NULL) + 2;
    int count_p = -1, count_n = -1;

    for (int i = 0; i < count_gate; ++i)
    {
        count_p = max(count_p, gate_p_edge1[i]);
        count_p = max(count_p, gate_p_edge2[i]);
        count_n = max(count_n, gate_n_edge1[i]);
        count_n = max(count_n, gate_n_edge2[i]);
    }

    restructure_edge(finfet, gate_p_edge1, gate_p_edge2, gate_n_edge1, gate_n_edge2, path_p, path_n, count_gate);
    double previous_hpwl_init = hpwl_calculate(path_p, path_n, width_p, width_n, count_p, count_n);

    double previous_hpwl = previous_hpwl_init;
    double normalize = 0.0 + previous_hpwl;
    double temp_hpwl = 0.0, delta_cost = 0.0;

    int finfet_copy[100];
    int gate_p_edge1_copy[100], gate_p_edge2_copy[100];
    int gate_n_edge1_copy[100], gate_n_edge2_copy[100];
    int mm_p_copy[100], mm_n_copy[100];

    int finfet_min[100];
    int gate_p_edge1_min[100], gate_p_edge2_min[100];
    int gate_n_edge1_min[100], gate_n_edge2_min[100];
    int mm_p_min[100], mm_n_min[100];

    memcpy(finfet_copy, finfet, 100 * sizeof(int));
    memcpy(gate_p_edge1_copy, gate_p_edge1, 100 * sizeof(int));
    memcpy(gate_p_edge2_copy, gate_p_edge2, 100 * sizeof(int));
    memcpy(gate_n_edge1_copy, gate_n_edge1, 100 * sizeof(int));
    memcpy(gate_n_edge2_copy, gate_n_edge2, 100 * sizeof(int));
    memcpy(mm_p_copy, mm_p, 100 * sizeof(int));
    memcpy(mm_n_copy, mm_n, 100 * sizeof(int));

    memcpy(finfet_min, finfet, 100 * sizeof(int));
    memcpy(gate_p_edge1_min, gate_p_edge1, 100 * sizeof(int));
    memcpy(gate_p_edge2_min, gate_p_edge2, 100 * sizeof(int));
    memcpy(gate_n_edge1_min, gate_n_edge1, 100 * sizeof(int));
    memcpy(gate_n_edge2_min, gate_n_edge2, 100 * sizeof(int));
    memcpy(mm_p_min, mm_p, 100 * sizeof(int));
    memcpy(mm_n_min, mm_n, 100 * sizeof(int));

    int init_times = 100;

    for (int times = 0; times < init_times; ++times)
    {
        Disturb(finfet, count_gate, count_p, count_n,
                gate_p_edge1, gate_p_edge2, gate_n_edge1, gate_n_edge2,
                mm_p, mm_n, gate_map, &seed_0, &seed_1);
        restructure_edge(finfet, gate_p_edge1, gate_p_edge2, gate_n_edge1, gate_n_edge2, path_p, path_n, count_gate);

        temp_hpwl = hpwl_calculate(path_p, path_n, width_p, width_n, count_p, count_n);
        normalize += temp_hpwl;

        if (temp_hpwl > previous_hpwl)
            delta_cost += temp_hpwl - previous_hpwl;

        previous_hpwl = temp_hpwl;
    }

    normalize /= init_times;

    memcpy(finfet, finfet_copy, 100 * sizeof(int));
    memcpy(gate_p_edge1, gate_p_edge1_copy, 100 * sizeof(int));
    memcpy(gate_p_edge2, gate_p_edge2_copy, 100 * sizeof(int));
    memcpy(gate_n_edge1, gate_n_edge1_copy, 100 * sizeof(int));
    memcpy(gate_n_edge2, gate_n_edge2_copy, 100 * sizeof(int));
    memcpy(mm_p, mm_p_copy, 100 * sizeof(int));
    memcpy(mm_n, mm_n_copy, 100 * sizeof(int));

    int accept = 0, iter = 10000;
    double alpha = 0.95, P = 0.99;
    double T = 0.0001;

    previous_hpwl = previous_hpwl_init;

    double min_hpwl = previous_hpwl, global_min = previous_hpwl;
    double previous_cost = previous_hpwl / normalize;
    double cost = 0.0;

    double Temprature_init = -1 * delta_cost / normalize / log(P);
    double Temprature = Temprature_init;

    // cout << "Temprature init: " << Temprature_init << endl;
    // cout << "Normalize: " << normalize << endl;

    while (Temprature > T)
    {
        double Probability = Temprature / Temprature_init;
        delta_cost = 0.0;

        for (int i = 0; i < iter; ++i)
        {
            Disturb(finfet, count_gate, count_p, count_n,
                    gate_p_edge1, gate_p_edge2, gate_n_edge1, gate_n_edge2,
                    mm_p, mm_n, gate_map, &seed_0, &seed_1);
            restructure_edge(finfet, gate_p_edge1, gate_p_edge2, gate_n_edge1, gate_n_edge2, path_p, path_n, count_gate);

            temp_hpwl = hpwl_calculate(path_p, path_n, width_p, width_n, count_p, count_n);
            cost = temp_hpwl / normalize;

            if (cost > previous_cost) // Delta(cost) > 0
            {
                delta_cost += cost - previous_cost;
                seed_2 = rand_r((unsigned int *)&seed_2);

                if ((double)seed_2 / RAND_MAX <= Probability) // Up-hill accepted
                {
                    accept = 1;
                }
            }
            else // Delta(cost) < 0
            {
                accept = 1;
            }

            if (accept == 1)
            {
                memcpy(finfet_copy, finfet, 100 * sizeof(int));
                memcpy(gate_p_edge1_copy, gate_p_edge1, 100 * sizeof(int));
                memcpy(gate_p_edge2_copy, gate_p_edge2, 100 * sizeof(int));
                memcpy(gate_n_edge1_copy, gate_n_edge1, 100 * sizeof(int));
                memcpy(gate_n_edge2_copy, gate_n_edge2, 100 * sizeof(int));
                memcpy(mm_p_copy, mm_p, 100 * sizeof(int));
                memcpy(mm_n_copy, mm_n, 100 * sizeof(int));

                previous_cost = cost;
                min_hpwl = temp_hpwl;

                if (min_hpwl < global_min)
                {
                    memcpy(finfet_min, finfet, 100 * sizeof(int));
                    memcpy(gate_p_edge1_min, gate_p_edge1, 100 * sizeof(int));
                    memcpy(gate_p_edge2_min, gate_p_edge2, 100 * sizeof(int));
                    memcpy(gate_n_edge1_min, gate_n_edge1, 100 * sizeof(int));
                    memcpy(gate_n_edge2_min, gate_n_edge2, 100 * sizeof(int));
                    memcpy(mm_p_min, mm_p, 100 * sizeof(int));
                    memcpy(mm_n_min, mm_n, 100 * sizeof(int));

                    global_min = min_hpwl;
                }
            }
            else
            {
                memcpy(finfet, finfet_copy, 100 * sizeof(int));
                memcpy(gate_p_edge1, gate_p_edge1_copy, 100 * sizeof(int));
                memcpy(gate_p_edge2, gate_p_edge2_copy, 100 * sizeof(int));
                memcpy(gate_n_edge1, gate_n_edge1_copy, 100 * sizeof(int));
                memcpy(gate_n_edge2, gate_n_edge2_copy, 100 * sizeof(int));
                memcpy(mm_p, mm_p_copy, 100 * sizeof(int));
                memcpy(mm_n, mm_n_copy, 100 * sizeof(int));
            }

            accept = 0;
        }

        delta_cost /= iter;

        // cout << "time: " << times << ", temprature: " << Temprature << endl;
        // cout << "average delta: " << delta_cost << endl;
        // cout << "Minimum hpwl: " << min_hpwl << endl;
        // cout << "Probability: " << Probability << endl;
        // cout << "----------------------------------------------------------------" << endl;

        Temprature = Temprature * alpha;
    }

    memcpy(finfet, finfet_min, 100 * sizeof(int));
    memcpy(gate_p_edge1, gate_p_edge1_min, 100 * sizeof(int));
    memcpy(gate_p_edge2, gate_p_edge2_min, 100 * sizeof(int));
    memcpy(gate_n_edge1, gate_n_edge1_min, 100 * sizeof(int));
    memcpy(gate_n_edge2, gate_n_edge2_min, 100 * sizeof(int));
    memcpy(mm_p, mm_p_min, 100 * sizeof(int));
    memcpy(mm_n, mm_n_min, 100 * sizeof(int));

    *hpwl = global_min;
}

inline double hpwl_calculate(vector<int> &path_p, vector<int> &path_n,
                             int width_p, int width_n,
                             int count_p, int count_n)
{
    int length = path_p.size();
    int pin_size = max(count_p, count_n);
    double temp_hpwl = 0.0;

    int start[pin_size + 1], end[pin_size + 1];
    bool check_p[pin_size + 1], check_n[pin_size + 1];

    memset(start, length + 1, sizeof(start));
    memset(end, -1, sizeof(end));
    memset(check_p, 0, sizeof(check_p));
    memset(check_n, 0, sizeof(check_n));

    // Checking pmos row
    for (int i = 0; i < length; ++i)
    {
        if (path_p[i] != -1)
        {
            start[path_p[i]] = min(start[path_p[i]], i);
            end[path_p[i]] = max(end[path_p[i]], i);
            check_p[path_p[i]] = true;
        }
    }

    // Checking nmos row
    for (int i = 0; i < length; ++i)
    {
        if (path_n[i] != -1)
        {
            start[path_n[i]] = min(start[path_n[i]], i);
            end[path_n[i]] = max(end[path_n[i]], i);
            check_n[path_n[i]] = true;
        }
    }

    double column = (double)width_p / 2 + (double)width_n / 2 + 27.0;

    for (int i = 0; i <= pin_size; ++i)
    {
        double temp = 0.0;

        if (check_p[i] && check_n[i])
        {
            temp += column;
        }

        int distance = abs(start[i] - end[i]);

        if (distance != 0)
        {
            temp += (start[i] == 0) ? 12.5 : 17.0;
            temp += (end[i] == length - 1) ? 12.5 : 17.0;
            temp += distance * 20.0 + (distance - 1) * 34.0;
        }

        temp_hpwl += temp;
    }

    return temp_hpwl;
}
