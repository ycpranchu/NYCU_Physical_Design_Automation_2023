#pragma GCC optimize(3, "Ofast", "inline")
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <queue>
#include <string>
#include <algorithm>
#include <random>
using namespace std;

struct ListNode
{
    int uid;
    int val = 0;
    ListNode *pev;
    ListNode *next;
    ListNode() : uid(0), pev(nullptr), next(nullptr) {}
    ListNode(int x) : uid(x), pev(nullptr), next(nullptr) {}
    ListNode(int x, ListNode *pev, ListNode *next) : uid(x), pev(pev), next(next) {}
};

inline int input_file(string filename, vector<vector<int>> &Net, vector<vector<int>> &Cell, float *balance_factor);
inline int output_file(string filename, vector<int> &final_partition, int Cutsize);
inline int initial_partition(vector<int> &partition);
inline int initial_gain(vector<ListNode *> &cell_record, vector<int> &partition, vector<vector<int>> &Net, vector<vector<ListNode *>> &Bucket_List);
inline int constraint_check(int group_size[], int group_index, float constraint_down, float constraint_up);
inline int find_element(int *max_gain_index0, int *max_gain_index1, int group_size[], vector<vector<ListNode *>> &Bucket_List, float constraint_down, float constraint_up);
inline int update_gain(int max_node, int *max_gain_index0, int *max_gain_index1, vector<vector<int>> &Net, vector<ListNode *> &cell_record, vector<vector<int>> &Cell, vector<int> &partition, vector<vector<ListNode *>> &Bucket_List);

int cell_nums = 0;
int P_max = 0;

inline int
input_file(string filename, vector<vector<int>> &Net, vector<vector<int>> &Cell, float *balance_factor)
{
    ifstream ifs;
    ifs.open(filename);

    if (!ifs.is_open())
    {
        cout << "Failed to open the input file.\n";
        return 1; // EXIT_FAILURE
    }

    string temp;
    ifs >> temp;
    *balance_factor = stof(temp);

    vector<int> item;
    set<int> repeat_check;

    while (getline(ifs, temp, ' ')) // Net array;
    {
        if (temp[0] == 'c')
        {
            int i = stoi(temp.substr(1));

            if (repeat_check.count(i))
            {
                continue;
            }
            else
            {
                cell_nums = max(i, cell_nums); // Calculate # of Cell;
                repeat_check.emplace(i);
                item.emplace_back(i - 1);
            }
        }
        if (temp[0] == ';')
        {
            Net.emplace_back(item);
            item.clear();
            repeat_check.clear();
        }
    }

    Cell.resize(cell_nums);
    for (int i = 0; i < (int)Net.size(); ++i) // Cell array;
    {
        for (auto &index : Net[i])
        {
            Cell[index].emplace_back(i);
        }
    }

    for (auto &it : Cell) // Calculate P_max;
        P_max = max(P_max, (int)it.size());

    ifs.close();
    return 0;
}

inline int output_file(string filename, vector<int> &final_partition, int Cutsize)
{
    ofstream ofs;
    ofs.open(filename);

    if (!ofs.is_open())
    {
        cout << "Failed to open the output file.\n";
        return 1; // EXIT_FAILURE
    }

    int n = final_partition.size();
    int group2_size = reduce(final_partition.begin(), final_partition.end());
    vector<int> partition_a, partition_b;

    for (int i = 0; i < n; ++i)
    {
        if (final_partition[i] == 0)
            partition_a.emplace_back(i);
        else
            partition_b.emplace_back(i);
    }

    ofs << "Cutsize = " << Cutsize << "\n";
    ofs << "G1 " << final_partition.size() - group2_size << "\n";
    for (auto &i : partition_a)
        ofs << "c" << i + 1 << " ";
    ofs << ";"
        << "\n"
        << "G2 " << group2_size << "\n";
    for (auto &i : partition_b)
        ofs << "c" << i + 1 << " ";
    ofs << ";"
        << "\n";

    cout << "G1: " << final_partition.size() - group2_size << " G2: " << group2_size << "\n";

    ofs.close();
    return 0;
}

inline int initial_partition(vector<int> &partition)
{
    for (int i = 0; i < cell_nums / 2; ++i)
        partition[i] = 1;

    auto rd = random_device{};
    auto rng = default_random_engine{rd()};
    shuffle(partition.begin(), partition.end(), rng);
    return 0;
}

inline int initial_gain(vector<ListNode *> &cell_record, vector<int> &partition, vector<vector<int>> &Net, vector<vector<ListNode *>> &Bucket_List)
{
    vector<int> gain(cell_nums, 0);

    Bucket_List[0].clear();
    Bucket_List[1].clear();

    for (int i = 0; i < P_max * 2 + 1; ++i)
    {
        Bucket_List[0].emplace_back(new ListNode(i - P_max, nullptr, nullptr));
        Bucket_List[1].emplace_back(new ListNode(i - P_max, nullptr, nullptr));
    }

    for (auto &item : Net)
    {
        int checker = 0; // number of cells in partition B;
        for (auto &i : item)
            checker += partition[i];

        if (checker == 0 || checker == (int)item.size()) // All cells in the same partition;
        {
            for (auto &i : item)
                --gain[i];
        }

        if (checker == 1) // Single cell in the partition B;
        {
            for (auto &i : item)
                if (partition[i] == 1)
                {
                    ++gain[i];
                    break;
                }
        }

        if (checker == (int)item.size() - 1) // Single cell in partition A;
        {
            for (auto &i : item)
                if (partition[i] == 0)
                {
                    ++gain[i];
                    break;
                }
        }
    }

    for (int i = 0; i < cell_nums; ++i) // Initialize Bucket_List;
    {
        ListNode *node = cell_record[i];
        ListNode *head = Bucket_List[partition[i]][gain[i] + P_max];

        node->val = gain[i];
        node->next = head->next;
        node->pev = head;
        head->next = node;
        if (node->next != nullptr)
            node->next->pev = node;
    }

    return 0;
}

inline int constraint_check(int group_size[], int group_index, float constraint_down, float constraint_up)
{
    if (group_index == 0)
    {
        if (constraint_down <= (float)group_size[0] - 1 && (float)group_size[1] + 1 <= constraint_up)
        {
            --group_size[0];
            ++group_size[1];
            return 1;
        }
    }
    else
    {
        if (constraint_down <= (float)group_size[1] - 1 && (float)group_size[0] + 1 <= constraint_up)
        {
            ++group_size[0];
            --group_size[1];
            return 1;
        }
    }

    return 0;
}

inline int find_element(int *max_gain_index0, int *max_gain_index1, int group_size[], vector<vector<ListNode *>> &Bucket_List, float constraint_down, float constraint_up)
{
    int index = -1;
    ListNode *node;

    if (group_size[0] >= group_size[1])
    {
        for (int i = 0; i < 2; ++i) // Find max element and check its constraint;
        {
            if (constraint_check(group_size, i, constraint_down, constraint_up) == 0) // Constraint correctly not satisfied;
                continue;

            node = (i == 0) ? Bucket_List[i][*max_gain_index0] : Bucket_List[i][*max_gain_index1];
            index = node->next->uid;
            break;
        }
    }
    else
    {
        for (int i = 1; i >= 0; --i) // Find max element and check its constraint;
        {
            if (constraint_check(group_size, i, constraint_down, constraint_up) == 0) // Constraint correctly not satisfied;
                continue;

            node = (i == 1) ? Bucket_List[i][*max_gain_index1] : Bucket_List[i][*max_gain_index0];
            index = node->next->uid;
            break;
        }
    }

    return index;
}

inline int update_gain(int max_node, int *max_gain_index0, int *max_gain_index1, vector<vector<int>> &Net, vector<ListNode *> &cell_record, vector<vector<int>> &Cell, vector<int> &partition, vector<vector<ListNode *>> &Bucket_List)
{
    ListNode *node = cell_record[max_node];

    // Delete the moved node;
    node->pev->next = node->next;
    if (node->next != nullptr)
        node->next->pev = node->pev;

    node->val = -P_max - 1;
    node->pev = nullptr;
    node->next = nullptr;

    int partition_index = partition[max_node];
    int gain_offset[cell_nums] = {0};
    int cutsize_offset = 0;
    int checker = 0, F = 0, T = 0;
    set<int> gain_changed;

    for (auto &net_element : Cell[max_node])
    {
        vector<int> cell_element = Net[net_element];

        checker = 0; // number of cells in partition B;

        for (auto &i : cell_element)
        {
            checker += partition[i];
        }

        F = 0, T = 0; // calculate F(n) and T(n);

        if (partition_index == 0)
        {
            F = cell_element.size() - checker;
            T = checker;
        }
        else
        {
            F = checker;
            T = cell_element.size() - checker;
        }

        // Update gains before the move;
        if (T == 0)
        {
            for (auto &i : cell_element)
            {
                ++gain_offset[i];
                gain_changed.emplace(i);
            }
            ++cutsize_offset;
        }
        else if (T == 1)
        {
            for (auto &i : cell_element) // Find the only T cell on n;
            {
                if (partition[i] != partition_index)
                {
                    --gain_offset[i];
                    gain_changed.emplace(i);
                    break;
                }
            }
        }

        F -= 1;

        // Update gains after the move;
        if (F == 0)
        {
            for (auto &i : cell_element)
            {
                --gain_offset[i];
                gain_changed.emplace(i);
            }
            --cutsize_offset;
        }
        else if (F == 1)
        {
            for (auto &i : cell_element) // Find the only T cell on n;
            {
                if (i != max_node && partition[i] == partition_index)
                {
                    ++gain_offset[i];
                    gain_changed.emplace(i);
                    break;
                }
            }
        }
    }

    partition[max_node] = 1 - partition_index;

    // Update the Bucket_List;
    for (auto &i : gain_changed)
    {
        ListNode *node = cell_record[i];

        if (gain_offset[i] != 0 && node->val != -P_max - 1)
        {
            ListNode *node = cell_record[i];
            node->val = node->val + gain_offset[i];

            node->pev->next = node->next;
            if (node->next != nullptr)
                node->next->pev = node->pev;

            ListNode *head = Bucket_List[partition[i]][node->val + P_max];

            if (partition[i] == 0)
                *max_gain_index0 = max(*max_gain_index0, node->val + P_max);
            else
                *max_gain_index1 = max(*max_gain_index1, node->val + P_max);

            node->next = head->next;
            node->pev = head;
            head->next = node;
            if (node->next != nullptr)
                node->next->pev = node;
        }
    }

    return cutsize_offset;
}

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string input_filename = argv[1];
    string output_filename = argv[2];

    cout << "----------------------------------------------------------------" << endl;
    cout << "[Initialize]" << endl;

    vector<vector<int>> Net;
    vector<vector<int>> Cell;
    float balance_factor = 0.0;

    if (input_file(input_filename, Net, Cell, &balance_factor) == 0)
        cout << "Reading the input file..." << endl;

    float constraint_down = 0.0, constraint_up = 0.0;
    constraint_down = float((float)(cell_nums * 0.5) - (int)(balance_factor * cell_nums * 0.5));
    constraint_up = float((float)(cell_nums * 0.5) + (int)(balance_factor * cell_nums * 0.5));

    vector<int> partition(cell_nums, 0);
    vector<int> final_partition, temp_partition;
    if (initial_partition(partition) == 0)
        cout << "Initializing the Random Partition..." << endl;

    // Initial the Bucket_List;
    vector<vector<ListNode *>> Bucket_List(2);

    // Initial the Cell Recording Vector;
    vector<ListNode *> cell_record(cell_nums);
    for (int i = 0; i < cell_nums; ++i)
    {
        ListNode *node = new ListNode(i);
        node->val = -1;
        cell_record[i] = node;
    }

    if (initial_gain(cell_record, partition, Net, Bucket_List) == 0)
        cout << "Computing initial gains..." << endl;

    int final_epoch = -1;
    int global_min_cutsize = INT32_MAX;

    int group_size[2];
    group_size[1] = reduce(partition.begin(), partition.end());
    group_size[0] = cell_nums - group_size[1];

    int Epochs = 25;
    int random_size = cell_nums * 0.001;
    for (int epoch = 0; epoch < Epochs; ++epoch)
    {
        temp_partition.assign(partition.begin(), partition.end());

        cout << "----------------------------------------------------------------" << endl;
        cout << "[Running the algorithm, epoch " << epoch << "]" << endl;

        // Calculate the Cutsize; ----------------------------------------------------------------
        int cutsize = 0;
        for (int i = 0; i < (int)Net.size(); ++i)
        {
            auto cell_element = Net[i];
            int checker = 0;
            for (auto &i : cell_element)
                checker += partition[i];
            if (checker != 0 && checker != (int)cell_element.size())
            {
                ++cutsize;
            }
        }
        cout << "Computing initial cutsize..." << endl;
        // ----------------------------------------------------------------

        int max_gain_index0 = P_max * 2, max_gain_index1 = P_max * 2;
        int count = 0, max_node = -1;
        int earlystop_size = INT32_MAX;

        while (count < cell_nums)
        {
            // Update the max_gain index; ----------------------------------------------------------------
            for (int j = max_gain_index0; j >= 0; --j)
            {
                max_gain_index0 = j;
                if (Bucket_List[0][j]->next != nullptr)
                {
                    break;
                }
            }

            for (int j = max_gain_index1; j >= 0; --j)
            {
                max_gain_index1 = j;
                if (Bucket_List[1][j]->next != nullptr)
                {
                    break;
                }
            }
            // ----------------------------------------------------------------

            max_node = find_element(&max_gain_index0, &max_gain_index1, group_size, Bucket_List, constraint_down, constraint_up);

            if (max_node == -1)
            {
                cout << "Can't make a partition!!" << endl;
                break;
            }

            cutsize += update_gain(max_node, &max_gain_index0, &max_gain_index1, Net, cell_record, Cell, partition, Bucket_List);

            if (count % 15000 == 0)
            {
                if (cutsize < global_min_cutsize)
                {
                    final_partition.assign(partition.begin(), partition.end());
                    final_epoch = epoch;

                    global_min_cutsize = cutsize;
                }

                cout << "Done move " << count << " | Cutsize " << global_min_cutsize << endl;

                if (global_min_cutsize == earlystop_size)
                {
                    cout << "Early Stop!!..." << endl;
                    break;
                }

                earlystop_size = global_min_cutsize;
            }

            ++count;
        }

        cout << "----------------------------------------------------------------" << endl;

        if (epoch < Epochs - 1) // Re-Initialize Bucket_List
        {
            // Set some cells have random partition;
            auto rd = random_device{};
            auto rng = default_random_engine{rd()};
            shuffle(partition.begin(), partition.begin() + random_size, rng);

            if (initial_gain(cell_record, partition, Net, Bucket_List) == 0)
                cout << "Re-Computing initial gains..." << endl;
        }
    }

    cout << "[Found the Solution]" << endl;
    cout << "Total Epochs: " << Epochs << endl;
    cout << "Min Cutsize: " << global_min_cutsize << " on epoch " << final_epoch << endl;

    if (output_file(output_filename, final_partition, global_min_cutsize) == 0)
        cout << "Writing the output file..." << endl;

    return 0;
}
