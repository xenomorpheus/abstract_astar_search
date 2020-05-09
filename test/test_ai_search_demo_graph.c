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

#include <ai_search.h>
#include <math.h>
#include <minunit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Action specific data. In this implementation a graph of nodes, and moving to
// one of them.
typedef struct my_action_data_struct {
  char *node_name;
} my_action_data;

// Model State specific data. In this implementation a graph of nodes, and at
// one of them.
typedef struct my_model_state_data_struct {
  char *node_name;
} my_model_state_data;

typedef struct my_node_transition_struct {
  char *node_name;
  float cost;
  struct my_node_transition_struct *next;
} my_node_transition;

typedef struct my_node_data_set_struct {
  const char *node_name;
  float heuristic;
  my_node_transition *transition;
} my_node_data_set;

my_node_transition s_transition_2 = {
    .node_name = "B",
    .cost = 1.f,
    .next = NULL,
};
my_node_transition s_transition = {
    .node_name = "A", .cost = 2.f, .next = &s_transition_2};

my_node_transition a_transition_3 = {
    .node_name = "B",
    .cost = 1.f,
    .next = NULL,
};
my_node_transition a_transition_2 = {
    .node_name = "C",
    .cost = 3.f,
    .next = &a_transition_3,
};
my_node_transition a_transition = {
    .node_name = "D", .cost = 1.f, .next = &a_transition_2};

my_node_transition b_transition_2 = {
    .node_name = "G",
    .cost = 10.f,
    .next = NULL,
};
my_node_transition b_transition = {
    .node_name = "D", .cost = 5.f, .next = &b_transition_2};

my_node_transition c_transition = {
    .node_name = "G",
    .cost = 7.f,
    .next = NULL,
};

my_node_transition d_transition = {
    .node_name = "G",
    .cost = 4.f,
    .next = NULL,
};

my_node_data_set test_transition_set[] = {
    {.node_name = "S", .heuristic = 0.0f, .transition = &s_transition},
    {.node_name = "A", .heuristic = 3.0f, .transition = &a_transition},
    {.node_name = "B", .heuristic = 3.0f, .transition = &b_transition},
    {.node_name = "C", .heuristic = 1.0f, .transition = &c_transition},
    {.node_name = "D", .heuristic = 2.0f, .transition = &d_transition},
    {.node_name = NULL, .heuristic = 0.0f, .transition = NULL},
};

my_node_data_set *_node_data_lookup(my_node_data_set my_successors[],
                                    char *node_name) {
  int count = 0;
  while (my_successors[count].node_name != NULL) {

    if (strcmp(my_successors[count].node_name, node_name) == 0) {
      return &my_successors[count];
    }
    count++;
  }
  return NULL;
}

/*
 * Successor Function - Demo implementation
 * From a given Model State:
 * 1) Determine a list of possible Actions
 * 2) From each action, use the transition function to find the resultant Model
 * State.
 *
 * In this example Actions are Right, Up, Left, Down.
 * In this example the new Model States have the Agent moved one unit in the
 * Action's direction.
 */
ai_successor *
my_successor_function(ai_model_state *model_state,
                      ai_transition_function transition_function) {

  ai_successor *successor_head = NULL; // Head of successor objects to return.
  ai_successor *successor_tail = NULL; // Tail of successor objects to return.

  char *current_node_name =
      ((my_model_state_data *)model_state->data)->node_name;
  my_node_data_set *node_data =
      _node_data_lookup(test_transition_set, current_node_name);
  if (node_data == NULL) {
    return NULL;
  }
  my_node_transition *transition = node_data->transition;
  while (transition) {
    my_action_data *new_action_data =
        (my_action_data *)malloc(sizeof(my_action_data));
    new_action_data->node_name =
        transition->node_name; // Don't duplicate immutable data.
    ai_action *new_action = ai_action_constructor(new_action_data);
    ai_model_state *new_model_state =
        transition_function(model_state, new_action);
    ai_successor *new_successor =
        ai_successor_constructor(new_model_state, new_action, transition->cost);
    if (successor_head) {
      successor_tail->next = new_successor;
      successor_tail = new_successor;
    } else {
      successor_head = successor_tail = new_successor;
    }
    transition = transition->next;
  }
  return successor_head;
}

// Duplicate the custom data stored in the Model State.
// In this case it is a simple struct, so can be duplicated in RAM.
// In this case it is a pointer to an immutable string
void *my_model_state_data_duplicator(void *data) {
  my_model_state_data *new_data =
      (my_model_state_data *)malloc(sizeof(my_model_state_data));
  check(new_data, "my_model_state_data_duplicator malloc failed");
  memcpy(new_data, data, sizeof(my_model_state_data));
  return new_data;
error:
  return NULL;
}

// Free the custom data stored in the Model State.
void my_model_state_data_free(void *data) {
  if (data) {
    free(data);
  } else {
    printf("my_model_state_data_free called on NULL");
  }
}

// Duplicate the custom data stored in the Action.
// In this case it is a simple struct, so can be duplicated in RAM.
// In this case it is a pointer to an immutable string
void *my_action_data_duplicator(void *data) {
  my_action_data *new_data = (my_action_data *)malloc(sizeof(my_action_data));
  check(new_data, "my_action_data_duplicator malloc failed");
  memcpy(new_data, data, sizeof(my_action_data));
  return new_data;
error:
  return NULL;
}

// Free the custom data stored in the Action.
void my_action_data_free(void *data) {
  if (data) {
    free(data);
  } else {
    printf("my_action_data_free called on NULL");
  }
}

// Derive the new Model State from current Model State, and an Action.
// In this case the Agent's location becomes the node specified in the Action.
ai_model_state *my_transition_function(ai_model_state *model_state,
                                       ai_action *action) {
  my_model_state_data *new_data =
      (my_model_state_data *)my_model_state_data_duplicator(model_state->data);
  check(new_data, "my_transition_function malloc failed");
  my_action_data *action_data = action->data;
  new_data->node_name = action_data->node_name;
  return ai_model_state_constructor(new_data);
error:
  return NULL;
}

/*
 * Return true if we are at a Goal Model State.
 * In this case the Agent's position is Goal state "G"
 */
int my_is_goal_state_function(ai_model_state *model_state) {
  my_model_state_data *data = (my_model_state_data *)(model_state->data);
  return strcmp(data->node_name, "G") == 0;
}

/* This is an estimate of the remaining cost (distance) to the Goal Model State.
 * It must NEVER over estimate the cost.
 * In this implementation the straight line distance is used.
 */
float my_goal_est_cost_function(ai_model_state *model_state) {
  my_model_state_data *data = (my_model_state_data *)(model_state->data);
  char *node_name = data->node_name;
  my_node_data_set *node_data =
      _node_data_lookup(test_transition_set, node_name);
  if (node_data == NULL) {
    return 0.0f;
  }
  return node_data->heuristic;
}

// Setup
static ai_model_state_evaluator evaluator = {
    .successor_function = my_successor_function,
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
 * Already at a Goal. Returned Path contains no actions.
 */
char *test_ai_search_demo_at_goal(void) {

  // Setup
  static my_model_state_data model_state_data = {
      .node_name = "G",
  };
  ai_model_state *model_state = ai_model_state_constructor(&model_state_data);
  ai_search_astar *astar = ai_search_astar_constructor(&evaluator);
  // Run
  ai_path *path = astar->find_path_to_goal(astar, model_state);
  mu_assert(path == NULL, "ai_search_demo_at_goal: path NULL.");
  return NULL;
}

/*
 * Demo AStar search.
 * Multiple Actions to reach Goal.
 * S->A->D->G
 */
char *test_ai_search_demo_multi(void) {

  // Setup
  static my_model_state_data model_state_data = {
      .node_name = "S",
  };
  ai_model_state *model_state = ai_model_state_constructor(&model_state_data);
  ai_search_astar *astar = ai_search_astar_constructor(&evaluator);
  // Run
  ai_path *path = astar->find_path_to_goal(astar, model_state);
  // Test
  // Action 0
  mu_assert(path != NULL, "ai_search_demo_straight: path NOT NULL.");
  mu_assert(path->data != NULL, "ai_search_demo_straight: path data NOT NULL.");
  ai_path *old_path;
  my_action_data *data = (my_action_data *)path->data;
  mu_assert(strcmp(data->node_name, "A") == 0,
            "ai_search_demo_straight: action[0]  = A.");
  // Action 1
  old_path = path;
  path = path->next;
  free(old_path);
  mu_assert(path != NULL, "ai_search_demo_straight: action[1] NOT NULL.");
  mu_assert(path->data != NULL,
            "ai_search_demo_straight: action[1] data NOT NULL.");
  data = (my_action_data *)path->data;
  mu_assert(strcmp(data->node_name, "D") == 0,
            "ai_search_demo_straight: action[1] = D.");
  // Action 2
  old_path = path;
  path = path->next;
  free(old_path);
  mu_assert(path != NULL, "ai_search_demo_straight: action[2] NOT NULL.");
  mu_assert(path->data != NULL,
            "ai_search_demo_straight: action[2] data NOT NULL.");
  data = (my_action_data *)path->data;
  mu_assert(strcmp(data->node_name, "G") == 0,
            "ai_search_demo_straight: action[2] = G.");
  // Action 3
  old_path = path;
  path = path->next;
  free(old_path);
  mu_assert(path == NULL, "ai_search_demo_straight: action[3] NULL.");
  return NULL;
}

/*
 * Run the test suite.
 */
char *all_tests() {
  mu_suite_start();
  mu_run_test(test_ai_search_demo_at_goal);
  mu_run_test(test_ai_search_demo_multi);
  return NULL;
}

RUN_TESTS(all_tests);
