# 4chipZ80
Z80 system using ATmega as IO/boot controller.  4 chips total.
---
This is a minimalist Z80 system based on a breadboard design by "Just4Fun" at hackaday.io:
https://hackaday.io/project/19000

My main contribution so far is the creation of a PCB, for a more permenant construction than the original breadboard.
This involved converting the printed schematic to an EAGLE version, as well as laying out the PCB. 


This project has won an **"Anything Goes" HackaDay prize** (for Just4Fun), and at this point has been built by a relatively impressive number of people in a rather impressive variety of different ways (not all of them using this PCB.)

The initial V1 PCB was sent for fabrication at PCBWAY.COM on 20170111.  It's been received, reviewed, and built (fully built as of 20170202), and it works, but it has some bugs:

 1. Short of I2C line to Ground plain.
 2. Missing pullup (IORQ) and pulldown (RESET) resistors.
 3. Rx/Tx swapped on serial connection.
 
Fixing these manually results in a working board.  It's not clear that the pull-resistors are needed.

There are now several versions of the EAGLE Schematic/PCB design posted here:

 1. Gerbers-v1bad.Z80-4chip-PCBWAY-BAD.\* - the design as sent to PCBWAY, complete with errors (except that the errors are "marked.")
With hand-done re-work, this design is known to work.  See the notes in the "To-Do" layer of the board, and the assorted "fix\*" files in that directory.
 1. Z80-4chip-v2.1.\* is the "corrected" design.  It has the fixes for the known bugs.  It also features a clarified (multi-sheet) schematic, *BUT* I have not personally fabbed or tested it, but several people on Hackaday.io have built V2 boards and say that they work.
 2. Z80-4chip-v3.\* Adds sockets for I2C EEPROM "Floppy Disks" that should permit use of CP/M and perhaps other Operating Systems.  Just4Fun has implemented the CP/M and QP/M BIOS and etc.  This has also not yet been fabbed or tested (but I have a volunteer!)  (Also Gerbers-V3/\* has pre-made geber files.)

I don't know if I'll be sending the "new" design(s) to actually be built.  A guy can only use so many Z80 boards, and I'm not up for being "in business."  I'll try to keep this update if other people use the design successfully.

Potential users may want to note that the original developer of the 4-chip Z80 system (Just4Fun) has published a new and improved version of the design that includes a PCB, at https://hackaday.io/project/159973
