# PACE2023

This is a heuristic solver for computing the twin-width of a graph, used in the Eight Edition of Parameterized Algorithms and Computational Experiments.

# Required

C++11 or higher

# How to run

Compile and run the main.cpp file.

# Algorithm Overview

All the heuristic algorithms in our solution are based on the same idea : find the next contraction that results in the best outcome the smallest red degree created.

The problem is that we cannot test all the N^2 pairs of nodes at each step. So we have to only test some of these pairs at a step.

The main algorithm is an adaptive algorithm that always tries to compute the contraction sequence in 150 seconds.
 * The nodes are kept in a set sorted in ascending order by ALPHA * red_degree + BETA * black_degree.
 * The ALPHA and BETA values are first found by trying some values on the first N/10 steps of the contraction sequence.
 * The best values are usually ALPHA = 2.0 and BETA = 0.5 or ALPHA = 2.0 and BETA = 0.0001.
 * Now we take the first X nodes from the set and try to find the best pair. The algorithm is adaptive because it changes the X based on the current time. If it is estimated that it will take more than 150 second X gets smaller, otherwise bigger.

We also have other algorithms for special cases of the graphs. An important one is to create the contraction sequence based on the BFS tree of the graph starting from the node with the highest starting degree. This works great on graphs with a small average starting degree (m / n).

# Authors

  * Alexandru Enache
  * Adrian Miclaus
  * Lucian Trepteanu
  * Alexandru Popa
