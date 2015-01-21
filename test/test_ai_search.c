#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Custom
#include <ai_search.h>
#include <minunit.h>

/*
 * Test ai_action_constructor.
 */
char * test_ai_action_constructor() {

	char *action_data = "some data";
	ai_action *action = ai_action_constructor(action_data);
	mu_assert(action != NULL, "ai_action_constructor: action NOT NULL.");
	mu_assert(action->data == action_data, "ai_action_constructor: data value.");
	return NULL;
}

/*
 * Test ai_path_constructor.
 */
char * test_ai_path_constructor() {

	char *path_data = "some data";
	ai_path *path = ai_path_constructor(path_data);
	mu_assert(path != NULL, "ai_path_constructor: path NOT NULL.");
	mu_assert(path->data == path_data, "ai_path_constructor: data value.");
	return NULL;
}

/*
 * Test _ai_path_append_action.
 */
void _ai_path_append_action(ai_path **path, ai_action *action);
char * test__ai_path_append_action() {

	ai_path *path = NULL;
	ai_action *action1 = ai_action_constructor("action_data1");
	ai_action *action2 = ai_action_constructor("action_data2");
	ai_action *action3 = ai_action_constructor("action_data3");
	// Add 1st action
	_ai_path_append_action(&path, action1);
	mu_assert(path == action1,
			"_ai_path_append_action: path start NULL, append action. path equals action1.");
	// Add 2nd action
	_ai_path_append_action(&path, action2);
	mu_assert(path == action1,
			"_ai_path_append_action: path start action1, append action2. path equals action1.");
	mu_assert(path->next == action2,
			"_ai_path_append_action: path start action1, append action2. path's next equals action2.");
	// Add 3rd action
	_ai_path_append_action(&path, action3);
	mu_assert(path == action1,
			"_ai_path_append_action: append action3. path equals action1.");
	mu_assert(path->next == action2,
			"_ai_path_append_action: append action3. path's next equals action2.");
	mu_assert(path->next->next == action3,
			"_ai_path_append_action: append action3. path's next^2 equals action3.");
	return NULL;
}

/*
 * Test _ai_path_duplicate.
 */

void *_my_data_duplicator(void *data) {
	return (void *) strdup((char *) data);
}

ai_path *_ai_path_duplicate(ai_action_data_duplicator action_data_duplicator,
		ai_path *old);
char * test__ai_path_duplicate() {

	// Setup
	ai_path *old_path = NULL;
	ai_action *action1 = ai_action_constructor("action_data1");
	ai_action *action2 = ai_action_constructor("action_data2");
	_ai_path_append_action(&old_path, action1);
	_ai_path_append_action(&old_path, action2);
	// Run
	ai_path *new_path = _ai_path_duplicate(_my_data_duplicator, old_path);
	// Test

	mu_assert(new_path, "_ai_path_duplicate: new_path not NULL");
	mu_assert(new_path != old_path, "_ai_path_duplicate: new_path not old_path");
	mu_assert(new_path->data != old_path->data,
			"_ai_path_duplicate: new and old data not the same pointer.");
	mu_assert(strcmp(new_path->data, old_path->data) == 0,
			"_ai_path_duplicate: new and old data have equivalent contents");
	// jump to next element and compare again.
	new_path = new_path->next;
	old_path = old_path->next;
	mu_assert(new_path, "_ai_path_duplicate: (2) new_path not NULL");
	mu_assert(new_path != old_path,
			"_ai_path_duplicate: (2) new_path not old_path");
	mu_assert(new_path->data != old_path->data,
			"_ai_path_duplicate: (2) new and old data not the same pointer.");
	mu_assert(strcmp(new_path->data, old_path->data) == 0,
			"_ai_path_duplicate: (2) new and old data have equivalent contents");
	// jump to next element and compare again.
	new_path = new_path->next;
	old_path = old_path->next;
	mu_assert(old_path == NULL, "_ai_path_duplicate: (3) old_path is NULL");
	mu_assert(new_path == NULL, "_ai_path_duplicate: (3) new_path is NULL");
	return NULL;
}

/*
 * Test ai_model_state_constructor.
 */
char * test_ai_model_state_constructor() {

	char *model_state_data = "some data";
	ai_model_state *model_state = ai_model_state_constructor(model_state_data);
	mu_assert(model_state != NULL,
			"ai_model_state_constructor: model_state NOT NULL.");
	mu_assert(model_state->data == model_state_data,
			"ai_model_state_constructor: data value.");
	return NULL;
}

/*
 * Test ai_successor_constructor.
 */
char * test_ai_successor_constructor() {

	// Setup
	char *model_state_data = "some data";
	ai_model_state *model_state = ai_model_state_constructor(model_state_data);
	char *action_data = "some data";
	ai_action *action = ai_action_constructor(action_data);
	float cost = 1.23;

	// Run
	ai_successor *successor = ai_successor_constructor(model_state, action,
			cost);

	// Test
	mu_assert(successor != NULL,
			"ai_successor_constructor: successor NOT NULL.");
	mu_assert(successor->model_state == model_state,
			"ai_successor_constructor: model_state value.");
	mu_assert(successor->action == action,
			"ai_successor_constructor: action value.");
// TODO float	mu_assert(successor->cost == cost, "ai_successor_constructor: cost value.");
	mu_assert(successor->next == NULL, "ai_successor_constructor: next value.");
	return NULL;
}

/*
 * Test ai_fringe_element_constructor.
 */
char * test_ai_fringe_element_constructor() {

	char *model_state_data = "model_state data";
	ai_model_state *model_state = ai_model_state_constructor(model_state_data);
	char *path_data = "path data";
	ai_path *path = ai_path_constructor(path_data);
	ai_fringe_element *fe = ai_fringe_element_constructor(model_state, path,
			1.2f, 3.4f);
	mu_assert(fe != NULL, "ai_model_state_constructor: fe NOT NULL.");
	mu_assert(fe->model_state == model_state,
			"ai_model_state_constructor: model_state.");
	mu_assert(fe->path_so_far == path, "ai_model_state_constructor: path.");
	// TODO floats
	mu_assert(fe->next == NULL, "ai_model_state_constructor: next.");
	return NULL;
}

/*
 * Test _ai_fringe_element_list_add_by_total_cost.
 */
void _ai_fringe_element_list_add_by_total_cost(
		ai_fringe_element **fringe_element_list, ai_fringe_element *node);
char * test__ai_fringe_element_list_add_by_total_cost() {

	// Setup
	ai_fringe_element *list = NULL;
	ai_fringe_element *node1 = ai_fringe_element_constructor(NULL, NULL, 0.0f,
			2.0f);
	ai_fringe_element *node2 = ai_fringe_element_constructor(NULL, NULL, 0.0f,
			1.0f);
	ai_fringe_element *node3 = ai_fringe_element_constructor(NULL, NULL, 0.0f,
			3.0f);
	mu_assert(list == NULL,
			"_ai_fringe_element_list_add_by_total_cost: setup : list NULL.");

	// Run 1
	_ai_fringe_element_list_add_by_total_cost(&list, node1);

	// Test 1
	mu_assert(list == node1,
			"_ai_fringe_element_list_add_by_total_cost: test1 : list equals node1.");
	mu_assert(list->next == NULL,
			"_ai_fringe_element_list_add_by_total_cost: test1 : list next NULL.");

	// Run 2
	_ai_fringe_element_list_add_by_total_cost(&list, node2);

	// Test 2
	mu_assert(list == node2,
			"_ai_fringe_element_list_add_by_total_cost: test2 : list equals node2.");
	mu_assert(list->next == node1,
			"_ai_fringe_element_list_add_by_total_cost: test2 : list next equals node1.");
	mu_assert(list->next->next == NULL,
			"_ai_fringe_element_list_add_by_total_cost: test2 : list next next NULL.");

	// Run 3
	_ai_fringe_element_list_add_by_total_cost(&list, node3);

	// Test 3
	mu_assert(list == node2,
			"_ai_fringe_element_list_add_by_total_cost: test3 : list equals node2.");
	mu_assert(list->next == node1,
			"_ai_fringe_element_list_add_by_total_cost: test3 : list next equals node1.");
	mu_assert(list->next->next == node3,
			"_ai_fringe_element_list_add_by_total_cost: test3 : list next next equals node3.");
	mu_assert(list->next->next->next == NULL,
			"_ai_fringe_element_list_add_by_total_cost: test3 : list next next next NULL.");

	return NULL;
}

/*
 * Test _ai_fringe_element_list_pop.
 */
ai_fringe_element *_ai_fringe_element_list_pop(
		ai_fringe_element **fringe_element_list);
char * test__ai_fringe_element_list_pop() {

	// Setup
	ai_fringe_element *list = NULL;
	ai_fringe_element *node = NULL;
	ai_fringe_element *node1 = ai_fringe_element_constructor(NULL, NULL, 0.0f,
			1.0f);
	ai_fringe_element *node2 = ai_fringe_element_constructor(NULL, NULL, 0.0f,
			2.0f);
	ai_fringe_element *node3 = ai_fringe_element_constructor(NULL, NULL, 0.0f,
			3.0f);

	mu_assert(list == NULL, "_ai_fringe_element_list_pop: setup : list NULL.");

	// Run 1
	_ai_fringe_element_list_add_by_total_cost(&list, node1);
	_ai_fringe_element_list_add_by_total_cost(&list, node2);
	_ai_fringe_element_list_add_by_total_cost(&list, node3);
	node = _ai_fringe_element_list_pop(&list);
	// Test 1
	mu_assert(node == node1, "_ai_fringe_element_list_pop: test1 : node1.");

	// Run 2

	// Test 2
	node = _ai_fringe_element_list_pop(&list);
	mu_assert(node == node2, "_ai_fringe_element_list_pop: test2 : node2.");

	// Test 3
	node = _ai_fringe_element_list_pop(&list);
	mu_assert(node == node3, "_ai_fringe_element_list_pop: test3 : node3.");

	// Test 4
	node = _ai_fringe_element_list_pop(&list);
	mu_assert(node == NULL, "_ai_fringe_element_list_pop: test4 : NULL.");

	return NULL;
}

/*
 * Test ai_search_astar_constructor.
 */
char * test_ai_search_astar_constructor() {

	ai_model_state_evaluator model_state_evaluator;
	ai_search_astar *astar = ai_search_astar_constructor(
			&model_state_evaluator);
	mu_assert(astar != NULL, "ai_search_astar_constructor: astart NOT NULL.");
	mu_assert(astar->model_state_evaluator == &model_state_evaluator,
			"ai_search_astar_constructor: model_state_evaluator.");
	mu_assert(astar->find_path_to_goal != NULL,
			"ai_search_astar_constructor: find_path_to_goal.");
	mu_assert(astar->fringe_expansion_count == 0,
			"ai_search_astar_constructor: fringe_expansion_count.");
	mu_assert(astar->fringe_expansion_max == 0,
			"ai_search_astar_constructor: fringe_expansion_max.");

	return NULL;
}

/*
 * Run the test suite.
 */
char * all_tests() {
	mu_suite_start();
	mu_run_test(test_ai_action_constructor);
	mu_run_test(test_ai_path_constructor);
	mu_run_test(test__ai_path_append_action);
	mu_run_test(test__ai_path_duplicate);
	mu_run_test(test_ai_model_state_constructor);
	mu_run_test(test_ai_successor_constructor);
	mu_run_test(test_ai_fringe_element_constructor);
	mu_run_test(test__ai_fringe_element_list_add_by_total_cost);
	mu_run_test(test__ai_fringe_element_list_pop);
	mu_run_test(test_ai_search_astar_constructor);
	return NULL;
}

RUN_TESTS(all_tests);
