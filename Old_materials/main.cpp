
// Для успешной компиляции, необходимо использовать стандарт С++11

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <time.h>
#include <map>


// #include <bits/stdc++.h>

using namespace std;
int LEVEL = 44;
int TIMES = 50;
int VER1 = 2;
int VER2 = 5;
int HUNTED = 7;
bool ABSTRACT_STOCK = 1;
float E = 0.00001;
float START_WEIGT = 0;

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

// vector<int> find_rombs(vector<int> floor_parents);
// bool if_not_exists_diagram(vector<int> new_diagram, vector<vector<int>> flore_list, int cur_index, int index_prev_level);
void add_vertex(int new_index, int cur_index);

vector<Vertex> vertexList;

int main()
{
    // Открываем файл для записи результатов
    ofstream outf(to_string(VER1) + "to" + to_string(VER2) + ".txt");
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

    // Заполняем граф Юнга вершинами и ребрами до нужного этажа
    // cout << 1 << endl;
    vector<vector<vector<int> > > young_list;
    vector<vector<int> > prev_flore_list;
    vector<vector<int> > flore_list;
    map<vector<int>, int> floor_map;
    vector<int> curr_diagram;

    vector<vector<int> > v2;
    vector<int> v1;
    v1.push_back(0); //v1.push_back(2); // Для двустрочных диаграмм
    v2.push_back(v1);

    young_list.push_back(v2);
    v2[0][0] = 1; // v2.push_back(1); // Для двустрочных диаграмм
    young_list.push_back(v2);

    vertexList.push_back(Vertex(0));
    int index_prev_level = 1;
    int cur_index = 1;
    int new_index = 1;
    int hunted_index = 1;
    int start_idx = 1;
    vertexList.push_back(Vertex(cur_index));

    for (int level = 1; level < LEVEL; level++) // По уровням
    {
        // cout << "1 " << young_list.at(level).size() << endl;
        prev_flore_list = young_list.at(level);
        for (int k = 0; k < prev_flore_list.size(); k++) // По диаграммам на уровне
        {
            // cout << "2 " << prev_flore_list.at(k).at(0) << endl;
            curr_diagram = prev_flore_list.at(k);
            vector<int> new_diagram(curr_diagram.begin(), curr_diagram.end());
            new_diagram.push_back(1);
            if (floor_map.count(new_diagram) == 0)
            {
                new_index++;
                add_vertex(new_index, cur_index);
                floor_map[new_diagram] = new_index;
                flore_list.push_back(new_diagram);
                // cout << "index " << new_index << endl;
            }
            else
            {
                int sticked_index = floor_map.at(new_diagram);
                vertexList.at(cur_index).add_child(sticked_index, START_WEIGT);
                vertexList.at(sticked_index).add_parent(cur_index, START_WEIGT);
            }

            for (int i = 0; i < curr_diagram.size(); i++) // По столбцам диаграммы
            {
                // cout << "3 " << curr_diagram.at(i) << endl;
                int cur_elem = curr_diagram.at(i);
                // if (cur_elem == 2) { continue; } // Для двустрочных диаграмм
                if (i > 0)
                {
                    if (curr_diagram.at(i - 1) == cur_elem) { continue; }
                }
                vector<int> new_diagram(curr_diagram.begin(), curr_diagram.end());
                new_diagram.at(i) = cur_elem + 1;
                if (floor_map.count(new_diagram) == 0)
                {
                    new_index++;
                    add_vertex(new_index, cur_index);
                    floor_map[new_diagram] = new_index;
                    flore_list.push_back(new_diagram);
                    // cout << "new_index " << new_index << endl;
                }
                else
                {
                    int sticked_index = floor_map.at(new_diagram);
                    vertexList.at(cur_index).add_child(sticked_index, START_WEIGT);
                    vertexList.at(sticked_index).add_parent(cur_index, START_WEIGT);
                }
            }
            cur_index++;
        }
        young_list.push_back(flore_list);
        floor_map.clear();
        // Для стока в вершину на последнем уровне
        if (level == LEVEL - 1)
        {
            hunted_index = index_prev_level + HUNTED;
        }

        // Для стока в абстрактную вершину
        if ((level == LEVEL - 1) & (ABSTRACT_STOCK))
        {
            new_index++;
            // cout << "last index " << new_index << endl;
            start_idx = index_prev_level + 1;
            add_vertex(new_index, start_idx);
            start_idx++;
            hunted_index = new_index;
            for (int k = 1; k < flore_list.size(); k++) // По диаграммам на уровне
            {
                vertexList.at(start_idx).add_child(new_index, START_WEIGT);
                vertexList.at(new_index).add_parent(start_idx, START_WEIGT);
                start_idx++;
            }
        }
        index_prev_level = new_index;
        flore_list.clear();
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

    // Начальные вероятности по Ричардсону
    if (!START_WEIGT)
    {
        cout << "added " << START_WEIGT << endl;
        for (int i = 1; i < vertexList.size(); i++)
        {
            for (int j: vertexList.at(i).children_idx)
            {
                vertexList.at(i).children.at(j) = 1/vertexList.at(i).children_idx.size();
                vertexList.at(j).parents.at(i) = 1/vertexList.at(i).children_idx.size();
            }
        }
    }

    // Определим подграф и переопределим детей
    if (!ABSTRACT_STOCK)
    {
        vector<int> neededVertexList;
        vector<int> cur_parents;
        unordered_map<int, int> floor_parents;
        vector<int> level_parents;
        neededVertexList.push_back(hunted_index);
        cur_parents.push_back(hunted_index);
        while (cur_parents.at(0) != 1)
        {
            for (int root_vertex: cur_parents)
            {
                for (int cur_vertex: vertexList.at(root_vertex).parents_idx)
                {
                    if (floor_parents.count(cur_vertex) == 0)
                    {
                        vertexList[cur_vertex].clear_children();
                        floor_parents[cur_vertex] = cur_vertex;
                        level_parents.push_back(cur_vertex);
                        neededVertexList.push_back(cur_vertex);
                    }
                    // cout << "added " << cur_vertex << " path " << root_vertex << endl;
                    vertexList[cur_vertex].add_child(root_vertex, START_WEIGT);
                }
            }
            cur_parents = level_parents;
            floor_parents.clear();
            level_parents.clear();
        }
        sort(neededVertexList.begin(), neededVertexList.end());
    }

    clock_t start = clock();
    // Ищем ромбы
    vector<vector<vector<int> > > rombsLists; // Вектор ромбов, образующихся для вершин-родителей
    // for (int i: neededVertexList)         ////// Для стока в впу
    for (int i = 1; i < vertexList.size(); i++)
    {
        vector<int> level0Children = vertexList.at(i).children_idx; // Дети рассматриваемой вершины
        vector<vector<int> > level1Children; // Массив детей для каждого ребенка
        for (auto& idx : level0Children)
        {
            vector<int> level1ChildrenIdx = vertexList.at(idx).children_idx;
            level1Children.push_back(level1ChildrenIdx);
        }

        // Поиск ромбов
        for (int k = 0; k < level0Children.size(); k++)
        {
            // Переопределим вероятности

            int sisse = level0Children.size();
            float pk = 1.0 / sisse;
            // cout << i << " new_index " << level0Children.at(k) << " " << pk << endl;
            vertexList.at(i).children.at(level0Children.at(k)) = pk;
            vertexList.at(level0Children.at(k)).parents.at(i) = pk;
            //
            vector<vector<int> > rombs;
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
            if (rombs.size())
            {
                rombsLists.push_back(rombs);
                rombs.clear();
            }

        }
    }

    vector <float> weights_last_level;
    vector <float> weights_this_level;
    for (int i = 1; i < vertexList.size(); i++)
    {
        for (int j: vertexList.at(i).children_idx)
        {
            weights_last_level.push_back(START_WEIGT);
        }
    }
    for (int times = 0; times < TIMES; times++)
    {
        // Пройдемся по всем ромбам
        for (const vector<vector<int> > & rombs : rombsLists)
        {
            int pick;
            float pickSum = 0;

//            for (const vector<int>& romb : rombs)
//            {
//                for (int value : romb) { cout << value << " "; }
//                cout << endl;
//            }

            for (const vector<int>& romb : rombs)
            {
                // Центрирование
                pick = romb.at(0);
                // cout << pick << " to " << romb.at(1) << " weight " << vertexList.at(romb.at(0)).children.at(romb.at(1)) << endl;
                // cout << pick << " to " << romb.at(3) << " weight " << vertexList.at(romb.at(0)).children.at(romb.at(3)) << endl;
                // cout << romb.at(1) << " to " << romb.at(3) << " weight " << vertexList.at(romb.at(1)).children.at(romb.at(2)) << " to " << vertexList.at(romb.at(2)).parents.at(romb.at(3)) << endl;

                float p3 = (vertexList.at(romb.at(0)).children.at(romb.at(1)) *
                            vertexList.at(romb.at(1)).children.at(romb.at(2))) /
                           vertexList.at(romb.at(2)).parents.at(romb.at(3));
                vertexList.at(romb.at(0)).children.at(romb.at(3)) = p3;
                vertexList.at(romb.at(3)).parents.at(romb.at(0)) = p3;
                // cout << pick << " to " << romb.at(3) << " central_weight " << p3 << endl;
            }
            // Нормирование
            for (int oval = 0; oval < vertexList.at(pick).children_idx.size(); oval++)
            {
                pickSum += vertexList.at(pick).children.at(vertexList.at(pick).children_idx.at(oval));
                // cout << pick << " to " << vertexList.at(pick).children_idx.at(oval) << " sum " << pickSum << endl;

            }
            for (int j = 0; j < vertexList.at(pick).children_idx.size(); j++)
            {
                int childIdx = vertexList.at(pick).children_idx.at(j);
                float normal = vertexList.at(pick).children.at(childIdx) / pickSum;
                vertexList.at(pick).children.at(childIdx) = normal;
                vertexList.at(childIdx).parents.at(pick) = normal;
                // cout << pick << " to " << vertexList.at(pick).children_idx.at(j) << " normal " << normal << endl;
            }
        }
        // cout << endl;

        // Выводим информацию по ребру в файл
        for (int i = 1; i < vertexList.size(); i++) // for (int i: neededVertexList) // Для стока в впу
        {
            for (int j: vertexList.at(i).children_idx)
            {
                weights_this_level.push_back(vertexList.at(i).children.at(j));
            }
        }
        float diff = 0;
        for (int i = 0; i < weights_last_level.size(); i++)
        {
            // cout << "    diffchick    " << diff << endl;
            // diff += abs(weights_last_level.at(i) - weights_this_level.at(i)); // Для критерия рм
            diff += (weights_last_level.at(i) - weights_this_level.at(i))*(weights_last_level.at(i) - weights_this_level.at(i));
        }
        diff = sqrt(diff/weights_this_level.size()); // Для критерия ск
        weights_last_level = weights_this_level;
        weights_this_level.clear();
        cout << "Times " << times << endl;

        // Для стока в впу
        // if ((vertexList.at(VER1).children.count(VER2) != 0) & (find(neededVertexList.begin(), neededVertexList.end(), VER2) != neededVertexList.end()))
        if ((vertexList.at(VER1).children.count(VER2) != 0) &
            (VER2 != vertexList.size() - 1))
        {
            outf << vertexList.at(VER1).children.at(VER2) << endl;
            // cout << VER1 << " to " << VER2 << " = " << vertexList.at(VER1).children.at(VER2) << endl;
            outfile << diff << endl;
            // cout << "Diff " << diff << endl;
            // cout << endl;
        }
        else
        {
            outf << endl;
            outf << "В подграфе нет такого ребра" << endl;
            // cout << "В подграфе нет такого ребра" << endl;
            // cout << endl;
            return 5;
        }

        if (diff < E)
        {
            cout << "Times " << times << endl;
            weights_last_level.clear();
            clock_t end = clock();
            double result = (double)(end - start)/CLOCKS_PER_SEC;
            cout << "ff " << result << endl;
            return 7;
        }
    }

    outf.close();
    outfile.close();


    return 9;
}

void add_vertex(int new_index, int cur_index)
{
    vertexList.push_back(Vertex(new_index));
    vertexList.at(cur_index).add_child(new_index, START_WEIGT);
    vertexList.at(new_index).add_parent(cur_index, START_WEIGT);
}


// OUT OF ORDER

//
//vector<int> find_rombs(vector<int> floor_parents)
//{
//    unordered_map<int,int> next_floor_parents;
//    unordered_map<int,vector<int>> romb;
//    vector<unordered_map<int,vector<int>>> rombs;
//    for (auto& cur_vertex : floor_parents) // Проходимся по всем корневым вершинам (нижняя вершина ромба)
//    {
//        cur_parents = vertexList.at(cur_vertex).parents_idx;
//        for (int i = 0; i < cur_parents.size(); i++) // Проходимся по всем родителям рассматриваемой вершины (боковая вершина ромба)
//        {
//            for (auto& idx1 : vertexList.at(cur_parents.at(i)).parents_idx)
//            {
//                if (next_floor_parents.count(idx1) == 0)
//                {
//                    next_floor_parents.insert({idx1, idx1})
//                }
//                if (i != cur_parents.size() - 1)
//                {
//                    for (int j = i+1; i < cur_parents.size(); i++) // Проходимся по всем родителям после рассматриваемого родителя данной вершины (боковая вершина ромба)
//                    {
//                        for (auto& idx2 : vertexList.at(cur_parents.at(j)).parents_idx)
//                        {
//                            if (idx1 == idx2) // Нашли верхнюю вершину ромба
//                            {
//                                if (cur_parents.at(i) < level0Children.at(j))
//                                {
//                                    vector<int> romb = {idx1, cur_parents.at(i), cur_vertex, cur_parents.at(j)};
//                                }
//                                else
//                                {
//                                    vector<int> romb = {idx1, cur_parents.at(j), cur_vertex, cur_parents.at(i)};
//                                }
//                                vector<vector<int>> indexed_rombs;
//                                if (rombs.count(idx1) == 0)
//                                {
//                                    indexed_rombs.push_back(romb)
//                                    rombs.insert({idx1: indexed_rombs})
//                                }
//                                else
//                                {
//                                    vector<int> romb
//                                }
//
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//    for ()
//}


// OUT OF USAGE

// bool if_not_exists_diagram(vector<int> new_diagram, vector<vector<int>> flore_list, int cur_index, int index_prev_level)
// {
//     if (new_diagram.size() == 1)
//     {
//         return true;
//     }
//     for (int d = 0; d < flore_list.size(); d++)
//     {
//         // cout << "d " << flore_list.size() << endl;
//         int count = 0;
//         if (flore_list.at(d).size() == new_diagram.size())
//         {
//             // cout << "dd " << new_diagram.size() << endl;
//             for (int m = 0; m < new_diagram.size(); m++)
//             {
//                 // cout << "m " << new_diagram.size() << endl;
//                 if (flore_list.at(d).at(m) != new_diagram.at(m))
//                 {
//                     break;
//                 }
//                 else { count++; }
//             }
//             if (count == new_diagram.size())
//             {
//                 // cout << "sticked " << new_diagram.size() << endl;
//                 vertexList.at(cur_index).add_child(index_prev_level + d + 1, START_WEIGT);
//                 vertexList.at(index_prev_level + d + 1).add_parent(cur_index, START_WEIGT);
//                 return false;
//             }
//         }
//     }
//     return true;
// }