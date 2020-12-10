
#include "CPU.h"

#include "GBC_instruction_set.h"

extern CPU_Registers *CHIP_8;


//char memory[0xFFFF] = Load from binary file!!!
char memory[3] = {0x00, 0x1F ,0x76 };

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
			// Load two bytes to BC register
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
			break;
		}

		case LD_BC_A:
		{
			// Load A register to BC register
			CHIP_8->CPU_Regs_t.PairReg.BC  = 0x0;
			CHIP_8->CPU_Regs_t.PairReg.BC = CHIP_8->CPU_Regs_t.SingleReg.A;
			break;
		}

		case INC_BC:
		{
			// increment BC register by 1
			CHIP_8->CPU_Regs_t.PairReg.BC += 1;
			break;
		}

		case INC_B:
		{
			// increment B register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.B;
			CHIP_8->CPU_Regs_t.SingleReg.B += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if (((culc_value & 0xf) + (1 & 0xf) & 0x10) == 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_B:
		{
			// Decrement B register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.B;
			CHIP_8->CPU_Regs_t.SingleReg.B -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((culc_value & 0xf) - (1 & 0xf) > 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_B_IMM:
		{
			// Load one byte to B register
			char immediate_value = memory[CHIP_8->PC + 1];
			CHIP_8->CPU_Regs_t.SingleReg.B = immediate_value;

			inst_len = 2;

			break;
		}

		case RLCA:
		{
			// Rotate A register left by one bit
			char tmp_flag  = CHIP_8->CPU_Regs_t.SingleReg.A >> 7;
			CHIP_8->CPU_Regs_t.SingleReg.A <<=1;
			CHIP_8->CPU_Regs_t.SingleReg.A |= tmp_flag;
			CHIP_8->CPU_flags_t.flags.C = tmp_flag;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;
		}

		case LD_SP_IMM_ADDRESS:
		{
			// Put Stack pointer at immediate tow bit address
			unsigned short immediate_value = memory[CHIP_8->PC + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->PC + 2];

			CHIP_8->SP = immediate_value;

			inst_len = 3;

			break;
		}

		case ADD_HL_BC:
		{
			// Add BC to HL
			if( (CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) +
				(CHIP_8->CPU_Regs_t.PairReg.BC & 0x00FF) & 0x0100 == 0x0100)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.PairReg.HL += CHIP_8->CPU_Regs_t.PairReg.BC;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.PairReg.HL > 0xFFFF)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;
		}

		case LD_A_BC:
		{
			// Load BC  to A register
			CHIP_8->CPU_Regs_t.SingleReg.A = CHIP_8->CPU_Regs_t.PairReg.BC;

			break;
		}

		case DEC_BC:
		{
			// Decrement BC register by 1
			CHIP_8->CPU_Regs_t.PairReg.BC -=1;

			break;
		}

		case INC_C:
		{
			// increment C register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.C;
			CHIP_8->CPU_Regs_t.SingleReg.C += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.C == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if (((culc_value & 0xf) + (1 & 0xf) & 0x10) == 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_C:
		{
			// Decrement C register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.C;
			CHIP_8->CPU_Regs_t.SingleReg.C -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.C == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 0x01;
			}

			if ((culc_value & 0xf) - (1 & 0xf) > 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 0x01;
			}

			break;
		}

		case LD_C_IMM:
		{
			// Load one byte to C register
			char immediate_value = memory[CHIP_8->PC + 1];
			CHIP_8->CPU_Regs_t.SingleReg.C = immediate_value;

			inst_len = 2;

			break;
		}

		case RRCA:
		{
			// Rotate A register right by one bit
			char tmp = CHIP_8->CPU_Regs_t.SingleReg.A & 1;
			CHIP_8->CPU_flags_t.flags.C = tmp;
			CHIP_8->CPU_Regs_t.SingleReg.A >>=1;
			tmp <<=7;
			CHIP_8->CPU_Regs_t.SingleReg.A |= tmp;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;
		}

		case STOP:
		{
			// Need to stop execution - change the "On" status
			CHIP_8->CPU_is_On = FALSE;
			inst_len = 2;

			break;
		}

		case LD_DE_IMM:
		{
			// Load two bytes to DE register
			unsigned short immediate_value = memory[CHIP_8->PC + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->PC + 2];

			CHIP_8->CPU_Regs_t.PairReg.DE = immediate_value;

			inst_len = 3;

			break;
		}

		case LD_DE_A:
		{
			// Load A register to DE register
			CHIP_8->CPU_Regs_t.PairReg.DE  = 0x0;
			CHIP_8->CPU_Regs_t.PairReg.DE = CHIP_8->CPU_Regs_t.SingleReg.A;

			break;
		}

		case INC_DE:
		{
			// increment DE register by 1
			CHIP_8->CPU_Regs_t.PairReg.DE += 1;

			break;
		}

		case INC_D:
		{
			// increment D register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.D;
			CHIP_8->CPU_Regs_t.SingleReg.D += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.D == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if (((culc_value & 0xf) + (1 & 0xf) & 0x10) == 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_D:
		{
			// Decrement D register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.D;
			CHIP_8->CPU_Regs_t.SingleReg.D -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.D == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((culc_value & 0xf) - (1 & 0xf) > 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_D_IMM:
		{
			// Load one byte to D register
			char immediate_value = memory[CHIP_8->PC + 1];
			CHIP_8->CPU_Regs_t.SingleReg.D = immediate_value;

			inst_len = 2;

			break;
		}

		case RLA:
		{
			// Rotate A register left by one bit through carry flag
			char old_carry = CHIP_8->CPU_flags_t.flags.C;
			char bit7 = CHIP_8->CPU_Regs_t.SingleReg.A >> 7;
			CHIP_8->CPU_flags_t.flags.C = bit7;
			CHIP_8->CPU_Regs_t.SingleReg.A <<= 1;
			CHIP_8->CPU_Regs_t.SingleReg.A |= old_carry;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;
		}

		case JR_r8:
		{
			//Add immediate to current address and jump to it
			short immediate_value = (signed)memory[CHIP_8->PC + 1];
			inst_len = immediate_value;

			break;
		}

		case ADD_HL_DE:
		{
			// Add DE to HL
			if( (CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) +
				(CHIP_8->CPU_Regs_t.PairReg.DE & 0x00FF) & 0x0100 == 0x0100)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.PairReg.HL += CHIP_8->CPU_Regs_t.PairReg.DE;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.PairReg.HL > 0xFFFF)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;
		}

		case LD_A_DE:
		{
			// Load DE  to A register
			CHIP_8->CPU_Regs_t.SingleReg.A = CHIP_8->CPU_Regs_t.PairReg.DE;

			break;
		}

		case DEC_DE:
		{
			// Decrement DE register by 1
			CHIP_8->CPU_Regs_t.PairReg.DE -=1;

			break;
		}

		case INC_E:
		{
			// increment E register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.E;
			CHIP_8->CPU_Regs_t.SingleReg.E += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.E == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if (((culc_value & 0xf) + (1 & 0xf) & 0x10) == 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_E:
		{
			// Decrement E register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.E;
			CHIP_8->CPU_Regs_t.SingleReg.E -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.E == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((culc_value & 0xf) - (1 & 0xf) > 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_E_IMM:
		{
			// Load one byte to E register
			char immediate_value = memory[CHIP_8->PC + 1];
			CHIP_8->CPU_Regs_t.SingleReg.E = immediate_value;

			inst_len = 2;

			break;
		}

		case RRA:
		{
			// Rotate A register right by one bit through carry flag
			char old_carry = CHIP_8->CPU_flags_t.flags.C;
			char bit0 = CHIP_8->CPU_Regs_t.SingleReg.A & 1;
			CHIP_8->CPU_flags_t.flags.C = bit0;
			CHIP_8->CPU_Regs_t.SingleReg.A >>= 1;
			old_carry <<= 7;
			CHIP_8->CPU_Regs_t.SingleReg.A |= old_carry;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;
		}

		case JR_NZ_r8:
		{
			//if z are rest add the immediate value to PC
			if(CHIP_8->CPU_flags_t.flags.Z == 0)
			{
				short immediate_value = (signed)memory[CHIP_8->PC + 1];
				inst_len = immediate_value;
			}

			break;

		}

		case LD_HL_IMM:
		{
			// Load two bytes to HL register
			unsigned short immediate_value = memory[CHIP_8->PC + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->PC + 2];

			CHIP_8->CPU_Regs_t.PairReg.HL = immediate_value;

			inst_len = 3;

			break;
		}

		case LD_HL_plus_A :
		{
			// Put A into memory address HL Increment HL
			memory[CHIP_8->CPU_Regs_t.PairReg.HL] = CHIP_8->CPU_Regs_t.SingleReg.A;
			CHIP_8->CPU_Regs_t.PairReg.HL +=1;

			break;

		}

		case INC_HL:
		{
			// increment HL register by 1
			CHIP_8->CPU_Regs_t.PairReg.HL += 1;

			break;
		}

		case INC_H:
		{
			// increment H register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.H;
			CHIP_8->CPU_Regs_t.SingleReg.H += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.H == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if (((culc_value & 0xf) + (1 & 0xf) & 0x10) == 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_H:
		{
			// Decrement H register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.H;
			CHIP_8->CPU_Regs_t.SingleReg.H -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.H == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((culc_value & 0xf) - (1 & 0xf) > 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_H_IMM:
		{
			// Load one byte to H register
			char immediate_value = memory[CHIP_8->PC + 1];
			CHIP_8->CPU_Regs_t.SingleReg.H = immediate_value;

			inst_len = 2;

			break;

		}

		case DAA:
		{
			/*Decimal adjust register A.
			This instruction adjusts register A so that the correct
			representation of Binary Coded Decimal is obtained*/
			char LS_A_value = CHIP_8->CPU_Regs_t.SingleReg.A &= 0xF;

			if(( LS_A_value <= 9) && CHIP_8->CPU_flags_t.flags.N == 0 &&
				CHIP_8->CPU_flags_t.flags.Z == 0 && CHIP_8->CPU_flags_t.flags.H == 0 &&
				CHIP_8->CPU_flags_t.flags.C == 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 0;
				CHIP_8->CPU_flags_t.flags.Z = 0;
			}

			if(( LS_A_value > 9) && CHIP_8->CPU_flags_t.flags.N == 1 &&
				CHIP_8->CPU_flags_t.flags.Z == 0 && CHIP_8->CPU_flags_t.flags.H == 1 &&
				CHIP_8->CPU_flags_t.flags.C == 0)
			{
				CHIP_8->CPU_Regs_t.SingleReg.A += 0x06;
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 0;
				CHIP_8->CPU_flags_t.flags.Z = 0;
			}

			if(( LS_A_value < 9) && CHIP_8->CPU_flags_t.flags.N == 0 &&
				CHIP_8->CPU_flags_t.flags.Z == 0 && CHIP_8->CPU_flags_t.flags.H == 0 &&
				CHIP_8->CPU_flags_t.flags.C == 1)
			{
				CHIP_8->CPU_Regs_t.SingleReg.A += 0x60;
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 1;
				CHIP_8->CPU_flags_t.flags.Z = 0;
			}

			if(( LS_A_value > 9) && CHIP_8->CPU_flags_t.flags.N == 0 &&
				CHIP_8->CPU_flags_t.flags.Z == 0 && CHIP_8->CPU_flags_t.flags.H == 0 &&
				CHIP_8->CPU_flags_t.flags.C == 1)
			{
				CHIP_8->CPU_Regs_t.SingleReg.A += 0x66;
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 1;
				CHIP_8->CPU_flags_t.flags.Z = 0;
			}

			break;

		}

		case JR_Z_r8:
		{
			//if z are set add the immediate value to PC
			if(CHIP_8->CPU_flags_t.flags.Z == 1)
			{
				short immediate_value = (signed)memory[CHIP_8->PC + 1];
				inst_len = immediate_value;
			}

			break;

		}

		case ADD_HL_HL:
		{
			// Add HL to HL
			if( (CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) +
				(CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) & 0x0100 == 0x0100)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.PairReg.HL += CHIP_8->CPU_Regs_t.PairReg.HL;

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.PairReg.HL > 0xFFFF)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;
		}

		case LD_A_HL_plus:
		{
			// Put value at address HL into A. Increment HL.
			CHIP_8->CPU_Regs_t.SingleReg.A = memory[CHIP_8->CPU_Regs_t.PairReg.HL];
			CHIP_8->CPU_Regs_t.PairReg.HL += 1;

			break;
		}

		case DEC_HL:
		{
			// Decrement HL register by 1
			CHIP_8->CPU_Regs_t.PairReg.HL -=1;

			break;
		}

		case INC_L:
		{
			// increment L register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.L;
			CHIP_8->CPU_Regs_t.SingleReg.L += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.L == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if (((culc_value & 0xf) + (1 & 0xf) & 0x10) == 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_L:
		{
			// Decrement E register by 1
			char culc_value = CHIP_8->CPU_Regs_t.SingleReg.L;
			CHIP_8->CPU_Regs_t.SingleReg.L -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.L == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((culc_value & 0xf) - (1 & 0xf) > 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_L_IMM:
		{
			// Load one byte to L register
			char immediate_value = memory[CHIP_8->PC + 1];
			CHIP_8->CPU_Regs_t.SingleReg.L = immediate_value;

			inst_len = 2;

			break;
		}

		case CPL:
		{
			//Complement A register Flip all bits
			CHIP_8->CPU_Regs_t.SingleReg.A = ~(CHIP_8->CPU_Regs_t.SingleReg.A);
			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;
			break;

		}

		case JR_NC_r8:
		{
			//if C are rest add the immediate value to PC
			if(CHIP_8->CPU_flags_t.flags.C == 0)
			{
				short immediate_value = (signed)memory[CHIP_8->PC + 1];
				inst_len = immediate_value;
			}

			break;

		}

		case LD_SP_IMM:
		{
			// Load two bytes to SP register
			unsigned short immediate_value = memory[CHIP_8->PC + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->PC + 2];

			CHIP_8->SP= immediate_value;

			inst_len = 3;

			break;
		}

		case LD_HL_minus_A :
		{
			// Put A into memory address HL  Decrement HL
			memory[CHIP_8->CPU_Regs_t.PairReg.HL] = CHIP_8->CPU_Regs_t.SingleReg.A;
			CHIP_8->CPU_Regs_t.PairReg.HL -=1;

			break;

		}

		case INC_SP:
		{
			// increment SP register by 1
			CHIP_8->SP += 1;

			break;

		}

		case INC_hl:
		{
			// increment HL register by 1
			char culc_value = CHIP_8->CPU_Regs_t.PairReg.HL;
			CHIP_8->CPU_Regs_t.PairReg.HL += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.PairReg.HL == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if (((culc_value & 0xf) + (1 & 0xf) & 0x10) == 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;

		}

		case DEC_hl:
		{
			// Decrement HL register by 1
			char culc_value = CHIP_8->CPU_Regs_t.PairReg.HL;
			CHIP_8->CPU_Regs_t.PairReg.HL -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.PairReg.HL == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((culc_value & 0xf) - (1 & 0xf) > 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
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
