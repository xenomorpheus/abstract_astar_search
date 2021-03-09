A* Search written in C.
=============================

  [![Build Status](https://travis-ci.org/xenomorpheus/abstract_astar_search_c.svg?branch=master)](https://travis-ci.org/xenomorpheus/abstract_astar_search_c)

This A* Search is written in an abstract manner, that is independant of
the implementation of the ModelState etc.

To prove this A* Search is independant of implementation, two different
implementations are provided:

1) Graph A* Search:
test/test_ai_search_demo_graph.c

2) Grid A* Search:
test/test_ai_search_demo_grid.c

This is a CMake project with unit tests.

