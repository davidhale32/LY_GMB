
#include "CPU.h"

#include "GBC_instruction_set.h"

extern CPU_Registers *CHIP_8;


//char memory[0xFFFF] = Load from binary file!!!
char memory[5] = { 0x00, 0x01 , 0xBB , 0xCC , 0x76 };

int Decode(const char Opcode)
{
	// Signify the length of this instruction in memory.
	char inst_len = 1;

	// This is the main execute loop
	switch ( Opcode )
	{
		case NOP:
		{
			// Need to delay for 4 cycles
			break;
		}

		case LD_BC_IMM:
		{
			// load two bytes to BC register
			unsigned short immediate_value = memory[CHIP_8->PC + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->PC + 2];

			CHIP_8->CPU_Regs_t.PairReg.BC = immediate_value;

			inst_len = 3;

			break;
		}

		case HALT:
		{
			// Need to stop execution - change the "On" status
			CHIP_8->CPU_is_On = FALSE;
		}
	}

//	unsigned short tmp_res = CHIP_8->CPU_Regs_t.SingleReg.A +
//							 CHIP_8->CPU_Regs_t.SingleReg.B ;
//
//	//Could be truncated! check for overflow.
//	CHIP_8->CPU_Regs_t.SingleReg.A = tmp_res ;
//
//	// Check if upper 8-bits are set
//	if ( tmp_res & 0xFF00 )
//	{
//		CHIP_8->CPU_flags_t.flags.C = 1;
//	}

	return inst_len;
}

int RunCPU ()
{

	while ( CHIP_8->CPU_is_On )
	{
		// Fetch the next instruction
		char Opcode = memory[CHIP_8->PC];

		//Decode stage, decode the instruction
		CHIP_8->PC += Decode(Opcode); // Advance the program counter
	}

	return 1;
}
