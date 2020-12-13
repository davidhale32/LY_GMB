
#include "CPU.h"

#include "GBC_instruction_set.h"

extern CPU_Registers *CHIP_8;


//char memory[0xFFFF] = Load from binary file!!!
char memory[] = {0x00,0x80,0x27,0x76 };

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
			if( (CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) +
				(CHIP_8->CPU_Regs_t.PairReg.BC & 0x00FF) & 0x0100 == 0x0100)
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
			if( (CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) +
				(CHIP_8->CPU_Regs_t.PairReg.DE & 0x00FF) & 0x0100 == 0x0100)
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

			if(((hs_a_value < 9) && (ls_a_value > 9) && (h_flag == 0) && (c_flag == 0))|
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

			if(((hs_a_value > 9) && (ls_a_value <= 9) && (h_flag == 0) && (c_flag == 0))|
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

			if(((hs_a_value > 8) && (ls_a_value > 9) && (h_flag == 0) && (c_flag == 0))|
			  ((hs_a_value > 9) && (ls_a_value < 3) && (h_flag == 1) && (c_flag == 0))|
			  ((hs_a_value < 3) && (ls_a_value > 9) && (h_flag == 0) && (c_flag == 1))|
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

			if(((hs_a_value > 5) && (hs_a_value < 8)) && (ls_a_value > 5) && (h_flag == 1) &&
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
			if( (CHIP_8->CPU_Regs_t.PairReg.HL & 0x00FF) +
				(CHIP_8->SP & 0x00FF) & 0x0100 == 0x0100)
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
			CHIP_8->CPU_Regs_t.SingleReg.B;

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
			CHIP_8->CPU_Regs_t.SingleReg.C;

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
			CHIP_8->CPU_Regs_t.SingleReg.D;

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
			CHIP_8->CPU_Regs_t.SingleReg.E;

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
			CHIP_8->CPU_Regs_t.SingleReg.H;

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
			CHIP_8->CPU_Regs_t.SingleReg.L;

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
			CHIP_8->CPU_Regs_t.SingleReg.A;

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

		case ADD_A_B_Carry:
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

		case ADD_A_C_Carry:
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

		case ADD_A_D_Carry:
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

		case ADD_A_E_Carry:
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

		case ADD_A_H_Carry:
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

		case ADD_A_L_Carry:
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

		case ADD_A_HL_Carry:
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

		case ADD_A_A_Carry:
		{
			//Add HL + Carry flag to A.
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




	}


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
