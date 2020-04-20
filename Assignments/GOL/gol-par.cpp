/**
 * 
 * ---------- Parallel version (threads) ----------
 * 
 * This file represents the parallel implementation of
 * Conway's Game Of Life.
 * 
 * The game regards the evolution of cells living in a board
 * of N rows and M columns. At start each cell can be alive or dead,
 * at each iteration of the game the cells will become alive, will die
 * or will stay dead/alive according to the number of cells alive in its
 * surrounding.
 * 
 * The board starts with a random number of alive cells.
 * The rules are, at each step:
 * - if a cell is surrounded by 2 or 3 alive cells, it survives;
 * - if a cell is surrounded by >3 or <2 cells it dies;
 * - if a cell is surrounded by exactly 3 alive cells and this cell is dead, it becomes alive.
 * 
*/

#include <iostream>
#include <vector>
#include <numeric>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <ctime>

using Row = std::vector<int>;
using Board = std::vector<Row>;

using hrclock = std::chrono::high_resolution_clock;

#define ALIVE_CELL "X "
#define DEAD_CELL "° "


/**
 * 
 * Auxiliary structure used to pass threads
 * the range of the board to work on.
 * 
*/
struct Range {
  int start;
  int end;
};


/**
 * 
 * Given the number of rows and cols, initializes the board
 * with random numbers (0 , 1) using the parameter seed for
 * the random number generator.
 * The seed number is necessary for reproducibility.
 * 
 * @param nrows number of rows in the board.
 * @param ncols number of columns in the board.
 * @param seed  integer used for random number generation.
 * @return      Board object representing the initialized board.
 * 
*/
Board initializeBoard(int nrows, int ncols, int seed) {
  Board board = {};
  srand(seed);

  for (int i = 0; i < nrows; i++) {
    Row row = {};
    for (int i = 0; i < ncols; i++) {
      row.push_back(rand()%2);
    }
    board.push_back(row);
  }
  
  return board;
}

/**
 * 
 * Given the current cell and the game board, it updates the status
 * of the current cell.
 * Following the rules of the game it returns the status to be set
 * on the current cell.
 * 
 * @param r current cell row
 * @param c current cell col
 * @param board game board at the current status
 * @return the new status of the current cell
 * 
*/
bool updateCell(int r, int c, Board board) {

  std::vector<int> range = {-1, 0, 1};
  int neighbours = 0;

  for(auto offset : range) {
    // Board's boundary check
    if((r+offset >= 0) && (r+offset < board.size())) {
      Row row = board.at(r + offset);
      for(auto offcol : range) {
        if((offcol || offset) && (c+offcol >= 0) && (c+offcol < row.size())) {
          neighbours += board.at(r+offset).at(c+offcol);
        }
      }
    }
  }

  return ( ((neighbours == 2) || (neighbours == 3)) && (board.at(r).at(c)) ) || ( !(board.at(r).at(c)) && (neighbours == 3) );

}


/**
 * 
 * Implements the logic of Game Of Life.
 * It updates the board using the rules of the game.
 * At the end the board contains the new status of the game.
 * 
 * @param b the board to be updated.
 * @param nworkers the number of threads working on the board
 * 
*/
void updateBoard(Board* b, int nworkers) {

  auto& board = *b;
  Board new_board(board.size());
  std::vector<std::thread> tids;
  std::vector<Range> ranges;

  auto range_size = board.size() / nworkers;
  auto rem = board.size() % nworkers;

  for(int i=0; i<nworkers; i++) {
    Range range;
    range.start = (i==0 ? 0 : ranges.back().end + 1);
    range.end   = (i != (nworkers-1) ? range.start+range_size - 1 : board.size()-1);
    
    // keep work balanced
    // Assign 1 extra row to first rem workers
    // rem is just the remainder of board.size()/nworkers
    if(i<rem) {
      range.end++;
    }

    ranges.push_back(range);
  }

  // Function to be passed to threads to update sub-ranges of the board
  auto compute_chunk = [&board, &new_board](Range range) {
    for(int r = range.start; r <= range.end; r++) {
        Row new_row = {};
        Row row = board.at(r);

        for(int i = 0; i < row.size(); i++) {
          new_row.push_back(updateCell(r, i , board));
        }
        new_board.at(r) = new_row;
    }
  };

  for (int i = 0; i < nworkers; i++) {
    tids.push_back(std::thread(compute_chunk, ranges[i]));
  }

  for(std::thread& t: tids) {
    t.join();
  }
  
  board = new_board;
}


/**
 * 
 * Prints to the screen the status of the board,
 * it should transform the 1s and 0s in the board
 * using the correct symbols to represents alive and dead cells.
 * 
 * @param board the board to print.
 * 
*/
void printBoard(Board board) {
  for(auto el : board) {
    for(int x : el) {
      std::cout << (x ? ALIVE_CELL : DEAD_CELL);
    }
    std::cout << std::endl;
  }
  std::cout << " ---------------------- " << std::endl;
}


int main(int argc, char const *argv[])
{
  if(argc != 7) {
    std::cout << "Usage: " << argv[0] << " nrows ncols iters seed sleeptime nworkers\n";
    return -1;
  }
  
  // We assume that the number of rows and cols is positive and >1
  int nrows = atoi(argv[1]);
  int ncols = atoi(argv[2]);
  int iters = atoi(argv[3]);
  int seed = atoi(argv[4]);
  int msec = atoi(argv[5]);
  int nw = atoi(argv[6]);

  // Bounding max parallelism degree with maximum number of rows
  // Not at all elegant, done it for simplicity
  if(nw > nrows) {
    std::cout << "Warning: bounding workers with nrows." << std::endl;
    nw = nrows;
  }

  Board board = initializeBoard(nrows, ncols, seed);

  auto start = hrclock::now();

  for (int i = 0; i < iters; i++) {
    updateBoard(&board, nw);

#ifdef DEBUG
    printBoard(board);
    usleep(msec * 1000);
#endif

  }

  auto elapsed = hrclock::now() - start;
  auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
  std::cout << "Simulation spent: " << usec / 1000 << " msec\n";

  return 0;
}
