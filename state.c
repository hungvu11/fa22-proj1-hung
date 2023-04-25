#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {

  game_state_t* new_game = malloc(1 * sizeof(game_state_t));
  new_game->num_rows = 18;
  new_game->num_snakes = 1;
  new_game->snakes = malloc(new_game->num_snakes * sizeof(snake_t));
  new_game->board = malloc(new_game->num_rows * sizeof(char*));
  char str1[] = "####################\0";
  char str2[] = "#                  #\0";
  char str3[] = "# d>D    *         #\0";
  for (int i=0; i<new_game->num_rows; i++) {
    new_game->board[i] = malloc(strlen(str1) * sizeof(char));
    if (i == 0 || i == 17) strcpy(new_game->board[i], str1);
    else if (i != 2) strcpy(new_game->board[i], str2);
    else strcpy(new_game->board[i], str3);
    // new_game->board[i][20] = '\0';
  }
  new_game->snakes[0].head_col = 4;
  new_game->snakes[0].head_row = 2;
  new_game->snakes[0].tail_col = 2;
  new_game->snakes[0].tail_row = 2;
  new_game->snakes[0].live = true;
  return new_game;
}

/* Task 2 */
void free_state(game_state_t* state) {
  for (int i=0; i<state->num_rows; i++) free(state->board[i]);
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  for (int i=0; i<state->num_rows; i++) {
    fprintf(fp,"%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if (c == 'w' || c == 'a' || c == 's' || c == 'd') return true;
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x') return true;
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  if (is_head(c) || is_tail(c)) return true;
  if (c == '^' || c == '<' || c == '>' || c == 'v') return true;
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch(c) {
    case '^': return 'w';
    case '<': return 'a';
    case 'v': return 's';
    default: return 'd';
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch(c) {
    case 'W': return '^';
    case 'A': return '<';
    case 'S': return 'v';
    default: return '>';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake = state->snakes[snum];
  char c = get_board_at(state, snake.head_row, snake.head_col);
  unsigned int next_row = get_next_row(snake.head_row, c);
  unsigned int next_col = get_next_col(snake.head_col, c);
  return get_board_at(state, next_row, next_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = &state->snakes[snum];
  char c = get_board_at(state, snake->head_row, snake->head_col);
  set_board_at(state, snake->head_row, snake->head_col, head_to_body(c));
  unsigned int next_row = get_next_row(snake->head_row, c);
  unsigned int next_col = get_next_col(snake->head_col, c);
  snake->head_col = next_col;
  snake->head_row = next_row;
  set_board_at(state, next_row, next_col, c);

}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = &state->snakes[snum];
  char c = get_board_at(state, snake->tail_row, snake->tail_col);
  set_board_at(state, snake->tail_row, snake->tail_col, ' ');
  unsigned int next_row = get_next_row(snake->tail_row, c);
  unsigned int next_col = get_next_col(snake->tail_col, c);
  snake->tail_col = next_col;
  snake->tail_row = next_row;
  char cc = get_board_at(state, next_row, next_col);
  set_board_at(state, next_row, next_col, body_to_tail(cc));

}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  for (unsigned int i=0; i<state->num_snakes; i++) {
    char c = next_square(state, i);
    if (c == '#' || is_snake(c)) {
      set_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col, 'x');
      state->snakes[i].live = false;
    } else if (c == '*') {
      update_head(state, i);
      add_food(state);
    } else {
      update_head(state, i);
      update_tail(state, i);
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  game_state_t *state = malloc(sizeof(game_state_t));
  char c = '\0';
  unsigned int max_rows = 10;
  unsigned int max_cols = 10;
  unsigned int num_rows = 0;
  unsigned int num_cols = 0;
  state->board = (char **) malloc(max_rows * sizeof(char *));
  (state->board)[0] = (char *) malloc(max_cols * sizeof(char));
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    return NULL;
  }

  // Parse the board
  while ((c = (char) getc(fp)) != EOF) {
    // realloc memory if needed.
    if (num_cols >= max_cols) {
      max_cols *= 2;
      (state->board)[num_rows] = (char *) realloc((state->board)[num_rows], max_cols * sizeof(char));
    }

    if (c == '\n') {
      (state->board)[num_rows][num_cols] = '\0';
      ++num_cols;
      (state->board)[num_rows] = (char *) realloc((state->board)[num_rows], num_cols * sizeof(char));
      num_cols = 0;
      max_cols = 10;
      ++num_rows;
      if (num_rows >= max_rows) {
        max_rows *= 2;
        state->board = (char **) realloc(state->board, max_rows * sizeof(char *));
      }
      (state->board)[num_rows] = (char *) malloc(max_cols * sizeof(char));
    } else {
      (state->board)[num_rows][num_cols] = c;
      ++num_cols;
    }
  }
  state->board = (char **) realloc(state->board, num_rows * sizeof(char *));
  fclose(fp);
  
  // update the number of the board
  state->num_rows = num_rows;
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int cur_row = state->snakes[snum].tail_row;
  unsigned int cur_col = state->snakes[snum].tail_col;
  char c;
  while (is_head(c = get_board_at(state, cur_row, cur_col)) == false) {
    cur_col = get_next_col(cur_col, c);
    cur_row = get_next_row(cur_row, c);
  }
  state->snakes[snum].head_col = cur_col;
  state->snakes[snum].head_row = cur_row;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  state->num_snakes = 0;
  unsigned int max_snakes = 10;
  state->snakes = malloc(max_snakes * sizeof(snake_t));
  for (unsigned int i=0; i<state->num_rows; i++) {
    int len = (int) strlen(state->board[i]);
    for (unsigned int j=0; j<len; j++) {
     if (is_tail(state->board[i][j])) {
      if (state->num_snakes >= max_snakes) {
        max_snakes *= 2;
        state->snakes = realloc(state->snakes, max_snakes * sizeof(snake_t));
      }
      state->snakes[state->num_snakes].tail_col = j;
      state->snakes[state->num_snakes].tail_row = i;
      find_head(state, state->num_snakes);
      state->snakes[state->num_snakes].live = true;
      state->num_snakes++;
     }
    }
  }
  return state;
}
