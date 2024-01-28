#pragma GCC optimize(3, "Ofast", "inline")
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <string.h>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <random>
using namespace std;
#define E 2.718281

struct ContourNode
{
    int x, y;
    ContourNode *prev;
    ContourNode *next;
    ContourNode() : x(-1), y(-1), prev(nullptr), next(nullptr) {}
    ContourNode(int x, int y) : x(x), y(y), prev(nullptr), next(nullptr) {}
    ContourNode(int x, int y, ContourNode *prev, ContourNode *next) : x(x), y(y), prev(prev), next(next) {}
};

struct FP_Info
{
    double area;
    double h_w;
    FP_Info(double area, double h_w) : area(area), h_w(h_w) {}
};

double R_lowerbound = 0.0;
double R_upperbound = 0.0;
double Total_area = 0.0;
double normal = 0.0;

inline int Input_File(string filename, vector<int> &Height, vector<int> &Width, vector<int> &Block_map)
{
    ifstream ifs;
    ifs.open(filename);

    if (!ifs.is_open())
    {
        cout << "Failed to open the input file.\n";
        return 1; // EXIT_FAILURE
    }

    string temp, a, b, c;

    ifs >> temp;
    R_lowerbound = stod(temp); // Read the lowerbound
    ifs >> temp;
    R_upperbound = stod(temp); // Read the upperbound

    int count = 0;
    while (ifs >> a >> b >> c) // Read height and width of each block
    {
        int id = stoi(a.substr(1));
        int width = stoi(b);
        int height = stoi(c);

        Block_map.emplace_back(id);
        Height.emplace_back(height);
        Width.emplace_back(width);

        Total_area += width * height; // Calculate the total area
        count += 1;
    }

    ifs.close();
    return 0;
}

inline int Output_File(string filename, vector<int> &Block_map, int *Block_height, int *Block_width, int Root, int *Tree_left, int *Tree_right, int *Tree_reverse)
{
    ofstream ofs;
    ofs.open(filename);

    if (!ofs.is_open())
    {
        cout << "Failed to open the output file.\n";
        return 1; // EXIT_FAILURE
    }

    int block_num = Block_map.size();
    vector<int> Block_x(block_num, 0);
    vector<int> Block_y(block_num, 0);

    ContourNode *node_1 = new ContourNode(0, 0);
    ContourNode *node_2 = new ContourNode(INT32_MAX, 0);
    ContourNode *start = node_1;
    node_1->next = node_2;
    node_2->prev = node_1;

    int base_x = 0, next_x = 0, max_x = 0;
    int base_y = 0, next_y = 0, max_y = 0;
    int root = Root, update = 0;
    stack<int> stk;
    stack<ContourNode *> stk_contour;

    while (!stk.empty() || root != -1)
    {
        if (root == -1)
        {
            root = stk.top();
            stk.pop();
            start = stk_contour.top();
            stk_contour.pop();

            while (start->y != 0 && max_y >= start->prev->y)
            {
                start = start->prev;
                update = 1;
            }

            start = node_1;
        }

        if (Tree_right[root] != -1)
            stk.emplace(Tree_right[root]);

        if (start->y > start->next->y)
        {
            start->y = start->next->y;
        }

        base_x = start->x;
        Block_x[root] = base_x;
        next_x = base_x + Block_width[root];
        base_y = start->y;

        while (next_x > start->next->x)
        {
            base_y = max(base_y, start->next->y);
            start->next->next->prev = start;
            delete (start->next);
            start->next = start->next->next;
        }

        Block_y[root] = base_y;
        next_y = base_y + Block_height[root];

        ContourNode *new_node1 = new ContourNode(base_x, next_y);
        ContourNode *new_node2 = new ContourNode(next_x, next_y);
        ContourNode *new_node3 = new ContourNode(next_x, base_y);
        max_x = max(max_x, next_x);
        max_y = max(max_y, next_y);

        new_node3->next = start->next;
        if (start != node_1)
        {
            start->prev->next = new_node1;
            new_node1->prev = start->prev;
            delete (start);
        }
        else
        {
            start->next = new_node1;
            new_node1->prev = start;
        }

        new_node1->next = new_node2;
        new_node2->next = new_node3;

        new_node2->prev = new_node1;
        new_node3->prev = new_node2;
        new_node3->next->prev = new_node3;

        // Check front
        if (new_node1->x == new_node1->prev->x && new_node1->y == new_node1->prev->y)
        {
            new_node1->prev->prev->next = new_node1;
            new_node1->prev = new_node1->prev->prev;
        }

        // Check back
        if (new_node3->x == new_node3->next->x && new_node3->y == new_node3->next->y)
        {
            new_node3->next->next->prev = new_node3;
            new_node3->next = new_node3->next->next;
        }

        if (update == 1 && !stk_contour.empty())
        {
            stk_contour.pop();
            stk_contour.emplace(new_node1);
            update = 0;
        }

        if (Tree_right[root] != -1)
            stk_contour.emplace(new_node1);

        start = new_node3;
        root = Tree_left[root];
    }

    ContourNode *tmp;
    while (node_1 != nullptr)
    {
        tmp = node_1;
        node_1 = node_1->next;
        delete (tmp);
    }

    ofs << "A = " << max_x * max_y << "\n";
    cout << "A = " << max_x * max_y << "\n";
    ofs << "R = " << (double)max_x / max_y << "\n";
    cout << "R = " << (double)max_x / max_y << "\n";

    for (int i = 0; i < block_num; ++i)
    {
        ofs << "b" << Block_map[i] << " " << Block_x[i] << " " << Block_y[i];
        if (Tree_reverse[i] == 1)
            ofs << " R";

        if (i != block_num - 1)
            ofs << "\n";
    }

    return 0;
}

inline int Init_Tree(int Block_num, int *Tree_left, int *Tree_right, int *Tree_prev)
{
    int bound = sqrt(Block_num), id = 0, temp = 0;
    bool begin = true;

    while (id < Block_num - 1)
    {
        if (id % bound == 0)
        {
            if (begin == false)
            {
                Tree_left[id - 1] = -1;
                Tree_right[temp] = id;
                Tree_prev[id] = temp;
            }
            else
                begin = false;

            temp = id;
        }

        Tree_left[id] = id + 1;
        Tree_prev[id + 1] = id;
        ++id;
    }

    return 0;
}

inline FP_Info Area(int *Block_height, int *Block_width, int Root, int *Tree_left, int *Tree_right)
{
    ContourNode *node_1 = new ContourNode(0, 0);
    ContourNode *node_2 = new ContourNode(INT32_MAX, 0);
    ContourNode *start = node_1, *temp;
    node_1->next = node_2;
    node_2->prev = node_1;

    int base_x = 0, next_x = 0, max_x = 0;
    int base_y = 0, next_y = 0, max_y = 0;

    int root = Root, update = 0;
    stack<int> stk;
    stack<ContourNode *> stk_contour;

    while (!stk.empty() || root != -1)
    {
        if (root == -1)
        {
            root = stk.top();
            stk.pop();
            start = stk_contour.top();
            stk_contour.pop();

            while (start->y != 0 && max_y >= start->prev->y)
            {
                start = start->prev;
                update = 1;
            }

            start = node_1;
        }

        if (Tree_right[root] != -1)
            stk.emplace(Tree_right[root]);

        if (start->y > start->next->y)
        {
            start->y = start->next->y;
        }

        base_x = start->x;
        next_x = base_x + Block_width[root];
        base_y = start->y;

        while (next_x > start->next->x)
        {
            base_y = max(base_y, start->next->y);
            start->next->next->prev = start;
            delete (start->next);
            start->next = start->next->next;
        }

        next_y = base_y + Block_height[root];

        ContourNode *new_node1 = new ContourNode(base_x, next_y);
        ContourNode *new_node2 = new ContourNode(next_x, next_y);
        ContourNode *new_node3 = new ContourNode(next_x, base_y);
        max_x = max(max_x, next_x);
        max_y = max(max_y, next_y);

        new_node3->next = start->next;
        if (start != node_1)
        {
            start->prev->next = new_node1;
            new_node1->prev = start->prev;
            delete (start);
        }
        else
        {
            start->next = new_node1;
            new_node1->prev = start;
        }

        new_node1->next = new_node2;
        new_node2->next = new_node3;

        new_node2->prev = new_node1;
        new_node3->prev = new_node2;
        new_node3->next->prev = new_node3;

        // Check front
        if (new_node1->x == new_node1->prev->x && new_node1->y == new_node1->prev->y)
        {
            new_node1->prev->prev->next = new_node1;
            new_node1->prev = new_node1->prev->prev;
        }

        // Check back
        if (new_node3->x == new_node3->next->x && new_node3->y == new_node3->next->y)
        {
            new_node3->next->next->prev = new_node3;
            new_node3->next = new_node3->next->next;
        }

        if (update == 1 && !stk_contour.empty())
        {
            stk_contour.pop();
            stk_contour.emplace(new_node1);
            update = 0;
        }

        if (Tree_right[root] != -1)
            stk_contour.emplace(new_node1);

        start = new_node3;
        root = Tree_left[root];
    }

    ContourNode *tmp;
    while (node_1 != nullptr)
    {
        tmp = node_1;
        node_1 = node_1->next;
        delete (tmp);
    }

    FP_Info *fp_info = new FP_Info((double)max_x * max_y, (double)max_x / max_y);
    return *fp_info;
}

inline void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

inline int Disturb(int Block_num, int *Block_height, int *Block_width, int *Root, int *Tree_left, int *Tree_right, int *Tree_prev, int *Tree_reverse, int *seed0, int *seed1)
{
    *seed0 = rand_r((unsigned int *)seed0);
    int type = *seed0 % 2;

    if (type == 0) // rotate a macro
    {
        int node_1 = *seed0 % Block_num;
        int temp = Block_width[node_1];
        Block_width[node_1] = Block_height[node_1];
        Block_height[node_1] = temp;

        Tree_reverse[node_1] = (Tree_reverse[node_1] == 0) ? 1 : 0;
    }
    else if (type == 1) // swap 2 nodes
    {
        int node_1 = *seed0 % Block_num;
        int node_2 = node_1;

        while (node_2 == node_1)
        {
            *seed1 = rand_r((unsigned int *)seed1);
            node_2 = *seed1 % Block_num;
        }

        // cout << "type2 - " << node_1 << " " << node_2 << endl;

        if (Tree_prev[node_1] == -1)
            *Root = node_2;
        else if (Tree_prev[node_2] == -1)
            *Root = node_1;

        // Swap Tree_left and Tree_right of parent nodes
        if (Tree_prev[node_1] != -1)
        {
            if (Tree_left[Tree_prev[node_1]] == node_1)
                Tree_left[Tree_prev[node_1]] = node_2;
            else
                Tree_right[Tree_prev[node_1]] = node_2;
        }

        if (Tree_prev[node_2] != -1)
        {
            if (Tree_left[Tree_prev[node_2]] == node_2)
                Tree_left[Tree_prev[node_2]] = node_1;
            else
                Tree_right[Tree_prev[node_2]] = node_1;
        }

        swap(&Tree_prev[node_1], &Tree_prev[node_2]);
        swap(&Tree_left[node_1], &Tree_left[node_2]);
        swap(&Tree_right[node_1], &Tree_right[node_2]);

        // Update Tree_prev of child nodes
        if (Tree_left[node_1] != -1)
            Tree_prev[Tree_left[node_1]] = node_1;
        if (Tree_right[node_1] != -1)
            Tree_prev[Tree_right[node_1]] = node_1;
        if (Tree_left[node_2] != -1)
            Tree_prev[Tree_left[node_2]] = node_2;
        if (Tree_right[node_2] != -1)
            Tree_prev[Tree_right[node_2]] = node_2;

        if (*Root != -1)
        {
            Tree_prev[*Root] = -1;
        }
    }

    return 0;
}

inline double Cost(double A, double A_normal, double H_W, double alpha)
{
    double cost = 0.0;
    double bound = max(max(R_lowerbound - H_W, 0.0), max(H_W - R_upperbound, 0.0));
    cost = alpha * A / A_normal + (1 - alpha) * pow(bound, 2);

    return cost;
}

inline void Fast_Simulated_Annealing(int Block_num, int *Block_height, int *Block_width, int *Root, int *Tree_left, int *Tree_right, int *Tree_prev, int *Tree_reverse, int init_times, double T, int c, int k, int s, double alpha)
{
    int seed0 = time(NULL), seed1 = time(NULL) << 1, seed2 = time(NULL) << 2;

    FP_Info fp_info = Area(Block_height, Block_width, *Root, Tree_left, Tree_right);
    double previous_area = fp_info.area;
    double average_area = previous_area;
    double previous_hw = fp_info.h_w;
    double cost = 0.0;

    int Block_height_copy[Block_num];
    int Block_width_copy[Block_num];
    int Tree_left_copy[Block_num];
    int Tree_right_copy[Block_num];
    int Tree_reverse_copy[Block_num];
    int Tree_prev_copy[Block_num];

    for (int i = 0; i < Block_num; i++)
    {
        Block_height_copy[i] = Block_height[i];
        Block_width_copy[i] = Block_width[i];
        Tree_left_copy[i] = Tree_left[i];
        Tree_right_copy[i] = Tree_right[i];
        Tree_prev_copy[i] = Tree_prev[i];
        Tree_reverse_copy[i] = Tree_reverse[i];
    }
    int root_copy = *Root;

    double previous_cost = Cost(previous_area, average_area, previous_hw, alpha);
    double previous_cost_init = previous_cost;
    double delta_cost = previous_cost;

    for (int times = 2; times <= init_times + 1; ++times)
    {
        for (int i = 0; i < s; ++i)
        {
            Disturb(Block_num, Block_height, Block_width, Root, Tree_left, Tree_right, Tree_prev, Tree_reverse, &seed0, &seed1);
        }

        fp_info = Area(Block_height, Block_width, *Root, Tree_left, Tree_right);
        average_area += fp_info.area;

        cost = Cost(fp_info.area, average_area / times, fp_info.h_w, alpha);

        if (cost > previous_cost_init)
            delta_cost += cost - previous_cost_init;

        previous_cost_init = cost;
    }

    for (int i = 0; i < Block_num; i++)
    {
        Block_height[i] = Block_height_copy[i];
        Block_width[i] = Block_width_copy[i];
        Tree_left[i] = Tree_left_copy[i];
        Tree_right[i] = Tree_right_copy[i];
        Tree_prev[i] = Tree_prev_copy[i];
        Tree_reverse[i] = Tree_reverse_copy[i];
    }
    *Root = root_copy;

    int feasible = 0, accept = 0, reject = 0, times = 2;
    double Temprature_init = 0.0, Temprature = 0.0, P = 0.99, Probability = 0.0;

    average_area = previous_area;
    Temprature_init = -1 * delta_cost / init_times / log(P);
    Temprature = Temprature_init;
    delta_cost = previous_cost;

    while (Temprature > T && reject < s * 0.99)
    {
        reject = 0;

        for (int i = 0; i < s; ++i)
        {
            Disturb(Block_num, Block_height, Block_width, Root, Tree_left, Tree_right, Tree_prev, Tree_reverse, &seed0, &seed1);

            fp_info = Area(Block_height, Block_width, *Root, Tree_left, Tree_right);
            average_area += fp_info.area;
            cost = Cost(fp_info.area, average_area / times, fp_info.h_w, alpha);

            if (cost > previous_cost && fp_info.h_w < R_upperbound && fp_info.h_w > R_lowerbound) // Delta(cost) > 0
            {
                Probability = min(100000.0, pow(E, (-1) * (cost - previous_cost) / Temprature));
                seed2 = rand_r((unsigned int *)&seed2);

                if ((double)(seed2 % 100000) < Probability) // Up-hill accepted
                    accept = 1;
                else
                    reject += 1;
            }
            else if (fp_info.h_w < R_upperbound && fp_info.h_w > R_lowerbound) // Delta(cost) < 0
            {
                accept = 1;
            }
            else
            {
                reject += 1;
            }

            delta_cost += abs(cost - previous_cost);

            if (accept == 1)
            {
                for (int i = 0; i < Block_num; i++)
                {
                    Block_height_copy[i] = Block_height[i];
                    Block_width_copy[i] = Block_width[i];
                    Tree_left_copy[i] = Tree_left[i];
                    Tree_right_copy[i] = Tree_right[i];
                    Tree_prev_copy[i] = Tree_prev[i];
                    Tree_reverse_copy[i] = Tree_reverse[i];
                }
                root_copy = *Root;

                previous_cost = cost;
                feasible += 1;
            }
            else
            {
                for (int i = 0; i < Block_num; i++)
                {
                    Block_height[i] = Block_height_copy[i];
                    Block_width[i] = Block_width_copy[i];
                    Tree_left[i] = Tree_left_copy[i];
                    Tree_right[i] = Tree_right_copy[i];
                    Tree_prev[i] = Tree_prev_copy[i];
                    Tree_reverse[i] = Tree_reverse_copy[i];
                }
                *Root = root_copy;
            }

            accept = 0;
        }

        if (times <= k + 1)
        {
            Temprature = Temprature_init * ((double)delta_cost / times) / ((double)times * c);
        }
        else
        {
            Temprature = Temprature_init * ((double)delta_cost / times) / (double)times;
        }

        // cout << times << "\n";
        ++times;
    }
}

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    string input_filename = argv[1];
    string output_filename = argv[2];

    cout << "----------------------------------------------------------------" << endl;
    cout << "[Initialize]" << endl;

    vector<int> Block_map;
    vector<int> Height;
    vector<int> Width;

    if (Input_File(input_filename, Height, Width, Block_map) == 0)
        cout << "Reading the input file..." << endl;

    cout << R_lowerbound << " " << R_upperbound << endl;
    cout << Total_area << endl;

    int Block_num = Height.size(), Root = 0;
    int Block_height[Block_num];
    int Block_width[Block_num];
    int Tree_left[Block_num];
    int Tree_right[Block_num];
    int Tree_prev[Block_num];
    int Tree_reverse[Block_num];

    for (int i = 0; i < Block_num; ++i)
    {
        Block_height[i] = Height[i];
        Block_width[i] = Width[i];
        Tree_left[i] = -1;
        Tree_right[i] = -1;
        Tree_prev[i] = -1;
        Tree_reverse[i] = 0;
    }

    if (Init_Tree(Block_num, Tree_left, Tree_right, Tree_prev) == 0)
        cout << "Initialized the first B* Tree..." << endl;

    cout << "----------------------------------------------------------------" << endl;
    cout << "[Simulated Annealing]" << endl;

    int init_times = 10, c = 100, k = 8, s = 10000;
    double T = 0.00005, alpha = 0.8;

    Fast_Simulated_Annealing(Block_num, Block_height, Block_width, &Root, Tree_left, Tree_right, Tree_prev, Tree_reverse, init_times, T, c, k, s, alpha);

    cout << "----------------------------------------------------------------" << endl;
    if (Output_File(output_filename, Block_map, Block_height, Block_width, Root, Tree_left, Tree_right, Tree_reverse))
        cout << "Writing the output file..." << endl;

    return 0;
}