/*
 ============================================================================
 Name        : GBC_emu.c
 Author      : Dan_Levi
 Version     :
 Copyright   : Dan_Levi
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#include "CPU.h"
#include "Memory.h"

CPU_Registers *CHIP_8;



int main (void)
{
	CPU_Registers cpu;

	// Initialize the general purpose registers.
	memset(&cpu, 0x0, sizeof(cpu));
	Memory.I_ME_flag = 1;

	CHIP_8 = &cpu;

	// Initialize the first program counter
	CHIP_8->PC = 0x0  ; // Here we have the bootROM
	CHIP_8->SP = 0xFFFE; // Stack pointer
	CHIP_8->CPU_Regs_t.SingleReg.B = 0x01;

	//Turn on CPU
	CHIP_8->CPU_is_On = TRUE;

	RunCPU();
}
