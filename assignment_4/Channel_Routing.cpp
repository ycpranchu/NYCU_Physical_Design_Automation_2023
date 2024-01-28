#include "Channel_Routing.h"

Channel_Routing::Channel_Routing(vector<vector<int>> &a, vector<vector<int>> &b, int c[], int d[], int e)
{
    Boundary_top = a;
    Boundary_bottom = b;

    for (int i = 0; i < 50; i++)
    {
        Max_id = max(Max_id, c[i]);
        Max_id = max(Max_id, d[i]);

        Terminal_top[i] = c[i];
        Terminal_bottom[i] = d[i];

        current_pin[i] = -1;
        record_times[i] = 0;
    }

    Terminal_Count = e;

    // setting uppp!

    for (int i = 0; i < Terminal_Count; i++)
    {
        record_times[Terminal_top[i]] += 1;
        record_times[Terminal_bottom[i]] += 1;

        Max_boundary = max(Max_boundary, *max_element(Boundary_top[i].begin(), Boundary_top[i].end()));
        Max_boundary = max(Max_boundary, *max_element(Boundary_bottom[i].begin(), Boundary_bottom[i].end()));
    }

    Max_boundary += 1;
}

Channel_Routing::~Channel_Routing()
{
}

bool compare_from(const Terminal *x, const Terminal *y)
{
    return x->from < y->from;
}

void Channel_Routing::Building_Graph()
{
    for (int i = 0; i < Terminal_Count; i++)
    {
        if (Terminal_top[i] != 0) // check up
        {
            if (current_pin[Terminal_top[i]] == -1)
            {
                current_pin[Terminal_top[i]] = i;
            }
            else
            {
                Terminal terminal;
                terminal.id = Terminal_top[i];
                terminal.from = current_pin[Terminal_top[i]];
                terminal.to = i;

                current_pin[Terminal_top[i]] = i;
                Terminals_map[Terminal_top[i]].emplace_back(terminal);
            }
        }

        if (Terminal_bottom[i] != 0) // check bottom
        {
            if (current_pin[Terminal_bottom[i]] == -1)
            {
                current_pin[Terminal_bottom[i]] = i;
            }
            else
            {
                Terminal terminal;
                terminal.id = Terminal_bottom[i];
                terminal.from = current_pin[Terminal_bottom[i]];
                terminal.to = i;

                current_pin[Terminal_bottom[i]] = i;
                Terminals_map[Terminal_bottom[i]].emplace_back(terminal);
            }
        }

        record_times[Terminal_top[i]] -= 1;
        record_times[Terminal_bottom[i]] -= 1;
    }

    // sorting by position

    for (auto &t_list : Terminals_map)
    {
        for (auto &terminal : t_list)
        {
            Terminals_list.emplace_back(&terminal);

            if (Terminal_top[terminal.from] == terminal.id)
                Terminals_top_map[terminal.from].emplace_back(&terminal);
            else
                Terminals_bottom_map[terminal.from].emplace_back(&terminal);

            if (Terminal_top[terminal.to] == terminal.id)
                Terminals_top_map[terminal.to].emplace_back(&terminal);
            else
                Terminals_bottom_map[terminal.to].emplace_back(&terminal);
        }
    }

    // for (auto &item : Terminals_list)
    //     cout << item->id << " " << item->from << " " << item->to << endl;

    Terminals_list.sort(&compare_from);

    // building VCG graph

    for (int i = 0; i < Terminal_Count; i++)
    {
        if (Terminal_bottom[i] == 0 || Terminal_top[i] == 0)
            continue;

        // cout << i << " top: ";
        // for (auto top : Terminals_top_map[i])
        //     cout << top << " " << top->id << " ";

        // cout << " bottom: ";
        // for (auto bottom : Terminals_bottom_map[i])
        //     cout << bottom << " " << bottom->id << " ";
        // cout << endl;

        for (auto &bottom : Terminals_bottom_map[i])
        {
            for (auto &top : Terminals_top_map[i])
            {
                bottom->prev_count += 1;
                top->next_terminals.emplace_back(bottom);
            }
        }
    }

    // checking the terminal position

    for (auto &item : Terminals_list)
    {
        if (Terminal_top[item->from] == item->id)
            item->from_position = 0;
        else if (Terminal_bottom[item->from] == item->id)
            item->from_position = 1;

        if (Terminal_top[item->to] == item->id)
            item->to_position = 0;
        else if (Terminal_bottom[item->to] == item->id)
            item->to_position = 1;

        // cout << item->id << " " << item->from << " " << item->to << endl;
    }
}

void Channel_Routing::Routing()
{
    vector<Terminal *> delete_nodes;
    int common_channel[Max_boundary] = {0};
    int watermark[Terminal_Count];

    while (!Terminals_list.empty())
    {
        // cout << "Channel_Count: " << Channel_Count << endl;

        for (int i = 0; i < Terminal_Count; i++)
            watermark[i] = 0;

        int finish = 0;

        while (finish == 0)
        {
            finish = 1;
            int prev_terminal = -1;
            int prev_boundary_top = -1;
            int prev_boundary_bottom = -1;

            for (iter = Terminals_list.begin(); iter != Terminals_list.end(); iter++)
            {
                if ((*iter)->arranged == true)
                    continue;

                if ((*iter)->prev_count != 0)
                    continue;

                // check for bonus channel
                int bonus_top = 0;
                int bonus_bottom = 0;

                if ((*iter)->from_position == (*iter)->to_position)
                {
                    if ((*iter)->from_position == 0) // up-up common channel
                    {
                        for (int i = (*iter)->from + 1; i <= (*iter)->to; i++)
                        {
                            for (auto &b : Boundary_top[i])
                            {
                                common_channel[b] += 1;
                            }
                        }

                        if (prev_terminal == (*iter)->id)
                        {
                            common_channel[prev_boundary_top] += 1;
                        }
                        else
                        {
                            for (auto &b : Boundary_top[(*iter)->from])
                                common_channel[b] += 1;
                        }

                        int wire_length = (*iter)->to - (*iter)->from + 1;
                        for (int i = 1; i < Max_boundary; i++)
                        {
                            if (common_channel[i] == 0)
                                continue;

                            if (common_channel[i] == wire_length)
                                bonus_top = i;
                        }

                        for (int i = 0; i < Max_boundary; i++)
                            common_channel[i] = 0;
                    }
                    else // down - down
                    {
                        for (int i = (*iter)->from + 1; i <= (*iter)->to; i++)
                        {
                            for (auto &b : Boundary_bottom[i])
                            {
                                common_channel[b] += 1;
                            }
                        }

                        if (prev_terminal == (*iter)->id)
                        {
                            common_channel[prev_boundary_bottom] += 1;
                        }
                        else
                        {
                            for (auto &b : Boundary_bottom[(*iter)->from])
                                common_channel[b] += 1;
                        }

                        int wire_length = (*iter)->to - (*iter)->from + 1;
                        for (int i = 1; i < Max_boundary; i++)
                        {
                            if (common_channel[i] == 0)
                                continue;

                            if (common_channel[i] == wire_length)
                                bonus_bottom = i;
                        }

                        for (int i = 0; i < Max_boundary; i++)
                            common_channel[i] = 0;
                    }
                }

                if (bonus_top > 0 || bonus_bottom > 0) // use bonus channel
                {
                    if (bonus_top > 0)
                    {
                        (*iter)->channel_top = bonus_top;

                        // free boundary
                        for (int i = (*iter)->from + 1; i <= (*iter)->to; i++)
                            Boundary_top[i][bonus_top] = 0;

                        if (prev_terminal != (*iter)->id)
                            Boundary_top[(*iter)->from][bonus_top] = 0;
                    }
                    else if (bonus_bottom > 0)
                    {
                        (*iter)->channel_bottom = bonus_bottom;

                        // free boundary
                        for (int i = (*iter)->from + 1; i <= (*iter)->to; i++)
                            Boundary_bottom[i][bonus_bottom] = 0;

                        if (prev_terminal != (*iter)->id)
                            Boundary_bottom[(*iter)->from][bonus_bottom] = 0;
                    }
                }
                else // use the standard channel
                {
                    int check = 0;

                    for (int i = (*iter)->from + 1; i <= (*iter)->to; i++)
                    {
                        if (watermark[i] == 1)
                        {
                            check = 1;
                            break;
                        }
                    }

                    if (prev_terminal != (*iter)->id)
                        if (watermark[(*iter)->from] == 1)
                            check = 1;

                    if (check == 1)
                        continue;

                    // pass

                    for (int i = (*iter)->from; i <= (*iter)->to; i++)
                        watermark[i] = 1;

                    (*iter)->channel = Channel_Count;
                }

                // cout << "Channel - " << Channel_Count << endl;
                // cout << (*iter)->id << " " << (*iter)->from << " " << (*iter)->to << " - " << bonus_top << " " << bonus_bottom << endl;

                // cout << "Channel - " << Channel_Count << endl;
                // for (int i = 0; i < Terminal_Count; i++)
                //     cout << watermark[i] << " ";
                // cout << endl;

                finish = 0;

                // free node
                for (auto &item : (*iter)->next_terminals)
                    item->prev_count -= 1;

                delete_nodes.emplace_back(*iter);
                (*iter)->arranged = true;

                prev_terminal = (*iter)->id;
                prev_boundary_top = bonus_top;
                prev_boundary_bottom = bonus_bottom;
            }

            if (finish == 0)
            {
                for (auto &item : delete_nodes)
                    Terminals_list.remove(item);

                delete_nodes.clear();
            }
        }

        Channel_Count += 1;
    }
}

void Channel_Routing::Output(string filename)
{
    ofstream ofs;
    ofs.open(filename);
    ofs << "Channel density: " << Channel_Count - 1 << endl;

    for (int i = 1; i <= Max_id; i++)
    {
        int from, to, check = 0;
        string prev_position = "";
        string position = "";

        vector<string> wire;
        vector<string> dogleg;

        ofs << "Net " << i << endl;

        for (auto &item : Terminals_map[i])
        {
            if (item.channel != 0)
            {
                position = "C" + to_string(Channel_Count - item.channel);
            }
            else if (item.channel_top != 0)
            {
                position = "T" + to_string(item.channel_top - 1);
            }
            else if (item.channel_bottom != 0)
            {
                position = "B" + to_string(item.channel_bottom - 1);
            }

            if (prev_position == "")
            {
                from = item.from;
                to = item.to;
            }

            if (prev_position == position) // update to position
            {
                to = item.to;
            }

            if (prev_position != "" && prev_position != position)
            {
                ofs << prev_position << " " << from << " " << to << endl;

                if (check == 1)
                {
                    check = 0;
                    ofs << "Dogleg " << from << endl;
                }

                from = item.from;
                to = item.to;
                check = 1;
            }

            prev_position = position;
        }

        ofs << prev_position << " " << from << " " << to << endl;

        if (check == 1)
            ofs << "Dogleg " << from << endl;
    }
}