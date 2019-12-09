/*
* Project Option 4 for the PHY2027 module
* Author: Lewis Howell, ljh252@exeter.ac.uk, 670023355
* Date: 08/12/19
*/

/*
 Program description: Fully functional simulator for Conway's Game of Life with toroidal or fixed boundaries.
                        Allows the user to run the simulation multiple times, animating results and option to save to file.
                        Option to read in results from file to pick up where you left off.
                        Allow user to change the game rules to several pre-sets.
                        Options include: 1) Start from a grid of random state cells.
                                            - Size of grid is chosen, cell alive/dead state random
                                         2) Start from a customised grid of cells
                                            - Size of grid is chosen
                                            - User can add as many living cells as desired
                                         3) Start from a pre-set grid or custom grid, reading plain text array from a file.
                                            - Files contain grids for:
                                                * Pulsar
                                                * Penta-Decathlon Oscillator
                                                * Glider
                                                * Spaceship
                                                * Glider Gun
                                                * 'Die Hard' Eliminator
                                                * Custom grid i.e one saved previously or written in plain text file.
 */

// Libraries needed
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h> // Add delay into animations

#define ALIVE 1
#define DEAD 0
#define TIME_INTERVAL 100 // Time interval between animation frmes (milliseconds)
#define GAME_EPOCHS 50 // Default number of iterations for animation
#define NMAX 50 // maximum tested number of rows and columns for game
#define ASCII_ADJUST 48 // Map ASCII for integers to their denary (48 in ASCII -> 0 )
#define NEWLINE_CHAR '\n' // The newline char used in files
#define CUSTOM_BOARD_FILE "custom_board.txt" // The file to store the 'custom' board when save is chosen

// Define a structure for a cell with a state 'alive' which can be either ALIVE or DEAD and store the number of neighbours
typedef struct cell{
    bool alive;
    int n_alive_neighbrs;
}Cell;

// Prototype function definitions
int get_n_elements(void);
Cell** create_board(int n_rows, int n_cols); // set up double pointer to the board
void update_rules(int *death_overpop, int *death_underpop, int *birth_repro);
void delay(int number_of_seconds); // create delay between animation frames
int up_coord(int coord, int bound); // toroidal boundary conditions
int down_coord(int coord, int bound); // toroidal boundary conditions
void add_living_cell(Cell **board, int n_rows, int n_cols, int x, int y);
void calc_n_neighbours(Cell **board, int n_rows, int n_cols);
void print_board(Cell **board, int n_rows, int n_cols);
void free_board(Cell **board, int n_rows, int n_cols); // free dynamic memory
int update_board(Cell **board, int n_rows, int n_cols, int fixed_bounds, int death_overpop, int death_underpop, int birth_repro);
void play_game(Cell **board, int n_rows, int n_cols, int fixed_bounds, int death_overpop, int death_underpop, int birth_repro);
void save_board(Cell **board, int n_rows, int n_cols, int fixed_bounds);

int main(){


	printf("-------------------------------------------\n");
    printf("           Conway's Game of Life           \n");
    printf("-------------------------------------------\n");

	// Simple menu to select setup for grid
    unsigned int option = 0; // flag for option chosen
    int death_overpop = 3, death_underpop = 2, birth_repro = 3; // The default game rules

    do{
        printf("\n\nSelect Gamemode:\n");
        printf("\t1: Random Grid\n");
        printf("\t2: Custom Grid\n");
        printf("\t3: Pre-set Grids\n");
        printf("\t4: Change game rules\n");
        printf("\t5: Quit\n");
        scanf("%d",&option); // Take user input

        int fixed_bounds = 0; // Whether to use hard boundaries -> 1, or toroidal boundary conditions -> 0 (grid is wrapped about x and y)
        int n_rows, n_cols; // Number rows and columns in the Game of Life Board


        switch (option){
            case 1:{ // Random Grid Selected

                // Ask for input of n rows and n columns
                printf("Please input number of rows as a integer:\n ");
                n_rows = get_n_elements();
                printf("Please input number of columns as a integer:\n ");
                n_cols = get_n_elements();

                // Declare the board
                Cell **board = create_board(n_rows,n_cols);

                ////Fill the board with random states, either ALIVE or DEAD.
                for (int i = 0; i < n_rows; i++){
                    for (int j = 0; j < n_cols; j++){
                        board[i][j].alive = rand()%2; // random value for ALIVE or DEAD
                    }
                }

                // Run the simulation
                play_game(board, n_rows, n_cols, fixed_bounds, death_overpop, death_underpop, birth_repro);

                option = 0; // Reset option to allow user to play again
                break;}
            case 2:{ // Custom Grid Selected

                // Ask for input of n rows and n columns
                printf("Please input number of rows as a integer: ");
                n_rows = get_n_elements();
                printf("Please input number of columns as a integer: ");
                n_cols = get_n_elements();

                // Declare the board
                Cell **board = create_board(n_rows,n_cols);

                // Allow user to add living cells to the board as many times as desired
                int x, y; // integers to hold coords of cell to add to grid
                bool add_cell=true; // flag for when user is finished adding cells

                while(add_cell==true){
                    printf("Add cell (or -1 to run simulation)\nx = ");
                    scanf("%d",&x); // Take user input for x coord

                    if (x == -1){ // if -1, user indicated finished adding cells
                        add_cell = false;
                    }else{
                        printf("y = ");
                        scanf("%d",&y); // Take user input for y coord of cell to add
                        add_living_cell(board,n_rows,n_cols,x,y); // add cell to grid (with error checking)
                        system("cls"); // clear console
                        print_board(board,n_rows,n_cols); // print the board
                    }
                }

                // Run the simulation
                play_game(board, n_rows, n_cols, fixed_bounds, death_overpop, death_underpop, birth_repro);

                option = 0; // Reset option to allow user to play again
                break;}
            case 3:{ // Pre defined grids

                // Menu for choosing pre set boards from files
                printf("\nPre-set Options\n");
                printf("\t1: Pulsar - Oscillator - Period 3\n");
                printf("\t2: Penta-Decathlon - Oscillator - Period 15\n");
                printf("\t3: Glider - Diagonal translation\n");
                printf("\t4: Spaceship - Horizontal translation\n");
                printf("\t5: Glider Gun - Produces a glider every 30 generations\n");
                printf("\t6: Die Hard - Methuselah - dies after 130 generations\n");
                printf("\t7: Custom grid from file %s\n",CUSTOM_BOARD_FILE);
                scanf("%d",&option); // Take user input

                FILE *readfile; // Pointer to a file
                do{
                    switch (option){
                        case 1:{ // Pulsar
                            readfile = fopen("pulsar.txt", "r"); // Try to open file to read in grid from
                            break;
                        }case 2:{ // Penta-decathlon
                            readfile = fopen("Penta-decathlon.txt", "r");
                            break;
                        }case 3:{ // Glider
                            readfile = fopen("glider.txt", "r");
                            break;
                        }case 4:{ // Medium Spaceship
                            readfile = fopen("spaceship.txt", "r");
                            break;
                        }case 5:{ // Gospal Glider gun
                            readfile = fopen("glider_gun.txt", "r");
                            break;
                        }case 6:{ // Die-hard eliminator
                            readfile = fopen("die_hard.txt", "r");
                            break;
                        }case 7:{ // Custom board from file
                            readfile = fopen(CUSTOM_BOARD_FILE, "r");
                            break;
                        }default:{ // Error if wrong number chosen
                            printf("[ERROR] Please choose a pre-set from the menu");
                        }
                    }
                }while(option > 7 || option < 1); // loop until user chooses valid value

                // Error check file is found and opened correctly
                if (readfile == NULL){
                    printf("[ERROR]: File does not exist");
                    exit(EXIT_FAILURE);
                }
                // Read in the number of rows and cols given in first line of the file.
                if (fscanf(readfile, "n_rows:%d, n_cols:%d, fixed_bounds:%d", &n_rows, &n_cols, &fixed_bounds) == EOF){
                    printf("[ERROR]: Grid size header missing from file. Specify as n_rows,n_columns"); // Error if header missing
                    exit(EXIT_FAILURE);
                }

                // Declare the board
                Cell **board = create_board(n_rows,n_cols);

                // Read in the cell states in the grid from file.
                char c; // Hold each cell state. '1' Represents living cell, '0' represents dead cell
                int col = 0, row = -1; // flags for coords of each cell in grid
                while((c = fgetc(readfile)) != EOF) { // loop until end of file, taking in 1 char at a time
                    if (row > n_rows+1 || col > n_cols){
                        // Error check that the grid is rectangular and the correct size specified, preventing overflow
                        printf("\n[ERROR]: Read grid from file failed, dimensions (%d,%d) exceed those specified in the file header (%d,%d)",row,col,n_rows,n_cols);
                        exit(EXIT_FAILURE);
                        break;
                    }
                    if (c == NEWLINE_CHAR){ // If newline char reached add 1 to the row and reset the column flag to 0
                        row += 1;
                        col = 0;
                        printf("\n");
                    }else{
                        if (c-ASCII_ADJUST != 0 && c-ASCII_ADJUST !=1){ // Cast ASCII code to int
                            // Error check that only 1's and 0's in the grid
                            printf("\n[ERROR]: Anomalous value in file. Ensure that only 0's and 1's are in the board file");
                            exit(EXIT_FAILURE);
                        }else{
                            // Set the living states in the board grid
                            board[row][col].alive = c-ASCII_ADJUST; // Cast ASCII code to int
                            printf("%d ",board[row][col].alive);
                        }
                        col += 1; // Increment the column
                    }
                }
                fclose(readfile); // Close the file

                // Run the simulation
                play_game(board, n_rows, n_cols, fixed_bounds, death_overpop, death_underpop, birth_repro);

                option = 0; // Reset option to allow user to play again
                break;}
            case 4:{ // Change the game rules
                printf("Select game rules");
                update_rules(&death_overpop, &death_underpop, &birth_repro); // Update the rules passing pointers to variables
                option = 0; // Reset option to allow user to play again
                break;}
            case 5:{ // User selected quit
                printf("Thanks for playing Conway's Game of Life. Now quitting...\n");
                break;}
            default:{ // Other value input
                printf("[ERROR] Please choose a gamemode from the menu or press 3 to quit");
            }
        }
    }while(option > 5 || option < 1); // loop until user picks value from the menu

    return 0;
}

void play_game(Cell **board, int n_rows, int n_cols, int fixed_bounds,
               int death_overpop, int death_underpop, int birth_repro){
    /*
    Run the Conway's Game of life simulation with specified rules based on initial conditions and boundary conditions.
    Inputs: board - Double pointer to the board with initial conditions (declared by create_board)
            n_rows, n_cols - number of rows and columns of board.
            fixed_bounds - type of boundary conditions (1 for fixed boundaries, 0 for toroidal boundaries)
            death_overpop - if number of neighbours greater than death_overpop cell dies due to overpopulation
            death_underpop - if number of neighbours greater than death_underpop cell dies due to underpopulation
            birth_repro - if number of neighbours equal to birth_repro cell becomes alive due to reproduction
    - Allow user to run simulation as many times as desired.
    - Print the number of living cells after every GAME_EPOCHS generations
    - Allow the user option to save the results to a file
    */

    int cells_alive = 1; // flag for number of cells alive
    int n_generations = 0; // counter for the total number of generations elapsed
    int keep_playing = 1; // flag for user to keep running simulations

    while (keep_playing == 1){
        int i = 0; // Run simulation of GAME_EPOCHS steps
        while (i < GAME_EPOCHS && cells_alive > 0){ // Run until limit of epochs reached or no living cells remain (improve efficiency)
            i++;
            system("cls"); // Clear terminal
            // Update the board based on the game rules and print to console
            cells_alive = update_board(board,n_rows,n_cols,fixed_bounds, death_overpop, death_underpop, birth_repro);
            print_board(board,n_rows,n_cols);
            delay(TIME_INTERVAL); // 'Animate' results by introducing delay
        }
        n_generations += i; // count total number of generations elapsed
        printf("\nAfter %d generations, %d cells survive\n",n_generations,cells_alive);

        if (cells_alive > 0){ // If cells are still alive, allow user to continue animation
            printf("\nWould you like continue (%d more iterations)?\n",GAME_EPOCHS);
            printf("\t1: Yes\n");
            printf("\t0: No\n");
            scanf("%d",&keep_playing); // Give user option to keep playing
        }else{
            keep_playing = 0; // default to break condition
        }

    }

    if (cells_alive > 0){ // If cells are still, give the user the option to save the board
        save_board(board, n_rows, n_cols, fixed_bounds);
    }

    free_board(board, n_rows, n_cols); // Free the dynamically allocated memory for the board
}

void update_rules(int *death_overpop, int *death_underpop, int *birth_repro){
    /*
    Update the game rules changing the conditions for death due to overpopulation and underpopulation and for birth from reproduction.
    Inputs: *death_overpop - pointer to var storing the value for overpopulation. If cell has < death_overpop neighbours it dies
            *death_underpop - pointer to var storing the value for underpopulation. If cell has < death_underpop neighbours it dies
            *birth_repro - pointer to var storing the value for reproduction. If cell has == birth_repro neighbours it becomes alive
    */
    printf("\n\nSelect Game Rules for n neighbours:\n");
    printf("\t1: Classic Rules: Overpopulation: n > 3, Underpopulation: n < 2, Reproduction n = 3\n");
    printf("\t2: Custom Rules 1: Overpopulation: n > 6, Underpopulation: n < 3, Reproduction n = 4\n");
    printf("\t3: Custom Rules 2: Overpopulation: n > 4, Underpopulation: n < 2, Reproduction n = 3\n");

    int option = 0;
    scanf("%d",&option); // Take user input

    switch (option){
        case 1:{ // Random Grid Selected
            *death_overpop = 3; *death_underpop = 2; *birth_repro = 3; break;
        }case 2:{ // Random Grid Selected
            *death_overpop = 6; *death_underpop = 3; *birth_repro = 5; break;
        }case 3:{ // Random Grid Selected
            *death_overpop = 4; *death_underpop = 2; *birth_repro = 3; break;
        }default:{
            printf("[ERROR] Please select option from menu.\n");
        }
    }
    printf("Overpopulation: n > %d, Underpopulation: n < %d, Reproduction n = %d\n",*death_overpop,*death_underpop,*birth_repro);
}

Cell ** create_board(int n_rows, int n_cols){
    /*
    Creates n_rows*n_cols double pointer array of Cells (structure containing info about each cell).
    - Error checks for memory overflow
    - All cells set to be dead on declaration
    - Return the double pointer to empty board.
    */

    Cell **board = (Cell **)malloc(n_rows * sizeof(Cell *)); // Allocate memory for array (rows)

    if (board == NULL){ // Error check for memory
        printf("[ERROR] Out of memory whilst creating the board\n");
        exit(EXIT_FAILURE);
    }

	for (int i=0; i<n_rows; i++){ // Allocate memory for each column
		board[i] = (Cell *)malloc(n_cols * sizeof(Cell));
		if (board[i] == NULL){
            printf("[ERROR] Out of memory whilst creating the board\n");
            exit(EXIT_FAILURE);
        }
    }

    // Fill the board with dead cells to start with (n_neighbours also set to 0).
    for (int i = 0; i < n_rows; i++){
        for (int j = 0; j < n_cols; j++){
                board[i][j].alive = DEAD; // Set all cells to be dead on declaration
                board[i][j].n_alive_neighbrs = 0; // No neighbours
        }
    }
    return board;
}

void calc_n_neighbours(Cell **board, int n_rows, int n_cols){
    /*
    Calculate the 8 cell neighbourhood and update Cell.n_neighbours in place for the board.
    Inputs: board - Double pointer to the board (declared by create_board)
            n_rows, n_cols - number of rows and columns of board.
    */
    for (int i = 0; i < n_rows; i++){ // loop over whole grid
        for (int j = 0; j < n_cols; j++){
            // Take 8 cell neighbourhood for each cell 'o' (applying the toroidal boundary conditions
            //[1,2,3
            // 4,o,5
            // 6,7,8]
            board[i][j].n_alive_neighbrs = 0 + board[down_coord(i,n_rows)][down_coord(j,n_cols)].alive // 1
                        + board[i][down_coord(j,n_cols)].alive // 2
                        + board[up_coord(i,n_rows)][down_coord(j,n_cols)].alive // 3
                        + board[down_coord(i,n_rows)][j].alive // 4
                        + board[up_coord(i,n_rows)][j].alive // 5
                        + board[down_coord(i,n_rows)][up_coord(j,n_cols)].alive // 6
                        + board[i][up_coord(j,n_cols)].alive // 7
                        + board[up_coord(i,n_rows)][up_coord(j,n_cols)].alive; // 8
        }
	}
}

void print_board(Cell **board, int n_rows, int n_cols){
    /*
    Print the alive/dead state for the whole board to the console.
    Inputs: board - Double pointer to the board (declared by create_board)
            n_rows, n_cols - number of rows and columns of board.
    */
    for (int i = 0; i < n_rows; i++){ // loop over rows and cols
        for (int j = 0; j < n_cols; j++){
            if (board[i][j].alive == ALIVE){ // If cell is alive print a 'o'
                printf("o ");
            }else if (board[i][j].alive == DEAD){ // If cell is dead print blank space
                printf("  ");
            }else{ // catch errors
                printf("[ERROR] print_board - Encountered invalid value for board.alive");
                exit(EXIT_FAILURE);
            }
        }
        printf("\n");
	}
}

int update_board(Cell **board, int n_rows, int n_cols, int fixed_bounds,
                 int death_overpop, int death_underpop, int birth_repro){
    /*
    Update the board based on the game rules. Return the number of living cells after update.
    Inputs: board - Double pointer to the board (declared by create_board)
            n_rows, n_cols - number of rows and columns of board.
            fixed_bounds - type of boundary conditions (1 for fixed boundaries, 0 for toroidal boundaries)
            death_overpop - if number of neighbours greater than death_overpop cell dies due to overpopulation
            death_underpop - if number of neighbours greater than death_underpop cell dies due to underpopulation
            birth_repro - if number of neighbours equal to birth_repro cell becomes alive due to reproduction
    - Call method to calculate number of neighbours for each cell
    - Apply game rules to determine whether cell becomes alive or dead
    - Return number of living cells
    */
    calc_n_neighbours(board,n_rows,n_cols); // Calculate the number of neighbours for each cell

    int cells_alive = 0; // number of living cells on the board

    for (int i = fixed_bounds; i < n_rows-fixed_bounds; i++){ // loop over board within applied boundary conditions
        for (int j = fixed_bounds; j < n_cols-fixed_bounds; j++){

            if (board[i][j].n_alive_neighbrs < death_underpop){ // overpopulation condition met
                board[i][j].alive = DEAD; // cell dies
            }else if (board[i][j].n_alive_neighbrs > death_overpop){ // underpopulation condition met
                board[i][j].alive = DEAD; // cell dies
            }else if (board[i][j].n_alive_neighbrs == birth_repro){ // reproduction condition met
                board[i][j].alive = ALIVE; // cell becomes alive
                cells_alive++; // add to living cell count
            }else if (board[i][j].alive == ALIVE){ // no change but cell alive
                cells_alive++; // add to living cell count
            }
        }
    }
    return cells_alive;
}

void add_living_cell(Cell **board, int n_rows, int n_cols, int x, int y){
    /*
    Add a living cell to the grid inplace.
    Inputs: board - Double pointer to the board (declared by create_board)
            n_rows, n_cols - number of rows and columns of board.
            x, y - coords of living cell to add
    - Checks to ensure coords of cell don't overflow board
    - Add living cell in place
    */
    x -= 1; y -= 1; // decrement x and y to 'natural' coords starting from 1 as passed by user (indices of array start from 0)

    if (x >= 0 && x < n_rows && y >= 0 && y < n_cols){ // Error check that coords within bounds of board
        board[x][y].alive = ALIVE; // make cell at coords alive.
    }else{
        printf("[ERROR] add_living_cell - Overflow error: coords exceed size of grid\n");
    }
}

int get_n_elements(void){
    /*
    Take user input of a integer number of rows or columns 'n' from console.
    - Return 'n' after sanity checks to ensure value is within expected range.
    */
    int n = 0;
    scanf("%d",&n); // input from console
    if (n<1){ // number of terms must be > 0
        printf("[ERROR]: n must be an integer greater than 0\n");
        exit(EXIT_FAILURE);
    }else if(n>NMAX){ // Tested up to n = NMAX, greater than this may lead to overflow
        printf("[ERROR]: n is too large, overflow error. \n");
        exit(EXIT_FAILURE);
    }
    return n;
}

int up_coord(int coord, int bound){
    // Introduce periodic boundary conditions: create mapping such that x and y are wrapped toroidally.
    if (coord +1 > bound -1){ // if coords exceed grid return coord as zero
        return 0;
    }else{
        return coord+1; // otherwise increment the coord
    }
}

int down_coord(int coord, int bound){
    // Introduce periodic boundary conditions: create mapping such that x and y are wrapped toroidally.
    if (coord -1 < 0){ // if coords exceed grid return coord as the boundary
        return bound -1;
    }else{
        return coord-1; // otherwise decrement the coord
    }
}

void free_board(Cell **board, int n_rows, int n_cols){
    /*
    Free the dynamically allocated memory for the board.
    Inputs: Double pointer to the board (declared by create_board), n_rows and n_cols of board.
    */
	for (int i = 0; i < n_rows; ++i) // free each element then free up structure
        free(board[i]);
    free(board);
}

void delay(int milli_seconds) {
    // function using the clock function in <time.h> to create a delay to 'animate' progression
    clock_t start_time = clock(); // store start time
    while (clock() < start_time + milli_seconds); // looping until required time is achieved
}

void save_board(Cell **board, int n_rows, int n_cols, int fixed_bounds){
   /*
    Ask user whether they want to save the current alive/dead state of cells
    Save board state to the file given by CUSTOM_BOARD_FILE with corresponding header.
    Inputs: board - Double pointer to the board (declared by create_board)
            n_rows, n_cols - number of rows and columns of board.
            fixed_bounds - type of boundary conditions (1 for fixed boundaries, 0 for toroidal boundaries)
    */
    int save = -1; // flag

    printf("\nWould you like to save the board?\n");
    printf("\t1: Yes\n");
    printf("\t0: No\n");
    scanf("%d",&save); // Take user input

    if (save == 1){
        printf("Saving grid to %s\n",CUSTOM_BOARD_FILE);
        FILE *file = fopen(CUSTOM_BOARD_FILE, "w"); // open file

        if (file == NULL){ // error check file found
                printf("[ERROR]: Save file %s does not exists\n",CUSTOM_BOARD_FILE);
                exit(EXIT_FAILURE);
        }
        fprintf(file, "n_rows:%d, n_cols:%d, fixed_bounds:%d\n",n_rows,n_cols,fixed_bounds); // print header row
        for (int i = 0; i < n_rows; i++){
            for (int j = 0; j < n_cols; j++){
                fprintf(file,"%d",board[i][j].alive); // print states to file
            }
            fprintf(file,"\n");
        }
        fclose(file); // close the file
        printf("Save successful");

    }else if(save != 0){ // Invalid selection
        printf("[ERROR] Please choose an option from the menu");
        exit(EXIT_FAILURE);
    }
}

/* DEMONSTRATION OF PROGRAM OUTPUTS
- PLEASE CLONE FROM GITHUB TO TEST FOR YOURSELF!
- https://github.com/ljhowell/conways_game_of_life

///// Initial menu on runtime. Showing random grid first

-------------------------------------------
           Conway's Game of Life
-------------------------------------------

Select Gamemode:
        1: Random Grid
        2: Custom Grid
        3: Pre-set Grids
        4: Change game rules
        5: Quit
1
Please input number of rows as a integer:
 10
Please input number of columns as a integer:
 20

 ///// The grid initiates with random values of alive (o) and dead:
o o     o           o o o o o o o   o
o     o     o     o o   o   o   o o o
o o   o o   o o o   o     o o o o o o
  o                   o   o       o o
o o             o     o   o o       o o
o o o       o   o   o o       o o o o
    o   o o o   o       o       o o o o
o o o o o o o   o           o     o   o
  o   o   o o o     o         o   o
o   o o         o o   o   o o o   o o

///// It then updates according to rules and animates by
///// clearing the console and adding a delay of 0.1s between frames until 50 epochs:
                              o
o   o                           o o   o
o o                                 o o
o                             o
                              o
o o o       o o                     o o
    o       o o                   o
                                o o o
o                               o     o
o                             o o o   o
After 50 generations, 35 cells survive

Would you like continue (50 more iterations)?
        1: Yes
        0: No

///// Then gives the user the option to continue running the simulation
///// - In this case, the board is not at steady state so 'yes' chosen and another 50 frames happen
                                  o o


  o
o   o
o o         o o
            o o
                                  o o
                                o     o
                                o     o
After 100 generations, 17 cells survive
///// As before, but now the system is in steady state (3 x "still life's", 1 box, 1 boat and 1 beehive https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)
///// - Then asks the user if they want to save the board
...
Would you like to save the board?
        1: Yes
        0: No
1
Saving grid to custom_board.txt
Save successful

/////////////////////////////////////////////////////////////////////////////////////////////
///// We now demonstrate the custom grid feature: Lets add a blinker to a 5x5 grid by hand

Select Gamemode:
        1: Random Grid
        2: Custom Grid
        3: Pre-set Grids
        4: Change game rules
        5: Quit
2
Please input number of rows as a integer: 5
Please input number of columns as a integer: 5
Add cell (or -1 to run simulation)
x = 2
y = 3
/// (Console is cleared) Cell added to (2,3)
. . . . .
. . o . .
. . . . .
. . . . .
. . . . .
Add cell (or -1 to run simulation)
x = 3
y = 3
. . . . .
. . o . .
. . o . .
. . . . .
. . . . .
Add cell (or -1 to run simulation)
x = 4
y = 3
. . . . .
. . o . .
. . o . .
. . o . .
. . . . .
Add cell (or -1 to run simulation)
x = -1
///// Then simulation plays, (blinker oscillates between vertical and horizontal 3 in a row)

    o
    o
    o

After 50 generations, 3 cells survive
Would you like continue (50 more iterations)?
        1: Yes
        0: No
0

Would you like to save the board?
        1: Yes
        0: No
0
/////////////////////////////////////////////////////////////////////////////////////////////
///// We now demonstrate the pre-set grid features
Select Gamemode:
        1: Random Grid
        2: Custom Grid
        3: Pre-set Grids
        4: Change game rules
        5: Quit
3

Pre-set Options
        1: Pulsar - Oscillator - Period 3
        2: Penta-Decathlon - Oscillator - Period 15
        3: Glider - Diagonal translation
        4: Spaceship - Horizontal translation
        5: Glider Gun - Produces a glider every 30 generations
        6: Die Hard - Methuselah - dies after 130 generations
        7: Custom grid from file custom_board.txt
5
////// Board clears, board is read in from file, simulation animation for Gospel glider gun happens:

                                                      o o
                                                    o       o
                    o o                           o           o       o o
                    o o                           o       o   o o     o o
  o o       o             o o                     o           o
  o   o       o           o o o                     o       o
    o o o o o             o o                         o o
      o o o         o o                   o   o
                    o o                     o o
                                            o




                                                          o
                                                            o o
                                                          o o



After 50 generations, 59 cells survive
//// We see the glider gun has produced two gliders within 50 generations. The other pre-sets are cool too
//// We can also load in the grid we saved earlier...
...
Pre-set Options
        1: Pulsar - Oscillator - Period 3
        2: Penta-Decathlon - Oscillator - Period 15
        3: Glider - Diagonal translation
        4: Spaceship - Horizontal translation
        5: Glider Gun - Produces a glider every 30 generations
        6: Die Hard - Methuselah - dies after 130 generations
        7: Custom grid from file custom_board.txt
7
                                  o o


  o
o   o
o o         o o
            o o
                                  o o
                                o     o
                                o     o

After 50 generations, 17 cells survive

///// As we can see, the board was loaded in as it finished before and runs the animation (no changes as system in stationary state in this case)

/////////////////////////////////////////////////////////////////////////////////////////////
///// We now demonstrate the changing game rule feature
Select Gamemode:
        1: Random Grid
        2: Custom Grid
        3: Pre-set Grids
        4: Change game rules
        5: Quit
4
Select game rules

Select Game Rules for n neighbours:
        1: Classic Rules: Overpopulation: n > 3, Underpopulation: n < 2, Reproduction n = 3
        2: Custom Rules 1: Overpopulation: n > 6, Underpopulation: n < 3, Reproduction n = 4
        3: Custom Rules 2: Overpopulation: n > 4, Underpopulation: n < 2, Reproduction n = 3
3
Overpopulation: n > 4, Underpopulation: n < 2, Reproduction n = 3

///// With this rule, strange maze like grids form

Select Gamemode:
        1: Random Grid
        2: Custom Grid
        3: Pre-set Grids
        4: Change game rules
        5: Quit
1
Please input number of rows as a integer:
 10
Please input number of columns as a integer:
 10

o o     o   o   o
  o   o   o   o   o
  o   o   o   o   o
  o   o o   o     o
  o     o   o   o o
  o   o   o o     o
  o   o   o   o o o
  o   o   o       o
  o   o   o o o o o
  o o   o       o

After 50 generations, 51 cells survive

Would you like continue (50 more iterations)?
        1: Yes
        0: No
0

Would you like to save the board?
        1: Yes
        0: No
0


Select Gamemode:
        1: Random Grid
        2: Custom Grid
        3: Pre-set Grids
        4: Change game rules
        5: Quit
5
Thanks for playing Conway's Game of Life. Now quitting...

Process returned 0 (0x0)   execution time : 945.382 s
Press any key to continue.
*/
