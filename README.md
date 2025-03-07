# Tic-Tac-Toe AI with Minimax and Alpha-Beta Pruning

## Overview
This project is a console-based Tic-Tac-Toe game with an Computer opponent that uses the Minimax algorithm with Alpha-Beta Pruning for optimal decision-making. It visualizes the decision-making process using Graphviz.

## Features
- Playable Tic-Tac-Toe game in the terminal
- Computer opponent that plays optimally using the Minimax algorithm with Alpha-Beta pruning
- Graphviz integration to visualize the Minimax tree
- Simple binary representation of the game board for efficient calculations

## Requirements
To compile and run this project, ensure you have the following installed:
- CMake
- GCC compiler
- Graphviz library (`libgraphviz-dev` on Linux, `graphviz` for Windows/macOS)

## Compilation
Use the following command to compile the program:
```sh
mkdir -p build
cd build
cmake ..
make
```

## Running the Game
Execute the compiled program:
```sh
./ticTacToe
```

## How to Play
- The game is played on a 3x3 grid.
- The player (X) goes first and selects a position (0-8) to place their mark.
- The Computer (O) responds by making an optimal move using Minimax.
- The game continues until a player wins or the board is full (draw).

## Board Representation
The board is stored in an integer variable using bitwise operations:
- The first 9 bits represent X's positions.
- The next 9 bits represent O's positions.
- The 31st bit is used to track the turn (X or O).

## Minimax with Alpha-Beta Pruning
1. **Score Evaluation**: 
   - `+10`: Algorithm (O) wins
   - `-10`: Player (X) wins
   - `0`: Draw
2. **Recursive Minimax**: Algorithm explores all possible moves and selects the optimal move based on maximizing or minimizing scores.
3. **Alpha-Beta Pruning**: Improves efficiency by eliminating unnecessary branches, reducing computation time.
4. **Graphviz Visualization**: Generates a decision tree (`graph.jpeg`) to display the algorithm thought process.

## Example Output
```
+=+=+=+
|0|1|2|
+=+=+=+
|3|4|5|
+=+=+=+
|6|7|8|
+=+=+=+
X turn
You must choose positions 0-8:
```

## Future Improvements
- Enhance visualization with an interactive UI
- Support for different difficulty levels
- Implement alternative AI strategies (e.g., Monte Carlo Tree Search)

