
// Для успешной компиляции, необходимо использовать стандарт С++11

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <time.h>
#include <map>


using namespace std;
int LEVEL = 24;
int FLORES = 1;
int TASK = 1;
int TIMES = 200;
int VER1 = 65;
int VER2 = 134;
int HUNTED = 7;
bool ABSTRACT_STOCK = 1;
float E = 0.00001;
float START_WEIGHT = 0;
int FILE_NUM = 300;

class Vertex
{
public:
    Vertex(int index) // конструктор класса
    {
        set_vertex(index);
    }
    unordered_map<int, float> parents;
    vector<int> parents_idx;
    unordered_map<int, float> children;
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
        children_idx.push_back(child_index);
    }
    void clear_children()
    {
        children.clear();
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
    vector<int> indexes;
    vector<vector<vector<vector<int>>>> current_young_list;
    int new_level = LEVEL;
    int last_level = 1;
    int index_prev_level = 1;
    int index_last_level = 1;
    int index_2D = 1;
    int index_3D = 2000;
    int cur_index = 1;
    int hunted_index = 1;
    int romb_start_index = 1;
    vector<vector<vector<int>>> rombsLists;
    vector <float> weights_last_level;
    vector <float> weights_this_level;
    vector <ofstream> files;
    void add_vertex(int new_index, int cur_index)
    {
        vertexList.push_back(Vertex(new_index));
        vertexList.at(cur_index).add_child(new_index, START_WEIGHT);
        vertexList.at(new_index).add_parent(cur_index, START_WEIGHT);
    }
};

void open_files();
void young_level(Graph* graph);
void find_rombs(Graph* graph);
int central_normal(Graph* graph);
void delete_last(Graph* graph);
void writeArrayToFile(Graph* graph, int new_index, vector<vector<int>> cur_diag,  vector<vector<int>> new_diag);

ofstream outf(to_string(VER1) + " to " + to_string(VER2) + ".txt");
ofstream outfile(to_string(VER1) + "to" + to_string(VER2) + "diff.txt");
int main()
{
//    // open_files();
//    // Открываем файл для записи результатов
//
//    if (!outf)
//    {
//        // то выводим сообщение об ошибке и выполняем функцию exit()
//        cerr << "Uh oh, SomeText.txt could not be opened for writing!" << endl;
//        exit(1);
//    }
//    outf << "Здесь будут вероятности перехода, посчитанные для ребра "
//         << VER1 << " -- " << VER2 << " при количестве этажей -- " << LEVEL << endl;
//
//    // Открываем файл для записи diff
//
//    if (!outfile)
//    {
//        // то выводим сообщение об ошибке и выполняем функцию exit()
//        cerr << "Uh oh, file could not be opened for writing!" << endl;
//        exit(1);
//    }

    Graph graph;
    graph.current_young_list.push_back({{{0},{0}}}); // .push_back({{2}}); // Для двустрочных диаграмм
    graph.current_young_list.push_back({{{1},{0}}}); // .push_back({{2,1}}); // Для двустрочных диаграмм
    graph.vertexList.push_back(Vertex(0));
    graph.indexes.push_back(0);
    graph.vertexList.push_back(Vertex(1));
    graph.indexes.push_back(1);

    // Заполняем граф Юнга вершинами и ребрами до нужного этажа
    clock_t start = clock();
    young_level(&graph);
    find_rombs(&graph);

    for (int task = 0; task < TASK; task++)
    {
        for (int times = 0; times < TIMES; times++)
        {
            // cout << "1" << endl;
            int result = central_normal(&graph);
            if (result == 6)
            {
                // cout << endl << "Times " << times << endl << endl << endl << endl;
                break;
            }
            // cout << "Times " << times << endl;
        }
        if (task == TASK - 1)
        {
            // cout << "Task " << task << endl;
            clock_t end = clock();
            double result = (double)(end - start)/CLOCKS_PER_SEC;
            // cout << "ff " << result << endl;
            outf.close();
            outfile.close();
            return 7;
        }
        delete_last(&graph);
        graph.new_level = graph.new_level + FLORES;
        young_level(&graph);
        find_rombs(&graph);

        cout << endl << "Task " << task << endl << endl << endl << endl;
    }

    outf.close();
    outfile.close();


    return 9;
}


void open_files()
{
    // Открываем файл для записи результатов
    ofstream outf(to_string(VER1) + " to " + to_string(VER2) + ".txt");
    if (!outf)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        cerr << "Uh oh, SomeText.txt could not be opened for writing!" << endl;
        exit(1);
    }
    outf << "Здесь будут вероятности перехода, посчитанные для ребра "
         << VER1 << " -- " << VER2 << " при количестве этажей -- " << LEVEL << endl;

    // Открываем файл для записи diff
    ofstream outfile(to_string(VER1) + "to" + to_string(VER2) + "diff.txt");
    if (!outfile)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        cerr << "Uh oh, file could not be opened for writing!" << endl;
        exit(1);
    }
}

void young_level(Graph* graph)
{
    vector<vector<vector<int>>> prev_flore_list;
    vector<vector<vector<int>>> flore_list;
    map<vector<vector<int>>, int> floor_map;
    vector<vector<int>> curr_diagram;

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
            // Для 3Д графа изменена вложенность
            // cout << "index 3D why " << graph->cur_index - prev_flore_list.size() + k << " level " << level << " cur second = " << curr_diagram.at(1).at(0) << endl;
            if (curr_diagram.at(curr_diagram.size() - 1).at(0) == 0)
            {
                vector<vector<int>> new_diag_3D(curr_diagram.begin(), curr_diagram.end());
                new_diag_3D[curr_diagram.size() - 1][0] = 1;
                new_diag_3D.push_back({0});
                if (floor_map.count(new_diag_3D) == 0) {
                    new_index++;
                    if(graph->files.size() < FILE_NUM)
                    {
                        graph->index_3D ++;
                        graph->indexes.push_back(graph->index_3D);
                        ofstream out_edge(to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[new_index]) + ".txt");
                        if (!out_edge)
                        {
                            // то выводим сообщение об ошибке и выполняем функцию exit()
                            cerr << "Uh oh, " + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[new_index]) + ".txt could not be opened for writing!" << endl;
                            exit(1);
                        }
                        graph->files.push_back(move(out_edge));
                        writeArrayToFile(graph, graph->files.size()-1, curr_diagram, new_diag_3D);
                    }

                    graph->add_vertex(new_index, cur_index);
                    floor_map.insert({new_diag_3D, new_index});
                    flore_list.push_back(new_diag_3D);
                    // cout << "index 3D " << new_index << " level " << level << endl;
                } else {
                    int sticked_index = floor_map.at(new_diag_3D);
                    if(graph->files.size() < FILE_NUM)
                    {
                        ofstream out_edge(to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[sticked_index]) + ".txt");
                        if (!out_edge)
                        {
                            // то выводим сообщение об ошибке и выполняем функцию exit()
                            cerr << "Uh oh, " + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[sticked_index]) + ".txt could not be opened for writing!" << endl;
                            exit(1);
                        }
                        graph->files.push_back(move(out_edge));
                        writeArrayToFile(graph, graph->files.size()-1, curr_diagram, new_diag_3D);
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
                    if (graph->files.size() < FILE_NUM) {
                        if (new_diagram[1][0] >= 1) {
                            graph->index_3D++;
                            graph->indexes.push_back(graph->index_3D);
                        } else {
                            graph->index_2D++;
                            graph->indexes.push_back(graph->index_2D);
                        }
                        ofstream out_edge(
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
                        writeArrayToFile(graph, graph->files.size()-1, curr_diagram, new_diagram);
                    }

                    graph->add_vertex(new_index, cur_index);
                    floor_map.insert({new_diagram, new_index});
                    flore_list.push_back(new_diagram);
                    // cout << "index " << new_index << " level " << level << endl;
                } else {
                    int sticked_index = floor_map.at(new_diagram);
                    if (graph->files.size() < FILE_NUM) {
                        ofstream out_edge(
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
                        writeArrayToFile(graph, graph->files.size()-1, curr_diagram, new_diagram);
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
                            ofstream out_edge(to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[new_index]) + ".txt");
                            if (!out_edge)
                            {
                                // то выводим сообщение об ошибке и выполняем функцию exit()
                                cerr << "Uh oh, " + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[new_index]) + ".txt could not be opened for writing!" << endl;
                                exit(1);
                            }

                            graph->files.push_back(move(out_edge));
                            writeArrayToFile(graph, graph->files.size()-1, curr_diagram, new_diagram);
                        }

                        graph->add_vertex(new_index, cur_index);
                        floor_map.insert({new_diagram, new_index});
                        flore_list.push_back(new_diagram);
                        // cout << "index " << new_index << " level " << level << endl;
                    } else {
                        int sticked_index = floor_map.at(new_diagram);
                        if(graph->files.size() < FILE_NUM)
                        {
                            ofstream out_edge(to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[sticked_index]) + ".txt");
                            if (!out_edge)
                            {
                                // то выводим сообщение об ошибке и выполняем функцию exit()
                                cerr << "Uh oh, " + to_string(graph->indexes[cur_index]) + "to" + to_string(graph->indexes[sticked_index]) + ".txt could not be opened for writing!" << endl;
                                exit(1);
                            }

                            graph->files.push_back(move(out_edge));
                            writeArrayToFile(graph, graph->files.size()-1, curr_diagram, new_diagram);
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

//        // Для стока в вершину на последнем уровне
//        if (level == graph.new_level - 1) {
//            graph.hunted_index = index_prev_level + HUNTED;
//        }

        // Для стока в абстрактную вершину
        if ((level == graph->new_level - 1) & (ABSTRACT_STOCK)) {
            new_index++;
            // cout << "last index " << new_index << endl;
            int start_idx = index_prev_level + 1;
            graph->add_vertex(new_index, start_idx);
            start_idx++;
            graph->hunted_index = new_index;
            for (int k = 1; k < flore_list.size(); k++) // По диаграммам на уровне
            {
                graph->vertexList.at(start_idx).add_child(new_index, START_WEIGHT);
                graph->vertexList.at(new_index).add_parent(start_idx, START_WEIGHT);
                start_idx++;
            }
        }
        index_prev_level = new_index;
        graph->current_young_list.push_back(flore_list);
        graph->index_prev_level = index_prev_level - 1;
        flore_list.clear();
    }

    // Начальные вероятности
    // cout << "added " << START_WEIGHT << endl;
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
            // cout << "New weight = " << new_weight << endl;
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

void find_rombs(Graph* graph)
{
    // for (int i: neededVertexList)         ////// Для стока в впу
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
    for (const vector<vector<int> > &rombs: graph->rombsLists) {
        int pick;
        float pickSum = 0;

        for (const vector<int> &romb: rombs) {
            // Центрирование
            pick = romb.at(0);
            // cout << pick << " to " << romb.at(1) << " weight " << vertexList.at(romb.at(0)).children.at(romb.at(1)) << endl;
            // cout << pick << " to " << romb.at(3) << " weight " << vertexList.at(romb.at(0)).children.at(romb.at(3)) << endl;
            // cout << romb.at(1) << " to " << romb.at(3) << " weight " << vertexList.at(romb.at(1)).children.at(romb.at(2)) << " to " << vertexList.at(romb.at(2)).parents.at(romb.at(3)) << endl;

            // TODO
            float p3 = graph->vertexList.at(romb.at(0)).children.at(romb.at(1)) *
                       (graph->vertexList.at(romb.at(1)).children.at(romb.at(2)) /
                        graph->vertexList.at(romb.at(2)).parents.at(romb.at(3)));
            graph->vertexList.at(romb.at(0)).children.at(romb.at(3)) = p3;
            graph->vertexList.at(romb.at(3)).parents.at(romb.at(0)) = p3;
            // cout << pick << " to " << romb.at(3) << " central_weight " << p3 << endl;
        }
        // Нормирование
        for (int oval = 0; oval < graph->vertexList.at(pick).children_idx.size(); oval++) {
            pickSum += graph->vertexList.at(pick).children.at(graph->vertexList.at(pick).children_idx.at(oval));
            // cout << pick << " to " << graph->vertexList.at(pick).children_idx.at(oval) << " sum " << pickSum << endl;

        }
        for (int j = 0; j < graph->vertexList.at(pick).children_idx.size(); j++) {
            int childIdx = graph->vertexList.at(pick).children_idx.at(j);
            float normal = graph->vertexList.at(pick).children.at(childIdx) / pickSum;
            graph->vertexList.at(pick).children.at(childIdx) = normal;
            graph->vertexList.at(childIdx).parents.at(pick) = normal;
            // cout << pick << " to " << graph->vertexList.at(pick).children_idx.at(j) << " normal " << normal << endl;
        }
    }
    // cout << endl;

    // Выводим информацию по ребру в файл
    int count_edges = -1;
    for (int i = 1; i < graph->cur_index; i++) // for (int i: neededVertexList) // Для стока в впу
    {
        for (int j: graph->vertexList.at(i).children_idx) {
            graph->weights_this_level.push_back(graph->vertexList.at(i).children.at(j));
        }
    }
    float diff = 0;
    for (int i = 0; i < graph->weights_last_level.size(); i++) {
        // diff += abs(weights_last_level.at(i) - weights_this_level.at(i)); // Для критерия рм
        diff += (graph->weights_last_level.at(i) - graph->weights_this_level.at(i)) *
                (graph->weights_last_level.at(i) - graph->weights_this_level.at(i));

    }
    diff = sqrt(diff / graph->weights_this_level.size()); // Для критерия ск
    // cout << "    diffchick    " << diff << endl;
    outfile << diff << endl;
    outf << graph->vertexList.at(VER1).children.at(VER2) << endl;
    if (diff < E)
    {
        for (int i = 1; i < graph->cur_index; i++) // for (int i: neededVertexList) // Для стока в впу
        {
            for (int j: graph->vertexList.at(i).children_idx) {
                count_edges++;
                if (count_edges < FILE_NUM)
                {
                    graph->files.at(count_edges) << i << " to " << j << endl;
                    graph->files.at(count_edges) << graph->vertexList.at(i).children.at(j) << endl;

                }
                else {break;}
            }
        }
        graph->weights_last_level = graph->weights_this_level;
        graph->weights_this_level.clear();
        return 6;
    }
    graph->weights_last_level = graph->weights_this_level;
    graph->weights_this_level.clear();


    // Для стока в впу
    // if ((vertexList.at(VER1).children.count(VER2) != 0) & (find(neededVertexList.begin(), neededVertexList.end(), VER2) != neededVertexList.end()))
//    if (VER2 < graph->vertexList.size() - 1)
//    {
//        if (graph->vertexList.at(VER1).children.count(VER2) != 0)
//        {
//            outf << graph->vertexList.at(VER1).children.at(VER2) << endl;
//            // cout << VER1 << " to " << VER2 << " = " << graph->vertexList.at(VER1).children.at(VER2) << endl;
//            outfile << diff << endl;
//            // cout << "Diff " << diff << endl;
//            // cout << endl;
//        }
//    }

//    else
//    {
//        return 5;
//    }
    return 1;
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
//    in.close();



//    // Определим подграф и переопределим детей
//    if (!ABSTRACT_STOCK)
//    {
//        vector<int> neededVertexList;
//        vector<int> cur_parents;
//        unordered_map<int, int> floor_parents;
//        vector<int> level_parents;
//        neededVertexList.push_back(hunted_index);
//        cur_parents.push_back(hunted_index);
//        while (cur_parents.at(0) != 1)
//        {
//            for (int root_vertex: cur_parents)
//            {
//                for (int cur_vertex: vertexList.at(root_vertex).parents_idx)
//                {
//                    if (floor_parents.count(cur_vertex) == 0)
//                    {
//                        vertexList[cur_vertex].clear_children();
//                        floor_parents[cur_vertex] = cur_vertex;
//                        level_parents.push_back(cur_vertex);
//                        neededVertexList.push_back(cur_vertex);
//                    }
//                    // cout << "added " << cur_vertex << " path " << root_vertex << endl;
//                    vertexList[cur_vertex].add_child(root_vertex, START_WEIGHT);
//                }
//            }
//            cur_parents = level_parents;
//            floor_parents.clear();
//            level_parents.clear();
//        }
//        sort(neededVertexList.begin(), neededVertexList.end());
//    }



void writeArrayToFile(Graph* graph, int new_index, vector<vector<int>> cur_diag,  vector<vector<int>> new_diag) {

    // Проходим по массивам и записываем его элементы в файл
    for (int i = 0; i < cur_diag.size(); i++) {
        graph->files.at(new_index) << "{";
        for (int j = 0; j < cur_diag[i].size(); j++) {
            graph->files.at(new_index) << cur_diag[i][j];
            graph->files.at(new_index) << ',';
        }
        graph->files.at(new_index) << "0}";
    }

    graph->files.at(new_index)  << endl;

    for (int i = 0; i < new_diag.size(); i++) {
        graph->files.at(new_index) << "{";
        for (int j = 0; j < new_diag[i].size(); j++) {
            graph->files.at(new_index) << new_diag[i][j];
            graph->files.at(new_index) << ',';
        }
        graph->files.at(new_index) << "0}";
    }

    graph->files.at(new_index)  << endl;
}

