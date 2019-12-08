# conways_game_of_life
C Project - Conway's Game of Life

Implementation of Conway's Game of Life in C

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
                        Options include 1) Start from a grid of random state cells.
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
