/*
 * Disassembler.cpp
 *
 * Created on: Mar 10, 2019
 * Author: Brian Green
 */

#include <iostream>
#include <fstream>
using namespace std;

//TODO replace current Chip-8 syntax with the instructions from Cowgod's Technical Resource
// It is more concise and complete than my initial proof of concept instructions

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
	if (byte1 == 0x00)
	{
		if (byte2 == 0xE0)
		{
			printf("ERASE");
		}
		else if (byte2 == 0xEE)
		{
			printf("RETURN");
		}
		else
		{
			printf("EXECUTE ML");
		}
	}
	else
	{
		printf("EXECUTE ML");
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
		printf("V%X = V%X", x, y);
		break;
	case 0x1:
		printf("V%X = V%X / V%X;VF=~VF", x, x, y);
		break;
	case 0x2:
		printf("V%X = V%X & V%X;VF=~VF", x, x, y);
		break;
	case 0x4:
		printf("V%X = V%X + V%X;VF=0x00 or VF=0x01 (rollover)", x, x, y);
		break;
	case 0x5:
		printf("V%X = V%X - V%X;VF=0x00 or VF=0x01 (negative)", x, x, y);
		break;
	default:
		printf("Undocumented instruction X:%X Y:%X Mode:%X", x, y, mode);
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
		printf("SKIP NEXT IF V%X EQ HEX KEY", x);
		break;
	case 0xA1:
		printf("SKIP NEXT IF V%X NE HEX KEY", x);
		break;
	default:
		printf("Undocumented instruction X:%X Mode:%X", x, mode);
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
		printf("V%X = TIMER", x);
		break;
	case 0x0A:
		printf("V%X = HEX KEY (WAIT)", x);
		break;
	case 0x15:
		printf("TIMER = V%X (01 = 1/60 sec)", x);
		break;
	case 0x18:
		printf("TONE DURATION = V%X (01 = 1/60 sec)", x);
		break;
	case 0x1E:
		printf("I = I + V%X", x);
		break;
	case 0x29:
		printf("I = V%X (ADDRESS OF SPRITE)", x);
		break;
	case 0x33:
		printf("MI = V%X (3-DECIMAL DIGIT)", x);
		break;
	case 0x55:
		printf("MI = V0:V%X STORE REG(I = I + X + 1)", x);
		break;
	case 0x65:
		printf("V0:V%X MI FILL REG(I = I + X + 1)", x);
		break;
	default:
		printf("Undocumented instruction X:%X Mode:%X", x, mode);
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
	unsigned char instructionType = opcode[index] >> 4 & 0x0F;
	switch (instructionType)
	{
	case 0x0:
		processInstruction0(opcode, index);
		break;
	case 0x1:
		printf("GOTO %02X%02X", opcode[index] & 0x0F, opcode[index + 1]);
		break;
	case 0x2:
		printf("DO SUBROUTINE %02X%02X", opcode[index] & 0x0F,
				opcode[index + 1]);
		break;
	case 0x3:
		printf("SKIP NEXT IF V%X EQ %02X", opcode[index] & 0x0F,
				opcode[index + 1]);
		break;
	case 0x4:
		printf("SKIP NEXT IF V%X NE %02X", opcode[index] & 0x0F,
				opcode[index + 1]);
		break;
	case 0x5:
		printf("SKIP NEXT IF V%X EQ V%X", opcode[index] & 0x0F,
				opcode[index + 1] >> 4 & 0x0F);
		break;
	case 0x6:
		printf("V%X = %02X", opcode[index] & 0x0F, opcode[index + 1]);
		break;
	case 0x7:
		printf("V%X + %02X", opcode[index] & 0x0F, opcode[index + 1]);
		break;
	case 0x8:
		processInstruction8(opcode, index);
		break;
	case 0x9:
		printf("SKIP NEXT IF V%X NE V%X", opcode[index] & 0x0F,
				opcode[index + 1] >> 4 & 0x0F);
		break;
	case 0xA:
		printf("I = %02X%02X", opcode[index] & 0x0F, opcode[index + 1]);
		break;
	case 0xB:
		printf("GOTO %02X%02X + V0", opcode[index] & 0x0F, opcode[index + 1]);
		break;
	case 0xC:
		printf("V%X = RANDOM(%02X:%02X)", opcode[index] & 0x0F, 0,
				opcode[index + 1]);
		break;
	case 0xD:
		printf("SHOW %XMI@V%XV%X", opcode[index + 1] & 0x0F,
				opcode[index] & 0x0F, opcode[index + 1] >> 4 & 0x0F);
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
		return 1;
	}

	/**
	 * Open the binary file
	 */
	ifstream rom;
	rom.open(argv[1], ios::in | ios::binary);
	if (!rom)
	{
		printf("Unable to open %s\n", argv[1]);
		return 1;
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
		printf("%04X\t", pc);
		disassembleOpCode((unsigned char*) buffer, pc);
		pc += OP_CODE_SIZE;
	}

	/**
	 * Cleanup dynamic memory allocation
	 */
	delete[] buffer;

}
