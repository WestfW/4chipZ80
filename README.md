# 4chipZ80
Z80 system using ATmega as IO/boot controller.  4 chips total.
---
This is a minimalist Z80 system based on a breadboard design by "Just4Fun" at hackaday.io:
https://hackaday.io/project/19000

My main contribution so far is the creation of a PCB, for a more permenant construction than the original breadboard.
This involved converting the printed schematic to an EAGLE version, as well as laying out the PCB. 

The PCB is not yet tested (waiting for chips), so those files are not uploaded yet.
The initial V1 PCB was sent for fabrication at PCBWAY.COM on 20170111, but it has some bugs:
 1. Short of I2C line to Ground plain.
 2. Missing pullup (IORQ) and pulldown (RESET) resistors.
 3. Rx/Tx swapped on serial connection.
 
