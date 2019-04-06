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
void read_lines_from_file(FILE *fp, char **lines_array, int start_line, int n_lines)
{
    rewind(fp); //rewinds pointer to top of file

    char *array[n_lines];
    int i; //array index
    char *line = NULL;
    size_t lsize;
    int r;
    ssize_t line_size;
    int line_counter = 0;
    int writeflag;

    /* Get the first line of the file. */
    line_size = getline(&line, &lsize, fp);

    printf("reading thru file\n");
    /* Loop through until we are done with the file. */
    while (line_size >= 0)
    {
      /* Increment our line count */
      line_counter++;

      /* Show the line details */
      printf("line[%06d]: chars=%06zd, buf size=%06zu, contents: %s", line_counter,
          line_size, lsize, line);
      //flag goes up: ready for reading
      if(line_counter == start_line){
        writeflag = 1;
      }
      //flag goes down, done writing
      if(line_counter == (start_line + n_lines)){
        writeflag = 0;
      }
      if(writeflag){
        array[i] = line;
        i++;
      }
      /* Get the next line */
      line_size = getline(&line, &lsize, fp);
      //printf("Line is \n", line);
    }
    /*
    while ((r = getline(&line, &lsize, fp)) != -1) {
        if(startline == linecounter){
            array[i] = line;
            i++;
        }
        linecounter++;
    }
    */
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

//this function does tihs_____
// void get_lines_to_print(FILE *fp, char **lines_array, int start_line, int n_lines)
// {
//     int gutter_size;
//     char **raw_lines;
//     char **lines;
//
//     // get info about window size
//     // calculate gutter size
//     gutter_size = 2 + count_digits(top_line + n_env_lines);
//
//     // get raw lines
//     raw_lines = read_lines_from_file(fp, lines_array, start_line, end_line);
//
//     // trim lines
//     // lines = malloc(sizeof(char *[n_env_lines]));
//     // for (int )
//
//     // draw line numbers
// }

void print_lines(char **lines_array, int n_lines)
{
    for (int i = 0; i < n_lines; i++) {
        printf("%s\n", lines_array[i]);
    }
}

int main (int argc, char *argv[])
{
    // temp
    int top_line = 0;

    // declarations
    struct winsize w;
    int n_env_lines, n_env_cols;
    char **lines_array;

    // opens a file
    FILE *fp = fopen(argv[1], "r"); // opens file in read mode
    if (fp == NULL){
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    // get window dimensions
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) {
        printf("Unable to access line or column counts\n");
        exit(EXIT_FAILURE);
    }
    n_env_lines = w.ws_row;
    n_env_cols = w.ws_col;

    // allocate space for 5 pointers to strings
    lines_array = (char**) malloc(n_env_lines*sizeof(char*));
    read_lines_from_file(fp, lines_array, top_line, n_env_lines);

    print_lines(lines_array, n_env_lines);

    // read_lines_from_file(fp,lines_array, 3,3); //reads lines

    // this is how we get window dimensions

    //format_lines();

    fclose(fp);
    return 0;
}
