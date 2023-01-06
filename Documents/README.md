# coms327project

## Overview of part 1

#### Implemented features (part 1): All the features

 - Readme file, that describes implemented features (you are here)

 - Developers file, that gives an overview of our implementation

 - Working Makefile. Typing make creates an executable called check

 - Reads from filename passed as an argument

 - Reads from stdin if no filename argument

 - Normal output to stdout

 - Correctly formatted output on valid inputs

 - Reports correct number of covered cards on valid inputs

 - Reports correct numebr of stock cards on valid inputs

 - Reports correct number of waste cards on valid inputs

 - No error messages on valid inputs

 - Error messages to stderr

 - Formatting error messages include line numbers

 - Appropriate error messages for formatting errors

 - Check that the tableau piles are legal

 - check for duplicate cards

 - check for missing cards

 
## Overview of Part2
 
	 The scope of this second portion is to be able to create check from part1 and advance for part2
	 With advance, the goal is to scan the files moves and execute them if possible
	 If a move is invalid the game board should print its current status before the error and report which move is invalid
	 In addition to scanning the moves there are new cases to check before reading the file input
	 Switch cases in the command line need to be read that affect the number of moves to be read in the given file
	 Another switch case to look into is a given output file and how to display the information
 
#### Implemented features Part2: All the new features
 
 - Update to Readme (currently viewing)

 - Updated Developers with new methods and implementation changes

 - Updated Makefile to include helper classes, part2, and advance executable

 - Check for  argument switches -m -x -o (Still reads from stdinput or from an argument)

 - Error messages thrown when moves are invalid

 - Outputs the current state of the tableau in two different ways (Depends on argument -x)

 - Outputs to a txt file if -o argument is passed with a filename

## Overview of Part3

	The scope of this third portion is to be able to create an executable called winnable, which
	reads in an input file like the other two executables do, then performs a search on that game
	to find a winning sequence of moves. It is not required, but we do, find a shortest such sequence
	of moves. Additionally, certain switches on the command line affect the state of the program,
	limiting the depth of the search, forcing moves to the foundations, and printing diagnostic information
	as it is a hefty computation which may undergo several minutes of operation in some cases.
	After finding the shortest sequence or failing to, the program prints that and the sequence if one
	was found. However, it is left as an excersize to the interested user to take the outputted gamestate,
	guaranteed to be winnable, and actually play it through to its completion.

#### Implemented features of part3: All the new features

 - Updated Readme (You are here!)

 - Updated Developers file with new methods, implementation overview, and cool extra options

 - Updated Makefile to include more helper methods and classes, as well as some extra tools for the programmers

 - Finds and outputs a winning sequence to the game of Klondlike that is passed in

 - Checking for switches -m, -c (more or less), -f, and reading from a given filename or stdin

 - Output of the proper form (winnable/not winnable, move sequence) with -v switch giving extra information

 - Works for any Klondlike rules. Except the ones you make up when you start losing.

## Overview of Part 4

    The scope of the fourth and final part of the overall project was to implement a visual display of
    the game and be able to play it. Through the use of ncurses different areas on the display are filled
    out to show information on how to play, the foundation pile, waste and stock, and lastly the tableau.
    Displaying these elements, manipulating them and then updating them all from a simple text file or 
    randomly generated file. 
    
#### Implemented features of part4: All the new features

 - Updated Readme (for the final time) (You are here !)
 
 - Updated Developers file with new method information, and implementation overview (Mainly one file)
 
 - Checks for switches -f, -s (not -1, -3, -l)
 
 - Updated makefile to include game and build the display file
 
 - Creates a playable game
    - Can undo a move
    - Restart a game
    - Can be quit without issues
    - Can generate new random game
    - Can save current game to file