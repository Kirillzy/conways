## 🌟 Highlights

- Simulates Conway's Game of Life on a fixed-size grid, from a user-supplied seed file, for any number of generations
- Supports **independent vertical and horizontal wrapping**, so the world can behave like a flat grid, a cylinder, or a torus
- Strict, descriptive validation of every command-line argument and every field of the seed file, with clean exits on any error
- Builds the world as a true dynamically-allocated 2D array (no fixed-size buffers) and frees everything it allocates
- Outputs each generation to its own file using a provided output library (`gameOfLifeOutput`), keeping formatting/writing logic decoupled from simulation logic
- Optional BMP image output per generation, in addition to the required text format, for visualizing runs
- Compiles warning-free under `-std=c99 -pedantic -Wall` and has been checked for leaks with Valgrind
- Includes a `Makefile` with build, run, clean, and Valgrind targets

## ℹ️ Overview

This project is my final implementation for **CS 261 (C and Assembly Language Programming)**, in which the assignment was to build a working version of Conway's Game of Life in C. Beyond just getting the simulation "working," the assignment was really about practicing the fundamentals that C forces you to be deliberate about: dynamic memory management, working with a stack-allocated variable-length 2D array, parsing untrusted input safely, and integrating cleanly with a library (`gameOfLifeOutput`) I didn't write myself but had to use correctly and respectfully (treating its internal struct as opaque).

The program reads a "seed" text file describing an initial grid of dead/alive cells, then simulates the standard four Game of Life rules — underpopulation, survival, overpopulation, and reproduction — for a requested number of generations, treating all cell updates as simultaneous. It writes out one file per generation (generation 0 through N), and can optionally wrap the world vertically and/or horizontally so cells on opposite edges are treated as neighbors.

I'm keeping this repo mainly as a record of the project and to keep practicing writing documentation the way I'd want it for a real portfolio piece, not just a class submission.

### ✍️ Authors

Built by me as coursework for CS 261. Feel free to check out my other repos on GitHub — this one's part of my ongoing computer science coursework/portfolio.

## 🚀 Usage

The program takes between two and five command-line arguments:

```
./main <generations> <seed-file> [wrapVertical] [wrapHorizontal] [bmpOutput]
```

- `generations` — non-negative integer number of generations to simulate (0 is valid, and just outputs the seed)
- `seed-file` — path to a seed file (format below)
- `wrapVertical` — optional, `0` or `1` (default `0`)
- `wrapHorizontal` — optional, `0` or `1` (default `0`)
- `bmpOutput` — optional, `0` or `1` (default `0`); when `1`, also writes a `.bmp` image per generation alongside the required text output

Example, simulating 20 generations of a glider with both wrapping directions on:

```
./main 20 glider-10x10 1 1
```

This produces `glider-10x10_0` through `glider-10x10_20`, each an ASCII snapshot of the world at that generation (`.` = dead, `0` = alive).

Turning on image output as well:

```
./main 20 glider-10x10 1 1 1
```

produces the same text files, plus `glider-10x10_0.bmp` through `glider-10x10_20.bmp`.

![Glider pattern animating across a wrapped grid](assets/glider-demo.gif)

Seed files are plain text: two whitespace-separated integers (`rows cols`) followed by a description of the grid using `.` and `0` characters. A few sample seeds are included in this repo (`dot-3x5`, `glider-10x10`, `lwss-7x16`, `pulsar-17x17`, `gosper-13x38`, `wrap-10x10`) to try different patterns and grid sizes.

## ⬇️ Installation

Requires `gcc` and `make` (developed and tested on macOS/Linux).

```
git clone https://github.com/Kirillzy/Conways
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

## 🧠 Design Notes

A few decisions and tradeoffs worth calling out, mostly for my own future reference:

- **VLA-in-a-struct vs. array of pointers.** The assignment suggests an array of pointers (`world[r][c]` via `malloc`'d rows), but I used a stack-allocated variable-length array (`int world[rows][cols]`) sized at runtime from the seed file's header. It's simpler and avoids a bunch of `malloc`/`free` calls, but it does mean very large worlds could blow the stack — an array of `malloc`'d rows would scale to bigger grids more safely. If I revisited this, I'd switch to heap allocation for the world itself once grid sizes got large enough to matter.
- **`'0'`/`'1'` char convention internally, boolean convention at the library boundary.** I store dead/alive as the ASCII characters `'0'`/`'1'` in the world array (matching the seed file's alive marker), and only translate to the library's 0/nonzero boolean convention right at `writeToFile()`. That keeps the translation in exactly one place, but a cleaner design might use a `typedef` (e.g. `typedef enum { DEAD, ALIVE } cell_t;`) instead of relying on char literals, which would make illegal states harder to represent.
- **Optional 5th argument for BMP output.** Rather than a `-b`/`--bmp` style flag, I stuck with the existing `0`/`1` positional-argument pattern used for wrapping, since it was consistent with the rest of the CLI and didn't require adding a flag parser. A "real" CLI tool would probably use named flags (via `getopt`) instead of positional args once you're past 3-4 options.
- **`writeBmpToFile()` depends on call order.** It relies on `writeToFile()` having already populated the manager for the current generation via `golo_set_row()`, rather than re-populating it itself. That avoids redundant work, but it's an implicit ordering dependency that isn't enforced by the type system — a comment documents it, but a more defensive design would pass the populated row data explicitly rather than relying on shared mutable state in `golo_manager_t`.

## 💭 Feedback and Contributing

This is primarily a coursework/learning project, but I'm always looking to improve it and my C skills in general. If you spot a bug, a memory issue, or a cleaner way to do something, feel free to open an issue — I'd genuinely appreciate the feedback.
