## gbe
gbe is a simple cross-platform Nintendo Gameboy Emulator written in C. Does not support Gameboy Colour yet.
It has a simple frontend api for displaying the emulator via a framebuffer.

### Platforms
* Linux
* Windows [WIP]
* MacOS [WIP]

### Frontends
* SDL [Display + Controls]
* X11 [Display]
* Command line [Debug]

## What Works?

Games play in various degrees of accuracy.

### Mostly works
* Pokemon Blue/Green/Red/Yellow [Graphical glitches]
* Tetris
* Metroid II
* Super Mario Land [Graphical glitches]
* Legend of Zelda: Link's Awakening [Graphical glitches]

### Beyond title screen
* Kirby's Dream Land [Crash when pressing B button]

### Title screen

### Does not work
* ANY GAMEBOY COLOUR GAMES

## TODO list (in no particular order)
* Game saving
* Better frontend and ui
* Better degugging
* Compiling under Windows and MacOS
* Control remapping
* Gameboy Colour emulation
* Sound