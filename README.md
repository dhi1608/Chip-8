Chip-8 Emulator

Create a CPU emulator for the Chip-8 processor. The Chip-8 is a processor from the 70's that is an excellent introduction to hardware emulation as it has a limited instruction set and is very well documented. Writing the emulator from scratch will give you some very good exposure to the type of software we deal with. Here is a decent sequence of events...

1. Research Chip-8; gather data sheets / instruction set info; gather some sample Chip-8 ROMs.
2. Create a disassembler and make sure it produces sensible output for a known good ROM.
3. Emulate the CPU (i.e. handle sequences of opcodes with your virtual processor).
4. Emulate the peripheral devices like user input, sound / video output. This part is less important, so feel free to cheat on this part from existing code on the internet.

Requirements:

1. The emulator shall be capable of disassembling a Chip-8 ROM and producing source assembly listings.
2. The emulator shall be capable of executing a Chip-8 ROM without crashing.
3. The emulator shall be able to accept user input via keyboard and produce a graphical display.
