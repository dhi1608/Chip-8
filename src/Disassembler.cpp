/*
 * Disassembler.cpp
 *
 *  Created on: Mar 10, 2019
 *      Author: Brian Green
 */
#include <iostream>
#include <fstream>
using namespace std;


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
  unsigned int instructionType = opcode[index] >> 4 & 0x0F;
  switch(instructionType)
  {
    case 0x0:
            //TODO create method for multiple cases
      break;
    case 0x1:
      printf("GOTO %02X%02X", opcode[index] & 0x0F, opcode[index + 1]);
      break;
    case 0x2:
      printf("DO SUBROUTINE %02X%02X", opcode[index] & 0x0F, opcode[index + 1]);
      break;
    case 0x3:
      printf("SKIP NEXT IF V%X EQ %02X", opcode[index] & 0x0F, opcode[index + 1]);
      break;
    case 0x4:
      printf("SKIP NEXT IF V%X NE %02X", opcode[index] & 0x0F, opcode[index + 1]);
      break;
    case 0x5:
      printf("SKIP NEXT IF V%X EQ V%X", opcode[index] & 0x0F, opcode[index + 1] >> 4 & 0x0F);
      break;
    case 0x6:
      printf("V%X = %02X", opcode[index] & 0x0F, opcode[index + 1]);
      break;
    case 0x7:
      printf("V%X + %02X", opcode[index] & 0x0F, opcode[index + 1]);
      break;
    case 0x8:
            //TODO create method for multiple cases
      break;
    case 0x9:
      printf("SKIP NEXT IF V%X NE V%X", opcode[index] & 0x0F, opcode[index + 1] >> 4 & 0x0F);
      break;
    case 0xA:
      printf("I = %02X%02X", opcode[index] & 0x0F, opcode[index + 1]);
      break;
    case 0xB:
      printf("GOTO %02X%02X + V0", opcode[index] & 0x0F, opcode[index + 1]);
      break;
    case 0xC:
      printf("V%X = RANDOM(%02X:%02X)", opcode[index] & 0x0F, 0, opcode[index + 1]);
      break;
    case 0xD:
      printf("SHOW %XMI@V%XV%X", opcode[index + 1] & 0x0F, opcode[index] & 0x0F, opcode[index + 1] >> 4 & 0x0F);
      break;
    case 0xE:
	    //TODO create method for multiple cases
      break;
    case 0xF:
	    //TODO create method for multiple cases
      break;

  }
  cout << endl;
}

int main (const int argc, const char **argv)
{
  const int OP_CODE_SIZE = 2;

  /**
   * Confirm user has provided correct inputs and provide
   * help if they did not
   */
  if (argc != 2)
  {
      cerr << "Usage: " << argv[0] << "<filename>" << endl;
      return 1;
  }

  /**
   * Open the binary file
   */
  ifstream rom;
  rom.open(argv[1], ios::in|ios::binary);
  if(!rom)
  {
      cerr << "Unable to open: " <<  argv[1] << endl;
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
   while(pc < romSize)
   {
       printf("%04X\t", pc);
       disassembleOpCode((unsigned char*) buffer, pc);
       pc+=OP_CODE_SIZE;
   }
   /**
    * Cleanup dynamic memory allocation
    */
   delete[] buffer;
 }
