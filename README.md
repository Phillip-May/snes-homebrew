# snes-homebrew
A collection of snes homebrew written mostly in C for the WDC 65816 C compiler.

Each folder is a self contained project with a build and clean file.
Simply run build and the resulting binary file can run directly in emulators.

The reason that each project has it's own local copy of the libraries is so I 
can make drastic changes to the common libraries without breaking older projects.

All of the programs currently use LoROM unless specified otherwirse.
