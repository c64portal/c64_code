
# Invite Intro 

![color picker](https://csdb.dk/gfx/releases/214000/214452.gif)


This is a source code and resources of the small invitation intro for for the Commodore C64.

This program was created as a invitation for Moonshine Dragons and Teddy Beer parties in 2022.
You can find the invitation intro here on CSDB.dk:
https://csdb.dk/release/?id=214452

Please find some comments in the code in main.c file
Beside the code and resources you can also find the source files for CharPad Pro and SpritePad Pro tools. These two Windows based tools were used to create teddy sprite and the scrolling screen with dragon and the teddy.

# How to Compile?

To compile it you have to use KickC compiler version 0.85 (not the latest one).
To compile please use included shell script by typing:

$ ./make.sh main -e 

The "-e" option will force the KickC compiler to produce also the .prg file that can be executed on real Commodore 64 or an emulator.

