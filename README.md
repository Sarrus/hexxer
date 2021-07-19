# hexxer

A brute force solver for New Scientist puzzle number 90, 
'Colourful Beehives'.

You can see the original puzzle [here](https://www.newscientist.com/article/mg24833135-800-puzzle-89-get-your-felt-tip-pens-out-for-the-colourful-beehive/): 


My computer is better at puzzles than I am so I had it solve this
one for me. A keen eyed human solver might notice that there 
is more than one valid solution to the puzzle, hexxer can find
104 visually unique solutions, it can also output them in
HTML format for easy viewing.

## Building

You will need Cmake and your favorite C compiler. It definitely
works on MacOS and Linux and may work on other platforms. I've 
tested it on x86_64 and ARM64.

Prepare the build files:

    $ cd /path/to/source
    $ cmake ./

Run the build:

    $ cmake --build ./

## Running

With no arguments, hexxer will run in serial mode and output 
the ID of each valid solution found:

    $ ./hexxer 
    Solution found at hexagon no. 39990607, no visual matches found. 0.058194% of all hexagons tried, 1 solutions found so far, 1 visually unique.
    Solution found at hexagon no. 103552189, no visual matches found. 0.150688% of all hexagons tried, 2 solutions found so far, 2 visually unique.
    Solution found at hexagon no. 152953033, visually matches solution no. 103552189 0.222576% of all hexagons tried, 3 solutions found so far, 2 visually unique.
    ...

The ID of a solution can be used to reconstruct it in human 
readable form:

    $ ./hexxer -d 39990607
          /\  /\  /\
         /  \/  \/  \
        | B | B | R |
        /\  /\  /\  /\
       /  \/  \/  \/  \
      | Y | Y | Y | B |
      /\  /\  /\  /\  /\
     /  \/  \/  \/  \/  \
    | R | G | R | G | Y |
     \  /\  /\  /\  /\  /
      \/  \/  \/  \/  \/
      | G | R | R | R |
       \  /\  /\  /\  /
        \/  \/  \/  \/
        | R | R | R |
         \  /\  /\  /
          \/  \/  \/

Optimum performance on processors with multiple cores can be
obtained by running in parallel mode:

    $ ./hexxer -j [number of cores to use]

This is the recommended way to run. In parallel mode, hexxer
will reduce it's own priority (by setting it's niceness to 5) to 
avoid interfering with more serious processing.

To get the full set of run options:

    $ ./hexxer -h
    Usage: hexxer [options]
    -d  Display the hexagon specified by its ID then exit.
    -h  Display this help.
    -j  Number of parallel jobs to run. (Runs in serial mode if unspecified.)
    -m  Render and print visual matches.
    -o  Write the ID of each solution as it is found to the specified location. ('-' to write to stdout.)
    -p  Render and print all discovered solutions.
    -r  Generate an HTML report of unique solutions saved in the indicated location.
    -s  Stop when a solution is found.
    -t  Test a single solution specified by its ID to see if it is considered valid.
    -u  Write the ID of each visually unique solution as it is found to the specified location. ('-' to write to stdout.)

Depending on your hardware hexxer will take anywhere between
20 minutes (8 core Intel processor) to 90 minutes (Raspberry Pi
4 model B).

## Known Issues

* Solutions are rendered in the wrong orientation with the 
  'always red' hexagon on the bottom right instead of the 
  centre left.
* Solutions in the HTML report overlap.
* The progress ticker does not display in serial mode.