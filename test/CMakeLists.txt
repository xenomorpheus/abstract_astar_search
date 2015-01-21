# test ai_search library
add_executable(test_ai_search test_ai_search.c)

# Adding math lib for UNIX
#if(UNIX)
target_link_libraries(test_ai_search ai_search m logging bstring)
#else()
target_link_libraries(test_ai_search ai_search logging bstring)
#endif(UNIX)

# Demo Grid ai_search library
add_executable(test_ai_search_demo_grid test_ai_search_demo_grid.c)

# Adding math lib for UNIX
#if(UNIX)
target_link_libraries(test_ai_search_demo_grid ai_search m logging bstring)
#else()
target_link_libraries(test_ai_search_demo_grid ai_search logging bstring)
#endif(UNIX)

# Demo Graph ai_search library
add_executable(test_ai_search_demo_graph test_ai_search_demo_graph.c)

# Adding math lib for UNIX
#if(UNIX)
target_link_libraries(test_ai_search_demo_graph ai_search m logging bstring)
#else()
target_link_libraries(test_ai_search_demo_graph ai_search logging bstring)
#endif(UNIX)
