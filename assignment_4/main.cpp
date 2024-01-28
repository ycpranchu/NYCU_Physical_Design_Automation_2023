#include "main.h"

void Input_File(string filename)
{
    std::ifstream file(filename);

    if (!file)
    {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    vector<int> temp;
    string line;
    string level, from, to;
    int time = 0;

    while (getline(file, line))
    {
        if (line[0] == 'T' || line[0] == 'B') // boundary line
        {
            istringstream iss(line);
            iss >> level >> from >> to;

            for (int i = stoi(from); i <= stoi(to); i++)
            {
                int count = stoi(level.substr(1)); // T & B number

                if (level[0] == 'T')
                {
                    if (i == stoi(from) && i != 0) // choose tighter boundary
                    {
                        for (int j = 0; j <= count; j++)
                            temp.emplace_back(j);

                        Boundary_top[i] = (Boundary_top[i].size() < temp.size()) ? Boundary_top[i] : temp;
                        temp.clear();
                        continue;
                    }

                    for (int j = 0; j <= count; j++) // set boundary
                        Boundary_top[i].emplace_back(j);
                }
                else if (level[0] == 'B')
                {
                    if (i == stoi(from) && i != 0) // choose tighter boundary
                    {
                        for (int j = 0; j <= count; j++)
                            temp.emplace_back(j);

                        Boundary_bottom[i] = (Boundary_bottom[i].size() < temp.size()) ? Boundary_bottom[i] : temp;
                        temp.clear();
                        continue;
                    }

                    for (int j = 0; j <= count; j++) // set boundary
                        Boundary_bottom[i].emplace_back(j);
                }
            }
        }
        else
        {
            istringstream iss(line);
            string node;
            int count = 0;

            while (iss >> node)
            {
                if (time == 0)
                {
                    Terminal_top[count] = stoi(node);
                    Terminal_Count += 1;
                }
                else
                {
                    Terminal_bottom[count] = stoi(node);
                }
                count += 1;
            }

            time = 1;
        }
    }

    file.close();
}

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    input_filename = argv[1];
    output_filename = argv[2];

    Input_File(input_filename);

    Channel_Routing router(Boundary_top, Boundary_bottom, Terminal_top, Terminal_bottom, Terminal_Count);
    router.Building_Graph();
    router.Routing();
    router.Output(output_filename);

    return 0;
}
