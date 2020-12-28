
#include "CPU.h"
#include "Memory.h"

#include "GBC_instruction_set.h"

extern CPU_Registers *CHIP_8;
extern MEMORY Memory;

//char memory[0xFFFF] = Load from binary file!!!
char memory[] = {0x00,0xC3,0x00,0x05,0x76,0x04,0x76};

unsigned char Stack_map[] = {0x40,0x48,0x50};

int Decode(const unsigned char Opcode)
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
			CHIP_8->CPU_Regs_t.SingleReg.B += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if((CHIP_8->CPU_Regs_t.SingleReg.B & 0x0F) == 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_B:
		{
			// Decrement B register by 1
			CHIP_8->CPU_Regs_t.SingleReg.B -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((CHIP_8->CPU_Regs_t.SingleReg.B & 0x0F) == 0x0F)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_B_IMM:
		{
			// Load one byte to B register
			CHIP_8->CPU_Regs_t.SingleReg.B = memory[CHIP_8->PC + 1];

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
			if( (((CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) +
				(CHIP_8->CPU_Regs_t.PairReg.BC & 0x00FF)) & 0x0100) == 0x0100)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.PairReg.HL += CHIP_8->CPU_Regs_t.PairReg.BC;
			CHIP_8->CPU_flags_t.flags.N = 0;
			int over_flow = CHIP_8->CPU_Regs_t.PairReg.HL +
					        CHIP_8->CPU_Regs_t.PairReg.BC ;

			if(over_flow > 0xFFFF)
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
			CHIP_8->CPU_Regs_t.SingleReg.C += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.C == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if((CHIP_8->CPU_Regs_t.SingleReg.C & 0x0F) == 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_C:
		{
			// Decrement C register by 1
			CHIP_8->CPU_Regs_t.SingleReg.C -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.C == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 0x01;
			}

			if ((CHIP_8->CPU_Regs_t.SingleReg.C & 0x0F) == 0x0F)
			{
				CHIP_8->CPU_flags_t.flags.H = 0x01;
			}

			break;
		}

		case LD_C_IMM:
		{
			// Load one byte to C register
			CHIP_8->CPU_Regs_t.SingleReg.C  = memory[CHIP_8->PC + 1];

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
			CHIP_8->CPU_Regs_t.SingleReg.D += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.D == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if((CHIP_8->CPU_Regs_t.SingleReg.D & 0x0F) == 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_D:
		{
			// Decrement D register by 1
			CHIP_8->CPU_Regs_t.SingleReg.D -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.D == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((CHIP_8->CPU_Regs_t.SingleReg.D & 0x0F) == 0x0F)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_D_IMM:
		{
			// Load one byte to D register
			CHIP_8->CPU_Regs_t.SingleReg.D = memory[CHIP_8->PC + 1];

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
			inst_len  = (signed char)memory[CHIP_8->PC + 1];

			break;
		}

		case ADD_HL_DE:
		{
			// Add DE to HL
			if( (((CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) +
				(CHIP_8->CPU_Regs_t.PairReg.DE & 0x00FF)) & 0x0100) == 0x0100)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.PairReg.HL += CHIP_8->CPU_Regs_t.PairReg.DE;
			CHIP_8->CPU_flags_t.flags.N = 0;
			int over_flow = CHIP_8->CPU_Regs_t.PairReg.HL +
					        CHIP_8->CPU_Regs_t.PairReg.DE;

			if(over_flow > 0xFFFF)
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
			CHIP_8->CPU_Regs_t.SingleReg.E += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.E == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if((CHIP_8->CPU_Regs_t.SingleReg.E & 0x0F) == 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_E:
		{
			// Decrement E register by 1
			CHIP_8->CPU_Regs_t.SingleReg.E -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.E == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((CHIP_8->CPU_Regs_t.SingleReg.E & 0x0F) == 0x0F)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_E_IMM:
		{
			// Load one byte to E register
			CHIP_8->CPU_Regs_t.SingleReg.E = memory[CHIP_8->PC + 1];

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
			//if z are reset add the immediate value to PC
			if(CHIP_8->CPU_flags_t.flags.Z == 0)
			{
				inst_len = (signed char)memory[CHIP_8->PC + 1];
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
			CHIP_8->CPU_Regs_t.SingleReg.H += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.H == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if((CHIP_8->CPU_Regs_t.SingleReg.H & 0x0F) == 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_H:
		{
			// Decrement H register by 1
			CHIP_8->CPU_Regs_t.SingleReg.H -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.H == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((CHIP_8->CPU_Regs_t.SingleReg.H & 0x0F) == 0x0F)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_H_IMM:
		{
			// Load one byte to H register
			CHIP_8->CPU_Regs_t.SingleReg.H = memory[CHIP_8->PC + 1];

			inst_len = 2;

			break;

		}

		case DAA:
		{
			/*Decimal adjust register A.
			This instruction adjusts register A so that the correct
			representation of Binary Coded Decimal is obtained*/
			unsigned char ls_a_value = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			unsigned char hs_a_value = CHIP_8->CPU_Regs_t.SingleReg.A >> 4;
			char h_flag = CHIP_8->CPU_flags_t.flags.H;
			char c_flag = CHIP_8->CPU_flags_t.flags.C;

			if((hs_a_value <= 9) && (ls_a_value <= 9) && (h_flag == 0) && (c_flag == 0))
			{
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 0;

				if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
				{
					CHIP_8->CPU_flags_t.flags.Z = 1;
				}

				break;
			}

			if(((hs_a_value < 9) && (ls_a_value > 9) && (h_flag == 0) && (c_flag == 0))||
			  ((hs_a_value <= 9) && (ls_a_value < 4) && (h_flag == 1) && (c_flag == 0)))
			{
				CHIP_8->CPU_Regs_t.SingleReg.A += 0x06;
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 0;

				if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
				{
					CHIP_8->CPU_flags_t.flags.Z = 1;
				}

				break;
			}

			if(((hs_a_value > 9) && (ls_a_value <= 9) && (h_flag == 0) && (c_flag == 0))||
			  ((hs_a_value < 3) && (ls_a_value <= 9) && (h_flag == 0) && (c_flag == 1)))
			{
				CHIP_8->CPU_Regs_t.SingleReg.A += 0x60;
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 1;

				if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
				{
					CHIP_8->CPU_flags_t.flags.Z = 1;
				}

				break;
			}

			if(((hs_a_value > 8) && (ls_a_value > 9) && (h_flag == 0) && (c_flag == 0))||
			  ((hs_a_value > 9) && (ls_a_value < 3) && (h_flag == 1) && (c_flag == 0))||
			  ((hs_a_value < 3) && (ls_a_value > 9) && (h_flag == 0) && (c_flag == 1))||
			  ((hs_a_value < 4) && (ls_a_value < 4) && (h_flag == 1) && (c_flag == 1)))
			{
				CHIP_8->CPU_Regs_t.SingleReg.A += 0x66;
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 1;

				if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
				{
					CHIP_8->CPU_flags_t.flags.Z = 1;
				}

				break;
			}

			if((hs_a_value < 9) && (ls_a_value > 5) && (h_flag == 1) && (c_flag == 0))
			{
				CHIP_8->CPU_Regs_t.SingleReg.A += 0xFA;
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 0;

				if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
				{
					CHIP_8->CPU_flags_t.flags.Z = 1;
				}

				break;
			}

			if((hs_a_value > 6) && (ls_a_value <= 9) && (h_flag == 0) && (c_flag == 1))
			{
				CHIP_8->CPU_Regs_t.SingleReg.A += 0xA0;
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 1;

				if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
				{
					CHIP_8->CPU_flags_t.flags.Z = 1;
				}

				break;
			}

			if(((hs_a_value > 5) && (hs_a_value < 8)) && (ls_a_value > 5) && (h_flag == 1)&&
				(c_flag == 1))
			{
				CHIP_8->CPU_Regs_t.SingleReg.A += 0x9A;
				CHIP_8->CPU_flags_t.flags.H = 0;
				CHIP_8->CPU_flags_t.flags.C = 1;

				if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
				{
					CHIP_8->CPU_flags_t.flags.Z = 1;
				}

				break;
			}

			break;

		}

		case JR_Z_r8:
		{
			//if z are set add the immediate value to PC
			if(CHIP_8->CPU_flags_t.flags.Z == 1)
			{
				inst_len = (signed char)memory[CHIP_8->PC + 1];
			}

			break;

		}

		case ADD_HL_HL:
		{
			// Add HL to HL
			if( (((CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) +
				(CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF)) & 0x0100) == 0x0100)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.PairReg.HL += CHIP_8->CPU_Regs_t.PairReg.HL;
			CHIP_8->CPU_flags_t.flags.N = 0;

			int over_flow = CHIP_8->CPU_Regs_t.PairReg.HL +
					        CHIP_8->CPU_Regs_t.PairReg.HL;

			if(over_flow > 0xFFFF)
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
			CHIP_8->CPU_Regs_t.SingleReg.L += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.L == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if((CHIP_8->CPU_Regs_t.SingleReg.L & 0x0F) == 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_L:
		{
			// Decrement E register by 1
			CHIP_8->CPU_Regs_t.SingleReg.L -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.L == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((CHIP_8->CPU_Regs_t.SingleReg.L & 0x0F) == 0x0F)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_L_IMM:
		{
			// Load one byte to L register
			CHIP_8->CPU_Regs_t.SingleReg.L = memory[CHIP_8->PC + 1];

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
				inst_len = (signed char)memory[CHIP_8->PC + 1];
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
			CHIP_8->CPU_Regs_t.PairReg.HL += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.PairReg.HL == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if((CHIP_8->CPU_Regs_t.PairReg.HL & 0x0F) == 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;

		}

		case DEC_hl:
		{
			// Decrement HL register by 1
			CHIP_8->CPU_Regs_t.PairReg.HL -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.PairReg.HL == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((CHIP_8->CPU_Regs_t.PairReg.HL & 0x0F) == 0x0F)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_HL_d8:
		{
			// Load one byte to HL register
			CHIP_8->CPU_Regs_t.PairReg.HL  = 0x0;
			CHIP_8->CPU_Regs_t.PairReg.HL = memory[CHIP_8->PC + 1];

			inst_len = 2;

			break;

		}

		case SCF:
		{
			//Set Carry flag
			CHIP_8->CPU_flags_t.flags.N = 0;
			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 1;

			break;

		}

		case JR_C_r8:
		{
			//if C are set add the immediate value to PC
			if(CHIP_8->CPU_flags_t.flags.C == 1)
			{
				inst_len = (signed char)memory[CHIP_8->PC + 1];
			}

			break;

		}

		case ADD_HL_SP:
		{
			// Add SP to HL
			if( (((CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) +
				(CHIP_8->SP & 0x00FF)) & 0x0100) == 0x0100)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.PairReg.HL += CHIP_8->SP;
			CHIP_8->CPU_flags_t.flags.N = 0;

			int over_flow = CHIP_8->CPU_Regs_t.PairReg.HL + CHIP_8->SP;

			if(over_flow > 0xFFFF)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;
		}

		case LD_A_HL_minus:
		{
			// Put value at address HL into A. Decrement HL.
			CHIP_8->CPU_Regs_t.SingleReg.A = memory[CHIP_8->CPU_Regs_t.PairReg.HL];
			CHIP_8->CPU_Regs_t.PairReg.HL -= 1;

			break;

		}

		case DEC_SP:
		{
			// Decrement SP register by 1
			CHIP_8->SP -=1;

			break;
		}

		case INC_A:
		{
			// increment A register by 1
			CHIP_8->CPU_Regs_t.SingleReg.A += 1;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if((CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) == 0)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case DEC_A:
		{
			// Decrement A register by 1
			CHIP_8->CPU_Regs_t.SingleReg.A -= 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0x0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) == 0x0F)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			break;
		}

		case LD_A_IMM:
		{
			// Load one byte to L register
			CHIP_8->CPU_Regs_t.SingleReg.A = memory[CHIP_8->PC + 1];

			inst_len = 2;

			break;
		}

		case CCF:
		{
			//Complement carry flag
			CHIP_8->CPU_flags_t.flags.N = 0;
			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = ~(CHIP_8->CPU_flags_t.flags.C);

			break;

		}

		case LD_B_B:
		{
			//put B in B

			break;

		}

		case LD_B_C:
		{
			//put C in B
			CHIP_8->CPU_Regs_t.SingleReg.B = CHIP_8->CPU_Regs_t.SingleReg.C;

			break;

		}

		case LD_B_D:
		{
			//put D in B
			CHIP_8->CPU_Regs_t.SingleReg.B = CHIP_8->CPU_Regs_t.SingleReg.D;

			break;

		}

		case LD_B_E:
		{
			//put E in B
			CHIP_8->CPU_Regs_t.SingleReg.B = CHIP_8->CPU_Regs_t.SingleReg.E;

			break;

		}

		case LD_B_H:
		{
			//put H in B
			CHIP_8->CPU_Regs_t.SingleReg.B = CHIP_8->CPU_Regs_t.SingleReg.H;

			break;

		}

		case LD_B_L:
		{
			//put L in B
			CHIP_8->CPU_Regs_t.SingleReg.B = CHIP_8->CPU_Regs_t.SingleReg.L;

			break;

		}

		case LD_B_HL:
		{
			//put HL in B
			CHIP_8->CPU_Regs_t.SingleReg.B = CHIP_8->CPU_Regs_t.PairReg.HL;

			break;

		}

		case LD_B_A:
		{
			//put A in B
			CHIP_8->CPU_Regs_t.SingleReg.B = CHIP_8->CPU_Regs_t.SingleReg.A;

			break;

		}

		case LD_C_B:
		{
			//put B in C
			CHIP_8->CPU_Regs_t.SingleReg.C = CHIP_8->CPU_Regs_t.SingleReg.B;

			break;

		}

		case LD_C_C:
		{
			//put C in C


			break;

		}

		case LD_C_D:
		{
			//put D in B
			CHIP_8->CPU_Regs_t.SingleReg.C = CHIP_8->CPU_Regs_t.SingleReg.D;

			break;

		}

		case LD_C_E:
		{
			//put E in C
			CHIP_8->CPU_Regs_t.SingleReg.C = CHIP_8->CPU_Regs_t.SingleReg.E;

			break;

		}

		case LD_C_H:
		{
			//put H in C
			CHIP_8->CPU_Regs_t.SingleReg.C = CHIP_8->CPU_Regs_t.SingleReg.H;

			break;

		}

		case LD_C_L:
		{
			//put L in C
			CHIP_8->CPU_Regs_t.SingleReg.C = CHIP_8->CPU_Regs_t.SingleReg.L;

			break;

		}

		case LD_C_HL:
		{
			//put HL in C
			CHIP_8->CPU_Regs_t.SingleReg.C = CHIP_8->CPU_Regs_t.PairReg.HL;

			break;

		}

		case LD_C_A:
		{
			//put A in C
			CHIP_8->CPU_Regs_t.SingleReg.C = CHIP_8->CPU_Regs_t.SingleReg.A;

			break;

		}

		case LD_D_B:
		{
			//put B in D
			CHIP_8->CPU_Regs_t.SingleReg.D = CHIP_8->CPU_Regs_t.SingleReg.B;

			break;

		}

		case LD_D_C:
		{
			//put C in D
			CHIP_8->CPU_Regs_t.SingleReg.D = CHIP_8->CPU_Regs_t.SingleReg.C;

			break;

		}

		case LD_D_D:
		{
			//put D in D


			break;

		}

		case LD_D_E:
		{
			//put E in D
			CHIP_8->CPU_Regs_t.SingleReg.D = CHIP_8->CPU_Regs_t.SingleReg.E;

			break;

		}

		case LD_D_H:
		{
			//put H in D
			CHIP_8->CPU_Regs_t.SingleReg.D = CHIP_8->CPU_Regs_t.SingleReg.H;

			break;

		}

		case LD_D_L:
		{
			//put L in D
			CHIP_8->CPU_Regs_t.SingleReg.D = CHIP_8->CPU_Regs_t.SingleReg.L;

			break;

		}

		case LD_D_HL:
		{
			//put HL in D
			CHIP_8->CPU_Regs_t.SingleReg.D = CHIP_8->CPU_Regs_t.PairReg.HL;

			break;

		}

		case LD_D_A:
		{
			//put A in D
			CHIP_8->CPU_Regs_t.SingleReg.D = CHIP_8->CPU_Regs_t.SingleReg.A;

			break;

		}

		case LD_E_B:
		{
			//put B in E
			CHIP_8->CPU_Regs_t.SingleReg.E = CHIP_8->CPU_Regs_t.SingleReg.B;

			break;

		}

		case LD_E_C:
		{
			//put C in D
			CHIP_8->CPU_Regs_t.SingleReg.E = CHIP_8->CPU_Regs_t.SingleReg.C;

			break;

		}

		case LD_E_D:
		{
			//put D in E
			CHIP_8->CPU_Regs_t.SingleReg.E = CHIP_8->CPU_Regs_t.SingleReg.D;

			break;

		}

		case LD_E_E:
		{
			//put E in E


			break;

		}

		case LD_E_H:
		{
			//put H in E
			CHIP_8->CPU_Regs_t.SingleReg.E = CHIP_8->CPU_Regs_t.SingleReg.H;

			break;

		}

		case LD_E_L:
		{
			//put L in E
			CHIP_8->CPU_Regs_t.SingleReg.E = CHIP_8->CPU_Regs_t.SingleReg.L;

			break;

		}

		case LD_E_HL:
		{
			//put HL in E
			CHIP_8->CPU_Regs_t.SingleReg.E = CHIP_8->CPU_Regs_t.PairReg.HL;

			break;

		}

		case LD_E_A:
		{
			//put A in E
			CHIP_8->CPU_Regs_t.SingleReg.E = CHIP_8->CPU_Regs_t.SingleReg.A;

			break;

		}

		case LD_H_B:
		{
			//put B in H
			CHIP_8->CPU_Regs_t.SingleReg.H = CHIP_8->CPU_Regs_t.SingleReg.B;

			break;

		}

		case LD_H_C:
		{
			//put C in H
			CHIP_8->CPU_Regs_t.SingleReg.H = CHIP_8->CPU_Regs_t.SingleReg.C;

			break;

		}

		case LD_H_D:
		{
			//put D in H
			CHIP_8->CPU_Regs_t.SingleReg.H = CHIP_8->CPU_Regs_t.SingleReg.D;

			break;

		}

		case LD_H_E:
		{
			//put E in H
			CHIP_8->CPU_Regs_t.SingleReg.H = CHIP_8->CPU_Regs_t.SingleReg.E;

			break;

		}

		case LD_H_H:
		{
			//put H in H


			break;

		}

		case LD_H_L:
		{
			//put L in H
			CHIP_8->CPU_Regs_t.SingleReg.H = CHIP_8->CPU_Regs_t.SingleReg.L;

			break;

		}

		case LD_H_HL:
		{
			//put HL in H
			CHIP_8->CPU_Regs_t.SingleReg.H = CHIP_8->CPU_Regs_t.PairReg.HL;

			break;

		}

		case LD_H_A:
		{
			//put A in H
			CHIP_8->CPU_Regs_t.SingleReg.H = CHIP_8->CPU_Regs_t.SingleReg.A;

			break;

		}

		case LD_L_B:
		{
			//put B in L
			CHIP_8->CPU_Regs_t.SingleReg.L = CHIP_8->CPU_Regs_t.SingleReg.B;

			break;

		}

		case LD_L_C:
		{
			//put C in L
			CHIP_8->CPU_Regs_t.SingleReg.L = CHIP_8->CPU_Regs_t.SingleReg.C;

			break;

		}

		case LD_L_D:
		{
			//put D in L
			CHIP_8->CPU_Regs_t.SingleReg.L = CHIP_8->CPU_Regs_t.SingleReg.D;

			break;

		}

		case LD_L_E:
		{
			//put E in L
			CHIP_8->CPU_Regs_t.SingleReg.L = CHIP_8->CPU_Regs_t.SingleReg.E;

			break;

		}

		case LD_L_H:
		{
			//put H in L
			CHIP_8->CPU_Regs_t.SingleReg.L = CHIP_8->CPU_Regs_t.SingleReg.H;

			break;

		}

		case LD_L_L:
		{
			//put L in L


			break;

		}

		case LD_L_HL:
		{
			//put HL in L
			CHIP_8->CPU_Regs_t.SingleReg.L = CHIP_8->CPU_Regs_t.PairReg.HL;

			break;

		}

		case LD_L_A:
		{
			//put A in L
			CHIP_8->CPU_Regs_t.SingleReg.L = CHIP_8->CPU_Regs_t.SingleReg.A;

			break;

		}

		case LD_HL_B:
		{
			//put B in HL
			CHIP_8->CPU_Regs_t.PairReg.HL = 0x00;
			CHIP_8->CPU_Regs_t.PairReg.HL = CHIP_8->CPU_Regs_t.SingleReg.B;

			break;

		}

		case LD_HL_C:
		{
			//put C in HL
			CHIP_8->CPU_Regs_t.PairReg.HL = 0x00;
			CHIP_8->CPU_Regs_t.PairReg.HL = CHIP_8->CPU_Regs_t.SingleReg.C;

			break;

		}

		case LD_HL_D:
		{
			//put D in HL
			CHIP_8->CPU_Regs_t.PairReg.HL = 0x00;
			CHIP_8->CPU_Regs_t.PairReg.HL = CHIP_8->CPU_Regs_t.SingleReg.D;

			break;

		}

		case LD_HL_E:
		{
			//put E in HL
			CHIP_8->CPU_Regs_t.PairReg.HL = 0x00;
			CHIP_8->CPU_Regs_t.PairReg.HL = CHIP_8->CPU_Regs_t.SingleReg.E;

			break;

		}

		case LD_HL_H:
		{
			//put H in HL
			CHIP_8->CPU_Regs_t.PairReg.HL = 0x00;
			CHIP_8->CPU_Regs_t.PairReg.HL = CHIP_8->CPU_Regs_t.SingleReg.H;

			break;

		}

		case LD_HL_L:
		{
			//put L in HL
			CHIP_8->CPU_Regs_t.PairReg.HL = 0x00;
			CHIP_8->CPU_Regs_t.PairReg.HL = CHIP_8->CPU_Regs_t.SingleReg.L;

			break;

		}

		case HALT:
		{
			// Need to stop execution - change the "On" status
			CHIP_8->CPU_is_On = FALSE;

			break;
		}

		case LD_HL_A:
		{
			//put A in HL
			CHIP_8->CPU_Regs_t.PairReg.HL = 0x00;
			CHIP_8->CPU_Regs_t.PairReg.HL = CHIP_8->CPU_Regs_t.SingleReg.A;

			break;

		}

		case LD_A_B:
		{
			//put B in A
			CHIP_8->CPU_Regs_t.SingleReg.A = CHIP_8->CPU_Regs_t.SingleReg.B;

			break;

		}

		case LD_A_C:
		{
			//put C in A
			CHIP_8->CPU_Regs_t.SingleReg.A = CHIP_8->CPU_Regs_t.SingleReg.C;

			break;

		}

		case LD_A_D:
		{
			//put D in A
			CHIP_8->CPU_Regs_t.SingleReg.A = CHIP_8->CPU_Regs_t.SingleReg.D;

			break;

		}

		case LD_A_E:
		{
			//put E in A
			CHIP_8->CPU_Regs_t.SingleReg.A = CHIP_8->CPU_Regs_t.SingleReg.E;

			break;

		}

		case LD_A_H:
		{
			//put H in A
			CHIP_8->CPU_Regs_t.SingleReg.A = CHIP_8->CPU_Regs_t.SingleReg.H;

			break;

		}

		case LD_A_L:
		{
			//put L in A
			CHIP_8->CPU_Regs_t.SingleReg.A = CHIP_8->CPU_Regs_t.SingleReg.L;

			break;

		}

		case LD_A_HL:
		{
			//put HL in A
			CHIP_8->CPU_Regs_t.SingleReg.A = CHIP_8->CPU_Regs_t.PairReg.HL;

			break;

		}

		case LD_A_A:
		{
			//put A in A


			break;

		}

		case ADD_A_B:
		{
			//ADD B to A
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.B;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char b_low = CHIP_8->CPU_Regs_t.SingleReg.B & 0x0F;

			if((a_low + b_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A += CHIP_8->CPU_Regs_t.SingleReg.B;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADD_A_C:
		{
			//ADD C to A
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.C;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char c_low = CHIP_8->CPU_Regs_t.SingleReg.C & 0x0F;

			if((a_low + c_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A += CHIP_8->CPU_Regs_t.SingleReg.C;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADD_A_D:
		{
			//ADD D to A
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.D;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char d_low = CHIP_8->CPU_Regs_t.SingleReg.D & 0x0F;

			if((a_low + d_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A += CHIP_8->CPU_Regs_t.SingleReg.D;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADD_A_E:
		{
			//ADD E to A
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.E;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char e_low = CHIP_8->CPU_Regs_t.SingleReg.E & 0x0F;

			if((a_low + e_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A += CHIP_8->CPU_Regs_t.SingleReg.E;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADD_A_H:
		{
			//ADD H to A
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.H;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char h_low = CHIP_8->CPU_Regs_t.SingleReg.H & 0x0F;

			if((a_low + h_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A += CHIP_8->CPU_Regs_t.SingleReg.H;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADD_A_L:
		{
			//ADD L to A
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.L;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char l_low = CHIP_8->CPU_Regs_t.SingleReg.L & 0x0F;

			if((a_low + l_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A += CHIP_8->CPU_Regs_t.SingleReg.L;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADD_A_HL:
		{
			//ADD HL to A
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.PairReg.HL;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char hl_low = CHIP_8->CPU_Regs_t.PairReg.HL  & 0x0F;

			if((a_low + hl_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A += CHIP_8->CPU_Regs_t.PairReg.HL;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADD_A_A:
		{
			//ADD H to A
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.A;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char A_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;

			if((a_low + A_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A += CHIP_8->CPU_Regs_t.SingleReg.A;

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADC_A_B:
		{
			//Add B + Carry flag to A.
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.B + CHIP_8->CPU_flags_t.flags.C;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char b_low = (CHIP_8->CPU_Regs_t.SingleReg.B + CHIP_8->CPU_flags_t.flags.C)& 0x0F;

			if((a_low + b_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A  = (CHIP_8->CPU_Regs_t.SingleReg.A +
		    CHIP_8->CPU_Regs_t.SingleReg.B + CHIP_8->CPU_flags_t.flags.C);

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADC_A_C:
		{
			//Add C + Carry flag to A.
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.C + CHIP_8->CPU_flags_t.flags.C;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char c_low = (CHIP_8->CPU_Regs_t.SingleReg.C + CHIP_8->CPU_flags_t.flags.C)& 0x0F;

			if((a_low + c_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A  = (CHIP_8->CPU_Regs_t.SingleReg.A +
		    CHIP_8->CPU_Regs_t.SingleReg.C + CHIP_8->CPU_flags_t.flags.C);

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADC_A_D:
		{
			//Add D + Carry flag to A.
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.D + CHIP_8->CPU_flags_t.flags.C;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char d_low = (CHIP_8->CPU_Regs_t.SingleReg.D + CHIP_8->CPU_flags_t.flags.C)& 0x0F;

			if((a_low + d_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A  = (CHIP_8->CPU_Regs_t.SingleReg.A +
		    CHIP_8->CPU_Regs_t.SingleReg.D + CHIP_8->CPU_flags_t.flags.C);

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADC_A_E:
		{
			//Add E + Carry flag to A.
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.E + CHIP_8->CPU_flags_t.flags.C;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char e_low = (CHIP_8->CPU_Regs_t.SingleReg.E + CHIP_8->CPU_flags_t.flags.C)& 0x0F;

			if((a_low + e_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A  = (CHIP_8->CPU_Regs_t.SingleReg.A +
		    CHIP_8->CPU_Regs_t.SingleReg.E + CHIP_8->CPU_flags_t.flags.C);

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADC_A_H:
		{
			//Add H + Carry flag to A.
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.H + CHIP_8->CPU_flags_t.flags.C;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char h_low = (CHIP_8->CPU_Regs_t.SingleReg.H + CHIP_8->CPU_flags_t.flags.C)& 0x0F;

			if((a_low + h_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A  = (CHIP_8->CPU_Regs_t.SingleReg.A +
		    CHIP_8->CPU_Regs_t.SingleReg.H + CHIP_8->CPU_flags_t.flags.C);

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADC_A_L:
		{
			//Add L + Carry flag to A.
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.L + CHIP_8->CPU_flags_t.flags.C;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char l_low = (CHIP_8->CPU_Regs_t.SingleReg.L + CHIP_8->CPU_flags_t.flags.C)& 0x0F;

			if((a_low + l_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A  = (CHIP_8->CPU_Regs_t.SingleReg.A +
		    CHIP_8->CPU_Regs_t.SingleReg.L + CHIP_8->CPU_flags_t.flags.C);

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADC_A_HL:
		{
			//Add HL + Carry flag to A.
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.PairReg.HL + CHIP_8->CPU_flags_t.flags.C;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char hl_low = (CHIP_8->CPU_Regs_t.PairReg.HL + CHIP_8->CPU_flags_t.flags.C)& 0x0F;

			if((a_low + hl_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A  = (CHIP_8->CPU_Regs_t.SingleReg.A +
		    CHIP_8->CPU_Regs_t.PairReg.HL + CHIP_8->CPU_flags_t.flags.C);

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case ADC_A_A:
		{
			//Add A + Carry flag to A.
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         CHIP_8->CPU_Regs_t.SingleReg.A + CHIP_8->CPU_flags_t.flags.C;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char A_low = (CHIP_8->CPU_Regs_t.SingleReg.A + CHIP_8->CPU_flags_t.flags.C)& 0x0F;

			if((a_low + A_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A  = (CHIP_8->CPU_Regs_t.SingleReg.A +
		    CHIP_8->CPU_Regs_t.SingleReg.A + CHIP_8->CPU_flags_t.flags.C);

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;


		}

		case SUB_A_B:
		{
			//Subtract B from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.B & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)CHIP_8->CPU_Regs_t.SingleReg.B;

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SUB_A_C:
		{
			//Subtract C from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.C & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)CHIP_8->CPU_Regs_t.SingleReg.C;

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SUB_A_D:
		{
			//Subtract D from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.D & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)CHIP_8->CPU_Regs_t.SingleReg.D;

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SUB_A_E:
		{
			//Subtract E from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.E & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)CHIP_8->CPU_Regs_t.SingleReg.E;

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SUB_A_H:
		{
			//Subtract H from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.H & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)CHIP_8->CPU_Regs_t.SingleReg.H;

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SUB_A_L:
		{
			//Subtract L from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.L & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)CHIP_8->CPU_Regs_t.SingleReg.L;

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SUB_A_HL:
		{
			//Subtract HL from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.PairReg.HL & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)CHIP_8->CPU_Regs_t.PairReg.HL;

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SUB_A_A:
		{
			//Subtract C from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;
			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)CHIP_8->CPU_Regs_t.SingleReg.A;

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SBC_A_B:
		{
			//Subtract B + carry from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)((CHIP_8->CPU_Regs_t.SingleReg.B + old_carry ) & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)(CHIP_8->CPU_Regs_t.SingleReg.B + old_carry);

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SBC_A_C:
		{
			//Subtract C + carry from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)((CHIP_8->CPU_Regs_t.SingleReg.C + old_carry) & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)(CHIP_8->CPU_Regs_t.SingleReg.C + old_carry);

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SBC_A_D:
		{
			//Subtract D + carry from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)((CHIP_8->CPU_Regs_t.SingleReg.D + old_carry) & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)(CHIP_8->CPU_Regs_t.SingleReg.D + old_carry);

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SBC_A_E:
		{
			//Subtract E + carry from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)((CHIP_8->CPU_Regs_t.SingleReg.E + old_carry) & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)(CHIP_8->CPU_Regs_t.SingleReg.E + old_carry);

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SBC_A_H:
		{
			//Subtract H + carry from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)((CHIP_8->CPU_Regs_t.SingleReg.H + old_carry) & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)(CHIP_8->CPU_Regs_t.SingleReg.H + old_carry);

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SBC_A_L:
		{
			//Subtract L + carry from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)((CHIP_8->CPU_Regs_t.SingleReg.L + old_carry) & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)(CHIP_8->CPU_Regs_t.SingleReg.L + old_carry);

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SBC_A_HL:
		{
			//Subtract HL + carry from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)((CHIP_8->CPU_Regs_t.PairReg.HL +old_carry) & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)(CHIP_8->CPU_Regs_t.PairReg.HL + old_carry);

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case SBC_A_A:
		{
			//Subtract A + Carry from A.
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)((CHIP_8->CPU_Regs_t.SingleReg.A + old_carry) & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)(CHIP_8->CPU_Regs_t.SingleReg.A + old_carry);

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case AND_A_B:
		{
			//Logically AND B with A.
			CHIP_8->CPU_Regs_t.SingleReg.A &= CHIP_8->CPU_Regs_t.SingleReg.B;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case AND_A_C:
		{
			//Logically AND C with A.
			CHIP_8->CPU_Regs_t.SingleReg.A &= CHIP_8->CPU_Regs_t.SingleReg.C;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case AND_A_D:
		{
			//Logically AND D with A.
			CHIP_8->CPU_Regs_t.SingleReg.A &= CHIP_8->CPU_Regs_t.SingleReg.D;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case AND_A_E:
		{
			//Logically AND E with A.
			CHIP_8->CPU_Regs_t.SingleReg.A &= CHIP_8->CPU_Regs_t.SingleReg.E;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case AND_A_H:
		{
			//Logically AND H with A.
			CHIP_8->CPU_Regs_t.SingleReg.A &= CHIP_8->CPU_Regs_t.SingleReg.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case AND_A_L:
		{
			//Logically AND L with A.
			CHIP_8->CPU_Regs_t.SingleReg.A &=  CHIP_8->CPU_Regs_t.SingleReg.L;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case AND_A_HL:
		{
			//Logically AND HL with A.
			CHIP_8->CPU_Regs_t.SingleReg.A &= CHIP_8->CPU_Regs_t.PairReg.HL;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case AND_A_A:
		{
			//Logically AND A with A.
			CHIP_8->CPU_Regs_t.SingleReg.A &= CHIP_8->CPU_Regs_t.SingleReg.A;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case XOR_A_B:
		{
			//Logical exclusive OR B with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A ^= CHIP_8->CPU_Regs_t.SingleReg.B;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case XOR_A_C:
		{
			//Logical exclusive OR C with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A ^= CHIP_8->CPU_Regs_t.SingleReg.C;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case XOR_A_D:
		{
			//Logical exclusive OR D with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A ^= CHIP_8->CPU_Regs_t.SingleReg.D;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case XOR_A_E:
		{
			//Logical exclusive OR E with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A ^= CHIP_8->CPU_Regs_t.SingleReg.E;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case XOR_A_H:
		{
			//Logical exclusive OR H with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A ^= CHIP_8->CPU_Regs_t.SingleReg.H;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case XOR_A_L:
		{
			//Logical exclusive OR L with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A ^= CHIP_8->CPU_Regs_t.SingleReg.L;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case XOR_A_HL:
		{
			//Logical exclusive OR HL with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A ^= CHIP_8->CPU_Regs_t.PairReg.HL;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case XOR_A_A:
		{
			//Logical exclusive OR A with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A ^= CHIP_8->CPU_Regs_t.SingleReg.A;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case OR_A_B:
		{
			//Logical OR B with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A |= CHIP_8->CPU_Regs_t.SingleReg.B;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case OR_A_C:
		{
			//Logical OR C with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A |= CHIP_8->CPU_Regs_t.SingleReg.C;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case OR_A_D:
		{
			//Logical OR D with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A |= CHIP_8->CPU_Regs_t.SingleReg.D;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case OR_A_E:
		{
			//Logical OR E with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A |= CHIP_8->CPU_Regs_t.SingleReg.E;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case OR_A_H:
		{
			//Logical  OR H with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A |= CHIP_8->CPU_Regs_t.SingleReg.H;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case OR_A_L:
		{
			//Logical  OR L with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A |= CHIP_8->CPU_Regs_t.SingleReg.L;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case OR_A_HL:
		{
			//Logical  OR HL with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A |= CHIP_8->CPU_Regs_t.PairReg.HL;


			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case OR_A_A:
		{
			//Logical OR A with register A.
			CHIP_8->CPU_Regs_t.SingleReg.A |= CHIP_8->CPU_Regs_t.SingleReg.A;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case CP_A_B:
		{
			//Compare A with B.
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == CHIP_8->CPU_Regs_t.SingleReg.B)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.B & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			if(CHIP_8->CPU_Regs_t.SingleReg.B > CHIP_8->CPU_Regs_t.SingleReg.A)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;

		}

		case CP_A_C:
		{
			//Compare A with C.
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == CHIP_8->CPU_Regs_t.SingleReg.C)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.C & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			if(CHIP_8->CPU_Regs_t.SingleReg.C > CHIP_8->CPU_Regs_t.SingleReg.A)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;

		}

		case CP_A_D:
		{
			//Compare A with D.
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == CHIP_8->CPU_Regs_t.SingleReg.D)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.D & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			if(CHIP_8->CPU_Regs_t.SingleReg.D > CHIP_8->CPU_Regs_t.SingleReg.A)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;

		}

		case CP_A_E:
		{
			//Compare A with E.
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == CHIP_8->CPU_Regs_t.SingleReg.E)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.E & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			if(CHIP_8->CPU_Regs_t.SingleReg.E > CHIP_8->CPU_Regs_t.SingleReg.A)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;

		}

		case CP_A_H:
		{
			//Compare A with H.
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == CHIP_8->CPU_Regs_t.SingleReg.H)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.H & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			if(CHIP_8->CPU_Regs_t.SingleReg.H > CHIP_8->CPU_Regs_t.SingleReg.A)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;

		}

		case CP_A_L:
		{
			//Compare A with L.
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == CHIP_8->CPU_Regs_t.SingleReg.L)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.L & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			if(CHIP_8->CPU_Regs_t.SingleReg.L > CHIP_8->CPU_Regs_t.SingleReg.A)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;

		}

		case CP_A_HL:
		{
			//Compare A with HL.
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == CHIP_8->CPU_Regs_t.PairReg.HL)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.PairReg.HL  & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			if(CHIP_8->CPU_Regs_t.PairReg.HL > CHIP_8->CPU_Regs_t.SingleReg.A)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			break;

		}

		case CP_A_A:
		{
			//Compare A with A.
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			break;

		}

		case RET_NZ:
		{
			//Return if following condition is true if Z flag is reset
			if (CHIP_8->CPU_flags_t.flags.Z == 0)
			{
				unsigned short immediate_value = memory[CHIP_8->SP + 1];
				immediate_value <<= 8;
				immediate_value |= memory[CHIP_8->SP];

				CHIP_8->PC = immediate_value;

				CHIP_8->SP += 2;

			}

			inst_len = 0;

			break;

		}

		case POP_BC:
		{
			//Pop two bytes off stack into register pair BC, Increment Stack Pointer twice.
			unsigned short immediate_value = memory[CHIP_8->SP + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->SP];

			CHIP_8->CPU_Regs_t.PairReg.BC = immediate_value;

			CHIP_8->SP += 2;

			break;

		}

		case JP_NZ_D16:
		{
			////Jump if Z flag is reset to address two byte immediate value.
			if(CHIP_8->CPU_flags_t.flags.Z == 0)
			{
				unsigned short immediate_value = memory[CHIP_8->PC + 1];
				immediate_value = immediate_value << 8 ;
				immediate_value |= memory[CHIP_8->PC + 2];

				CHIP_8->PC = immediate_value;

				inst_len = 0;
			}

			break;

		}

		case JP_D16:
		{
			//Jump to address two byte immediate value.
			unsigned short immediate_value = memory[CHIP_8->PC + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->PC + 2];

			immediate_value = immediate_value - CHIP_8->PC;

			inst_len = immediate_value;

			break;

		}

		case CALL_NZ:
		{
			//Call address immediate value if the Z flag is reset.
			if(CHIP_8->CPU_flags_t.flags.Z == 0)
			{
				memory[CHIP_8->SP - 2] = (CHIP_8->PC + 1) & 0xFF;
				memory[CHIP_8->SP - 1] = (CHIP_8->PC + 1) >> 8;
				CHIP_8->SP -= 2;

				unsigned short immediate_value = memory[CHIP_8->PC + 2];
				immediate_value = immediate_value << 8 ;
				immediate_value |= memory[CHIP_8->PC + 3];

				CHIP_8->PC = immediate_value;
			}

			inst_len = 0;

			break;

		}

		case PUSH_BC:
		{
			//Push register pair BC onto stack , Decrement Stack Pointer (SP) twice.
			memory[CHIP_8->SP - 2] = CHIP_8->CPU_Regs_t.PairReg.BC & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->CPU_Regs_t.PairReg.BC >> 8;
			CHIP_8->SP -= 2;

			break;

		}

		case ADD_A_D8:
		{
			//Add immediate value to A.
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
			short test = CHIP_8->CPU_Regs_t.SingleReg.A + immediate_value;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char immediate_low = immediate_value & 0x0F;

			if((a_low + immediate_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A  += immediate_value;

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			inst_len = 2;

			break;

		}

		case RST_00H:
		{
			//Push present address onto stack ,Jump to address $0000 + 00.
			memory[CHIP_8->SP - 2] = CHIP_8->PC & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->PC >> 8;
			CHIP_8->SP -= 2;

			CHIP_8->PC = 0x0000 + 0x00;

			inst_len = 0;

			break;

		}

		case RET_Z:
		{
			//Return if following condition is true if Z flag set
			if (CHIP_8->CPU_flags_t.flags.Z == 1)
			{
				unsigned short immediate_value = memory[CHIP_8->SP + 1];
				immediate_value <<= 8;
				immediate_value |= memory[CHIP_8->SP];

				CHIP_8->PC = immediate_value;

				CHIP_8->SP += 2;

				inst_len = 0;
			}

			break;

		}

		case RET:
		{
			//Pop two bytes from stack & jump to that address.
			unsigned short immediate_value = memory[CHIP_8->SP + 1];
			immediate_value <<= 8;
			immediate_value |= memory[CHIP_8->SP];

			CHIP_8->PC = immediate_value;

			CHIP_8->SP += 2;

			inst_len = 0;

			break;

		}

		case JP_Z_D16:
		{
			////Jump if Z flag is set to address two byte immediate value.
			if(CHIP_8->CPU_flags_t.flags.Z == 1)
			{
				unsigned short immediate_value = memory[CHIP_8->PC + 1];
				immediate_value = immediate_value << 8 ;
				immediate_value |= memory[CHIP_8->PC + 2];

				CHIP_8->PC = immediate_value;

				inst_len = 0;
			}

			break;

		}

		case PREF_CB:
		{
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
			switch(immediate_value)
			{
				case RLC_B:
				{
					//Rotate B left.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.B >> 7;
					CHIP_8->CPU_Regs_t.SingleReg.B <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.B |= old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;
					CHIP_8->CPU_flags_t.flags.C = old_bit_7;

					if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RLC_C:
				{
					//Rotate C left.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.C >> 7;
					CHIP_8->CPU_Regs_t.SingleReg.C <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.C |= old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;
					CHIP_8->CPU_flags_t.flags.C = old_bit_7;

					if (CHIP_8->CPU_Regs_t.SingleReg.C == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RLC_D:
				{
					//Rotate D left.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.D >> 7;
					CHIP_8->CPU_Regs_t.SingleReg.D <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.D |= old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;
					CHIP_8->CPU_flags_t.flags.C = old_bit_7;

					if (CHIP_8->CPU_Regs_t.SingleReg.D == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RLC_E:
				{
					//Rotate E left.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.E >> 7;
					CHIP_8->CPU_Regs_t.SingleReg.E <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.E |= old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;
					CHIP_8->CPU_flags_t.flags.C = old_bit_7;

					if (CHIP_8->CPU_Regs_t.SingleReg.E == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RLC_H:
				{
					//Rotate H left.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.H >> 7;
					CHIP_8->CPU_Regs_t.SingleReg.H <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.H |= old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;
					CHIP_8->CPU_flags_t.flags.C = old_bit_7;

					if (CHIP_8->CPU_Regs_t.SingleReg.H == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RLC_L:
				{
					//Rotate L left.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.L >> 7;
					CHIP_8->CPU_Regs_t.SingleReg.L <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.L |= old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;
					CHIP_8->CPU_flags_t.flags.C = old_bit_7;

					if (CHIP_8->CPU_Regs_t.SingleReg.L == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RLC_HL:
				{
					//Rotate HL left.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.PairReg.HL >> 7;
					old_bit_7 &= 1;
					CHIP_8->CPU_Regs_t.PairReg.HL <<= 1;
					CHIP_8->CPU_Regs_t.PairReg.HL |= old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;
					CHIP_8->CPU_flags_t.flags.C = old_bit_7;

					if (CHIP_8->CPU_Regs_t.PairReg.HL == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RLC_A:
				{
					//Rotate A left.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.A >> 7;
					CHIP_8->CPU_Regs_t.SingleReg.A <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.A |= old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;
					CHIP_8->CPU_flags_t.flags.C = old_bit_7;

					if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RRC_B:
				{
					//Rotate B right.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.B & 1;
					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					CHIP_8->CPU_Regs_t.SingleReg.B >>= 1;
					old_bit_0 <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.B |= old_bit_0;

					if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}
				case RRC_C:
				{
					//Rotate C right.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.C & 1;
					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					CHIP_8->CPU_Regs_t.SingleReg.C >>= 1;
					old_bit_0 <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.C |= old_bit_0;

					if (CHIP_8->CPU_Regs_t.SingleReg.C == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RRC_D:
				{
					//Rotate D right.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.D & 1;
					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					CHIP_8->CPU_Regs_t.SingleReg.D >>= 1;
					old_bit_0 <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.D |= old_bit_0;

					if (CHIP_8->CPU_Regs_t.SingleReg.D == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RRC_E:
				{
					//Rotate E right.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.E & 1;
					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					CHIP_8->CPU_Regs_t.SingleReg.E >>= 1;
					old_bit_0 <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.E |= old_bit_0;

					if (CHIP_8->CPU_Regs_t.SingleReg.E == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RRC_H:
				{
					//Rotate H right.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.H & 1;
					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					CHIP_8->CPU_Regs_t.SingleReg.H >>= 1;
					old_bit_0 <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.H |= old_bit_0;

					if (CHIP_8->CPU_Regs_t.SingleReg.H == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RRC_L:
				{
					//Rotate L right.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.L & 1;
					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					CHIP_8->CPU_Regs_t.SingleReg.L >>= 1;
					old_bit_0 <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.L |= old_bit_0;

					if (CHIP_8->CPU_Regs_t.SingleReg.L == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RRC_HL:
				{
					//Rotate HL right.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.PairReg.HL & 1;
					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					CHIP_8->CPU_Regs_t.PairReg.HL >>= 1;
					old_bit_0 <<= 7;
					CHIP_8->CPU_Regs_t.PairReg.HL |= old_bit_0;

					if (CHIP_8->CPU_Regs_t.PairReg.HL == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RRC_A:
				{
					//Rotate A right.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.A & 1;
					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					CHIP_8->CPU_Regs_t.SingleReg.A >>= 1;
					old_bit_0 <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.A |= old_bit_0;

					if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RL_B:
				{
					//Rotate B left through Carry flag.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.B >> 7;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					CHIP_8->CPU_Regs_t.SingleReg.B <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.B |= carry;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RL_C:
				{
					//Rotate C left through Carry flag.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.C >> 7;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					CHIP_8->CPU_Regs_t.SingleReg.C <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.C |= carry;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.C == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RL_D:
				{
					//Rotate D left through Carry flag.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.D >> 7;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					CHIP_8->CPU_Regs_t.SingleReg.D <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.D |= carry;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.D == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RL_E:
				{
					//Rotate E left through Carry flag.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.E >> 7;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					CHIP_8->CPU_Regs_t.SingleReg.E <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.E |= carry;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.E == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RL_H:
				{
					//Rotate H left through Carry flag.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.H >> 7;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					CHIP_8->CPU_Regs_t.SingleReg.H <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.H |= carry;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.H == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RL_L:
				{
					//Rotate L left through Carry flag.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.L >> 7;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					CHIP_8->CPU_Regs_t.SingleReg.L <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.L |= carry;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.L == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RL_HL:
				{
					//Rotate HL left through Carry flag.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.PairReg.HL >> 7;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					CHIP_8->CPU_Regs_t.PairReg.HL <<= 1;
					CHIP_8->CPU_Regs_t.PairReg.HL|= carry;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.PairReg.HL == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RL_A:
				{
					//Rotate A left through Carry flag.
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.A >> 7;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					CHIP_8->CPU_Regs_t.SingleReg.A <<= 1;
					CHIP_8->CPU_Regs_t.SingleReg.A |= carry;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RR_B:
				{
					//Rotate B right through Carry flag.
					unsigned char old__bit_0 = CHIP_8->CPU_Regs_t.SingleReg.B & 1;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					carry <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.B >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.B |= carry;

					CHIP_8->CPU_flags_t.flags.C = old__bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RR_C:
				{
					//Rotate C right through Carry flag.
					unsigned char old__bit_0 = CHIP_8->CPU_Regs_t.SingleReg.C & 1;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					carry <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.C >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.C |= carry;

					CHIP_8->CPU_flags_t.flags.C = old__bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.C == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RR_D:
				{
					//Rotate D right through Carry flag.
					unsigned char old__bit_0 = CHIP_8->CPU_Regs_t.SingleReg.D & 1;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					carry <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.D >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.D |= carry;

					CHIP_8->CPU_flags_t.flags.C = old__bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.D == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RR_E:
				{
					//Rotate E right through Carry flag.
					unsigned char old__bit_0 = CHIP_8->CPU_Regs_t.SingleReg.E & 1;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					carry <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.E >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.E |= carry;

					CHIP_8->CPU_flags_t.flags.C = old__bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.E == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RR_H:
				{
					//Rotate C right through Carry flag.
					unsigned char old__bit_0 = CHIP_8->CPU_Regs_t.SingleReg.H & 1;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					carry <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.H >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.H |= carry;

					CHIP_8->CPU_flags_t.flags.C = old__bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.H == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RR_L:
				{
					//Rotate L right through Carry flag.
					unsigned char old__bit_0 = CHIP_8->CPU_Regs_t.SingleReg.L & 1;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					carry <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.L >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.L |= carry;

					CHIP_8->CPU_flags_t.flags.C = old__bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.L == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RR_HL:
				{
					//Rotate HL right through Carry flag.
					unsigned char old__bit_0 = CHIP_8->CPU_Regs_t.PairReg.HL & 1;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					carry <<= 7;
					CHIP_8->CPU_Regs_t.PairReg.HL >>= 1;
					CHIP_8->CPU_Regs_t.PairReg.HL |= carry;

					CHIP_8->CPU_flags_t.flags.C = old__bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.PairReg.HL == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case RR_A:
				{
					//Rotate A right through Carry flag.
					unsigned char old__bit_0 = CHIP_8->CPU_Regs_t.SingleReg.A & 1;
					unsigned char carry = CHIP_8->CPU_flags_t.flags.C;
					carry <<= 7;
					CHIP_8->CPU_Regs_t.SingleReg.A >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.A |= carry;

					CHIP_8->CPU_flags_t.flags.C = old__bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SLA_B:
				{
					//Shift B left into Carry
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.B >> 7;

					CHIP_8->CPU_Regs_t.SingleReg.B <<= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SLA_C:
				{
					//Shift C left into Carry
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.C >> 7;

					CHIP_8->CPU_Regs_t.SingleReg.C <<= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.C == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SLA_D:
				{
					//Shift D left into Carry
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.D >> 7;

					CHIP_8->CPU_Regs_t.SingleReg.D <<= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.D == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SLA_E:
				{
					//Shift E left into Carry
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.E >> 7;

					CHIP_8->CPU_Regs_t.SingleReg.E <<= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.E == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SLA_H:
				{
					//Shift H left into Carry
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.H >> 7;

					CHIP_8->CPU_Regs_t.SingleReg.H <<= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.H == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SLA_L:
				{
					//Shift L left into Carry
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.L >> 7;

					CHIP_8->CPU_Regs_t.SingleReg.L <<= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.L == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SLA_HL:
				{
					//Shift HL left into Carry
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.PairReg.HL >> 7;

					CHIP_8->CPU_Regs_t.PairReg.HL <<= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.PairReg.HL == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SLA_A:
				{
					//Shift A left into Carry
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.A >> 7;

					CHIP_8->CPU_Regs_t.SingleReg.A <<= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_7;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRA_B:
				{
					//Shift B right into Carry
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.B & 1;
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.B & 0x80;

					CHIP_8->CPU_Regs_t.SingleReg.B >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.B |= old_bit_7;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRA_C:
				{
					//Shift C right into Carry
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.C & 1;
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.C & 0x80;

					CHIP_8->CPU_Regs_t.SingleReg.C >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.C |= old_bit_7;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.C == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRA_D:
				{
					//Shift D right into Carry
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.D & 1;
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.D & 0x80;

					CHIP_8->CPU_Regs_t.SingleReg.D >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.D |= old_bit_7;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.D == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRA_E:
				{
					//Shift E right into Carry
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.E & 1;
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.E & 0x80;

					CHIP_8->CPU_Regs_t.SingleReg.E >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.E |= old_bit_7;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.D == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRA_H:
				{
					//Shift H right into Carry
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.H & 1;
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.H & 0x80;

					CHIP_8->CPU_Regs_t.SingleReg.H >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.H |= old_bit_7;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.H == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRA_L:
				{
					//Shift L right into Carry
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.L & 1;
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.L & 0x80;

					CHIP_8->CPU_Regs_t.SingleReg.L >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.L |= old_bit_7;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.L == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRA_HL:
				{
					//Shift B right into Carry
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.PairReg.HL & 1;
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.PairReg.HL & 0x80;

					CHIP_8->CPU_Regs_t.PairReg.HL >>= 1;
					CHIP_8->CPU_Regs_t.PairReg.HL |= old_bit_7;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.PairReg.HL == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRA_A:
				{
					//Shift A right into Carry
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.A & 1;
					unsigned char old_bit_7 = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

					CHIP_8->CPU_Regs_t.SingleReg.A >>= 1;
					CHIP_8->CPU_Regs_t.SingleReg.A |= old_bit_7;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SWAP_B:
				{
					//Swap upper & lower nibles of B.
					unsigned char lower = CHIP_8->CPU_Regs_t.SingleReg.B & 0x0F;
					lower <<= 4;
					CHIP_8->CPU_Regs_t.SingleReg.B >>= 4;
					CHIP_8->CPU_Regs_t.SingleReg.B |= lower;

					CHIP_8->CPU_flags_t.flags.C = 0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SWAP_C:
				{
					//Swap upper & lower nibles of C.
					unsigned char lower = CHIP_8->CPU_Regs_t.SingleReg.C & 0x0F;
					lower <<= 4;
					CHIP_8->CPU_Regs_t.SingleReg.C >>= 4;
					CHIP_8->CPU_Regs_t.SingleReg.C |= lower;

					CHIP_8->CPU_flags_t.flags.C = 0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.C == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SWAP_D:
				{
					//Swap upper & lower nibles of D.
					unsigned char lower = CHIP_8->CPU_Regs_t.SingleReg.D & 0x0F;
					lower <<= 4;
					CHIP_8->CPU_Regs_t.SingleReg.D >>= 4;
					CHIP_8->CPU_Regs_t.SingleReg.D |= lower;

					CHIP_8->CPU_flags_t.flags.C = 0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.D == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SWAP_E:
				{
					//Swap upper & lower nibles of E.
					unsigned char lower = CHIP_8->CPU_Regs_t.SingleReg.E & 0x0F;
					lower <<= 4;
					CHIP_8->CPU_Regs_t.SingleReg.E >>= 4;
					CHIP_8->CPU_Regs_t.SingleReg.E |= lower;

					CHIP_8->CPU_flags_t.flags.C = 0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.E == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SWAP_H:
				{
					//Swap upper & lower nibles of H.
					unsigned char lower = CHIP_8->CPU_Regs_t.SingleReg.H & 0x0F;
					lower <<= 4;
					CHIP_8->CPU_Regs_t.SingleReg.H >>= 4;
					CHIP_8->CPU_Regs_t.SingleReg.H |= lower;

					CHIP_8->CPU_flags_t.flags.C = 0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.H == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SWAP_L:
				{
					//Swap upper & lower nibles of L.
					unsigned char lower = CHIP_8->CPU_Regs_t.SingleReg.L & 0x0F;
					lower <<= 4;
					CHIP_8->CPU_Regs_t.SingleReg.L >>= 4;
					CHIP_8->CPU_Regs_t.SingleReg.B |= lower;

					CHIP_8->CPU_flags_t.flags.C = 0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.L == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SWAP_HL:
				{
					//Swap upper & lower nibles of B.
					unsigned char lower = CHIP_8->CPU_Regs_t.PairReg.HL & 0x0F;
					lower <<= 4;
					CHIP_8->CPU_Regs_t.PairReg.HL >>= 4;
					CHIP_8->CPU_Regs_t.PairReg.HL |= lower;

					CHIP_8->CPU_flags_t.flags.C = 0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.PairReg.HL == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SWAP_A:
				{
					//Swap upper & lower nibles of A.
					unsigned char lower = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
					lower <<= 4;
					CHIP_8->CPU_Regs_t.SingleReg.A >>= 4;
					CHIP_8->CPU_Regs_t.SingleReg.A |= lower;

					CHIP_8->CPU_flags_t.flags.C = 0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRL_B:
				{
					//Shift B right into Carry.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.B & 1;
					CHIP_8->CPU_Regs_t.SingleReg.B >>= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.B == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRL_C:
				{
					//Shift C right into Carry.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.C & 1;
					CHIP_8->CPU_Regs_t.SingleReg.C >>= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.C == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRL_D:
				{
					//Shift D right into Carry.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.D & 1;
					CHIP_8->CPU_Regs_t.SingleReg.D >>= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.D == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRL_E:
				{
					//Shift E right into Carry.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.E & 1;
					CHIP_8->CPU_Regs_t.SingleReg.E >>= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.E == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRL_H:
				{
					//Shift H right into Carry.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.H & 1;
					CHIP_8->CPU_Regs_t.SingleReg.H >>= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.H == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRL_HL:
				{
					//Shift HL right into Carry.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.PairReg.HL & 1;
					CHIP_8->CPU_Regs_t.PairReg.HL >>= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.PairReg.HL == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case SRL_A:
				{
					//Shift A right into Carry.
					unsigned char old_bit_0 = CHIP_8->CPU_Regs_t.SingleReg.A & 1;
					CHIP_8->CPU_Regs_t.SingleReg.A >>= 1;

					CHIP_8->CPU_flags_t.flags.C = old_bit_0;
					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 0;

					if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					break;

				}

				case BIT_0_B:
				{
					//Test bit 0 in register B

					if ((CHIP_8->CPU_Regs_t.SingleReg.B & 1) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_0_C:
				{
					//Test bit 0 in register C

					if ((CHIP_8->CPU_Regs_t.SingleReg.C & 1) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_0_D:
				{
					//Test bit 0 in register D

					if ((CHIP_8->CPU_Regs_t.SingleReg.D & 1) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_0_E:
				{
					//Test bit 0 in register E

					if ((CHIP_8->CPU_Regs_t.SingleReg.E & 1) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_0_H:
				{
					//Test bit 0 in register H

					if ((CHIP_8->CPU_Regs_t.SingleReg.H & 1) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_0_L:
				{
					//Test bit 0 in register L

					if ((CHIP_8->CPU_Regs_t.SingleReg.L & 1) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_0_HL:
				{
					//Test bit 0 in register HL

					if ((CHIP_8->CPU_Regs_t.PairReg.HL & 1) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_0_A:
				{
					//Test bit 0 in register A

					if ((CHIP_8->CPU_Regs_t.SingleReg.A & 1) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_1_B:
				{
					//Test bit 1 in register B
					if ((CHIP_8->CPU_Regs_t.SingleReg.B & 0x02) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_1_C:
				{
					//Test bit 1 in register C

					if ((CHIP_8->CPU_Regs_t.SingleReg.C & 0x02) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_1_D:
				{
					//Test bit 1 in register D

					if ((CHIP_8->CPU_Regs_t.SingleReg.D & 0x02) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_1_E:
				{
					//Test bit 1 in register E

					if ((CHIP_8->CPU_Regs_t.SingleReg.E & 0x02) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_1_H:
				{
					//Test bit 1 in register H

					if ((CHIP_8->CPU_Regs_t.SingleReg.H & 0x02) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_1_L:
				{
					//Test bit 1 in register L

					if ((CHIP_8->CPU_Regs_t.SingleReg.L & 0x02) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_1_HL:
				{
					//Test bit 1 in register HL

					if ((CHIP_8->CPU_Regs_t.PairReg.HL & 0x02) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_1_A:
				{
					//Test bit 1 in register A

					if ((CHIP_8->CPU_Regs_t.SingleReg.A & 0x02) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_2_B:
				{
					//Test bit 2 in register B
					if ((CHIP_8->CPU_Regs_t.SingleReg.B & 0x04) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_2_C:
				{
					//Test bit 2 in register C

					if ((CHIP_8->CPU_Regs_t.SingleReg.C & 0x04) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_2_D:
				{
					//Test bit 2 in register D

					if ((CHIP_8->CPU_Regs_t.SingleReg.D & 0x04) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_2_E:
				{
					//Test bit 2 in register E

					if ((CHIP_8->CPU_Regs_t.SingleReg.E & 0x04) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_2_H:
				{
					//Test bit 2 in register H

					if ((CHIP_8->CPU_Regs_t.SingleReg.H & 0x04) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_2_L:
				{
					//Test bit 2 in register L

					if ((CHIP_8->CPU_Regs_t.SingleReg.L & 0x04) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_2_HL:
				{
					//Test bit 2 in register HL

					if ((CHIP_8->CPU_Regs_t.PairReg.HL & 0x04) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_2_A:
				{
					//Test bit 2 in register A

					if ((CHIP_8->CPU_Regs_t.SingleReg.A & 0x04) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_3_B:
				{
					//Test bit 3 in register B
					if ((CHIP_8->CPU_Regs_t.SingleReg.B & 0x08) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_3_C:
				{
					//Test bit 3 in register C

					if ((CHIP_8->CPU_Regs_t.SingleReg.C & 0x08) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_3_D:
				{
					//Test bit 3 in register D

					if ((CHIP_8->CPU_Regs_t.SingleReg.D & 0x08) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_3_E:
				{
					//Test bit 3 in register E

					if ((CHIP_8->CPU_Regs_t.SingleReg.E & 0x08) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_3_H:
				{
					//Test bit 3 in register H

					if ((CHIP_8->CPU_Regs_t.SingleReg.H & 0x08) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_3_L:
				{
					//Test bit 3 in register L

					if ((CHIP_8->CPU_Regs_t.SingleReg.L & 0x08) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_3_HL:
				{
					//Test bit 3 in register HL

					if ((CHIP_8->CPU_Regs_t.PairReg.HL & 0x08) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_3_A:
				{
					//Test bit 3 in register A

					if ((CHIP_8->CPU_Regs_t.SingleReg.A & 0x08) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_4_B:
				{
					//Test bit 4 in register B
					if ((CHIP_8->CPU_Regs_t.SingleReg.B & 0x10) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_4_C:
				{
					//Test bit 4 in register C

					if ((CHIP_8->CPU_Regs_t.SingleReg.C & 0x10) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_4_D:
				{
					//Test bit 4 in register D

					if ((CHIP_8->CPU_Regs_t.SingleReg.D & 0x10) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_4_E:
				{
					//Test bit 4 in register E

					if ((CHIP_8->CPU_Regs_t.SingleReg.E & 0x10) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_4_H:
				{
					//Test bit 4 in register H

					if ((CHIP_8->CPU_Regs_t.SingleReg.H & 0x10) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_4_L:
				{
					//Test bit 4 in register L

					if ((CHIP_8->CPU_Regs_t.SingleReg.L & 0x10) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_4_HL:
				{
					//Test bit 4 in register HL

					if ((CHIP_8->CPU_Regs_t.PairReg.HL & 0x10) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_4_A:
				{
					//Test bit 4 in register A

					if ((CHIP_8->CPU_Regs_t.SingleReg.A & 0x10) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_5_B:
				{
					//Test bit 5 in register B
					if ((CHIP_8->CPU_Regs_t.SingleReg.B & 0x20) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_5_C:
				{
					//Test bit 5 in register C

					if ((CHIP_8->CPU_Regs_t.SingleReg.C & 0x20) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_5_D:
				{
					//Test bit 5 in register D

					if ((CHIP_8->CPU_Regs_t.SingleReg.D & 0x20) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_5_E:
				{
					//Test bit 5 in register E

					if ((CHIP_8->CPU_Regs_t.SingleReg.E & 0x20) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_5_H:
				{
					//Test bit 5 in register H

					if ((CHIP_8->CPU_Regs_t.SingleReg.H & 0x20) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_5_L:
				{
					//Test bit 5 in register L

					if ((CHIP_8->CPU_Regs_t.SingleReg.L & 0x20) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_5_HL:
				{
					//Test bit 5 in register HL

					if ((CHIP_8->CPU_Regs_t.PairReg.HL & 0x20) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_5_A:
				{
					//Test bit 5 in register A

					if ((CHIP_8->CPU_Regs_t.SingleReg.A & 0x20) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_6_B:
				{
					//Test bit 6 in register B
					if ((CHIP_8->CPU_Regs_t.SingleReg.B & 0x40) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_6_C:
				{
					//Test bit 6 in register C

					if ((CHIP_8->CPU_Regs_t.SingleReg.C & 0x40) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_6_D:
				{
					//Test bit 6 in register D

					if ((CHIP_8->CPU_Regs_t.SingleReg.D & 0x40) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_6_E:
				{
					//Test bit 6 in register E

					if ((CHIP_8->CPU_Regs_t.SingleReg.E & 0x40) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_6_H:
				{
					//Test bit 6 in register H

					if ((CHIP_8->CPU_Regs_t.SingleReg.H & 0x40) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_6_L:
				{
					//Test bit 6 in register L

					if ((CHIP_8->CPU_Regs_t.SingleReg.L & 0x40) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_6_HL:
				{
					//Test bit 6 in register HL

					if ((CHIP_8->CPU_Regs_t.PairReg.HL & 0x40) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_6_A:
				{
					//Test bit 6 in register A

					if ((CHIP_8->CPU_Regs_t.SingleReg.A & 0x40) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_7_B:
				{
					//Test bit 7 in register B
					if ((CHIP_8->CPU_Regs_t.SingleReg.B & 0x80) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_7_C:
				{
					//Test bit 7 in register C

					if ((CHIP_8->CPU_Regs_t.SingleReg.C & 0x80) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_7_D:
				{
					//Test bit 7 in register D

					if ((CHIP_8->CPU_Regs_t.SingleReg.D & 0x80) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_7_E:
				{
					//Test bit 7 in register E

					if ((CHIP_8->CPU_Regs_t.SingleReg.E & 0x80) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_7_H:
				{
					//Test bit 7 in register H

					if ((CHIP_8->CPU_Regs_t.SingleReg.H & 0x80) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_7_L:
				{
					//Test bit 7 in register L

					if ((CHIP_8->CPU_Regs_t.SingleReg.L & 0x80) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_7_HL:
				{
					//Test bit 7 in register HL

					if ((CHIP_8->CPU_Regs_t.PairReg.HL & 0x80) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case BIT_7_A:
				{
					//Test bit 7 in register A

					if ((CHIP_8->CPU_Regs_t.SingleReg.A & 0x80) == 0)
					{
						CHIP_8->CPU_flags_t.flags.Z = 1;
					}

					CHIP_8->CPU_flags_t.flags.N = 0;
					CHIP_8->CPU_flags_t.flags.H = 1;

					break;

				}

				case RES_0_B:
				{
					//Reset bit 0 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B &= 0x7E;

					break;

				}

				case RES_0_C:
				{
					//Reset bit 0 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C &= 0x7E;

					break;

				}

				case RES_0_D:
				{
					//Reset bit 0 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D &= 0x7E;

					break;

				}

				case RES_0_E:
				{
					//Reset bit 0 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E &= 0x7E;

					break;

				}

				case RES_0_H:
				{
					//Reset bit 0 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H &= 0x7E;

					break;

				}

				case RES_0_L:
				{
					//Reset bit 0 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L &= 0x7E;

					break;

				}

				case RES_0_HL:
				{
					//Reset bit 0 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL &= 0xFF7E;

					break;

				}

				case RES_0_A:
				{
					//Reset bit 0 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A &= 0x7E;

					break;

				}

				case RES_1_B:
				{
					//Reset bit 1 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B &= 0xFD;

					break;

				}

				case RES_1_C:
				{
					//Reset bit 1 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C &= 0xFD;

					break;

				}

				case RES_1_D:
				{
					//Reset bit 1 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D &= 0xFD;

					break;

				}

				case RES_1_E:
				{
					//Reset bit 1 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E &= 0xFD;

					break;

				}

				case RES_1_H:
				{
					//Reset bit 1 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H &= 0xFD;

					break;

				}

				case RES_1_L:
				{
					//Reset bit 1 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L &= 0xFD;

					break;

				}

				case RES_1_HL:
				{
					//Reset bit 1 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL &= 0xFFFD;

					break;

				}

				case RES_1_A:
				{
					//Reset bit 1 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A &= 0xFD;

					break;

				}

				case RES_2_B:
				{
					//Reset bit 2 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B &= 0xFB;

					break;

				}

				case RES_2_C:
				{
					//Reset bit 2 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C &= 0xFB;

					break;

				}

				case RES_2_D:
				{
					//Reset bit 2 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D &= 0xFB;

					break;

				}

				case RES_2_E:
				{
					//Reset bit 2 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E &= 0xFB;

					break;

				}

				case RES_2_H:
				{
					//Reset bit 2 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H &= 0xFB;

					break;

				}

				case RES_2_L:
				{
					//Reset bit 2 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L &= 0xFB;

					break;

				}

				case RES_2_HL:
				{
					//Reset bit 2 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL &= 0xFFFB;

					break;

				}

				case RES_2_A:
				{
					//Reset bit 2 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A &= 0xFB;

					break;

				}

				case RES_3_B:
				{
					//Reset bit 3 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B &= 0xF7;

					break;

				}

				case RES_3_C:
				{
					//Reset bit 3 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C &= 0xF7;

					break;

				}

				case RES_3_D:
				{
					//Reset bit 3 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D &= 0xF7;

					break;

				}

				case RES_3_E:
				{
					//Reset bit 3 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E &= 0xF7;

					break;

				}

				case RES_3_H:
				{
					//Reset bit 3 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H &= 0xF7;

					break;

				}

				case RES_3_L:
				{
					//Reset bit 3 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L &= 0xF7;

					break;

				}

				case RES_3_HL:
				{
					//Reset bit 3 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL &= 0xFFF7;

					break;

				}

				case RES_3_A:
				{
					//Reset bit 3 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A &= 0xF7;

					break;

				}

				case RES_4_B:
				{
					//Reset bit 4 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B &= 0xEF;

					break;

				}

				case RES_4_C:
				{
					//Reset bit 4 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C &= 0xEF;

					break;

				}

				case RES_4_D:
				{
					//Reset bit 4 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D &= 0xEF;

					break;

				}

				case RES_4_E:
				{
					//Reset bit 4 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E &= 0xEF;

					break;

				}

				case RES_4_H:
				{
					//Reset bit 4 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H &= 0xEF;

					break;

				}

				case RES_4_L:
				{
					//Reset bit 4 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L &= 0xEF;

					break;

				}

				case RES_4_HL:
				{
					//Reset bit 4 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL &= 0xFFEF;

					break;

				}

				case RES_4_A:
				{
					//Reset bit 4 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A &= 0xEF;

					break;

				}

				case RES_5_B:
				{
					//Reset bit 5 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B &= 0xDF;

					break;

				}

				case RES_5_C:
				{
					//Reset bit 5 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C &= 0xDF;

					break;

				}

				case RES_5_D:
				{
					//Reset bit 5 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D &= 0xDF;

					break;

				}

				case RES_5_E:
				{
					//Reset bit 5 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E &= 0xDF;

					break;

				}

				case RES_5_H:
				{
					//Reset bit 5 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H &= 0xDF;

					break;

				}

				case RES_5_L:
				{
					//Reset bit 5 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L &= 0xDF;

					break;

				}

				case RES_5_HL:
				{
					//Reset bit 5 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL &= 0xFFDF;

					break;

				}

				case RES_5_A:
				{
					//Reset bit 5 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A &= 0xDF;

					break;

				}

				case RES_6_B:
				{
					//Reset bit 6 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B &= 0xBF;

					break;

				}

				case RES_6_C:
				{
					//Reset bit 6 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C &= 0xBF;

					break;

				}

				case RES_6_D:
				{
					//Reset bit 6 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D &= 0xBF;

					break;

				}

				case RES_6_E:
				{
					//Reset bit 6 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E &= 0xBF;

					break;

				}

				case RES_6_H:
				{
					//Reset bit 6 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H &= 0xBF;

					break;

				}

				case RES_6_L:
				{
					//Reset bit 6 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L &= 0xBF;

					break;

				}

				case RES_6_HL:
				{
					//Reset bit 6 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL &= 0xFFBF;

					break;

				}

				case RES_6_A:
				{
					//Reset bit 6 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A &= 0xBF;

					break;

				}

				case RES_7_B:
				{
					//Reset bit 7 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B &= 0x7F;

					break;

				}

				case RES_7_C:
				{
					//Reset bit 7 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C &= 0x7F;

					break;

				}

				case RES_7_D:
				{
					//Reset bit 7 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D &= 0x7F;

					break;

				}

				case RES_7_E:
				{
					//Reset bit 7 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E &= 0x7F;

					break;

				}

				case RES_7_H:
				{
					//Reset bit 7 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H &= 0x7F;

					break;

				}

				case RES_7_L:
				{
					//Reset bit 7 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L &= 0x7F;

					break;

				}

				case RES_7_HL:
				{
					//Reset bit 7 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL &= 0xFF7F;

					break;

				}

				case RES_7_A:
				{
					//Reset bit 7 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A &= 0x7F;

					break;

				}

				case SET_0_B:
				{
					//Set bit 0 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B |= 1;

					break;

				}

				case SET_0_C:
				{
					//Set bit 0 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C |= 1;

					break;

				}

				case SET_0_D:
				{
					//Set bit 0 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D |= 1;

					break;

				}

				case SET_0_E:
				{
					//Set bit 0 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E |= 1;

					break;

				}

				case SET_0_H:
				{
					//Set bit 0 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H |= 1;

					break;

				}

				case SET_0_L:
				{
					//Set bit 0 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L |= 1;

					break;

				}

				case SET_0_HL:
				{
					//Set bit 0 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL |= 1;

					break;

				}

				case SET_0_A:
				{
					//Set bit 0 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A |= 1;

					break;

				}

				case SET_1_B:
				{
					//Set bit 1 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B |= 0x02;

					break;

				}

				case SET_1_C:
				{
					//Set bit 1 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C |= 0x02;

					break;

				}

				case SET_1_D:
				{
					//Set bit 1 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D |= 0x02;

					break;

				}

				case SET_1_E:
				{
					//Set bit 1 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E |= 0x02;

					break;

				}

				case SET_1_H:
				{
					//Set bit 1 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H |= 0x02;

					break;

				}

				case SET_1_L:
				{
					//Set bit 1 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L |= 0x02;

					break;

				}

				case SET_1_HL:
				{
					//Set bit 1 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL |= 0x02;

					break;

				}

				case SET_1_A:
				{
					//Set bit 1 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A |= 0x02;

					break;

				}

				case SET_2_B:
				{
					//Set bit 2 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B |= 0x04;

					break;

				}

				case SET_2_C:
				{
					//Set bit 2 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C |= 0x04;

					break;

				}

				case SET_2_D:
				{
					//Set bit 2 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D |= 0x04;

					break;

				}

				case SET_2_E:
				{
					//Set bit 2 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E |= 0x04;

					break;

				}

				case SET_2_H:
				{
					//Set bit 2 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H |= 0x04;

					break;

				}

				case SET_2_L:
				{
					//Set bit 2 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L |= 0x04;

					break;

				}

				case SET_2_HL:
				{
					//Set bit 2 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL |= 0x04;

					break;

				}

				case SET_2_A:
				{
					//Set bit 2 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A |= 0x04;

					break;

				}

				case SET_3_B:
				{
					//Set bit 3 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B |= 0x08;

					break;

				}

				case SET_3_C:
				{
					//Set bit 3 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C |= 0x08;

					break;

				}

				case SET_3_D:
				{
					//Set bit 3 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D |= 0x08;

					break;

				}

				case SET_3_E:
				{
					//Set bit 3 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E |= 0x08;

					break;

				}

				case SET_3_H:
				{
					//Set bit 3 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H |= 0x08;

					break;

				}

				case SET_3_L:
				{
					//Set bit 3 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L |= 0x08;

					break;

				}

				case SET_3_HL:
				{
					//Set bit 3 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL |= 0x08;

					break;

				}

				case SET_3_A:
				{
					//Set bit 3 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A |= 0x08;

					break;

				}

				case SET_4_B:
				{
					//Set bit 4 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B |= 0x10;

					break;

				}

				case SET_4_C:
				{
					//Set bit 4 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C |= 0x10;

					break;

				}

				case SET_4_D:
				{
					//Set bit 4 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D |= 0x10;

					break;

				}

				case SET_4_E:
				{
					//Set bit 4 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E |= 0x10;

					break;

				}

				case SET_4_H:
				{
					//Set bit 4 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H |= 0x10;

					break;

				}

				case SET_4_L:
				{
					//Set bit 4 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L |= 0x10;

					break;

				}

				case SET_4_HL:
				{
					//Set bit 4 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL |= 0x10;

					break;

				}

				case SET_4_A:
				{
					//Set bit 4 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A |= 0x10;

					break;

				}

				case SET_5_B:
				{
					//Set bit 5 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B |= 0x20;

					break;

				}

				case SET_5_C:
				{
					//Set bit 5 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C |= 0x20;

					break;

				}

				case SET_5_D:
				{
					//Set bit 5 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D |= 0x20;

					break;

				}

				case SET_5_E:
				{
					//Set bit 5 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E |= 0x20;

					break;

				}

				case SET_5_H:
				{
					//Set bit 5 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H |= 0x20;

					break;

				}

				case SET_5_L:
				{
					//Set bit 5 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L |= 0x20;

					break;

				}

				case SET_5_HL:
				{
					//Set bit 5 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL |= 0x20;

					break;

				}

				case SET_5_A:
				{
					//Set bit 5 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A |= 0x20;

					break;

				}

				case SET_6_B:
				{
					//Set bit 6 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B |= 0x40;

					break;

				}

				case SET_6_C:
				{
					//Set bit 6 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C |= 0x40;

					break;

				}

				case SET_6_D:
				{
					//Set bit 6 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D |= 0x40;

					break;

				}

				case SET_6_E:
				{
					//Set bit 6 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E |= 0x40;

					break;

				}

				case SET_6_H:
				{
					//Set bit 6 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H |= 0x40;

					break;

				}

				case SET_6_L:
				{
					//Set bit 6 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L |= 0x40;

					break;

				}

				case SET_6_HL:
				{
					//Set bit 6 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL |= 0x40;

					break;

				}

				case SET_6_A:
				{
					//Set bit 6 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A |= 0x40;

					break;

				}

				case SET_7_B:
				{
					//Set bit 7 in register B.
					CHIP_8->CPU_Regs_t.SingleReg.B |= 0x80;

					break;

				}

				case SET_7_C:
				{
					//Set bit 7 in register C.
					CHIP_8->CPU_Regs_t.SingleReg.C |= 0x80;

					break;

				}

				case SET_7_D:
				{
					//Set bit 7 in register D.
					CHIP_8->CPU_Regs_t.SingleReg.D |= 0x80;

					break;

				}

				case SET_7_E:
				{
					//Set bit 7 in register E.
					CHIP_8->CPU_Regs_t.SingleReg.E |= 0x80;

					break;

				}

				case SET_7_H:
				{
					//Set bit 7 in register H.
					CHIP_8->CPU_Regs_t.SingleReg.H |= 0x80;

					break;

				}

				case SET_7_L:
				{
					//Set bit 7 in register L.
					CHIP_8->CPU_Regs_t.SingleReg.L |= 0x80;

					break;

				}

				case SET_7_HL:
				{
					//Set bit 7 in register HL.
					CHIP_8->CPU_Regs_t.PairReg.HL |= 0x80;

					break;

				}

				case SET_7_A:
				{
					//Set bit 7 in register A.
					CHIP_8->CPU_Regs_t.SingleReg.A |= 0x80;

					break;

				}
			}

			inst_len = 2;

			break;
		}

		case CALL_Z:
		{
			//Call address immediate value if the Z flag is set.
			if(CHIP_8->CPU_flags_t.flags.Z == 1)
			{
				memory[CHIP_8->SP - 2] = (CHIP_8->PC + 1) & 0xFF;
				memory[CHIP_8->SP - 1] = (CHIP_8->PC + 1) >> 8;
				CHIP_8->SP -= 2;

				unsigned short immediate_value = memory[CHIP_8->PC + 2];
				immediate_value = immediate_value << 8 ;
				immediate_value |= memory[CHIP_8->PC + 3];

				CHIP_8->PC = immediate_value;
			}

			inst_len = 0;

			break;

		}

		case CALL:
		{
			//Push address of next instruction onto stack and then jump to address.
			memory[CHIP_8->SP - 2] = (CHIP_8->PC + 1) & 0xFF;
			memory[CHIP_8->SP - 1] = (CHIP_8->PC + 1) >> 8;
			CHIP_8->SP -= 2;

			unsigned short immediate_value = memory[CHIP_8->PC + 2];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->PC + 3];

			CHIP_8->PC = immediate_value;

			inst_len = 0;

			break;

		}

		case ADC_A_D8:
		{
			//Add immediate value + Carry flag to A.
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
			short test = CHIP_8->CPU_Regs_t.SingleReg.A +
				         immediate_value + CHIP_8->CPU_flags_t.flags.C;

			if (test & 0xFF00)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F;
			char immediate_low = (immediate_value + CHIP_8->CPU_flags_t.flags.C)& 0x0F;

			if((a_low + immediate_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A  = CHIP_8->CPU_Regs_t.SingleReg.A +
					                          immediate_value + CHIP_8->CPU_flags_t.flags.C;

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			inst_len = 2;

			break;

		}

		case RST_08H:
		{
			//Push present address onto stack ,Jump to address $0000 + 08.
			memory[CHIP_8->SP - 2] = CHIP_8->PC & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->PC >> 8;
			CHIP_8->SP -= 2;

			CHIP_8->PC = 0x0000 + 0x08;

			inst_len = 0;

			break;

		}

		case RET_NC:
		{
			//Return if following condition is true if C flag is reset
			if (CHIP_8->CPU_flags_t.flags.C == 0)
			{
				unsigned short immediate_value = memory[CHIP_8->SP + 1];
				immediate_value <<= 8;
				immediate_value |= memory[CHIP_8->SP];

				CHIP_8->PC = immediate_value;

				CHIP_8->SP += 2;

				inst_len = 0;
			}

			break;

		}

		case POP_DE:
		{
			//Pop two bytes off stack into register pair DE, Increment Stack Pointer twice.
			unsigned short immediate_value = memory[CHIP_8->SP + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->SP];

			CHIP_8->CPU_Regs_t.PairReg.DE = immediate_value;

			CHIP_8->SP += 2;

			break;

		}

		case JP_NC_D16:
		{
			////Jump if C flag is reset to address two byte immediate value.
			if(CHIP_8->CPU_flags_t.flags.C == 0)
			{
				unsigned short immediate_value = memory[CHIP_8->PC + 1];
				immediate_value = immediate_value << 8 ;
				immediate_value |= memory[CHIP_8->PC + 2];

				CHIP_8->PC = immediate_value;

				inst_len = 0;
			}

			break;

		}

		case CALL_NC:
		{
			//Call address immediate value if the C flag is reset.
			if(CHIP_8->CPU_flags_t.flags.C == 0)
			{
				memory[CHIP_8->SP - 2] = (CHIP_8->PC + 1) & 0xFF;
				memory[CHIP_8->SP - 1] = (CHIP_8->PC + 1) >> 8;
				CHIP_8->SP -= 2;

				unsigned short immediate_value = memory[CHIP_8->PC + 2];
				immediate_value = immediate_value << 8 ;
				immediate_value |= memory[CHIP_8->PC + 3];

				CHIP_8->PC = immediate_value;
			}


			inst_len = 0;

			break;

		}

		case PUSH_DE:
		{
			//Push register pair DE onto stack , Decrement Stack Pointer (SP) twice.
			memory[CHIP_8->SP - 2] = CHIP_8->CPU_Regs_t.PairReg.DE & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->CPU_Regs_t.PairReg.DE >> 8;
			CHIP_8->SP -= 2;

			break;

		}

		case SUB_A_D8:
		{
			//Subtract immediate value from A.
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(immediate_value & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)immediate_value;

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			inst_len = 2;

			break;

		}

		case RST_10H:
		{
			//Push present address onto stack ,Jump to address $0000 + 10.
			memory[CHIP_8->SP - 2] = CHIP_8->PC & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->PC >> 8;
			CHIP_8->SP -= 2;

			CHIP_8->PC = 0x0000 + 0x10;

			inst_len = 0;

			break;

		}

		case RET_C:
		{
			//Return if following condition is true if C flag is set
			if (CHIP_8->CPU_flags_t.flags.C == 0)
			{
				unsigned short immediate_value = memory[CHIP_8->SP + 1];
				immediate_value <<= 8;
				immediate_value |= memory[CHIP_8->SP];

				CHIP_8->PC = immediate_value;

				CHIP_8->SP += 2;

				inst_len = 0;
			}

			break;

		}

		case RETI:
		{
			//Pop two bytes from stack & jump to that address and then enable interrupt.
			unsigned short immediate_value = memory[CHIP_8->SP + 1];
			immediate_value <<= 8;
			immediate_value |= memory[CHIP_8->SP];

			CHIP_8->PC = immediate_value;

			CHIP_8->SP += 2;

			CHIP_8->IME = TRUE;

			inst_len = 0;

			break;

		}


		case JP_C_D16:
		{
			////Jump if C flag is set to address two byte immediate value.
			if(CHIP_8->CPU_flags_t.flags.C == 1)
			{
				unsigned short immediate_value = memory[CHIP_8->PC + 1];
				immediate_value = immediate_value << 8 ;
				immediate_value |= memory[CHIP_8->PC + 2];

				CHIP_8->PC = immediate_value;

				inst_len = 0;
			}

			break;

		}

		case CALL_C:
		{
			//Call address immediate value if the C flag is reset.
			if(CHIP_8->CPU_flags_t.flags.C == 1)
			{
				memory[CHIP_8->SP - 2] = (CHIP_8->PC + 1) & 0xFF;
				memory[CHIP_8->SP - 1] = (CHIP_8->PC + 1) >> 8;
				CHIP_8->SP -= 2;

				unsigned short immediate_value = memory[CHIP_8->PC + 2];
				immediate_value = immediate_value << 8 ;
				immediate_value |= memory[CHIP_8->PC + 3];

				CHIP_8->PC = immediate_value;
			}


			inst_len = 0;

			break;

		}

		case SBC_A_D8:
		{
			//Subtract immediate value + carry from A.
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
			unsigned char old_carry = CHIP_8->CPU_flags_t.flags.C;
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			unsigned char borrow = CHIP_8->CPU_Regs_t.SingleReg.A & 0x80;

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)((immediate_value + old_carry ) & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			CHIP_8->CPU_Regs_t.SingleReg.A = (signed char)CHIP_8->CPU_Regs_t.SingleReg.A -
							                 (signed char)(immediate_value + old_carry);

			unsigned char test = CHIP_8->CPU_Regs_t.SingleReg.A & 0x40;

			if(test < borrow)
			{
				CHIP_8->CPU_flags_t.flags.C = old_carry;
			}

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			inst_len = 2;

			break;

		}

		case RST_18H:
		{
			//Push present address onto stack ,Jump to address $0000 + 18.
			memory[CHIP_8->SP - 2] = CHIP_8->PC & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->PC >> 8;
			CHIP_8->SP -= 2;

			CHIP_8->PC = 0x0000 + 0x18;

			inst_len = 0;

			break;

		}

		case LDH_A8_A:
		{
			//Put A into memory address $FF00+ immediate_value.
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
			unsigned short address = 0xFF00 + immediate_value;
			memory[address] = CHIP_8->CPU_Regs_t.SingleReg.A;

			inst_len = 2;

			break;

		}

		case POP_HL:
		{
			//Pop two bytes off stack into register pair HL, Increment Stack Pointer twice.
			unsigned short immediate_value = memory[CHIP_8->SP + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->SP];

			CHIP_8->CPU_Regs_t.PairReg.HL = immediate_value;

			CHIP_8->SP += 2;

			break;

		}

		case LD_C_IMM_A:
		{
			//Put A into address $FF00 + register C.
			unsigned short address = 0xFF00 + CHIP_8->CPU_Regs_t.SingleReg.C;
			memory[address] = CHIP_8->CPU_Regs_t.SingleReg.A;

			inst_len = 2;

			break;

		}

		case PUSH_HL:
		{
			//Push register pair HL onto stack , Decrement Stack Pointer (SP) twice.
			memory[CHIP_8->SP - 2] = CHIP_8->CPU_Regs_t.PairReg.HL & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->CPU_Regs_t.PairReg.HL >> 8;
			CHIP_8->SP -= 2;

			break;

		}

		case AND_A_D8:
		{
			//Logically AND immediate value with A.
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
	        CHIP_8->CPU_Regs_t.SingleReg.A &= immediate_value;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			inst_len = 2;

			break;

		}

		case RST_20H:
		{
			//Push present address onto stack ,Jump to address $0000 + 20.
			memory[CHIP_8->SP - 2] = CHIP_8->PC & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->PC >> 8;
			CHIP_8->SP -= 2;

			CHIP_8->PC = 0x0000 + 0x20;

			inst_len = 0;

			break;

		}

		case ADD_SP_R8:
		{
			//Add immediate value to SP.
			char immediate_value = memory[CHIP_8->PC + 1];
			int test = CHIP_8->SP + immediate_value;

			if (test > 0xFFFF)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char a_low = CHIP_8->SP & 0x0F;
			char immediate_low = immediate_value & 0x0F;

			if((a_low + immediate_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->SP  += immediate_value;

			CHIP_8->CPU_flags_t.flags.N = 0;

			if(CHIP_8->SP == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			inst_len = 2;

			break;

		}

		case JP_HL:
		{
			//Jump to address contained in HL.
			CHIP_8->SP = CHIP_8->CPU_Regs_t.PairReg.HL;

			inst_len = 0;

			break;

		}

		case LD_A_IMM_VALUE:
		{
			// Put A at immediate tow bit address
			unsigned short immediate_value = memory[CHIP_8->PC + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->PC + 2];

			memory[immediate_value] = CHIP_8->CPU_Regs_t.SingleReg.A ;

			inst_len = 3;

			break;

		}

		case XOR_A_D8:
		{
			//Logical exclusive OR immediate value with register A.
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
			CHIP_8->CPU_Regs_t.SingleReg.A ^= immediate_value;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			inst_len = 2;

			break;

		}

		case RST_28H:
		{
			//Push present address onto stack ,Jump to address $0000 + 28.
			memory[CHIP_8->SP - 2] = CHIP_8->PC & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->PC >> 8;
			CHIP_8->SP -= 2;

			CHIP_8->PC = 0x0000 + 0x28;

			inst_len = 0;

			break;

		}

		case LDH_A_A8:
		{
			//Put memory address $FF00 + immediate_value into A.
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
			unsigned short address = 0xFF00 + immediate_value;
			CHIP_8->CPU_Regs_t.SingleReg.A = memory[address];

			inst_len = 2;

			break;

		}

		case POP_AF:
		{
			//Pop two bytes off stack into register pair AF, Increment Stack Pointer twice.
			unsigned short immediate_value = memory[CHIP_8->SP + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->SP];

			CHIP_8->CPU_Regs_t.PairReg.AF = immediate_value;

			CHIP_8->SP += 2;

			break;

		}

		case LD_A_IMM_C:
		{
			//Put value at address $FF00 + register C into A.
			unsigned short address = 0xFF00 + CHIP_8->CPU_Regs_t.SingleReg.C;
			CHIP_8->CPU_Regs_t.SingleReg.A = memory[address];

			inst_len = 2;

			break;

		}

		case DI:
		{
			CHIP_8->IME = FALSE;

			break;

		}

		case PUSH_AF:
		{
			//Push register pair AF onto stack , Decrement Stack Pointer (SP) twice.
			memory[CHIP_8->SP - 2] = CHIP_8->CPU_Regs_t.PairReg.AF & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->CPU_Regs_t.PairReg.AF >> 8;
			CHIP_8->SP -= 2;

			break;

		}

		case OR_A_D8:
		{
			//Logical  OR immediate value with register A.
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
			CHIP_8->CPU_Regs_t.SingleReg.A |= immediate_value;

			CHIP_8->CPU_flags_t.flags.H = 0;
			CHIP_8->CPU_flags_t.flags.C = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			if (CHIP_8->CPU_Regs_t.SingleReg.A == 0)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			break;

		}

		case RST_30H:
		{
			//Push present address onto stack ,Jump to address $0000 + 30.
			memory[CHIP_8->SP - 2] = CHIP_8->PC & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->PC >> 8;
			CHIP_8->SP -= 2;

			CHIP_8->PC = 0x0000 + 0x30;

			inst_len = 0;

			break;

		}

		case LD_HL_SP_R8:
		{
			//Put SP + n effective address into HL.
			int test = CHIP_8->SP + (signed char)memory[CHIP_8->PC + 1];
			unsigned short address =  CHIP_8->SP  + (signed char)memory[CHIP_8->PC + 1];

			CHIP_8->CPU_Regs_t.PairReg.HL = address;

			if (test > 0xFFFF)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			char HL_low = CHIP_8->SP & 0x0F;
			char immediate_low = (signed char)memory[CHIP_8->PC + 1] & 0x0F;

			if((HL_low + immediate_low) & 0x10)
			{
				CHIP_8->CPU_flags_t.flags.H = 1;
			}

			CHIP_8->CPU_flags_t.flags.Z = 0;
			CHIP_8->CPU_flags_t.flags.N = 0;

			inst_len = 2;

			break;

		}

		case LD_SP_HL:
		{
			//Put HL into Stack Pointer (SP).
			CHIP_8->SP = CHIP_8->CPU_Regs_t.PairReg.HL;

			break;

		}

		case LD_IMM_VALUE_A:
		{
			// Put immediate_value into A.
			unsigned short immediate_value = memory[CHIP_8->PC + 1];
			immediate_value = immediate_value << 8 ;
			immediate_value |= memory[CHIP_8->PC + 2];

			CHIP_8->CPU_Regs_t.SingleReg.A = immediate_value;

			inst_len = 3;

			break;

		}

		case EI:
		{
			CHIP_8->IME = TRUE;
			break;

		}

		case CP_A_D8:
		{
			//Compare A with immediate value.
			unsigned char immediate_value = memory[CHIP_8->PC + 1];
			unsigned char old_half_carry = CHIP_8->CPU_flags_t.flags.H;

			CHIP_8->CPU_flags_t.flags.H = 1;
			CHIP_8->CPU_flags_t.flags.N = 1;

			if(CHIP_8->CPU_Regs_t.SingleReg.A == immediate_value)
			{
				CHIP_8->CPU_flags_t.flags.Z = 1;
			}

			if ((signed char)(CHIP_8->CPU_Regs_t.SingleReg.A & 0x0F) -
				(signed char)(immediate_value & 0x0F) < 0)
			{
				CHIP_8->CPU_flags_t.flags.H = old_half_carry;
			}

			if(immediate_value > CHIP_8->CPU_Regs_t.SingleReg.A)
			{
				CHIP_8->CPU_flags_t.flags.C = 1;
			}

			inst_len = 2;

			break;

		}

		case RST_38H:
		{
			//Push present address onto stack ,Jump to address $0000 + 38.
			memory[CHIP_8->SP - 2] = CHIP_8->PC & 0xFF;
			memory[CHIP_8->SP - 1] = CHIP_8->PC >> 8;
			CHIP_8->SP -= 2;

			CHIP_8->PC = 0x0000 + 0x38;

			inst_len = 0;

			break;

		}




	}

	return inst_len;
}

int RunCPU ()
{

	while ( CHIP_8->CPU_is_On )
	{
		//check for interrupt mode.
		if(CHIP_8->IME && (Memory.interrupt_t.IF & 1))
		{

		}
		else if(CHIP_8->IME && (Memory.interrupt_t.IF & 0x02))
		{

		}
		else if(CHIP_8->IME && (Memory.interrupt_t.IF & 0x04))
		{

		}
		else if(CHIP_8->IME && (Memory.interrupt_t.IF & 0x08))
		{

		}
		else if(CHIP_8->IME && (Memory.interrupt_t.IF & 0x10))
		{

		}
		// run mode.
		else
		{
			// Fetch the next instruction
			char Opcode = memory[CHIP_8->PC];

			//Decode stage, decode the instruction
			CHIP_8->PC += Decode(Opcode); // Advance the program counter
		}
	}

	return 1;
}
