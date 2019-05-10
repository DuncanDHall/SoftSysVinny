# Vinny: a Command-Line Text Editor in C

*May 8, 2019*

*Ilya Besancon and Duncan Hall*

# NB: this is not complete yet, we're still working on bugs

## Description

This is Vinny, a command-line text editor in C that clones some of vim's basic features. Vinny currently has limited functionality, but the core structure of a text editor was the primary goal for the project. Vinny allows switching of modes, navigation of a cursor, an insertion and deletion of characters, and a command mode for saving and quitting, all with a basic view displaying lines, line numbers, mode, and cursor.

## Progress

We started by spending some time figuring out the sort of code structure that would best support the objective, and have settled on a set of functions which sequentially call one another for this purpose. We will have a main loop in which we `handle_input()` and `draw_lines()`, passing a shared `State` to both routines. The program is currently reading lines from a file, formatting that lines array with extra blank lines, or trimming those lines according to the position of the "window" into the file, and printing the result to the terminal window. We are  working on fixing a small bug in that process, and adding a gutter containing line numbers. Next up will be to display a cursor using color codes, then creating a loop with keyboard input and execution of those commands. That will get us dangerously near our MVP!

If there is anything holding our project back at this point, it is our own personal bandwidth. Although the semester is coming to an overwhelming end, we will power through!



3) What were you able to get done?  Include in the report whatever evidence is appropriate to demonstrate the outcome of the project.

We were able to get Vinny somewhat working. Our code structure is logically split into two main functions that work with a shared `State`: `update_state` and `print_state`. Our program reads lines from a file and copies them into an array, formats the lines into a new array to include line numbers and blank lines (trimmed to length depending on the width of the window). It prints these formatted lines following a header indicating the current mode and file being edited. [[cursor]] [[keyboard input and model update]] [[model view controller]]


## Reflection: Did we achieve our learning goals? where did the project end up in terms of MVP

We want to learn more about creating text editors and using the command line more effectively. We'd also like to gain more experience with vim (a very useful tool for developers to work faster and more efficiently). We see this as an excellent project to put the concepts we've been learning about this year into practice. Creating a text editor will require careful memory management, file read/write, command line arguments, objects, and a carefully constructed architecture.

## Links

- [Github](https://github.com/DuncanDHall/vinny.git)
- [Trello](https://trello.com/b/1oTw5qhM/vinny)
