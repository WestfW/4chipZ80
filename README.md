# 4chipZ80
Z80 system using ATmega as IO/boot controller.  4 chips total.
---
This is a minimalist Z80 system based on a breadboard design by "Just4Fun" at hackaday.io:
https://hackaday.io/project/19000

My main contribution so far is the creation of a PCB, for a more permenant construction than the original breadboard.
This involved converting the printed schematic to an EAGLE version, as well as laying out the PCB. 

The initial V1 PCB was sent for fabrication at PCBWAY.COM on 20170111.  It's been received, reviewed, and built (fully built as of 20170202), and it works, but it has some bugs:
 1. Short of I2C line to Ground plain.
 2. Missing pullup (IORQ) and pulldown (RESET) resistors.
 3. Rx/Tx swapped on serial connection.
 
Fixing these manually results in a working board.  It's not clear that the pull-resistors are needed.

There are now TWO versions of the EAGLE Schematic/PCB design posted here:
 1. Gerbers-v1bad.Z80-4chip-PCBWAY-BAD.\* - the design as sent to PCBWAY, complete with errors (except that the errors are "marked.")
With hand-done re-work, this design is known to work.  See the notes in the "To-Do" layer of the board,
and the assorted "fix\*" files in that directory.
 1. Z80-4chip7.\* is the "corrected" design.  It has the fixes for the known bugs.  It also features a clarified (multi-sheet) schematic,
*BUT* it has not been fabbed or tested :-(

I don't know if I'll be sending the "new" design to actually be built.  A guy can only use so many Z80 boards,
and I'm not up for being "in business."
