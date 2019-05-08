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

#define KNRM  "\x1B[0m" //standard color
#define KMAG  "\x1B[35m" //magenta
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

// Read all of the lines from file and put into an array
// line in an array pointed to by lines_array_ptrd .
int read_lines_from_file(char *file_name, char ***lines_array_ptr)
{
    char *line = NULL;
    size_t lsize;
    int n_lines = 0;

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
        if (c == '\n') n_lines++;
    }
    char **lines_array = malloc(sizeof(char *) * n_lines);


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
    // *n_lines_ptr = line_counter;
    return n_lines;
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
/*
void get_formatted_lines(State *state, char ***lines_array_ptr, int n_lines, int n_cols)
{
    int gutter_size;
    char **raw_lines;
    char *line;

    // get info about window size
    // calculate gutter size
    gutter_size = 2 + count_digits(start_line + n_lines);

    // get raw lines
    //read_lines_from_file(fp, &raw_lines, start_line, n_lines);

    int line_length = strlen(raw_lines[i])+1;

    char **formatted_line_ptrs = malloc(sizeof(char*)*n_lines);
    // write raw lines
    for (int i = 0; i < n_lines; i++) {
        line = malloc(n_cols * sizeof(char));
        *formatted_line_ptrs[i] = line;
        // memcopy raw into line (destination, source, number of bytes)
        if(line_length + gutter_size < n_cols){
            memcpy(
                (void *)(&line + gutter_size),
                (void *) raw_lines[i],
                (line_length) * sizeof(char)
            );
        }
    }
    *lines_array_ptr = raw_lines;

    // trim lines
    // lines = malloc(sizeof(char *[n_env_lines]));
    // for (int )

    // draw line numbers
}
*/


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
    state->n_lines = read_lines_from_file(file_name, &(state->lines)); //TODO change to num_lines
    strcpy(state->file_name, file_name);
    state->top_line = 0;
    // state->window_width = num_cols;
    // state->window_height = num_rows;
    state->mode = normal;
    state->cursor_row = 0;
    state-> cursor_col = 0;

    return state;
}


void print_lines(char **lines_array_ptr, int n_lines, int top)
{
    // printf("printing lines\n");
    printf("top: %d", top);
    for (int j = top; j < (top + n_lines); j++) {
        printf("%s",lines_array_ptr[j]);
    }
}

/*   Prints out the current "state" object
    -formatted lines
    -printed to window
    -prints cursor with color indicator
    -prints Mode
*/
void print_state(State *state, int num_columns, int num_rows){
    //print u"\u001b[31mHelloWorld\u001b[0m";
    int mode = state->mode;
    char mode_word[10];
    switch(mode){
        case 0:
            strcpy(mode_word, "normal");
            break;
        case 1:
            strcpy(mode_word, "insert");
            break;
        case 2:
            strcpy(mode_word, "command");
            break;
    }
    //printf("\033[1;31m");
    printf(KMAG "FILE: %s," KCYN " MODE: %s.\n", state->file_name, mode_word);
    printf(KNRM);
    // printf("Number of lines in file is: %d\n", state->num_lines);
    // int n_lines
    int gutter_size;
    // char **raw_lines;
    char *new_line;
    char spaces[5] = "    ";
    char zero[3] = "0";
    // char numbers[3];
    // calculate gutter size
    gutter_size = 2 + count_digits(state->top_line + num_rows);
    // printf("Gutter is %d\n", gutter_size);

    char **formatted_line_ptrs = malloc(sizeof(char*) * num_rows);
    // write raw lines

    char strC[50];

    char *raw_line;
    int line_length;
    int b;

    for (int i = state->top_line; i < (num_rows + state->top_line); i++) {
        // printf("i is %d\n", i);
        new_line = malloc((num_columns + 1) * sizeof(char));
        // printf("Line length is %d\n", line_length);

        if(i < state->n_lines){
            // printf("IN IF STATEMENT #1\n");
            raw_line = (state->lines)[i];
            // printf("giberrish/ :%s\n", newline);
             //accounting for endchar
        }
        else {
            // printf("IN ELSE\n");
            raw_line = "something\n";
        }

        line_length = strlen(raw_line);
        // printf("newline is: %s\n", newline);

        //fill gutter with spaces
        for(b =0; b < gutter_size; b++) {
            new_line[b] = ' ';
        }
        new_line[b] = 'c'; //adding null terminator

        // char line_number[gutter_size - 2];
        int offset = gutter_size - 2 - count_digits(i);
        // printf("offset is: %d\n", offset);
        sprintf(new_line + offset, "%d", i); //convert int to string
        strcat(new_line, "  ");// // printf("%s\n", new_line);

        if(line_length + gutter_size < num_columns){
            strncat(new_line, raw_line, line_length);
        }
        else{
            strncat(new_line, raw_line, num_columns-gutter_size); //dest, source, size
        }
        formatted_line_ptrs[i] = new_line; //line;
        // printf("line is: %s\n", line);
    }
    print_lines(formatted_line_ptrs, num_rows - 1, state->top_line);
    // print_lines((state->lines), 24);
}



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

        // print(state, width, height)
        print_state(state, n_env_cols, n_env_lines);

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
