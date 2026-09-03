# Minesweeper

**Subject:** Programming

**Author:** Ivan Kysel

**Year:** 2025/2026

---

## Game Description

Minesweeper is a logic puzzle game where the player's goal is to reveal all safe cells without hitting a mine. The game runs in the terminal using the `ncurses` library through the `world` wrapper layer.

The board is 20 × 15 cells and contains 45 mines. The first click is always safe — mines are placed outside a safe zone of at least 3 × 3 cells around it.

<p align="center">
  <img src="minesweeper gamepl.png" alt="Minesweeper gameplay screenshot" width="700" />
</p>

---

## Key Challenges

A few parts of the implementation required extra care:

- **Flood-fill on reveal** - when an empty cell (no adjacent mines) is opened, the game needs to recursively reveal the connected region of empty cells and their bordering numbered cells, without re-processing cells already revealed or running into stack issues on large empty areas.
- **Guaranteed-safe first click** - mines can't be placed until after the player's first move, since the first click must always land on a safe zone. This means mine placement and neighbor-mine counting happen dynamically after the first input, not at game start.
- **Rendering through a fixed wrapper** - all drawing goes through the `world` library's fixed API (`set_cell`, `set_color_cell`, `set_message`, etc.), so the challenge was building a clean rendering layer (`draw_game`, `show_bar`, `show_title`, etc.) on top of a small, unchangeable set of primitives rather than calling `ncurses` directly.
- **State management without dynamic reallocation** - restarting the game (`reset_game`) reuses the existing `struct game` allocation via `memset` instead of freeing and reallocating memory, keeping the game loop simple and avoiding unnecessary heap churn.

---

## Build and Run

### Requirements

- GCC compiler
- `ncurses` library
- `make`

### Build

```bash
make
```

### Run

```bash
./minesweeper
```

### Clean

```bash
make clean
```

---

## How to Play

### Controls

| Key | Action |
|--------|-------|
| `Arrows` | Move the cursor across the board |
| `Space` or `Enter` | Reveal a cell |
| `F` | Place / remove a flag on a cell |
| `R` | Restart the game |
| `ESC` | Quit the game |

---

### Board Symbols

| Symbol | Meaning |
|--------|--------|
| `#` | Unrevealed cell |
| `F` | Flagged (suspected mine) cell |
| _(number on cell)_ | Number of mines in the 8 neighboring cells |
| _(empty white)_ | Revealed safe cell with no adjacent mines |
| `*` | Mine (shown after losing) |
| _(yellow background)_ | Current cursor position |

---

### Win/Loss Conditions

- **Win:** The player reveals all safe cells; the remaining ones are flagged with a count matching the number of mines.
- **Loss:** The player reveals a cell containing a mine. All mines are then shown on the board.

---

## Project Structure

```
minesweeper/
├── main.c    - Main function and start_world()
├── game.c    - Game logic, rendering, key handling
├── game.h    - Cell and game structs, board constants, function declarations
├── world.c   - Prebuilt world library functions using ncurses
├── world.h   - World library interface, event types
├── Makefile  - Project build file
└── README.md - Documentation
```

---

## Program Description

### Structures

#### `Cell` (game.h)

Represents a single cell of the game board.

---

#### `struct game` (game.h)

Main structure holding the game state. Contains the board, cursor position, game state, and elapsed time.

---

### Functions in game.c

#### `void move_cursor(struct game* st, int dx, int dy)`
Moves the cursor by the given offsets, with boundary checking against the board edges.

---

#### `void place_mines(struct game* st, int safe_x, int safe_y)`
Randomly places mines on the board, always respecting the safe zone around the player's first click, and calculates the number of adjacent mines for each cell.

---

#### `void reveal_cell(struct game* st, int x, int y)`
Reveals the cell at the given position. If the cell contains a mine, sets `game_state = -1` and ends the game. If the cell has no adjacent mines, it recursively reveals the surrounding area using a flood-fill algorithm. Checks the win condition after every reveal.

---

#### `static void reset_game(struct game* st)`
Resets the entire game state using `memset` and repositions the cursor to the center. Does not allocate new memory — it operates directly on the existing structure.

---

#### `void* init_game()`
Allocates and initializes the `struct game` structure. The cursor is placed at the center of the board. Returns a pointer to the game state, which the `world` library then passes to every call of `game_event`.

---

#### `void show_bar(int x, int y)` / `show_title(int x, int y)` / `show_win(int x, int y)` / `show_lose(int x, int y)`
Helper functions that render decorative elements: a control-scheme panel, a large ASCII-art `MINESWEEPER` title, and win/loss messages. All headings are built from `#` characters.

---

#### `static short num_color(int n)`
Determines the display color based on the given number.

---

#### `static void draw_game(struct game* st, struct event* event)`
Renders the entire current game state on screen. Calls `clear_screen()` and then draws: the top and bottom borders with the game title, the info panel (mine count, flag count, time), the game board, the side panel (`show_bar`), and the ASCII-art title / win / loss message.

---

#### `int game_event(struct event* event, void* game)`
The main game loop function, called by the `world` library on every event. Handles:
- `EVENT_START` (sets the game speed and draws the initial state)
- `EVENT_KEY` (cursor movement, reveal, flag, restart)
- `EVENT_ESC` (quits the game)
- Updates the timer and calls `draw_game`.

---

## Modifications to the World Library

No changes were made to `world.c` / `world.h`. All `ncurses` calls go exclusively through the functions of the `world` library:

- `set_cell()` - draws a character (white on black)
- `set_color_cell()` - draws a character with a chosen color
- `set_message()` - draws a string
- `clear_screen()` - clears the screen
- `game_speed()` - sets the timer speed
- `start_world()` - starts the game loop

---

## References

- `world` library repository: https://github.com/hladek/world
- Flood-fill algorithm: https://en.wikipedia.org/wiki/Flood_fill
