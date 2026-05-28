# Asymptotic Combinatorics

## Finding of Central Measure on an Oriented Graph
1. Generate graph, run an algorithm of finding central measure --> get transition probabilities of it's edges
2. Analyse transition probabilities dataset, confirm hypothesis --> derive a formula of central mesure graph transition probabilities

---

## Research Questions
- How transition probabilities behave across increasing graph layers
- Whether transition probabilities demonstrate stable asymptotic patterns
- How probability values change as the number of edges increase on the graphs with undescribed central measures
- Whether the observed central measures for this graphs can be described by mathematical functions

---

##### Graphs
- Pascal graph
- Young graph
- The graph of two-row Young diagrams
- The graph of three-row Young diagrams -- central measures are not fully determined in literature
- The graph of three-dimensional Young diagrams (3D Young Graph) -- central measures are not fully determined in literature

---

##### Algorithms
- The Algorithm of Dual Paths (iterative)
- The Algorithm of Kops (edges orientation changing)

---

## Analysis

### Graph Generation

- Developed a C++ program to generate subgraphs of Young graph and related graded graph structures
- Automated the process of graph construction for different graph types and dimensions
- Added a feature of working with multiple graphs simmultaniously

---

### Central Measure Calculation

- Implemented algorithms to calculate central measures on generated subgraphs
- Extracted transition probabilities for graph edges depending of graph dimension
- Produced structured output files containing probability values in order of graph levels amount increasing by 1

---

### Transition Probability Analysis

- Analyzed large-scale numerical datasets produced by graph algorithms in gnuplot
- Studied how transition probabilities change as graph levels increase at the maximum server performance

---

### Asymptotic Pattern Detection

- Identified patterns in probability behavior across graph structures -- H<sup>0</sup>: approximated with hyperbolic function
- The mistake between approximated probability and theoretical one for graphs with existing formulas for transition probabilities 
- The mistake was less than 10<sup>-5</sup> so H<sup>0</sup> was confirmed

<img width="336" height="273" alt="image" src="https://github.com/user-attachments/assets/b4adf45c-24c8-40a8-99a4-07db752b12fd" />

### Data Validation

- Checked generated outputs for consistency across different algorithms and start weights
- Extremely increase the number of iterations and graph dimensions and compare results
- Checked the behaviour of different groups of edges

<img width="361" height="307" alt="image" src="https://github.com/user-attachments/assets/ca103bb3-c37b-4ede-8dbd-66b301eb4b03" />

### Deriving a formula for an undetermined in literature central measure

- The new schematic picture of the beginning of the three-row Young diagram graph was suggested
- The edges with teoretically similar weights were naimed and put on the scheme
- Graph transition probabilities were approximated with hyperbolic function
- The rational numbers, close to the approximated were chosen
- The properpty of centrality were checked on this scheme
- The formula for one group of the edges was derived

<img width="162" height="50" alt="image" src="https://github.com/user-attachments/assets/1cdf363e-5e41-47fd-94cc-0f9fceb36935" />

TODO
- Derive a formula for transition probabitities of the next two ot three group of edges
- Find a general formula for central mesure transition probabilities for the graph of three-row Young diagrams

---

## Tech Stack

- C++
- Gnuplot
- Mathcad lib
- Algorithms and data structures
- Computational and asympthotic mathematics
- Graph theory
- Large-scale numerical data analysis
- Mathematical modeling
- Data validation
- Research documentation

---

## Key Takeaways

- Generated numerical datasets for multiple oriented graded graphs
- Implemented algorithms for finding central measures
- Analyzed transition probabilities across increasing graph complexity
- Identified asymptotic behavior in probability values
- Derive a formula for transition probabitities of one group of edges for the graph of three-row Young diagrams

---

## Notes

This repository contains raw materials of the research project, including source code and generated numerical outputs.

The project reflects research work in asymptotic combinatorics, graph algorithms, computational mathematics, and large-scale numerical analysis.
