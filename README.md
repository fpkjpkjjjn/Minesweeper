# Minesweeper

**Predmet:** Programovanie  

**Autor:** Ivan Kysel

**Rok:** 2025/2026

---

## Popis hry

Minesweeper je logická hra, v ktorej je cieľom hráča otvoriť všetky bezpečné bunky bez toho, aby narazil na mínu. Hra sa hrá termináli pomocou knižnice ncurses cez obaľovaciu vrstvu world.

Pole má rozmer 20 × 15 buniek a obsahuje 45 mín. Prvé kliknutie je vždy bezpečné, míny sa objavia mimo bezpečnej zóny (minimálne 3 x 3 bunky).

---

## Kompilácia a spustenie

### Požiadavky

- GCC kompilátor
- Knižnica `ncurses`
- `make`

### Kompilácia

```bash
make
```

### Spustenie

```bash
./minesweeper
```

### Vyčistenie

```bash
make clean
```

---

## Návod na hranie

### Ovládanie

| Kláves | Akcia |
|--------|-------|
| `Arrows` | Pohyb kurzora po poli |
| `Space` alebo `Enter` | Odkryť bunku |
| `F` | Postaviť / odstrániť vlajku na bunke |
| `R` | Reštartovať hru |
| `ESC` | Ukončiť hru |

---

### Symboly na poli

| Symbol | Význam |
|--------|--------|
| `#` | Neodkrytá bunka |
| `F` | Označenie podozrivej bunky |
| _(číslo na bunke)_ | Počet mín v susedných 8 bunkách |
| _(prázdna biela)_ | Odkrytá bezpečná bunka bez susedných mín |
| `*` | Mína (zobrazí sa po prehratí) |
| _(žlté pozadie)_ | Aktuálna poloha kurzora |

---

### Podmienky ukončenia hry

- **Výhra:** Hráč otvorí všetky bezpečné bunky, zvyšné majú počet vlajok zodpovedajúci počtu mín.
- **Prehra:** Hráč odkryje bunku, pod ktorou je mína. Po prehre sa zobrazia všetky míny na poli.

---

## Štruktúra projektu

```
minesweeper/
├── main.c    - Hlavné funkcie main a start_world()
├── game.c    - Herná logika, vykresľovanie, spracovanie klávesov
├── game.h    - Štruktúry Cell a game, konštanty poľa, deklarácie funkcií
├── world.c   - Predpripravené funkcie knižnice world s ncurses
├── world.h   - Rozhranie knižnice world, typy udalostí
├── Makefile  - Súbor kompilácie projektu
└── README.md - Dokumentácia
```

---

## Popis programu

### Štruktúry

#### `Cell` (game.h)

Reprezentuje jednu bunku herného poľa.

---

#### `struct game` (game.h)

Hlavná štruktúra uchovávajúca stav hry. Obsahuje herné pole, polohu kurzora, stav hry a čas.

---

### Funkcie súboru game.c

#### `void move_cursor(struct game* st, int dx, int dy)`
Posúva kurzor na zadané čísla s kontrolou nad hranicami polí.

---

#### `void place_mines(struct game* st, int safe_x, int safe_y)`
Náhodne umiestňuje míny na hraciu plochu, pričom vždy berie do úvahy bezpečnú zónu okolo prvého kliknutia hráča a počíta počet susedných mín pre každú bunku.

---

#### `void reveal_cell(struct game* st, int x, int y)`
Odkrýva bunku na pozícii. Ak bunka obsahuje mínu, nastaví `game_state = -1` a hra sa skončí. Taktiež, ak bunka neobsahuje žiadne míny v okolí, otvorí tieto bunky pomocou algoritmu flood-fill a otvorí väčšiu oblasť. Po každom odkrytí kontroluje podmienku výhry.

---

#### `static void reset_game(struct game* st)`
Vynuluje celý stav hry pomocou memset a znova umiestni kurzor do stredu. Nealokuje novú pamäť a pracuje priamo s existujúcou štruktúrou.

---

#### `void* init_game()`
Alokuje a inicializuje štruktúru `struct game`. Kurzor sa umiestni do stredu poľa. Vracia ukazovateľ na stav hry, ktorý knižnica `world` ďalej predáva do každého volania `game_event`.

---

#### `void show_bar(int x, int y)` / `show_title(int x, int y)` / `show_win(int x, int y)` / `show_lose(int x, int y)`
Pomocné funkcie na vykreslenie dekoratívnych prvkov: rámčeka s ovládaním, veľkého ASCII art nadpisu `MINESWEEPER` a správ o výhre/prehre. Všetky nadpisy sú zložené zo znakov `#`.

---

#### `static short num_color(int n)`
Určuje farbu na základe vstupnej číslice v parametri.

---

#### `static void draw_game(struct game* st, struct event* event)`
Vykreslí celý aktuálny stav hry na obrazovku. Volá funkciu `clear_screen()` a následne kreslí: hornú a dolnú hranice s názvom hry, informačný panel (počet mín, vlajok, čas), herné pole, bočný panel `show_bar`, ASCII art nadpis / výhernú / prehrávajúcu správu.

---

#### `int game_event(struct event* event, void* game)`
Hlavná funkcia hernej slučky, ktorú volá knižnica world pri každej udalosti. Spracúva:
- `EVENT_START` (nastaví rýchlosť hry a vykreslí úvodný stav)
- `EVENT_KEY` (pohyb kurzora, odkrytie, vlajka, reštart)
- `EVENT_ESC` (ukončenie hry)
- Aktualizuje časomier a volá draw_game.

---

## Modifikácie knižnice World

V knižnici `world.c` / `world.h` neboli vykonané žiadne zmeny. Všetky volania `ncurses` prechádzajú výhradne cez funkcie knižnice `world`:

- `set_cell()` - vykreslenie znaku (biela na čiernej)
- `set_color_cell()` - vykreslenie znaku so zvolenou farbou
- `set_message()` - vykreslenie reťazca
- `clear_screen()` - vymazanie obrazovky
- `game_speed()` - nastavenie rýchlosti časovača
- `start_world()` - spustenie hernej slučky

---

## Použité zdroje

- Repozitár knižnice world: https://github.com/hladek/world
- Flood-fill algoritmus: https://en.wikipedia.org/wiki/Flood_fill