
// Для успешной компиляции, необходимо использовать стандарт С++11

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <time.h>
#include <map>
#include "gmp-6.3.0/gmpxx.h" // Для больших целых чисел (brew install gmp)

#ifdef _WIN32
#include <direct.h>  // Для Windows (_mkdir)
    #define make_dir(name) _mkdir(name)
#else
#include <sys/stat.h>  // Для Linux/macOS (mkdir)
#include <sys/types.h>
#define make_dir(name) mkdir(name, 0777)
#endif


using namespace std;
bool fast_result = 0;           // '0' for all levels <= LEVEL result, '1' for only LEVEL result (should be TASK = 1)
int FLORES = 1;                 // to debug
int TASK = 0;                   // to debug
int TIMES = 200;                // to stop surching if looped
float E = 0.00001;              // Mistake
int VER1 = 1;                   // to debug
int VER2 = 2;                   // to debug
bool ABSTRACT_STOCK = 1;        // '0' to use graph without stock, else use '1'

string graph_name = "2D";       // "3D", "2D", "2D-2str", "2D-3str", "Pascal", "2parts"
string algorithm = "rombs";     // "kops", "rombs"
int LEVEL = 47;                 // number of levels
float START_WEIGHT = 0;         // '0' for Richardsons, '1' for random
int FILE_NUM = 300;              // Number of files
string special_folder_description = "rich";
string folder_name = graph_name + "_" + algorithm + "_" + to_string(LEVEL + (TASK * FLORES));


class Vertex
{
public:
    Vertex(int index) // конструктор класса
    {
        set_vertex(index);
    }
    mpz_class dim = 0;
    unordered_map<int, float> parents;
    vector<int> parents_idx;
    unordered_map<int, float> children;
    unordered_map<int, float> children_kops;
    vector<int> children_idx;
    void set_vertex(int index)
    {
        idx = index;
    }
    void add_parent(int parent_index, float parent_weight)
    {
        parents[parent_index] = parent_weight;
        parents_idx.push_back(parent_index);
    }
    void add_child(int child_index, float child_weight)
    {
        children[child_index] = child_weight;
        children_kops[child_index] = 0.0;
        children_idx.push_back(child_index);
    }
    void clear_children()
    {
        children.clear();
        children_kops.clear();
        children_idx.clear();
    }

private:
    int idx;
};

// класс графа
class Graph
{
public:
    vector<Vertex> vertexList;
    vector<int> indexes; // for file naming in 3D
    vector<vector<vector<int>>> current_young_list;
    vector<vector<vector<vector<int>>>> current_young_list_3D;
    int new_level = LEVEL;
    int last_level = 1;
    int index_prev_level = 1;
    int index_last_level = 1;
    int index_2D = 1; // for file naming in 3D
    int index_3D = 2000; // for file naming in 3D
    int cur_index = 1;
    int hunted_index = 1;
    int romb_start_index = 1;
    vector<vector<vector<int>>> rombsLists;
    vector <float> weights_last_level;
    vector <float> weights_this_level;
    vector <float> weights_last_task;
    vector <ofstream> files;
    void add_vertex(int new_index, int current_index)
    {
        vertexList.push_back(Vertex(new_index));
        vertexList.at(current_index).add_child(new_index, START_WEIGHT);
        vertexList.at(new_index).add_parent(current_index, START_WEIGHT);
    }
};

void open_files();
void young_level_3D(Graph* graph);
void young_level_2D(Graph* graph);
void find_kops(Graph* graph);
void find_rombs(Graph* graph);
int central_normal(Graph* graph);
void delete_last(Graph* graph);
void writeArrayToFile(Graph* graph, int new_index, vector<vector<int>> cur_diag,  vector<vector<int>> new_diag);
void writeArrayToFile2D(Graph* graph, int new_index, vector<int> cur_diag,  vector<int> new_diag);

//// TO DEBUG
ofstream outf(to_string(VER1) + " to " + to_string(VER2) + ".txt");
ofstream outfile("diff.txt");


int main()
{
    if (algorithm == "kops")
    {
        ABSTRACT_STOCK = 1;
        TASK = 0;
    }
    if (!special_folder_description.empty()) {folder_name = folder_name + "_" + special_folder_description;}
    const char* fn = folder_name.c_str();
    if (make_dir(fn) == 0) {
        cout << "Папка создана: " << folder_name << endl;
    } else {
        cout << "Папка уже существует: " << folder_name << endl;
    }

    if (!outfile)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        cerr << "Uh oh, file could not be opened for writing!" << endl;
        exit(1);
    }

    Graph graph;
    clock_t start = clock();
    // Заполняем граф Юнга вершинами и ребрами до нужного этажа
    if (graph_name == "3D")
    {
        graph.current_young_list_3D.push_back({{{0}, {0}}});
        graph.current_young_list_3D.push_back({{{1}, {0}}});
        graph.vertexList.push_back(Vertex(0));
        graph.indexes.push_back(0);
        graph.vertexList.push_back(Vertex(1));
        graph.indexes.push_back(1);
        young_level_3D(&graph);
    }
    if ((graph_name == "2D") or (graph_name == "2D-2str") or (graph_name == "2D-3str") or (graph_name == "Pascal"))
    {
        graph.current_young_list.push_back({{0}});
        graph.current_young_list.push_back({{1}});
        graph.vertexList.push_back(Vertex(0));
        graph.vertexList.push_back(Vertex(1));
        young_level_2D(&graph);
    }

    if (algorithm == "rombs") find_rombs(&graph);
    if (algorithm == "kops") find_kops(&graph);


    for (int task = 0; task <= TASK; task++)
    {
        if (task)
        {
            cout << endl << "Task " << task << " of " << TASK << endl;
        }
        for (int times = 0; times < TIMES; times++)
        {
            int result = central_normal(&graph);
            if (result == 6)
            {
                if (task)
                {
                    cout << "Success " << endl;
                }
                break;
            }
        }
        if (task == TASK)
        {
            clock_t end = clock();
            double result = (double)(end - start)/CLOCKS_PER_SEC;
            cout << "working time: " << result << endl;
            // outf.close();
            outfile.close();
            return 7;
        }
        if (ABSTRACT_STOCK) delete_last(&graph);
        graph.new_level = graph.new_level + FLORES;
        if (graph_name == "3D") young_level_3D(&graph);
        if ((graph_name == "2D") or (graph_name == "2D-2str") or (graph_name == "2D-3str") or (graph_name == "Pascal")) young_level_2D(&graph);
        find_rombs(&graph);
    }
    // outf.close();
    outfile.close();
    clock_t end = clock();
    double result = (double)(end - start)/CLOCKS_PER_SEC;
    cout << "working time: " << result << endl;
    return 9;
}


void open_files()
{
    // Открываем файл для записи результатов
    ofstream outf(to_string(VER1) + "to" + to_string(VER2) + ".txt");
    if (!outf)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        cerr << "Uh oh, " + to_string(VER1) + "to" + to_string(VER2) + ".txt could not be opened for writing!" << endl;
        exit(1);
    }
    outf << "Здесь будут вероятности перехода, посчитанные для ребра "
         << VER1 << " -- " << VER2 << " при количестве этажей -- " << LEVEL << endl;

    // Открываем файл для записи diff
    ofstream outfile(to_string(VER1) + "to" + to_string(VER2) + "diff.txt");
    if (!outfile)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        cerr << "Uh oh, " + to_string(VER1) + "to" + to_string(VER2) + ".txt could not be opened for writing!" << endl;
        exit(1);
    }
}

void young_level_3D(Graph* graph)
{
    vector<vector<vector<int>>> prev_flore_list;
    vector<vector<vector<int>>> flore_list;
    map<vector<vector<int>>, int> floor_map;
    vector<vector<int>> curr_diagram;

    graph->index_last_level = graph->cur_index;
    int index_prev_level = graph->index_prev_level;
    int new_index = graph->index_prev_level;
    int cur_index = graph->cur_index;
    int count_edges = -1;

    for (int level = graph->last_level; level < graph->new_level; level++) // По уровням
    {
        graph->cur_index = new_index + 1;
        prev_flore_list = graph->current_young_list_3D.at(level);
        for (int k = 0; k < prev_flore_list.size(); k++) // По диаграммам на уровне
        {
            curr_diagram = prev_flore_list.at(k);
            // Для 3Д графа изменена вложенность
            if (curr_diagram.at(curr_diagram.size() - 1).at(0) == 0)
            {
                vector<vector<int>> new_diag_3D(curr_diagram.begin(), curr_diagram.end());
                new_diag_3D[curr_diagram.size() - 1][0] = 1;
                new_diag_3D.push_back({0});
                if (floor_map.count(new_diag_3D) == 0) {
                    // Добавляем куб по координате z, если его не было
                    new_index++;
                    count_edges++;
                    if(graph->files.size() < FILE_NUM)
                    {
                        graph->index_3D ++;
                        graph->indexes.push_back(graph->index_3D);
                        ofstream out_edge(folder_name + "/" + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[new_index]) + ".txt");
                        if (!out_edge)
                        {
                            // то выводим сообщение об ошибке и выполняем функцию exit()
                            cerr << "Uh oh, " + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[new_index]) + ".txt could not be opened for writing!" << endl;
                            exit(1);
                        }
                        graph->files.push_back(move(out_edge));
                        writeArrayToFile(graph, count_edges, curr_diagram, new_diag_3D);
                    }
                    graph->add_vertex(new_index, cur_index);
                    floor_map.insert({new_diag_3D, new_index});
                    flore_list.push_back(new_diag_3D);
                    // cout << "index 3D " << new_index << " level " << level << endl;
                } else {
                    int sticked_index = floor_map.at(new_diag_3D);
                    count_edges++;
                    if(graph->files.size() < FILE_NUM)
                    {
                        ofstream out_edge(folder_name + "/" + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[sticked_index]) + ".txt");
                        if (!out_edge)
                        {
                            // то выводим сообщение об ошибке и выполняем функцию exit()
                            cerr << "Uh oh, " + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[sticked_index]) + ".txt could not be opened for writing!" << endl;
                            exit(1);
                        }
                        graph->files.push_back(move(out_edge));
                        writeArrayToFile(graph, count_edges, curr_diagram, new_diag_3D);
                    }
                    graph->vertexList.at(cur_index).add_child(sticked_index, START_WEIGHT);
                    graph->vertexList.at(sticked_index).add_parent(cur_index, START_WEIGHT);
                }
                new_diag_3D.clear();
            }

            for (int f = 0; f < curr_diagram.size()-1; f++) // По этажам диаграммы
            {
                if (f > 0){
                    if (curr_diagram[f - 1].size() == curr_diagram[f].size()) { continue;}
                }
                vector<vector<int>> new_diagram(curr_diagram.begin(), curr_diagram.end());
                new_diagram[f].push_back(1);
                if (floor_map.count(new_diagram) == 0) {
                    new_index++;
                    count_edges++;
                    if (graph->files.size() < FILE_NUM) {
                        if (new_diagram[1][0] >= 1) {
                            graph->index_3D++;
                            graph->indexes.push_back(graph->index_3D);
                        } else {
                            graph->index_2D++;
                            graph->indexes.push_back(graph->index_2D);
                        }
                        ofstream out_edge(folder_name + "/" +
                                to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[new_index]) +
                                ".txt");
                        if (!out_edge) {
                            // то выводим сообщение об ошибке и выполняем функцию exit()
                            cerr << "Uh oh, " + to_string(graph->indexes[cur_index]) + "to" +
                                    to_string(graph->indexes[new_index]) + ".txt could not be opened for writing!"
                                 << endl;
                            exit(1);
                        }

                        graph->files.push_back(move(out_edge));
                        writeArrayToFile(graph, count_edges, curr_diagram, new_diagram);
                    }
                    graph->add_vertex(new_index, cur_index);
                    floor_map.insert({new_diagram, new_index});
                    flore_list.push_back(new_diagram);
                } else {
                    int sticked_index = floor_map.at(new_diagram);
                    count_edges++;
                    if (graph->files.size() < FILE_NUM) {
                        ofstream out_edge(folder_name + "/" +
                                to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[sticked_index]) +
                                ".txt");
                        if (!out_edge) {
                            // то выводим сообщение об ошибке и выполняем функцию exit()
                            cerr << "Uh oh, " + to_string(graph->indexes[cur_index]) + "to" +
                                    to_string(graph->indexes[sticked_index]) + ".txt could not be opened for writing!"
                                 << endl;
                            exit(1);
                        }
                        graph->files.push_back(move(out_edge));
                        writeArrayToFile(graph, count_edges, curr_diagram, new_diagram);
                    }
                    graph->vertexList.at(cur_index).add_child(sticked_index, START_WEIGHT);
                    graph->vertexList.at(sticked_index).add_parent(cur_index, START_WEIGHT);
                }
                new_diagram.clear();
            }

            for (int f = 0; f < curr_diagram.size()-1; f++) // По этажам диаграммы
            {
                for (int i = 0; i < curr_diagram[f].size(); i++) // По столбцам диаграммы
                {
                    int cur_elem = curr_diagram[f].at(i);
                    if (i > 0) {
                        if (curr_diagram[f].at(i - 1) == cur_elem) { continue; }
                        if (f > 0) {
                            if (curr_diagram[f-1].at(i) == cur_elem) { continue; }
                        }
                    }
                    else if (f > 0){
                        if (curr_diagram[f-1].at(i) == cur_elem) { continue; }
                    }
                    vector<vector<int>> new_diagram(curr_diagram.begin(), curr_diagram.end());
                    new_diagram[f].at(i) = cur_elem + 1;
                    if (floor_map.count(new_diagram) == 0) {
                        new_index++;
                        count_edges++;
                        if(graph->files.size() < FILE_NUM)
                        {
                            if (new_diagram[1][0] >= 1)
                            {
                                graph->index_3D ++;
                                graph->indexes.push_back(graph->index_3D);
                            }
                            else
                            {
                                graph->index_2D ++;
                                graph->indexes.push_back(graph->index_2D);
                            }
                            ofstream out_edge(folder_name + "/" + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[new_index]) + ".txt");
                            if (!out_edge)
                            {
                                // то выводим сообщение об ошибке и выполняем функцию exit()
                                cerr << "Uh oh, " + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[new_index]) + ".txt could not be opened for writing!" << endl;
                                exit(1);
                            }

                            graph->files.push_back(move(out_edge));
                            writeArrayToFile(graph, count_edges, curr_diagram, new_diagram);
                        }

                        graph->add_vertex(new_index, cur_index);
                        floor_map.insert({new_diagram, new_index});
                        flore_list.push_back(new_diagram);
                    } else {
                        int sticked_index = floor_map.at(new_diagram);
                        count_edges++;
                        if(graph->files.size() < FILE_NUM)
                        {
                            ofstream out_edge(folder_name + "/" + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[sticked_index]) + ".txt");
                            if (!out_edge)
                            {
                                // то выводим сообщение об ошибке и выполняем функцию exit()
                                cerr << "Uh oh, " + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[sticked_index]) + ".txt could not be opened for writing!" << endl;
                                exit(1);
                            }

                            graph->files.push_back(move(out_edge));
                            writeArrayToFile(graph, count_edges, curr_diagram, new_diagram);
                        }

                        graph->vertexList.at(cur_index).add_child(sticked_index, START_WEIGHT);
                        graph->vertexList.at(sticked_index).add_parent(cur_index, START_WEIGHT);
                    }
                    new_diagram.clear();
                }
            }
            cur_index++;
        }
        floor_map.clear();

        // Для стока в абстрактную вершину
        if ((level == graph->new_level - 1) & (ABSTRACT_STOCK)) {
            new_index++;
            cout << "last index " << new_index << " on the level " << level + 1 << endl;
            int start_idx = index_prev_level + 1;
            graph->add_vertex(new_index, start_idx);
            start_idx++;
            graph->hunted_index = new_index;
            graph->vertexList.at(new_index).dim = 1;
            for (int k = 1; k < flore_list.size(); k++) // По диаграммам на уровне
            {
                graph->vertexList.at(start_idx).dim = 1;
                graph->vertexList.at(start_idx).add_child(new_index, START_WEIGHT);
                graph->vertexList.at(start_idx).children_kops.at(new_index) = 1.0;
                graph->vertexList.at(new_index).add_parent(start_idx, START_WEIGHT);
                start_idx++;
            }
        }
        index_prev_level = new_index;
        graph->current_young_list_3D.push_back(flore_list);
        graph->index_prev_level = index_prev_level - 1;
        flore_list.clear();
    }

    // Начальные вероятности
    for (int i = graph->index_last_level; i < graph->vertexList.size(); i++)
    {
        for (int j: graph->vertexList.at(i).children_idx)
        {
            float new_weight;
            if (!START_WEIGHT)
            {
                new_weight = 1.0/graph->vertexList.at(i).children_idx.size();
            }
            else if (START_WEIGHT == 1)
            {
                new_weight = 1.0/(rand() % 9 +1);
            }
            else new_weight = START_WEIGHT;
            graph->vertexList.at(i).children.at(j) = new_weight;
            graph->vertexList.at(j).parents.at(i) = new_weight;
            if (i < graph->cur_index)
            {
                graph->weights_last_level.push_back(new_weight);
            }
        }
    }
    graph->last_level = graph->new_level;
}

void young_level_2D(Graph* graph)
{
    vector<vector<int>> prev_flore_list;
    vector<vector<int>> flore_list;
    map<vector<int>, int> floor_map;
    vector<int> curr_diagram;

    graph->index_last_level = graph->cur_index;
    int index_prev_level = graph->index_prev_level;
    int new_index = graph->index_prev_level;
    int cur_index = graph->cur_index;

    for (int level = graph->last_level; level < graph->new_level; level++) // По уровням
    {
        graph->cur_index = new_index + 1;
        prev_flore_list = graph->current_young_list.at(level);
        for (int k = 0; k < prev_flore_list.size(); k++) // По диаграммам на уровне
        {
            curr_diagram = prev_flore_list.at(k);
            vector<int> new_diagram(curr_diagram.begin(), curr_diagram.end());
            new_diagram.push_back(1);
            if (floor_map.count(new_diagram) == 0) {
                // Добавляем к диаграмме столбец
                new_index++;
                if(graph->files.size() < FILE_NUM)
                {
                    ofstream out_edge(folder_name + "/" + to_string(cur_index) + "to" + to_string(new_index) + ".txt");
                    if (!out_edge)
                    {
                        // то выводим сообщение об ошибке и выполняем функцию exit()
                        cerr << "Uh oh, " + to_string(cur_index) + "to" + to_string(new_index) + ".txt could not be opened for writing!" << endl;
                        exit(1);
                    }
                    graph->files.push_back(move(out_edge));
                    writeArrayToFile2D(graph, graph->files.size() - 1, curr_diagram, new_diagram);
                }

                graph->add_vertex(new_index, cur_index);
                floor_map.insert({new_diagram, new_index});
                flore_list.push_back(new_diagram);
            } else {
                int sticked_index = floor_map.at(new_diagram);
                if(graph->files.size() < FILE_NUM)
                {
                    ofstream out_edge(folder_name + "/" + to_string(cur_index) + "to" + to_string(sticked_index) + ".txt");
                    if (!out_edge)
                    {
                        // то выводим сообщение об ошибке и выполняем функцию exit()
                        cerr << "Uh oh, " + to_string(cur_index) + "to" + to_string(sticked_index) + ".txt could not be opened for writing!" << endl;
                        exit(1);
                    }
                    graph->files.push_back(move(out_edge));
                    writeArrayToFile2D(graph,  graph->files.size() - 1, curr_diagram, new_diagram);
                }

                graph->vertexList.at(cur_index).add_child(sticked_index, START_WEIGHT);
                graph->vertexList.at(sticked_index).add_parent(cur_index, START_WEIGHT);
            }
            new_diagram.clear();

            for (int i = 0; i < curr_diagram.size(); i++) // По столбцам диаграммы
            {
                int cur_elem = curr_diagram.at(i);
                // Для двустрочных и трехстрочных диаграмм ограничиваем количество строк
                if (graph_name == "2D-2str")
                {
                    if (cur_elem == 2) { continue; }
                }
                if (graph_name == "2D-3str")
                {
                    if (cur_elem == 3) { continue; }
                }
                if (i > 0) {
                    if (graph_name == "Pascal")
                    {
                        break;
                    }
                    if (curr_diagram.at(i - 1) == cur_elem) { continue; }
                }
                vector<int> new_diagram(curr_diagram.begin(), curr_diagram.end());
                new_diagram.at(i) = cur_elem + 1;
                if (floor_map.count(new_diagram) == 0) {
                    // Добавляем к столбцу диаграммы квадрат, если можно
                    new_index++;
                    if(graph->files.size() < FILE_NUM)
                    {
                        ofstream out_edge(folder_name + "/" + to_string(cur_index) + "to" + to_string(new_index) + ".txt");
                        if (!out_edge)
                        {
                            // то выводим сообщение об ошибке и выполняем функцию exit()
                            cerr << "Uh oh, " + to_string(cur_index) + "to" + to_string(new_index) + ".txt could not be opened for writing!" << endl;
                            exit(1);
                        }
                        graph->files.push_back(move(out_edge));
                        writeArrayToFile2D(graph, graph->files.size() - 1, curr_diagram, new_diagram);
                    }

                    graph->add_vertex(new_index, cur_index);
                    floor_map.insert({new_diagram, new_index});
                    flore_list.push_back(new_diagram);
                } else {
                    int sticked_index = floor_map.at(new_diagram);
                    if(graph->files.size() < FILE_NUM)
                    {
                        ofstream out_edge(folder_name + "/" + to_string(cur_index) + "to" + to_string(sticked_index) + ".txt");
                        if (!out_edge)
                        {
                            // то выводим сообщение об ошибке и выполняем функцию exit()
                            cerr << "Uh oh, " + to_string(cur_index) + "to" + to_string(sticked_index) + ".txt could not be opened for writing!" << endl;
                            exit(1);
                        }
                        graph->files.push_back(move(out_edge));
                        writeArrayToFile2D(graph, graph->files.size() - 1, curr_diagram, new_diagram);
                    }

                    graph->vertexList.at(cur_index).add_child(sticked_index, START_WEIGHT);
                    graph->vertexList.at(sticked_index).add_parent(cur_index, START_WEIGHT);
                }
                new_diagram.clear();
            }
            cur_index++;
        }
        floor_map.clear();

        // Для стока в абстрактную вершину
        if ((level == graph->new_level - 1) & (ABSTRACT_STOCK)) {
            new_index++;
            cout << "last index " << new_index << " of the level " << level + 1 << endl;
            int start_idx = index_prev_level + 1;
            graph->add_vertex(new_index, start_idx);
            graph->vertexList.at(new_index).dim = 1;
            start_idx++;
            graph->hunted_index = new_index;
            for (int k = 1; k < flore_list.size(); k++) // По диаграммам на уровне
            {
                graph->vertexList.at(start_idx).dim = 1;
                graph->vertexList.at(start_idx).add_child(new_index, 1);
                graph->vertexList.at(start_idx).children_kops.at(new_index) = 1.0;
                graph->vertexList.at(new_index).add_parent(start_idx, 1);
                start_idx++;
            }
        }
        index_prev_level = new_index;
        graph->current_young_list.push_back(flore_list);
        graph->index_prev_level = index_prev_level - 1;
        flore_list.clear();
    }

    // Начальные вероятности
    for (int i = graph->index_last_level; i < graph->vertexList.size(); i++)
    {
        for (int j: graph->vertexList.at(i).children_idx)
        {
            float new_weight;
            if (!START_WEIGHT)
            {
                new_weight = 1.0/graph->vertexList.at(i).children_idx.size();
            }
            else if (START_WEIGHT == 1)
            {
                new_weight = 1.0/(rand() % 9 +1);
            }
            else new_weight = START_WEIGHT;
            graph->vertexList.at(i).children.at(j) = new_weight;
            graph->vertexList.at(j).parents.at(i) = new_weight;
            if (i < graph->cur_index)
            {
                graph->weights_last_level.push_back(new_weight);
            }
        }
    }

    graph->last_level = graph->new_level;
}


void find_kops(Graph* graph)
{
    for (int i = graph->vertexList.size() - 1; i >= 0; i--)
    {
        if (graph->vertexList.at(i).dim == 0)
        {
            for (auto& child : graph->vertexList.at(i).children_idx)
            {
                graph->vertexList.at(i).dim += graph->vertexList.at(child).dim;
            }
            for (auto& child : graph->vertexList.at(i).children_idx)
            {
                ////////////////////////////////////////////////////// DVDIONS /////////////////////////////////////////////////////////
                float dvd = graph->vertexList.at(child).dim.get_d() / graph->vertexList.at(i).dim.get_d() ;
                graph->vertexList.at(i).children_kops.at(child) = dvd;
            }
        }
    }
    int count_edges = -1;
    for (int i = 1; i < graph->cur_index; i++)
    {
        for (int j: graph->vertexList.at(i).children_idx) {
            count_edges++;
            if (count_edges < FILE_NUM)
            {
                graph->files.at(count_edges) << i << "to" << j << endl;
                graph->files.at(count_edges) << graph->vertexList.at(i).children_kops.at(j) << endl;
            }
            else {break;}
        }
    }
}


void find_rombs(Graph* graph)
{
    for (int i = graph->romb_start_index; i < graph->vertexList.size(); i++)
    {
        vector<int> level0Children = graph->vertexList.at(i).children_idx; // Дети рассматриваемой вершины
        vector<vector<int> > level1Children; // Массив детей для каждого ребенка
        for (auto& idx : level0Children)
        {
            vector<int> level1ChildrenIdx = graph->vertexList.at(idx).children_idx;
            level1Children.push_back(level1ChildrenIdx);
        }

        // Поиск ромбов
        for (int k = 0; k < level0Children.size(); k++)
        {
            vector<vector<int>> rombs;
            for (auto& currentChild : level1Children.at(k))
            {
                for (int j = k+1; j < level1Children.size(); j++)
                {
                    for (auto& idx : level1Children.at(j))
                    {
                        // У родителей общий ребенок
                        if (currentChild == idx)
                        {
                            if (level0Children.at(k) < level0Children.at(j))
                            {

                                vector<int> romb;
                                romb.push_back(i);
                                romb.push_back(level0Children.at(k));
                                romb.push_back(currentChild);
                                romb.push_back(level0Children.at(j));
                                rombs.push_back(romb);
                            }
                            else
                            {
                                vector<int> romb;
                                romb.push_back(i);
                                romb.push_back(level0Children.at(j));
                                romb.push_back(currentChild);
                                romb.push_back(level0Children.at(k));
                                rombs.push_back(romb);
                            }
                            break;
                        }
                    }
                }
            }
            if (!rombs.empty())
            {
                graph->rombsLists.push_back(rombs);
                rombs.clear();
            }
        }
    }
}

int central_normal(Graph* graph)
{
    // Пройдемся по всем ромбам
    vector<vector<vector<int>>> rombs_list_maybe_rebersed(graph->rombsLists.begin(), graph->rombsLists.end());
    if (fast_result)
    {
        reverse(rombs_list_maybe_rebersed.begin(), rombs_list_maybe_rebersed.end());
    }
    for (const vector<vector<int>> &rombs: rombs_list_maybe_rebersed) {
        int pick;
        float pickSum = 0;

        for (const vector<int> &romb: rombs) {
            // Центрирование
            pick = romb.at(0);

            // TODO here can be log
            float p3 = graph->vertexList.at(romb.at(0)).children.at(romb.at(1)) *
                       (graph->vertexList.at(romb.at(1)).children.at(romb.at(2)) /
                        graph->vertexList.at(romb.at(2)).parents.at(romb.at(3)));
            graph->vertexList.at(romb.at(0)).children.at(romb.at(3)) = p3;
            graph->vertexList.at(romb.at(3)).parents.at(romb.at(0)) = p3;
        }
        // Нормирование
        for (int oval = 0; oval < graph->vertexList.at(pick).children_idx.size(); oval++) {
            pickSum += graph->vertexList.at(pick).children.at(graph->vertexList.at(pick).children_idx.at(oval));

        }
        for (int j = 0; j < graph->vertexList.at(pick).children_idx.size(); j++) {
            int childIdx = graph->vertexList.at(pick).children_idx.at(j);
            float normal = graph->vertexList.at(pick).children.at(childIdx) / pickSum;
            graph->vertexList.at(pick).children.at(childIdx) = normal;
            graph->vertexList.at(childIdx).parents.at(pick) = normal;
        }
    }

    // Выводим информацию по ребру в файл
    int count_edges = -1;
    for (int i = 1; i < graph->cur_index; i++)
    {
        for (int j: graph->vertexList.at(i).children_idx) {
            graph->weights_this_level.push_back(graph->vertexList.at(i).children.at(j));
            ////////////////////////////////////////// Вывод для каждого этажа ///////////////////////////////////////////
            count_edges++;
            if (count_edges < FILE_NUM)
            {
                graph->files.at(count_edges) << graph->vertexList.at(i).children.at(j) << endl;
            }
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }
    }
    float diff = 0;
    count_edges = -1;
    for (int i = 0; i < graph->weights_last_level.size(); i++) {
        // diff += abs(weights_last_level.at(i) - weights_this_level.at(i)); // Для критерия рм
        diff += (graph->weights_last_level.at(i) - graph->weights_this_level.at(i)) *
                (graph->weights_last_level.at(i) - graph->weights_this_level.at(i));
    }
    diff = sqrt(diff / graph->weights_this_level.size()); // Для критерия ск
    if (diff < E)
    {
        for (int i = 1; i < graph->cur_index; i++)
        {
            for (int j: graph->vertexList.at(i).children_idx) {
                count_edges++;
                if (count_edges < FILE_NUM)
                {
                    // graph->files.at(count_edges) << graph->vertexList.at(i).children.at(j) << endl;
                    graph->files.at(count_edges) << i << " to " << j << " for levels " << graph->last_level << endl;
                }
                else {break;}
            }
        }
        graph->weights_last_level = graph->weights_this_level;
        graph->weights_this_level.clear();
        outfile << diff << endl;
        outf << graph->vertexList.at(VER1).children.at(VER2) << endl;
        return 6;
    }
    graph->weights_last_level = graph->weights_this_level;
    graph->weights_this_level.clear();

    outfile << diff << endl;
    outf << graph->vertexList.at(VER1).children.at(VER2) << endl;

    return 7;
}

void delete_last(Graph* graph)
{
    int count = 0;
    for (int i = graph->rombsLists.size() - 1; i >= 0; i--)
    {
        if (graph->rombsLists.at(i).at(0).at(2) == graph->hunted_index)
        {
            count++;
            graph->romb_start_index = graph->rombsLists.at(i).at(0).at(0);
        }
        else break;
    }
    for (int i = 0; i < count; i++)
    {
        graph->rombsLists.pop_back();
    }
    for (int i: graph->vertexList.at(graph->hunted_index).parents_idx)
    {
        graph->vertexList.at(i).children.erase(graph->hunted_index);
        graph->vertexList.at(i).children_idx.pop_back();
    }
    graph->vertexList.at(graph->hunted_index).parents_idx.clear();
    graph->vertexList.at(graph->hunted_index).parents.clear();
    graph->vertexList.pop_back();
}

// Вводим граф Юнга из файла

//    ifstream in("hello.txt");
//    if (!in)
//    {
//        cerr << "Uh oh, hello.txt could not be opened for reading!" << endl;
//        exit(1);
//    }
//    string line;
//    getline(in, line);
//    int n = stoi(line); // Количество циклов центрирования
//    getline(in, line);
//    int lastPick = stoi(line); // Последняя рассматриваемая вершина
//
//    for (int i = 0; i < lastPick + 1; i++)
//    {
//        vertexList.push_back(Vertex(i));
//    }
//
//    while (getline(in, line))
//    {
//        stringstream ss(line);
//        string substring;
//
//        getline(ss, substring, ' '); // Индекс вершины-родителя
//        int firstPick = stoi(substring);
//        getline(ss, substring, ' '); // Индекс вершины-ребенка
//        int secondPick = stoi(substring);
//        getline(ss, substring, ' '); // Вероятность перехода
//        float weight = stof(substring);
//
//        vertexList[firstPick].add_child(secondPick, weight);
//        vertexList[secondPick].add_parent(firstPick, weight);
//    }


void writeArrayToFile(Graph* graph, int new_index, vector<vector<int>> cur_diag,  vector<vector<int>> new_diag) {
    // Проходим по массивам и записываем его элементы в файл
    int counter = 0;
    for (int i = 0; i < cur_diag.size()-1; i++) {
        graph->files.at(new_index) << "{";
        for (int j = 0; j < cur_diag[i].size(); j++) {
            counter++;
            graph->files.at(new_index) << cur_diag[i][j];
            if (counter < cur_diag[i].size()) graph->files.at(new_index) << ',';
            else graph->files.at(new_index) << '}';
        }
        counter = 0;
    }

    graph->files.at(new_index)  << endl;

    for (int i = 0; i < new_diag.size()-1 ; i++) {
        graph->files.at(new_index) << "{";
        for (int j = 0; j < new_diag[i].size(); j++) {
            counter++;
            graph->files.at(new_index) << new_diag[i][j];
            if (counter < new_diag[i].size()) graph->files.at(new_index) << ',';
            else graph->files.at(new_index) << '}';
        }
    }

    graph->files.at(new_index) << endl;
}

void writeArrayToFile2D(Graph* graph, int new_index, vector<int> cur_diag,  vector<int> new_diag) {
    // Проходим по массивам и записываем его элементы в файл
    int counter = 0;
    graph->files.at(new_index) << "{";
    for (int i = 0; i < cur_diag.size(); i++) {
        counter++;
        graph->files.at(new_index) << cur_diag[i];
        if (counter < cur_diag.size()) graph->files.at(new_index) << ',';
        else graph->files.at(new_index) << '}';
    }

    counter = 0;
    graph->files.at(new_index) << endl;

    graph->files.at(new_index) << "{";
    for (int i = 0; i < new_diag.size(); i++) {
        counter++;
        graph->files.at(new_index) << new_diag[i];
        if (counter < new_diag.size()) graph->files.at(new_index) << ',';
        else graph->files.at(new_index) << '}';
    }

    graph->files.at(new_index) << endl;
}