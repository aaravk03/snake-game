#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
    game_state_t *new = malloc(sizeof(game_state_t));
    new->num_rows = 18;
    new->num_snakes = 1;
    new->board = malloc(new->num_rows * sizeof(char *));
    for (int x = 0; x < new->num_rows; x++) {
        new->board[x] = malloc(21 * sizeof(char));
    }
    for (int x = 0; x < new->num_rows; x++) {
        if (x == 0 || x == new->num_rows - 1) {
            strcpy(new->board[x], "####################");
        } else {
            new->board[x][0] = '#';
            for (int y = 1; y <19; y++) {
                new->board[x][y] = ' ';
            }
            new->board[x][19] = '#';
            new->board[x][20] = '\0';
        }
    }

    new->board[2][4] = 'D'; 
    new->board[2][3] = '>';  
    new->board[2][2] = 'd'; 
    new->board[2][9] = '*';  
    snake_t *snake = malloc(sizeof(snake_t));
snake->tail_row = 2;
    snake->tail_col = 2;
    snake->head_row = 2;
    snake->head_col = 4;
    snake->live = 1;
    new->snakes = snake;
    return new;
}
    
/* Task 2 */
void free_state(game_state_t *state) {
  // TODO: Implement this function.
  free(state->snakes);
  for(int i = 0; i<state->num_rows; i++){
      free(state->board[i]);
  }
  free(state->board);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
    char **board = state->board;
    int rows = state->num_rows;
    for (char **row = board; row < board+rows; row++) {
        fprintf(fp, "%s\n", *row);
    }
  // TODO: Implement this function.
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  return c == 'w' || c == 'a' || c == 's' ||c == 'd';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  return is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch(c){
      case '^': return 'w';
      case '>': return 'd';
      case 'v': return 's';
      case '<': return 'a';
      default: return '?';
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch(c){
      case 'W': return '^';
      case 'A': return '<';
      case 'S': return 'v';
      case 'D': return '>';
      default: return '?';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c=='v' || c == 's' || c == 'S') 
      return cur_row + 1;
  if (c=='^' || c == 'w' || c == 'W')
      return cur_row - 1;
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c=='>' || c=='d'|| c=='D')
      return cur_col + 1;
  if(c == '<'|| c=='a' || c=='A')
      return cur_col - 1;
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
    unsigned int row = state->snakes[snum].head_row;
    unsigned int col = state->snakes[snum].head_col;
    row = get_next_row(row, state->board[row][col]);
    col = get_next_col(col, state->board[row][col]);
    return state->board[row][col];
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
    /* snake_t *snake = &state->snakes[snum];
    unsigned int row = snake->head_row;
    unsigned int col = snake->head_col;
    char head = state->board[row][col];

    char head2 = head_to_body(head);
    row = get_next_row(row, head);
    col = get_next_col(col, head);
    state->board[row][col] = head2;
    snake->head_row = row;
    snake->head_col = col;
    */
    snake_t *snake = &state->snakes[snum];
    unsigned int row = snake->head_row;
    unsigned int col = snake->head_col;
    char head2 = state->board[row][col];
    unsigned int next_row = get_next_row(row, head2);
    unsigned int next_col = get_next_col(col, head2);
    state->board[row][col] = head_to_body(head2);
    state->board[next_row][next_col] = head2;
    snake->head_row = next_row;
    snake->head_col = next_col;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
    snake_t *snake = &state->snakes[snum];
    unsigned int row = snake->tail_row;
    unsigned int col = snake->tail_col;
    char tail = state->board[row][col];
    state->board[row][col] = ' ';
    row = get_next_row(row, tail);
    col = get_next_col(col, tail);
    state->board[row][col] = body_to_tail(state->board[row][col]);
    snake->tail_row = row;
    snake->tail_col = col;
}
/* Task 4.5 */

void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
    for (unsigned int i = 0; i < state->num_snakes; i++) {
        char next = next_square(state, i);

        if (is_snake(next) || next == '#') {
            state->board[state->snakes[i].head_row][state->snakes[i].head_col] = 'x';
            state->snakes[i].live = 0;
        } 
        else if (next == '*') {
            update_head(state, i);
            add_food(state);
        } 
        else {
            update_tail(state, i);
            update_head(state, i);
        }
    }
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
 /* char buffer[256];
  if (fgets(buffer, sizeof(buffer), fp) == NULL) {
      return NULL;
  }
  size_t l = strlen(buffer);
  if (buffer[l - 1] == '\n') {
      buffer[l - 1] = '\0';
  }
  char *line = malloc(l + 1);
  if (line == NULL) {
      return NULL;
  }
  return line;
  */
    /*size_t buffer_size = 128;
    char *buffer = malloc(buffer_size);
        }
    }
    */
    size_t bsize = 128;
    char *line = malloc(bsize);
    if (line == NULL) {
        return NULL;
    }
    size_t total_len = 0;
    while (fgets(line + total_len, bsize - total_len, fp)) {
        total_len += strlen(line + total_len);
        if (line[total_len - 1] == '\n') {
            return line;
        }
        bsize *= 2;
        char *line2 = realloc(line, bsize);
        if (line2 == NULL) {
            free(line);
            return NULL;
        }
        line = line2;
    }
    if (total_len == 0) {
        free(line);
        return NULL;
    }
    return line;
}


/* Task 5.2 */
  // TODO: Implement this function.
    /*game_state_t *game = malloc(sizeof(game_state_t));
    if (!game) {
        return NULL;
    while ((line = read_line(fp)) != NULL) {
    unsigned int num_rows = 0;
    unsigned int max_rows = 20;
    game->board = malloc(sizeof(char *) * max_rows);
    if (!game->board) {
        free(game);
        return NULL;
    }

    char *line;
    while ((line = read_line(fp)) != NULL) {
        if (num_rows == max_rows) {
            max_rows += 20;
            char **new_board = realloc(game->board, sizeof(char *) * max_rows);
            if (!new_board) {
                for (unsigned int i = 0; i < num_rows; i++) {
                    free(game->board[i]);
                }
                free(game->board);
                free(game);
                free(line);
                return NULL;
            }
            game->board = new_board;
        }
        game->board[num_rows++] = line;
    }
        */


game_state_t *load_board(FILE *fp) {
    unsigned int rows = 20, i = 0;
    game_state_t *game = malloc(sizeof(game_state_t));
    game->board = malloc(sizeof(char *) * rows);
    char *line = read_line(fp);
    while (line) {
        if (i >= rows) {
            rows += 20;
            game->board = realloc(game->board, sizeof(char *) * rows);
        }
        size_t len = strlen(line);
        if (len > 0 && line[len - 1]=='\n') {
            line[len - 1] ='\0';
        }

        game->board[i++] = line;
        line = read_line(fp);
    }
    if (i == 0) {
        free(game->board);
        free(game);
        return NULL;
    }
    game->board = realloc(game->board, sizeof(char *) * i);
    game->num_rows = i;
    game->num_snakes = 0;
    game->snakes = NULL;
    return game;
}
    

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
    snake_t *snake = &state->snakes[snum];
    unsigned int row = snake->tail_row;
    unsigned int col = snake->tail_col;
    char c = get_board_at(state, row, col);
    while (!is_head(c)) {
        unsigned int next_row = get_next_row(row, c);
        unsigned int next_col = get_next_col(col, c);
        row = next_row;
        col = next_col;
        c = get_board_at(state, row, col);
        if (!is_snake(c)) {
            break;
        }
    }
    snake->head_row = row;
    snake->head_col = col;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
    unsigned int snake_count = 0;
    for (unsigned int row = 0; row < state->num_rows; row++) {
        for (unsigned int col = 0; state->board[row][col] != '\0'; col++) {
            if (is_tail(state->board[row][col])) {
                snake_count++;
            }
        }
    }
    state->num_snakes = snake_count;
    state->snakes = malloc(sizeof(snake_t) * snake_count);
    unsigned int i = 0;
    for (unsigned int row = 0; row < state->num_rows; row++) {
        for (unsigned int col = 0; state->board[row][col] != '\0'; col++) {
            if (is_tail(state->board[row][col])) {
                state->snakes[i].tail_row = row;
                state->snakes[i].tail_col = col;
                find_head(state, i);
                i++;
            }
        }
    }
    return state;
}
