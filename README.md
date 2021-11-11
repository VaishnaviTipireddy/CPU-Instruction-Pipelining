# CPU-Instruction-Pipelining
Read instructions stored in a file (in little endian hex format) and execute in a
CPU instruction pipeline. Assuming no branch instructions nor data hazards.

Compile

g++ cpusim.cpp -o cpusim

Run

./cpusim [FILENAME].txt

Expected output

([value in register a0], [value in register a1])

