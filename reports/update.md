# Vinny: Project Update

*April 23, 2019*

*by Ilya Besancon and Duncan Hall*

## Project Goal

The goal of this project is to create **Vinny**: a command-line text editor in C that clones some of vim's basic features. At a lower bound, she will have the following features: up/down/left/right motions, toggle between normal, insert and command line modes, and character deletion (better for your pinky!), and saving to disk. This should be very reasonable to accomplish given the time-line of our project and our current competencies/resources. At an upper bound, we hope to further upgrade her skills so she can also understand more complex motions like multipliers, word motions, delete <motion>, change <motion>, string search, and any number of other vim-like behaviors.

## Learning Goals

We want to learn more about creating text editors and using the command line more effectively. We'd also like to gain more experience with vim (a very useful tool for developers to work faster and more efficiently). We see this as an excellent project to put the concepts we've been learning about this year into practice. Creating a text editor will require careful memory management, file read/write, command line arguments, objects, and a carefully constructed architecture.

## Progress

We started by spending some time figuring out the sort of code structure that would best support the objective, and have settled on a set of functions which sequentially call one another for this purpose. We will have a main loop in which we `handle_input()` and `draw_lines()`, passing a shared `State` to both routines. The program is currently reading lines from a file, formatting that lines array with extra blank lines, or trimming those lines according to the position of the "window" into the file, and printing the result to the terminal window. We are  working on fixing a small bug in that process, and adding a gutter containing line numbers. Next up will be to display a cursor using color codes, then creating a loop with keyboard input and execution of those commands. That will get us dangerously near our MVP!

If there is anything holding our project back at this point, it is our own personal bandwidth. Although the semester is coming to an overwhelming end, we will power through!

## Needed resources

We intentionally chose this project because we feel confident we can use it as a test of the knowledge we've gained in this class. Unlike Cynthesis (our previous project) we do not anticipate working with any frameworks or new external libraries.

## Next Steps

- Integrate a while loop structure with main (Done: redraw on keyboard input).
- Optimize printing to the full terminal window (Done: fix null bug, add gutter).
- Display location of cursor in file (Done: cursor position stored in `State` and drawn on each loop).
- Toggle between read and write modes (Done: banner at bottom to indicate mode, keyboard input parsed to switch between modes, write to file).

## Links

- [Github](https://github.com/DuncanDHall/vinny.git)
- [Trello](https://trello.com/b/1oTw5qhM/vinny)
