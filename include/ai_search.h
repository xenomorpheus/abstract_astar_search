#ifndef _AI_ASTAR_SEARCH_H_
#define _AI_ASTAR_SEARCH_H_

#include <float.h>

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
 */

/*
 * An Action is an option that can be taken by the Agent.
 * Performing an Action has a cost and leads to a new Model State.
 * The Action struct is an abstract wrapper for the actual Action data
 * which is the specific implementation of that action.
 * In graph theory Actions are often shown as edges between notes.
 */

/*
 * A Path is a series of Actions.
 * Cheating a bit here. Action has a next so they can form a linked list AKA a Path.
 * The data is implementation specific data for an action.
 */
typedef struct ai_action_struct {
	void *data;  // TODO some specific data
	// Cheat - Allow a linked list of successors
	struct ai_action_struct *next;
} ai_action, ai_path;

/*
 * Action Constructor
 * Example:
 * ai_action *action = ai_action_constructor(data);
 */
ai_action *ai_action_constructor(void *data);

ai_path *ai_path_constructor(void *data);

typedef struct ai_model_state_struct {
	void *data;
} ai_model_state;

/*
 * A Successor is a set containing a Model State, the action that immediately led to
 * that Model State, and the Cost of that single action.
 */
typedef struct ai_successor_struct {
	ai_model_state *model_state;
	ai_action *action;
	float cost;
	// Cheat - Allow a linked list of successors
	struct ai_successor_struct *next;
} ai_successor;

/* Successor Constructor
 * Example:
 * ai_successor *successor = ai_successor_constructor(model_state, action, cost);
 */
ai_successor *ai_successor_constructor(ai_model_state *model_state,
		ai_action *action, float cost);

/* A Transition Function calculates the new Model State when an Action is
 * is performed in the current Model State.
 */
typedef ai_model_state *(*ai_transition_function)(ai_model_state *model_state,
		ai_action *action);

/*
 * A Successor Function considers the current model state and calculates
 * the possible actions.
 * Returns a list of Successor objects.
 * Note: Successor, Action, and Model state objects will be freed by the search.
 */
typedef ai_successor *(*ai_successor_function)(ai_model_state *model_state,
		ai_transition_function transition_function);

// Is Goal State Function returns true if, and only if, the Model State is a Goal state.
typedef int (*ai_is_goal_state_function)(ai_model_state *model_state);

// The Goal Estimated Cost Function returns the estimated cost to reach a Goal state.
// This function must be Consistent, that is it may NEVER over-estimate the cost.
typedef float (*ai_goal_est_cost_function)(ai_model_state *model_state);

// A function that knows how to duplicate the implementation specific data of a Model State.
typedef void *(*ai_model_state_data_duplicator)(void *data);

// A function that knows how to free the implementation specific data of a Model State.
typedef void (*ai_model_state_data_free)(void *data);

// A function that knows how to duplicate the implementation specific data of an Action.
typedef void *(*ai_action_data_duplicator)(void *data);

// A function that knows how to free the implementation specific data of an Action.
typedef void (*ai_action_data_free)(void *data);

// The ai_model_state_evaluator holds the implementation specifics of the
// Model State and Action.
typedef struct ai_model_state_evaluator_struct {
	ai_successor_function successor_function;
	ai_transition_function transition_function;
	ai_is_goal_state_function is_goal_state_function;
	ai_goal_est_cost_function goal_est_cost_function;
	ai_model_state_data_duplicator model_state_data_duplicator;
	ai_model_state_data_free model_state_data_free;
	ai_action_data_duplicator action_data_duplicator;
	ai_action_data_free action_data_free;
} ai_model_state_evaluator;

// ai_model_state *model_state = ai_model_state_constructor(data);
ai_model_state *ai_model_state_constructor(void *data);

typedef struct ai_search_astar_struct {
	ai_model_state_evaluator *model_state_evaluator;
	ai_path *(*find_path_to_goal)(struct ai_search_astar_struct *astar,
			ai_model_state *model_state);
	int fringe_expansion_count;
	int fringe_expansion_max;
} ai_search_astar;

/*
 * A* Search Constructor
 *
 * Example:
 * ai_search_astar *astar = ai_search_astar_constructor(model_state_evaluator);
 * ai_path *path = astar->find_path_to_goal(astar, model_state );
 */
ai_search_astar *ai_search_astar_constructor(
		ai_model_state_evaluator *model_state_evaluator);

// Fringe Element - Used when searching
typedef struct ai_fringe_element_struct {
	ai_model_state *model_state;
	ai_path *path_so_far;
	float cost_so_far;
	float est_total_cost;
	// Cheat - Allow a linked list of successors
	struct ai_fringe_element_struct *next;
} ai_fringe_element;

/*
 * Fringe Element Constructor.
 *
 * Used when searching to hold Actions that are yet to be evaluated.
 * Example:
 * ai_fringe_element *fe = ai_fringe_element_constructor(model_state, path_so_far, 0.0f, 0.0f);
 */
ai_fringe_element *ai_fringe_element_constructor(ai_model_state *model_state,
		ai_path *path_so_far, float cost_so_far, float est_total_cost);

#endif // _AI_ASTAR_SEARCH_H_
