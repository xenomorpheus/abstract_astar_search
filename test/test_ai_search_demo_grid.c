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
 * This is an implementation example.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ai_search.h>
#include <minunit.h>
#include <math.h>
#include <string.h>

#define PI_VALUE 3.14159265358979323846f

// Action specific data. In this implementation a movement on an grid.
typedef struct my_action_data_struct {
	float x_diff;
	float y_diff;
} my_action_data;

// Model State specific data. In this implementation the Agent's location and Goal location.
typedef struct my_model_state_data_struct {
	float agent_x;
	float agent_y;
	float goal_x;
	float goal_y;
} my_model_state_data;

/*
 * Successor Function - Demo implementation
 * From a given Model State:
 * 1) Determine a list of possible Actions
 * 2) From each action, use the transition function to find the resultant Model State.
 *
 * In this example Actions are Right, Up, Left, Down.
 * In this example the new Model States have the Agent moved one unit in the Action's direction.
 */
ai_successor* my_successor_function(ai_model_state* model_state,
		ai_transition_function transition_function) {
	ai_successor* head = NULL;
	ai_successor* current = NULL;
	for (float x = 0.0f; x < 2.0f * PI_VALUE; x += PI_VALUE / 2.0f) {
		my_action_data* action_data = (my_action_data*) malloc(
				sizeof(my_action_data));
		action_data->x_diff = cosf(x);
		action_data->y_diff = sinf(x);
		ai_action* action = ai_action_constructor(action_data);
		ai_model_state* new_model_state = transition_function(model_state,
				action);
		ai_successor* successor = ai_successor_constructor(new_model_state,
				action, 1.f);
		if (head) {
			current->next = successor;
			current = successor;
		} else {
			head = current = successor;
		}
	}
	return head;
}

// Duplicate the custom data stored in the Model State.
// In this case it is a simple struct, so can be duplicated in RAM.
void* my_model_state_data_duplicator(void* data) {
	my_model_state_data* new_data = (my_model_state_data*) malloc(
			sizeof(my_model_state_data));
	check(new_data, "my_model_state_data_duplicator malloc failed");
	memcpy(new_data, data, sizeof(my_model_state_data));
	return new_data;
	error: return NULL;
}

// Free the custom data stored in the Model State.
void my_model_state_data_free(void* data) {
	if (data){
		free(data);
	}
	else{
		printf("my_model_state_data_free called on NULL");
	}
}

// Duplicate the custom data stored in the Action.
// In this case it is a simple struct, so can be duplicated in RAM.
void* my_action_data_duplicator(void* data) {
	my_action_data* new_data = (my_action_data*) malloc(sizeof(my_action_data));
	check(new_data, "my_action_data_duplicator malloc failed");
	memcpy(new_data, data, sizeof(my_action_data));
	return new_data;
	error: return NULL;
}

// Free the custom data stored in the Action.
void my_action_data_free(void* data) {
	if (data){
		free(data);
	}
	else{
		printf("my_action_data_free called on NULL");
	}
}

// Derive the new Model State from current Model State, and an Action.
// In this case add the Action's offset to the Agent's location.
ai_model_state* my_transition_function(ai_model_state* model_state,
		ai_action* action) {
	my_model_state_data* new_data =
			(my_model_state_data*) my_model_state_data_duplicator(
					model_state->data);
	check(new_data, "my_transition_function malloc failed");
	my_action_data* action_data = action->data;
	new_data->agent_x += action_data->x_diff;
	new_data->agent_y += action_data->y_diff;
	return ai_model_state_constructor(new_data);
	error: return NULL;
}

/*
 * Return true if we are at a Goal Model State.
 * In this case the Agent's position is compared to the Goal's position.
 * If close enough then we return true.
 */
int my_is_goal_state_function(ai_model_state* model_state) {
	my_model_state_data* data = (my_model_state_data*) (model_state->data);
	return (abs(data->agent_x - data->goal_x) < 0.5f)
			&& (abs(data->agent_y - data->goal_y) < 0.5f);
}

/* This is an estimate of the remaining cost (distance) to the Goal Model State.
 * It must NEVER over estimate the cost.
 * In this implementation the straight line distance is used.
 */
float my_goal_est_cost_function(ai_model_state* model_state) {
	my_model_state_data* data = (my_model_state_data*) (model_state->data);
	return sqrt(
			abs(data->agent_x - data->goal_x)
					+ abs(data->agent_y - data->goal_y));
}

// Setup
static ai_model_state_evaluator evaluator = { .successor_function =
    my_successor_function,
    .transition_function = my_transition_function,
    .is_goal_state_function = my_is_goal_state_function,
    .goal_est_cost_function = my_goal_est_cost_function,
    .model_state_data_duplicator = my_model_state_data_duplicator,
    .model_state_data_free = my_model_state_data_free,
    .action_data_duplicator = my_action_data_duplicator,
    .action_data_free = my_action_data_free,
};


/*
 * Demo AStar search.
 * Already at a goal. Returned Path contains no actions.
 */
char* test_ai_search_demo_at_goal(void) {

	static my_model_state_data model_state_data = { .agent_x = 3, .agent_y = 4,
			.goal_x = 3, .goal_y = 4, };
	ai_model_state* model_state = ai_model_state_constructor(&model_state_data);
	ai_search_astar* astar = ai_search_astar_constructor(&evaluator);
	// Run
	ai_path* path = astar->find_path_to_goal(astar, model_state);
	mu_assert(path == NULL, "ai_search_demo_at_goal: path NULL.");
	return NULL;
}

/*
 * Demo AStar search.
 * Simple direct line to goal.
 * Two actions, each a step y=1.0
 */
char* test_ai_search_demo_straight(void) {
	static float TOLLERANCE = 0.001f;

	// Setup
	static my_model_state_data model_state_data = { .agent_x = 3, .agent_y = 4,
			.goal_x = 3, .goal_y = 6, };
	ai_model_state* model_state = ai_model_state_constructor(&model_state_data);
	ai_search_astar* astar = ai_search_astar_constructor(&evaluator);
	// Run
	ai_path* path = astar->find_path_to_goal(astar, model_state);
	// Test
	// Action 0
	mu_assert(path != NULL, "ai_search_demo_straight: path NOT NULL.");
	mu_assert(path->data != NULL,
			"ai_search_demo_straight: path data NOT NULL.");
	my_action_data* data = path->data;
	mu_assert(abs(data->x_diff - 0.0f) < TOLLERANCE,
			"ai_search_demo_straight: action[0]x = 0.");
	mu_assert(abs(data->y_diff - 1.0f) < TOLLERANCE,
			"ai_search_demo_straight: action[0]1 = 1.");
    my_action_data_free(path->data);
	// Action 1
	path = path->next;
	mu_assert(path != NULL, "ai_search_demo_straight: action[1] NOT NULL.");
	mu_assert(path->data != NULL,
			"ai_search_demo_straight: action[1] data NOT NULL.");
	data = path->data;
	mu_assert(abs(data->x_diff - 0.0f) < TOLLERANCE,
			"ai_search_demo_straight: action[1]x = 0.");
	mu_assert(abs(data->y_diff - 1.0f) < TOLLERANCE,
			"ai_search_demo_straight: action[1]1 = 1.");
    my_action_data_free(path->data);
	// Action 2
	path = path->next;
	mu_assert(path == NULL, "ai_search_demo_straight: action[2] NULL.");
	return NULL;
}

/*
 * Run the test suite.
 */
char* all_tests() {
	mu_suite_start();
    mu_run_test(test_ai_search_demo_at_goal);
    mu_run_test(test_ai_search_demo_straight);
	return NULL;
}

RUN_TESTS(all_tests);
