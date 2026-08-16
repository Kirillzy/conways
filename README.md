## 🌟 Highlights

- Simulates Conway's Game of Life on a fixed-size grid, from a user-supplied seed file, for any number of generations
- Supports **independent vertical and horizontal wrapping**, so the world can behave like a flat grid, a cylinder, or a torus
- Strict, descriptive validation of every command-line argument and every field of the seed file, with clean exits on any error
- Builds the world as a true dynamically-allocated 2D array (no fixed-size buffers) and frees everything it allocates
- Outputs each generation to its own file using a provided output library (`gameOfLifeOutput`), keeping formatting/writing logic decoupled from simulation logic
- Compiles warning-free under `-std=c99 -pedantic -Wall` and has been checked for leaks with Valgrind
- Includes a `Makefile` with build, run, clean, and Valgrind targets

## ℹ️ Overview

This project is my final implementation for **CS 261 (C and Assembly Language Programming)**, in which the assignment was to build a working version of Conway's Game of Life in C. Beyond just getting the simulation "working," the assignment was really about practicing the fundamentals that C forces you to be deliberate about: dynamic memory management, working with a stack-allocated variable-length 2D array, parsing untrusted input safely, and integrating cleanly with a library (`gameOfLifeOutput`) I didn't write myself but had to use correctly and respectfully (treating its internal struct as opaque).

The program reads a "seed" text file describing an initial grid of dead/alive cells, then simulates the standard four Game of Life rules — underpopulation, survival, overpopulation, and reproduction — for a requested number of generations, treating all cell updates as simultaneous. It writes out one file per generation (generation 0 through N), and can optionally wrap the world vertically and/or horizontally so cells on opposite edges are treated as neighbors.

I'm keeping this repo mainly as a record of the project and to keep practicing writing documentation the way I'd want it for a real portfolio piece, not just a class submission.

### ✍️ Authors

Built by me as coursework for CS 261. Feel free to check out my other repos on GitHub — this one's part of my ongoing computer science coursework/portfolio.

## 🚀 Usage

The program takes between two and four command-line arguments:

```
./main <generations> <seed-file> [wrapVertical] [wrapHorizontal]
```

- `generations` — non-negative integer number of generations to simulate (0 is valid, and just outputs the seed)
- `seed-file` — path to a seed file (format below)
- `wrapVertical` — optional, `0` or `1` (default `0`)
- `wrapHorizontal` — optional, `0` or `1` (default `0`)

Example, simulating 20 generations of a glider with both wrapping directions on:

```
./main 20 glider-10x10 1 1
```

This produces `glider-10x10_0` through `glider-10x10_20`, each an ASCII snapshot of the world at that generation (`.` = dead, `0` = alive).

Seed files are plain text: two whitespace-separated integers (`rows cols`) followed by a description of the grid using `.` and `0` characters. A few sample seeds are included in this repo (`dot-3x5`, `glider-10x10`, `lwss-7x16`, `pulsar-17x17`, `gosper-13x38`, `wrap-10x10`) to try different patterns and grid sizes.

## ⬇️ Installation

Requires `gcc` and `make` (developed and tested on macOS/Linux).

```
git clone https://github.com/Kirillzy/conways
cd <this-repo>
make
```

That builds the `main` executable. To build **and** run it in one step, use the `run` target and pass arguments through `ARGS` (no spaces around the `=`):

```
make run ARGS="20 glider-10x10 0 0"
```

To remove all generated object files, the executable, and any generation output files:

```
make clean
```

## 💭 Feedback and Contributing

This is primarily a coursework/learning project, but I'm always looking to improve it and my C skills in general. If you spot a bug, a memory issue, or a cleaner way to do something, feel free to open an issue — I'd genuinely appreciate the feedback.