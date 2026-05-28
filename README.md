# Asymptotic Combinatorics

## Finding of Central Measure on an Oriented Graph

This is only the row materials of the project. Access to the full research documentation can be granted if you send a request by an email.

### all_in_one_pro.cpp

Generate graph, run an algorithm of finding central measure --> get transition probabilities of it's edges

##### Graphs
- Pascal graph
- Young graph
- The graph of two-row Young diagrams
- The graph of three-row Young diagrams
- The graph of three-dimensional Young diagrams (3D Young Graph)

##### Algorithms
- The Algorithm of Dual Paths (iterative)
- The Algorithm of Kops (edges orientation changing)

##### Arguments

| Argument | Usage |
| --- | --- |
| graph_name | Choose which graph would be generated |
| algorithm | Choose which graph would be used |
| E | Choose the allowed mistake of the algorithm |
| LEVEL | Choose how many levels of the graph would be generated |
| ABSTRACT_STOCK | Choose will the graph be ended with its last generated level or all of the vertexes of the last level would be connected with abstract one on the next level |
| START_WEIGHT | Choose start transition probabilities of the edges |
| FILE_NUM | Choose the amount of edges to get their transition probabilities |




HISTORICAL PROJECT INFO
### main.cpp
Граф Юнга с заданным количеством этажей и стоком, для всех вершин с предпоследнего уровня итеративно центрируется и нормируется с помощью алгоритма ромбов до получения центральной меры

Параметры:

int LEVEL -- этаж, до которого будет построен и обработан граф Юнга

int TIMES -- количество итераций приближения (для отладки 1, иначе n > LEVEL)

int VER1 -- индекс первой вершины, ребра, по которому будет выводиться вероятность перехода (индексация вершин с единицы)

int VER2 -- индекс второй вершины ребра

bool ABSTRACT_STOCK -- 1 для стока всех вершин этажа LEVEL в абстрактную вершину на уровне LEVEL + 1, иначе 0

float E -- 0.00001 параметр остановки итерационного приближения (Критерий остановки: среднее квадратическое последней и предпоследней итерации < E)

float START_WEIGT -- начальный вес ребер в графе


Не используется:
int HUNTED -- номер вершины на последнем уровне, в которую будет производиться сток (с дальнейшим переходом к подграфу) 


Алгоритм выводит 2 файла:
1) Вероятность перехода по запрашиваемому ребру в зависимости от итерации
2) СКО в зависимости от итерации

### main_endless.cpp
Граф Юнга с заданным количеством этажей и стоком для всех вершин с предпоследнего уровня итеративно центрируется и нормируется с помощью алгоритма ромбов до получения центральной меры, далее заданное количество раз происходит следующий процесс: удаляется сток и граф Юнга дополняется заданным количеством этажей со стоком, полученный граф итеративно центрируется и нормируется пока не будет получено достаточное приближение к центарльной мере

Параметры:

int LEVEL -- этаж, до которого будет построен и обработан граф Юнга

int FLORES -- количество этажей, которые будут добавляться к изначальному графу на каждом шагу TASK

int LEVEL -- количество итераций добавления этажей к текущему графу Юнга

int TIMES -- количество итераций приближения вероятностной меры к центральной (для отладки -- 1, иначе -- n > LEVEL + FLORES * TASK)

int VER1 -- индекс первой вершины, ребра, по которому будет выводиться вероятность перехода (индексация вершин с единицы)

int VER2 -- индекс второй вершины ребра

bool ABSTRACT_STOCK -- 1 для стока всех вершин этажа LEVEL в абстрактную вершину на уровне LEVEL + 1, иначе 0

float E -- 0.00001 параметр остановки итерационного приближения (Критерий остановки: среднее квадратическое последней и предпоследней итерации < E)

float START_WEIGT -- начальный вес ребер в графе (задать начальную вероятность по Ричардсону -- 0, рандомно на отрезке (0, 1) с точностью до десятых -- 1)


Не используется:
int HUNTED -- номер вершины на последнем уровне, в которую будет производиться сток (с дальнейшим переходом к подграфу) 


Алгоритм выводит 2 файла:
1) Вероятность перехода по запрашиваемому ребру в зависимости от итерации (учитываются итерации из каждого задания TASK)
2) СКО в зависимости от итерации (учитываются итерации из каждого задания TASK)
