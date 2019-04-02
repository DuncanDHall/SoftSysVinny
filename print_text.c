/**//*
 * This program will load a text file and print it to screen
 * 4/2/2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO get only lines in frame
char **read_lines_from_file(char *file_path)
{
    FILE *fp = fopen(file_path, "r"); // opens file in read mode

    if (fp == NULL){
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    printf("reading thru file\n");

    char *line = NULL;
    size_t lsize;
    int r;
    while ((r = getline(&line, &lsize, fp)) != -1) {
        printf("%s", line);
    }

    // TODO return lines
}

// TODO implement frame buffer/gui stuff

int main (int argc, char *argv[])
{
    // printf("Enter name of a file you wish to see\n");
    // fgets(file_name, 25, stdin);
    //
    // printf("The contents of %s file are:\n", file_name);

    read_lines_from_file(argv[1]); //reads lines
    return 0;
}
