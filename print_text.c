/**//*
 * VINNY IS ALMOST THERE
 * 5/8/2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <assert.h>
#include <signal.h>
#include <math.h>

#define KNRM  "\x1B[0m"  // standard color
#define KMAG  "\x1B[35m" // magenta
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define BACK1 "\x1B[34;42m" //background
#define BACK2 "\x1B[34;43m" //background
#define BACK3 "\x1B[34;44m" //background
#define NORM "\x1B[0m" //Reset:

void handle_sigint(int sig)
{
    printf("Caught signal %d\n", sig);
}

// Read all of the lines from file and put into an array
// line in an array pointed to by lines_array_ptrd .
int read_lines_from_file(char *file_name, char ***lines_array_ptr)
{
    char *line = NULL;
    size_t lsize;
    int n_lines = 0;

    // int start_of_line = 0;
    // int len_current_line = 0;
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
        line_counter++;
    }
    // free(line); //free line buffer
    fclose(fp);

    *lines_array_ptr = lines_array;
    // *n_lines_ptr = line_counter;
    return n_lines;
}


// inserts a string into another assuming appropriate buffer space
char *str_insert(char *destination, char *source, int loc) {
    char *rest = strdup(destination+loc);
    destination[loc] = '\0';
    strcat(destination, source);
    strcat(destination+strlen(source), rest);
    return destination;
}

//returns the number of digits in a number
int count_digits(int n)
{
    if(n == 0){
        return 1;
    }

    int c = 0;
    while (n) {
        n = n / 10;
        c++;
    }
    return c;
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
    state->n_lines = read_lines_from_file(file_name, &(state->lines)); //TODO change to num_lines
    strcpy(state->file_name, file_name);
    state->top_line = 0;
    // state->window_width = num_cols;
    // state->window_height = num_rows;
    state->mode = normal;
    state->cursor_row = 0;
    state->cursor_col = 0;

    return state;
}

/*
 * prints a 2d array of characters
 */
void print_lines(char **lines_array_ptr2, int n_lines, int top)
{
    // for (int j = top; j < (top + n_lines); j++) {
    //     printf("%d\n", (int) strlen(lines_array_ptr2[j]));
    // }
    // printf("top: %d  ", top);
    for (int j = top; j < (top + n_lines); j++) {
        // printf("here\n");
        printf("%s",lines_array_ptr2[j]);
        // if(j > 0) {
        //     break;
        // }
    }
}
/*   Prints out the current "state" object
    -formatted lines
    -printed to window
    -prints cursor with color indicator
    -prints Mode
*/
void print_state_no_lines(State *state, int num_columns, int num_rows){

    // print header
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
    printf(KMAG "\nFILE: %s," KCYN " MODE: %s," KYEL " CURSOR %d, %d," KRED " TYPE ':q' TO EXIT\n", state->file_name, mode_word, state->cursor_row, state->cursor_col);
    printf(KNRM);

    char **formatted_line_ptrs = malloc(sizeof(char*) * num_rows);
    int gutter_size = 2 + count_digits(state->top_line + state->window_height);
    char *raw_line;
    char *my_line;

    for (int i = state->top_line; i < (num_rows + state->top_line); i++) {

        // empty line for concatination
        char new_line[state->window_width+20];
        new_line[0] = '\0';

        // populate line if past end of file
        if (i >= state->n_lines){
            formatted_line_ptrs[i - state->top_line] = "\n";
            continue;
        }

        // get raw_line
        raw_line = (state->lines)[i];


        // truncate raw_line;
        if(strlen(raw_line) > state->window_width + 1 - gutter_size - 5){ // TODO get this right
            raw_line[state->window_width - gutter_size] = '\n';
            raw_line[state->window_width - gutter_size + 1] = '\0';
        }

        if(strlen(raw_line) + gutter_size <= num_columns + 1){
            // strcat(new_line, raw_line);
            str_insert(new_line, raw_line, 0);
        }

        //str_insert(char *destination, char *source, int loc);

        // printf(KNRM);
        if (i == state->cursor_row) {
            str_insert(new_line, NORM, state->cursor_col + 1);
            if (state->mode == normal) str_insert(new_line, BACK1, state->cursor_col);
            else if (state->mode == insert) str_insert(new_line, BACK2, state->cursor_col);
            else if (state->mode == command) str_insert(new_line, BACK3, state->cursor_col);
            // str_insert(new_line, BACK, state->cursor_col);
        }

        // insert a padded line number into the line
        // char *padded_line_number;
        // sprintf(padded_line_number, "%*d  ", gutter_size-2, i);
        // str_insert(new_line, padded_line_number, 0);

        //     char cursor_line[] = "\0";
        //     strncpy(cursor_line, new_line, state->cursor_col+gutter_size);
        //     strcat(cursor_line, BACK);
        //     strncat(cursor_line, new_line+(state->cursor_col)+gutter_size, 1);
        //     strcat(cursor_line, NORM);
        //     strcat(cursor_line, new_line+(state->cursor_col)+gutter_size+1);
        //
        //     strcpy(new_line, cursor_line);
        //     // printf("there\n");
        // }

        formatted_line_ptrs[i-state->top_line] = strdup(new_line); //line;
    }
    print_lines(formatted_line_ptrs, num_rows - 2, state->top_line);
}


/*   Prints out the current "state" object
    -formatted lines
    -printed to window
    -prints cursor with color indicator
    -prints Mode
*/
void print_state_with_lines(State *state, int num_columns, int num_rows){

    // print header
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
    printf(KMAG "\nFILE: %s," KCYN " MODE: %s," KYEL " CURSOR %d, %d," KRED " TYPE ':q' TO EXIT\n", state->file_name, mode_word, state->cursor_row, state->cursor_col);
    printf(KNRM);

    char **formatted_line_ptrs = malloc(sizeof(char*) * num_rows);
    int gutter_size = 2 + count_digits(state->top_line + state->window_height);
    char *raw_line;
    char *my_line;
    char *padded_line_number;

    for (int i = state->top_line; i < (num_rows + state->top_line); i++) {

        // empty line for concatination
        char new_line[state->window_width+20];
        new_line[0] = '\0';

        // populate line if past end of file
        if (i >= state->n_lines){
            formatted_line_ptrs[i - state->top_line] = " ~ \n";
            continue;
        }

        // get raw_line
        raw_line = (state->lines)[i];

        // truncate raw_line;
        // if(strlen(raw_line) > state->window_width + 1 - gutter_size -5){ // TODO get this right
        //     raw_line[state->window_width - gutter_size] = '\n';
        //     raw_line[state->window_width - gutter_size + 1] = '\0';
        // }

        // insert raw_line
        str_insert(new_line, raw_line, 0);

        // add the cursor
        if (i == state->cursor_row) {
            str_insert(new_line, NORM, state->cursor_col + 1);
            str_insert(new_line, BACK1, state->cursor_col);
        }

        // insert a padded line number into the line
        sprintf(padded_line_number, "%*d", gutter_size-2, i);
        str_insert(new_line, padded_line_number, 0);

        // if (i == state->cursor_row) {
        //     char cursor_line[] = "\0";
        //     strncpy(cursor_line, new_line, state->cursor_col+gutter_size);
        //     strcat(cursor_line, BACK);
        //     strncat(cursor_line, new_line+(state->cursor_col)+gutter_size, 1);
        //     strcat(cursor_line, NORM);
        //     strcat(cursor_line, new_line+(state->cursor_col)+gutter_size+1);
        //
        //     strcpy(new_line, cursor_line);
        //     // printf("there\n");
        // }

        formatted_line_ptrs[i-state->top_line] = strdup(new_line); //line;
    }
    print_lines(formatted_line_ptrs, num_rows - 2, state->top_line);
}


/*   Prints out the current "state" object
    -formatted lines
    -printed to window
    -prints cursor with color indicator
    -prints Mode
*/
void print_state(State *state, int num_columns, int num_rows){

    // print header
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
    printf(KMAG "\nFILE: %s," KCYN " MODE: %s," KYEL " CURSOR %d, %d," KRED " TYPE ':q' TO EXIT\n", state->file_name, mode_word, state->cursor_row, state->cursor_col);
    printf(KNRM);


    int gutter_size = 2 + count_digits(state->top_line + num_rows);
    char **formatted_line_ptrs = malloc(sizeof(char*) * num_rows);
    char *raw_line;
    char *my_line;
    char padded_line_number[gutter_size-1]; //TODO change to: gutter_size - 1

    // printf("MAX IS %d\n", num_rows + state->top_line);

    for (int i = state->top_line; i < (num_rows + state->top_line); i++) {
        printf(KYEL);

        char new_line[state->window_width+20]; // extra buffer for color codes
        new_line[0] = '\0';
        // new_line = malloc((num_columns + 1) * sizeof(char));

        // populate line if past end of file
        if (i >= state->n_lines){
            formatted_line_ptrs[i - state->top_line] = "\n";
            continue;
            // raw_line = (state->lines)[i];
        }
        // else {
        //     // printf("IN ELSE\n");
        //     raw_line = "something\n";
        // }

        // line_length = strlen(raw_line);
        // printf("Line length is: %d\n", line_length);
        // printf("new_line is: %s\n", new_line);

        // add the padded_line_ number
        // printf("padded line number length: %d\n", (int) strlen(padded_line_number));
        // sprintf(padded_line_number, "%*d", gutter_size-2, i);
        // printf("padded number is: %s\n", padded_line_number);
        // strcat(new_line, padded_line_number);
        // printf("new_line is: %s\n, and this long: %d\n", new_line, (int) strlen(new_line));

        // add gutter spaces
        // strcat(new_line, "  ");

        // get line
        raw_line = (state->lines)[i];

        // if no cursor add rest of line
        // if (i != state->cursor_row) {
            // no truncation
            if(strlen(raw_line) + gutter_size <= num_columns + 1){
                // printf("CAT raw string to new string\n");
                strcat(new_line, raw_line);
                // printf("new line + cat: %s\n", new_line);
            }
            // with truncation
            // else{
            //     strncat(new_line, raw_line, num_columns-gutter_size); //dest, source, size
            //     if (strchr(new_line, '\n') == NULL){
            //         strcat(new_line, "\n");
            //     }
            // }
        // }


        // if (i == state->cursor_row) {
        //     char cursor_line[] = "\0";
        //     strncpy(cursor_line, new_line, state->cursor_col+gutter_size);
        //     strcat(cursor_line, BACK);
        //     strncat(cursor_line, new_line+(state->cursor_col)+gutter_size, 1);
        //     strcat(cursor_line, NORM);
        //     strcat(cursor_line, new_line+(state->cursor_col)+gutter_size+1);
        //
        //     strcpy(new_line, cursor_line);
        //     // printf("there\n");
        // }

        //fill gutter with spaces
        // for(b = 0; b < gutter_size; b++) {
        //     new_line[b] = ' ';
        // }
        // new_line[b] = '\0'; //adding null terminator
        //
        // // char line_number[gutter_size - 2];
        // int offset = gutter_size - 2 - count_digits(i);
        // // printf("offset is: %d\n", offset);
        // sprintf(new_line + offset, "%d", i); //convert int to string
        // strcat(new_line, "  ");// // printf("%s\n", new_line);
        //
        //
        // // add cursor to raw_line
        // // printf("here\n");
        // printf("i is: %d\n", i);
        // memset(my_line, '\0', sizeof(my_line));
        // strcpy(my_line, new_line);
        formatted_line_ptrs[i-state->top_line] = strdup(new_line); //line;
        // printf("strdup line: %s\n", new_line);
        // printf("raw line is: %s", raw_line);
    }
    // printf("did we make it?\n");
    // printf(KRED);
    print_lines(formatted_line_ptrs, num_rows - 2, state->top_line);
    // print_lines((state->lines), 24);
    // TODO: for line in formatted_line_ptrs free(line)
    printf(KBLU);
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
    if (state->cursor_row > state->top_line + state->window_height){
        state->cursor_row = state->top_line + state->window_height;
    }

    state->cursor_col += d_col;
    if (state->cursor_col < 0) state->cursor_col = 0;
    if (state->cursor_col > line_len - 1) state->cursor_col = line_len - 1;
    // printf("cursor at row %d col %d\n", state->cursor_row, state->cursor_col);

    // adjust window position
    // TODO: this doesn't work
    // printf("state->top_line temporarily: %d\n", state->top_line);
    // printf("window_height: %d\n", state->window_height);
    // if (state->cursor_row > state->top_line + state->window_height) {
    //     // scroll down
    //     state->top_line = state->cursor_row - state->window_height;
    // }
    // else if (state->cursor_row < state->top_line) {
    //     // scroll up
    //     state->top_line = state->cursor_row;
    // }
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

    state->lines[state->cursor_row] = strdup(new_line);
    state->cursor_col++;
    // free(line);
}

void insert_mode_handler(State *state, char input)
{
    char s[2];
    s[0] = input;
    s[1] = '\0';
    switch (input) {
        case '\x1b': // 1b is the ESC character's hex value
            state->mode = normal;
            break;
        default:
            str_insert(state->lines[state->cursor_row], s, state->cursor_col);
            move_cursor(state, 0, 1);
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
        // printf("Normal mode handling input: %c\n", input);
        normal_mode_handler(state, input);
    }
    else if (state->mode == insert) {
        // printf("Insert mode handling input: %c\n", input);
        insert_mode_handler(state, input);
    }
    else if (state->mode == command) {
        // printf("Command mode handling input: %c\n", input);
        command_mode_handler(state, input);
    }
}



int main (int argc, char *argv[])
{
    // signal(SIGINT, handle_sigint);
    // declarations
    struct winsize w;
    int n_env_lines, n_env_cols;
    // char **lines_array;
    char input;
    // input = getch();
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
        state->window_width = n_env_cols;
        state->window_height = n_env_lines;

        // print(state, width, height)
        print_state_no_lines(state, n_env_cols, n_env_lines);
        printf(KMAG);

        // get input silently
        // input = getch();
        system ("/bin/stty raw");
        input = getchar();
        system ("/bin/stty cooked");

        // update(state)
        update_state(state, input);
    }

    // write file
    // write_state_to_file(argv[1]);
    // cleanup
    free(state);

    return 0;
}
