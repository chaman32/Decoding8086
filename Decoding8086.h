#ifndef DECODING8086_INCLUDE_H
#define DECODING8086_INCLUDE_H

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define FOPEN(filePointer, filename, mode) \
    ((filePointer) = fopen((filename), (mode)))


typedef unsigned char byte;

typedef struct
{
	byte byte1;
	byte byte2;
	byte byte3;
	byte byte4;
	byte byte5;
	byte byte6;
	

}Instruction;

typedef struct
{
	byte d_flag;
	byte w_flag;
	byte reg;
	byte rm;
	byte mod;

}register_field;

typedef struct
{
	Instruction pack_instruction;
	unsigned char  memory[64 * 1024];	// (size = 64k)	
	byte ip;
	size_t program_size;
	register_field reg_field;
	char* output_file;

}P8086;



P8086* proccessor;
 
void init();
void run(byte* program, size_t size);
void load_program(byte* program, size_t size, char* output_file);
const char* decode_opcode(byte instruction);
register_field decode_register_flags(byte byte1, byte byte2);
const char* decode_register_field(byte register, byte w_flag);
void write_output_file(char* opcode, char* r_dest, char* r_sour, char* output_file, char* asm_code);

#endif


 