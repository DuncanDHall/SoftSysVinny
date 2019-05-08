/**//*
 * This program will load a text file and print it to screen
 * 4/2/2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <assert.h>
// Read all of the lines from file and put into an array
// line in an array pointed to by lines_array_ptrd .
void read_lines_from_file(char *file_name, char ***lines_array_ptr, int *n_lines_ptr)
{
    char *line = NULL;
    size_t lsize;
    int num_lines = 0;

    int start_of_line = 0;
    int len_current_line = 0;
    char c;
    int line_counter = 0;

    // opens a file
    FILE *fp = fopen(file_name, "r"); // opens file in read mode
    if (fp == NULL){
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE); //bad news, quit program
    }

    // count number of lines in file
    while ((c = getc(fp)) != EOF) {
        if (c == '\n') num_lines++;
    }
    char **lines_array = malloc(sizeof(char *) * num_lines);


    fseek(fp, 0, SEEK_SET); //rewinds pointer to top of file

    //printf("reading thru file\n");
    /* Loop through until we are done with the file. */
    /* Get the next line */

    while(getline(&line, &lsize, fp) != -1) {
        lines_array[line_counter] = strdup(line);
        //printf("Line is %s\n", line);
        //
        // /* Show the line details */
        // printf("line[%06d]: chars=%06zd, buf size=%06zu, contents: %s", line_counter,
        //  line_size, lsize, line);
        // //printf("Conditional: %d, %d\n", line_counter >= start_line, line_counter < start_line + n_lines);
        // /* Increment our line counter */
        line_counter++;
    }
    free(line); //free line buffer
    fclose(fp);

    *lines_array_ptr = lines_array;
    *n_lines_ptr = line_counter;
}

//this function does this _____
int count_digits(int n)
{
    int c = 0;
    while (n) {
        n = n / 10;
        c++;
    }
    return c;
}

// takes in the file pointer, array of pointers to line contents,
// number of lines and number of columns
// formats them to fill the terminal display window
// adds line numbers with a gutter
// void get_formatted_lines(FILE *fp, char ***lines_array_ptr, int start_line, int n_lines, int n_cols)
// {
//     int gutter_size;
//     char **raw_lines;
//     char *line;
//
//     // get info about window size
//     // calculate gutter size
//     gutter_size = 2 + count_digits(start_line + n_lines);
//
//     // get raw lines
//     read_lines_from_file(fp, &raw_lines, start_line, n_lines);
//
//     // write raw lines
//     for (int i = 0; i < n_lines; i++) {
//         line = malloc(n_cols * sizeof(char));
//         memcpy(
//             (void *)(&line + gutter_size),
//             (void *) raw_lines[i],
//             (strlen(raw_lines[i])+1-gutter_size) * sizeof(char)
//         );
//         // memcopy raw into line
//     }
//     *lines_array_ptr = raw_lines;
//
//     // trim lines
//     // lines = malloc(sizeof(char *[n_env_lines]));
//     // for (int )
//
//     // draw line numbers
// }

void print_lines(char ***lines_array_ptr, int n_lines)
{
    for (int i = 0; i < n_lines; i++) {
        printf("%s", (*lines_array_ptr)[i]);
    }
}


//our three "modes" for the editor
enum Mode {
    normal = 0,
    insert = 1,
    command = 2,
};


// define the state of our window
typedef struct {
    char **lines; // array of lines from our file
    char file_name[20];
    int n_lines;
    int top_line;
    int window_height, window_width; // window dimensions
    int cursor_row, cursor_col; //referenced to lines not window
    enum Mode mode;
    // maybe some command history stuff
} State;

/* Make a State.
*/
State *make_state(char *file_name) {

    State *state = malloc(sizeof(State)); //dynamically allocate memory
    read_lines_from_file(file_name, &(state->lines), &state->n_lines);
    strcpy(state->file_name, file_name);
    state->top_line = 0;
    // state->window_width = num_cols;
    // state->window_height = num_rows;
    state->mode = normal;
    state->cursor_row = 0;
    state-> cursor_col = 0;

    return state;
}
    // for reference:
/*
    Matrix *matrix = malloc(sizeof(Matrix));
    matrix->num_rows = num_rows;
    matrix->num_cols = num_cols;
    matrix->rows = malloc(num_rows * sizeof(double*));
    for (int i=s; i<num_rows; i++) {
        matrix->rows[i] = calloc(num_cols, sizeof(double));
    }
    return matrix;
}
*/


/*
 * source: https://ubuntuforums.org/showthread.php?t=554845
 */
char getch()
{
    char c=0;

    struct termios org_opts, new_opts;
    int res=0;
    //-----  store old settings -----------
    res=tcgetattr(STDIN_FILENO, &org_opts);
    assert(res==0);
    //---- set new terminal parms --------
    memcpy(&new_opts, &org_opts, sizeof(new_opts));
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    c=getchar();
    //------  restore old settings ---------
    res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res==0);
    return(c);
}

void move_cursor(State *state, int d_row, int d_col)
{
    int line_len = strlen(state->lines[state->cursor_row]);

    state->cursor_row += d_row;
    if (state->cursor_row < 0) state->cursor_row = 0;
    if (state->cursor_row > state->n_lines) state->cursor_row = state->n_lines;

    state->cursor_col += d_col;
    if (state->cursor_col < 0) state->cursor_col = 0;
    if (state->cursor_col > line_len - 1) state->cursor_col = line_len - 1;
    printf("cursor at row %d col %d\n", state->cursor_row, state->cursor_col);

    // adjust window position
    // TODO: fill this in
    if (state->cursor_row > state->top_line + state->window_height) {
        // scroll down
        state->top_line = state->cursor_row - state->window_height;
    }
    if (state->cursor_row < state->top_line) {
        // scroll up
        state->top_line = state->cursor_row;
    }
}

void normal_mode_handler(State *state, char input)
{
    switch (input) {
        // mode changes
        case 'i':
            state->mode = insert;
            break;
        case ':':
            state->mode = command;
            break;
        // general navigation
        case 'h':
            move_cursor(state, 0, -1);
            break;
        case 'j':
            move_cursor(state, 1, 0);
            break;
        case 'k':
            move_cursor(state, -1, 0);
            break;
        case 'l':
            move_cursor(state, 0, 1);
            break;
        default:
            printf("unknown command %c in normal mode\n", input);
    }
}

void insert_char(State *state, char c)
{
    char *line = state->lines[state->cursor_row];
    char new_line[strlen(line) + 1];

    strncpy(new_line, line, state->cursor_col);                        // first part
    new_line[state->cursor_col] = c;                                   // new char
    strcpy(new_line+(state->cursor_col)+1, line+(state->cursor_col));  // last part

    // TODO remove this
    printf("new_line: %s", new_line);

    state->lines[state->cursor_row] = strdup(new_line);
    state->cursor_col++;
    free(line);
}

void insert_mode_handler(State *state, char input)
{
    switch (input) {
        case '\x1b': // 1b is the ESC character's hex value
            state->mode = normal;
            break;
        default:
            insert_char(state, input);
    }
}

void save_to_file(State *state)
{
    FILE *fp;
    int i;

    fp = fopen(state->file_name, "w");
    for (i = 0; i < state->n_lines; i++) {
        fputs(state->lines[i], fp);
    }
    fclose(fp);
}

void command_mode_handler(State *state, char input)
{
    switch (input) {
        case '\x1b': // 1b is the ESC character's hex value
            state->mode = normal;
            break;
        case 'w':
            save_to_file(state);
            state->mode = normal;
            break;
        case 'q':
            // TODO confirm?
            exit(0);
            break;
        default:
            printf("unknown command %c in command mode\n", input);
            state->mode = normal;
    }
}

void update_state(State *state, char input)
{
    if (state->mode == normal) {
        printf("Normal mode handling input: %c\n", input);
        normal_mode_handler(state, input);
    }
    else if (state->mode == insert) {
        printf("Insert mode handling input: %c\n", input);
        insert_mode_handler(state, input);
    }
    else if (state->mode == command) {
        printf("Command mode handling input: %c\n", input);
        command_mode_handler(state, input);
    }
}

int main (int argc, char *argv[])
{
    // declarations
    struct winsize w;
    int n_env_lines, n_env_cols;
    char **lines_array;
    char input;

    // make state (reading from file)
    State *state = make_state(argv[1]);

    // Loop
    while(1) {
        // get window dimensions
        if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) {
            printf("Unable to access line or column counts\n");
            exit(EXIT_FAILURE);
        }

        n_env_lines = w.ws_row;
        n_env_cols = w.ws_col;
//        printf("Number of lines is: %i\n", n_env_lines);

        // print state
        // print_state(state, n_env_cols, n_env_lines);

        // get input silently
        input = getch();

        // update(state)
        update_state(state, input);
    }

    // write file
    // write_state_to_file(argv[1]);
    // cleanup
    // free stuff





    return 0;
}
