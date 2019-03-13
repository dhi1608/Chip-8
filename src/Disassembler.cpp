/*
 * Disassembler.cpp
 *
 * Created on: Mar 10, 2019
 * Author: Brian Green
 */

#include <iostream>
#include <fstream>
using namespace std;

/**
 * Method processes instruction 0 types
 *
 * opCode - buffer
 * index - location within buffer
 *
 * Identifies unsupported/documented instructions
 */
void processInstruction0(unsigned char *opcode, int index)
{
	unsigned char byte1 = opcode[index] & 0xFF;
	unsigned char byte2 = opcode[index + 1] & 0xFF;

	if (byte1 == 0x00 && byte2 == 0xE0)
	{
		printf("CLS");
	}
	else if (byte1 == 0x00 && byte2 == 0xEE)
	{
		printf("RET");
	}
	else
	{
		printf("SYS %X%02X", byte1, byte2);
	}
}

/**
 * Method processes instruction 8 types
 *
 * opCode - buffer
 * index - location within buffer
 *
 * Identifies unsupported/documented instructions
 */
void processInstruction8(unsigned char *opcode, int index)
{
	unsigned char x = opcode[index] & 0x0F;
	unsigned char y = opcode[index + 1] >> 4 & 0x0F;
	unsigned char mode = opcode[index + 1] & 0x0F;

	switch (mode)
	{
	case 0x0:
		printf("LD V%X, V%X", x, y);
		break;
	case 0x1:
		printf("OR V%X, V%X", x, y);
		break;
	case 0x2:
		printf("AND V%X, V%X", x, y);
		break;
	case 0x3:
		printf("XOR V%X, V%X", x, y);
		break;
	case 0x4:
		printf("ADD V%X, V%X", x, y);
		break;
	case 0x5:
		printf("SUB V%X, V%X", x, y);
		break;
	case 0x6:
		printf("SHR V%X, {V%X}", x, y);
		break;
	case 0x7:
		printf("SUBN V%X, V%X", x, y);
		break;
	case 0xE:
		printf("SHL V%X, {V%X}", x, y);
		break;
	}
}

/**
 * Method processes instruction E types
 *
 * opCode - buffer
 * index - location within buffer
 *
 * Identifies unsupported/documented instructions
 */
void processInstructionE(unsigned char *opcode, int index)
{
	unsigned char x = opcode[index] & 0x0F;
	unsigned char mode = opcode[index + 1] & 0xFF;

	switch (mode)
	{
	case 0x9E:
		printf("SKP V%X", x);
		break;
	case 0xA1:
		printf("SKNP V%X", x);
		break;
	}
}

/**
 * Method processes instruction F types
 *
 * opCode - buffer
 * index - location within buffer
 *
 * Identifies unsupported/documented instructions
 */
void processInstructionF(unsigned char *opcode, int index)
{
	unsigned char x = opcode[index] & 0x0F;
	unsigned char mode = opcode[index + 1] & 0xFF;

	switch (mode)
	{
	case 0x07:
		printf("LD V%X, DT", x);
		break;
	case 0x0A:
		printf("LD V%X, K", x);
		break;
	case 0x15:
		printf("LD DT, V%X", x);
		break;
	case 0x18:
		printf("LD ST, V%X", x);
		break;
	case 0x1E:
		printf("ADD I, V%X", x);
		break;
	case 0x29:
		printf("LD F, V%X", x);
		break;
	case 0x33:
		printf("LD B, V%X", x);
		break;
	case 0x55:
		printf("LD [I], V%X", x);
		break;
	case 0x65:
		printf("LD V%X, [I]", x);
		break;
	}
}

/**
 * Method disassembles the OpCode based on the
 * 1st Nibble of the OpCode
 *
 * rBuffer - rom buffer
 * index - location within rom buffer
 */
void disassembleOpCode(unsigned char *opcode, int index)
{
	/**
	 * Print raw OpCode
	 */
	printf("%02X%02X\t", opcode[index], opcode[index + 1]);

	/**
	 * Print operation based on instruction type
	 */
	unsigned char byte1 = opcode[index] & 0x0F;
	unsigned char byte2 = opcode[index + 1];
	unsigned char mode = opcode[index] >> 4 & 0x0F;
	switch (mode)
	{
	case 0x0:
		processInstruction0(opcode, index);
		break;
	case 0x1:
		printf("JP %02X%02X",byte1, byte2);
		break;
	case 0x2:
		printf("CALL %02X%02X",byte1, byte2);
		break;
	case 0x3:
		printf("SE V%X, %02X",byte1, byte2);
		break;
	case 0x4:
		printf("SNE V%X, %02X", byte1, byte2);
		break;
	case 0x5:
		printf("SE V%X, V%X", byte1, byte2 >> 4 & 0x0F);
		break;
	case 0x6:
		printf("LD V%X, %02X",byte1, byte2);
		break;
	case 0x7:
		printf("ADD V%X, %02X",byte1, byte2);
		break;
	case 0x8:
		processInstruction8(opcode, index);
		break;
	case 0x9:
		printf("SNE V%X, V%X", byte1, byte2 >> 4 & 0x0F);
		break;
	case 0xA:
		printf("LD I, %02X%02X",byte1, byte2);
		break;
	case 0xB:
		printf("JP V0 %02X%02X",byte1, byte2);
		break;
	case 0xC:
		printf("RND V%X, %02X%02X", byte1, byte2);
		break;
	case 0xD:
		printf("DRW V%X, V%X %X", byte1, byte2 >> 4 & 0x0F, byte2 & 0x0F);
		break;
	case 0xE:
		processInstructionE(opcode, index);
		break;
	case 0xF:
		processInstructionF(opcode, index);
		break;
	}
	printf("\n");
}

/******************************************
 * Main method
 ******************************************/
int main(const int argc, const char **argv)
{
	const int OP_CODE_SIZE = 2;

	/**
	 * Confirm user has provided correct inputs and provide
	 * help if they did not
	 */
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	/**
	 * Open the binary file
	 */
	ifstream rom;
	rom.open(argv[1], ios::in | ios::binary);
	if (!rom)
	{
		printf("Unable to open %s\n", argv[1]);
		exit(1);
	}

	/**
	 * Get size of the ROM
	 */
	rom.seekg(0, rom.end);
	int romSize = rom.tellg();
	rom.seekg(0, rom.beg);

	/**
	 * Read ROM into memory and close file
	 */

	char *buffer = new char[romSize];
	rom.read(buffer, romSize);
	rom.close();

	int pc = 0;
	while (pc < romSize)
	{
		printf("%04X\t", pc+0x0200);
		disassembleOpCode((unsigned char*) buffer, pc);
		pc += OP_CODE_SIZE;
	}

	/**
	 * Cleanup dynamic memory allocation
	 */
	delete[] buffer;

}
