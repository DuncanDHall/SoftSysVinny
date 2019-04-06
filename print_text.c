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
char **read_lines_from_file(FILE *fp, int startline, int endline)
{
    printf("reading thru file\n");

    char *line = NULL;
    size_t lsize;
    int r;
    while ((r = getline(&line, &lsize, fp)) != -1) {
        array[i] = line;
        i++;
    }

    return array;
}

// TODO implement frame buffer/gui stuff
char **get_lines_to_print(FILE *fp, int top_line)
{
    int n_env_lines, n_env_cols;
    struct winsize w;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) {
        printf("Unable to access line or column counts\n");
        exit(1);
    }

    s_env_lines = w.ws_row;
    s_env_cols = w.ws_col;

    read_lines_from_file(fp, startline, endline);
}

int main (int argc, char *argv[])
{
    // printf("Enter name of a file you wish to see\n");
    // fgets(file_name, 25, stdin);
    //
    // printf("The contents of %s file are:\n", file_name);
    int top_line = 0;
    FILE *fp = fopen(argv[1], "r"); // opens file in read mode
    if (fp == NULL){
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    char * array[100];
    array = read_lines_from_file(fp,1,3); //reads lines

    // this is how we get window dimensions

    format_lines();

    fclose(fp);
    return 0;
}
