/*
 * AI - Computational Search Using A* Search Algorithm.
 *
 * http://en.wikipedia.org/wiki/A*_search_algorithm
 *
 * To allow this A* Search algorithm to be used on any problem,
 * the search is written independent of the Model State, the Actions, etc.
 *
 * Actions and Model States have 'data' field for data specific to a problem.
 * Likewise all implementation specific functions are passed into the search.
 * e.g. Successor Function, Transition Function.
 *
 * Please see the test_ai_search_demo.c for an implementation example.
 *
 *  Created on: 6 Jan 2015
 *      Author: xenomorpheus
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ai_search.h>
#include <logging.h>

// Constructor for an Action.
ai_action *ai_action_constructor(void *data) {
	ai_action *action = (ai_action *) malloc(sizeof(ai_action));
	check(action, "ai_action_constructor malloc failed");
	action->data = data;
	action->next = NULL;
	return action;
	error: return NULL;
}

// Alias for ai_action_constructor
ai_path *ai_path_constructor(void *data) {
	return (ai_path *) ai_action_constructor(data);
}

// Assumes that action's "next" is already set to correct value.
// E.g. NULL, or linked to next action if appending a path.
void _ai_path_append_action(ai_path **path, ai_action *action) {
	check(path, "_ai_path_append_action path was NULL");
	check(action, "_ai_path_append_action action was NULL");
	if (*path) {
		ai_path *ptr = *path;
		// Find end
		while (ptr->next) {
			ptr = ptr->next;
		}
		ptr->next = action;
	} else {
		*path = action;
	}
	return;
	error: return;
}

// ai_path *new_path = _ai_path_duplicate(old_path);
ai_path *_ai_path_duplicate(ai_action_data_duplicator action_data_duplicator,
		ai_path *old) {
	ai_path *new = NULL;
	while (old) {
		ai_action *new_action = ai_action_constructor(
				action_data_duplicator(old->data));
		_ai_path_append_action(&new, new_action);
		old = old->next;
	}

	return new;
}

// ai_model_state *model_state = ai_model_state_constructor(data);
ai_model_state *ai_model_state_constructor(void *data) {
	ai_model_state *model_state = (ai_model_state *) malloc(
			sizeof(ai_model_state));
	check(model_state, "ai_model_state_constructor malloc failed");
	model_state->data = data;
	return model_state;
	error: return NULL;
}

//	ai_successor *successor = ai_successor_constructor(model_state, action, cost);
ai_successor *ai_successor_constructor(ai_model_state *model_state,
		ai_action *action, float cost) {
	ai_successor *successor = (ai_successor *) malloc(sizeof(ai_successor));
	check(successor, "ai_successor_constructor malloc failed");
	successor->model_state = model_state;
	successor->action = action;
	successor->cost = cost;
	// TODO cheat? Allow a linked list of successors
	successor->next = NULL;
	return successor;
	error: return NULL;
}

// Constructor for a Fringe Element. Used when searching.
// ai_fringe_element *fe = ai_fringe_element_constructor(model_state, path_so_far, 0.0f, 0.0f);
ai_fringe_element *ai_fringe_element_constructor(ai_model_state *model_state,
		ai_path *path_so_far, float cost_so_far, float est_total_cost) {

	ai_fringe_element *fe = (ai_fringe_element *) malloc(
			sizeof(ai_fringe_element));
	check(fe, "ai_fringe_element_constructor malloc failed");
	fe->model_state = model_state;
	fe->path_so_far = path_so_far;
	fe->cost_so_far = cost_so_far;
	fe->est_total_cost = est_total_cost;
	fe->next = NULL;
	return fe;
	error: return NULL;
}

ai_fringe_element *_ai_fringe_element_list_pop(
		ai_fringe_element **fringe_element_list) {
	ai_fringe_element *head = *fringe_element_list;
	if (head) {
		*fringe_element_list = head->next;
	}
	return head;
}

// Add Fringe Element to the list.
// The list is sorted by est_total_cost, low first.
void _ai_fringe_element_list_add_by_total_cost(
		ai_fringe_element **fringe_element_list, ai_fringe_element *node) {
	ai_fringe_element *prev, *current;
	float est_total_cost = node->est_total_cost;
	// Remove this if we need to add lists to lists.
	node->next = NULL;
	if (!*fringe_element_list) {
		*fringe_element_list = node;
	} else {
		prev = NULL;
		current = *fringe_element_list;
		while (current && current->est_total_cost <= est_total_cost) {
			prev = current;
			current = current->next;
		}
		if (!current) {
			prev->next = node;
		} else {
			if (prev) {
				node->next = prev->next;
				prev->next = node;
			} else {
				node->next = *fringe_element_list;
				*fringe_element_list = node;
			}
		}
	}
}

// Free the path and any implementation specific data
void _ai_path_free(ai_path *ptr, ai_action_data_free action_data_free) {
	ai_path *next = NULL;
	while (ptr) {
		next = ptr->next;
		// Implementation should provide the method to free the action data
		// because action data may be immutable and reused, hence should not be freed.
		if (ptr->data && action_data_free) {
			action_data_free(ptr->data);
		}
		free(ptr);
		ptr = next;
	}
}

// Duplicate the Model State and data.
ai_model_state * _ai_model_state_duplicate(ai_model_state *model_state,
		ai_model_state_data_duplicator model_state_data_duplicator) {
	void *dup_data = model_state_data_duplicator(model_state->data);
	return ai_model_state_constructor(dup_data);
}

// Free the Model State and data.
void _ai_model_state_free(ai_model_state *model_state,
		ai_model_state_data_free model_state_data_free) {
	if (model_state) {
		if (model_state->data && model_state_data_free) {
			model_state_data_free(model_state->data);
		}
		free(model_state);
	}
}

// private - AStar search algorithm
ai_path *_ai_search_astar_find_path_to_goal(ai_search_astar *astar,
		ai_model_state *initial_model_state) {

	// Aliases for functions that are Model State and Action specific.
	ai_model_state_evaluator *model_state_evaluator =
			astar->model_state_evaluator;
	ai_successor_function successor_function =
			model_state_evaluator->successor_function;
	ai_transition_function transition_function =
			model_state_evaluator->transition_function;
	ai_is_goal_state_function is_goal_state_function =
			model_state_evaluator->is_goal_state_function;
	ai_goal_est_cost_function goal_est_cost_function =
			model_state_evaluator->goal_est_cost_function;
	ai_model_state_data_duplicator model_state_data_duplicator =
			model_state_evaluator->model_state_data_duplicator;
	ai_model_state_data_free model_state_data_free =
			model_state_evaluator->model_state_data_free;
	ai_action_data_duplicator action_data_duplicator =
			model_state_evaluator->action_data_duplicator;
	ai_action_data_free action_data_free =
			model_state_evaluator->action_data_free;

	// Init
	ai_model_state *dup_initial_model_state = _ai_model_state_duplicate(
			initial_model_state, model_state_data_duplicator);
	// Technically the cost should be remaining distance to goal, but
	// it is the only item in the fringe so will be popped regardless.
	ai_fringe_element *fringe_list = ai_fringe_element_constructor(
			dup_initial_model_state, NULL, 0, 0);
	ai_path *result_path = NULL;

	// Begin of fringe expansion loop.
	while ((fringe_list != NULL)
			&& ((astar->fringe_expansion_max == 0)
					|| (astar->fringe_expansion_count
							< astar->fringe_expansion_max))) {
		astar->fringe_expansion_count++;

		ai_fringe_element *fringe = _ai_fringe_element_list_pop(&fringe_list);
		ai_model_state *current_model_state = fringe->model_state;
		ai_path *current_path_so_far = fringe->path_so_far;
		float cost_so_far = fringe->cost_so_far;
		free(fringe);

		if (is_goal_state_function(current_model_state)) {
			result_path = current_path_so_far;
			break;
		}
		ai_successor *successor_list = successor_function(current_model_state,
				transition_function);
		ai_successor *successor_next = NULL;
		for (ai_successor *successor = successor_list; successor != NULL;
				successor = successor_next) {
			ai_model_state *successor_model_state = successor->model_state;
			// TODO check if we have seen this state before
			// TODO add this model state to states we have seen.
			ai_path *new_path_so_far = _ai_path_duplicate(
					action_data_duplicator, current_path_so_far);
			_ai_path_append_action(&new_path_so_far, successor->action);
			float new_cost_so_far = cost_so_far + successor->cost;
			successor_next = successor->next;
			free(successor);
			successor = NULL;

			float cost_to_goal_est = 0;
			if (goal_est_cost_function != NULL) {
				cost_to_goal_est = goal_est_cost_function(
						successor_model_state);
			}
			ai_fringe_element *fringe_element_new =
					ai_fringe_element_constructor(successor_model_state,
							new_path_so_far, new_cost_so_far,
							new_cost_so_far + cost_to_goal_est);
			_ai_fringe_element_list_add_by_total_cost(&fringe_list,
					fringe_element_new);
		}
		// TODO free as much mem as possible
		_ai_path_free(current_path_so_far, action_data_free);

		current_path_so_far = NULL;
        _ai_model_state_free(current_model_state, model_state_data_free);
		current_model_state = NULL;

	}
	// Free the fringe
	ai_fringe_element *current_fe = NULL;
	while ((current_fe = _ai_fringe_element_list_pop(&fringe_list)) != NULL) {
		_ai_model_state_free(current_fe->model_state, model_state_data_free);
		_ai_path_free(current_fe->path_so_far, action_data_free);
		free(current_fe);
	}
	return result_path;
}
// ai_search_astar *astar = ai_search_astar_constructor(model_state_evaluator);
// ai_path *path = astar->find_path_to_goal(astar, model_state );
ai_search_astar *ai_search_astar_constructor(
		ai_model_state_evaluator *model_state_evaluator) {
	ai_search_astar *astar = (ai_search_astar *) malloc(
			sizeof(ai_search_astar));
	check(astar, "ai_search_astar_constructor malloc failed");
	astar->find_path_to_goal = _ai_search_astar_find_path_to_goal;
	astar->model_state_evaluator = model_state_evaluator;
	astar->fringe_expansion_count = 0;
	astar->fringe_expansion_max = 0;
	return astar;
	error: return NULL;
}

