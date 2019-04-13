/**//*
 * This program will load a text file and print it to screen
 * 4/2/2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

// TODO get only lines in frame
void read_lines_from_file(FILE *fp, char ***lines_array_ptr, int start_line, int n_lines)
{
    char *line = NULL;
    size_t lsize;
    int line_counter = 0;
    //int index = 0;
    rewind(fp); //rewinds pointer to top of file

    //printf("reading thru file\n");
    /* Loop through until we are done with the file. */
    /* Get the next line */

    while(getline(&line, &lsize, fp) != -1) {
        //printf("Line is \n", line);

        /* Show the line details */
        //printf("line[%06d]: chars=%06zd, buf size=%06zu, contents: %s", line_counter,
        //  line_size, lsize, line);
        //printf("Conditional: %d, %d\n", line_counter >= start_line, line_counter < start_line + n_lines);
        if(line_counter >= start_line && line_counter < start_line + n_lines){
            (*lines_array_ptr)[line_counter - start_line] = strdup(line); //character pointer
            //printf("array %s\n", *lines_array_ptr[line_counter]);
            //printf("WROTE LINE\n");
            //printf("Line is: %s\n", line);
        }
        /* Increment our line count */
        line_counter++;
    }
    while(line_counter < start_line + n_lines) {
        (*lines_array_ptr)[line_counter] = strdup("\n");
        line_counter++;
    }
    //printf("Loop completed.\n");
    free(line); //free line buffer
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

// TODO: this function does tihs_____
void get_formatted_lines(FILE *fp, char ***lines_array_ptr, int start_line, int n_lines, int n_cols)
{
    int gutter_size;
    char **raw_lines;
    char *line;

    // get info about window size
    // calculate gutter size
    gutter_size = 2 + count_digits(start_line + n_lines);

    // get raw lines
    read_lines_from_file(fp, &raw_lines, start_line, n_lines);

    // write raw lines
    for (int i = 0; i < n_lines; i++) {
        line = malloc(n_cols * sizeof(char));
        memcpy(
            (void *)(&line + gutter_size),
            (void *) raw_lines[i],
            (strlen(raw_lines[i])+1-gutter_size) * sizeof(char)
        );
        // memcopy raw into line
    }

    // trim lines
    // lines = malloc(sizeof(char *[n_env_lines]));
    // for (int )

    // draw line numbers
}

void print_lines(char ***lines_array_ptr, int n_lines)
{
    for (int i = 0; i < n_lines; i++) {
        printf("%s", (*lines_array_ptr)[i]);
    }
}

int main (int argc, char *argv[])
{
    // temp
    int top_line = 3;

    // declarations
    struct winsize w;
    int n_env_lines, n_env_cols;
    char **lines_array;

    // opens a file
    FILE *fp = fopen(argv[1], "r"); // opens file in read mode
    if (fp == NULL){
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE); //bad news, quit program
    }

    // get window dimensions
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) {
        printf("Unable to access line or column counts\n");
        exit(EXIT_FAILURE);
    }

    n_env_lines = w.ws_row;
    printf("Number of lines is: %i\n", n_env_lines);
    n_env_cols = w.ws_col;

    // allocate space for 5 pointers to strings
    lines_array = malloc(n_env_lines * sizeof(char *)); // TODO: don't malloc

    //TODO create while loop here
    read_lines_from_file(fp, &lines_array, top_line, n_env_lines);
    // printf("Array firstline: %s\n", lines_array[1]);
    print_lines(&lines_array, n_env_lines);

    // read_lines_from_file(fp,lines_array, 3,3); //reads lines

    // this is how we get window dimensions

    //format_lines();
    // free(lines_array);
    // fclose(fp);
    return 0;
}
