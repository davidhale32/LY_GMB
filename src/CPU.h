#include <stdio.h>
#include <stdlib.h>

typedef enum bool_e
{
	FALSE,
	TRUE
}bool_t;

typedef struct Single_Reg_s
{
	// This define the cpu registers
	// as a single 8-bit register.
	unsigned char A;
	unsigned char F;
	unsigned char B;
	unsigned char C;
	unsigned char D;
	unsigned char E;
	unsigned char H;
	unsigned char L;

}Single_Reg_t;

typedef struct Pair_Reg_s
{
	// This define the CPU registers
	// as a single 16-bit register (unified)
	unsigned short  AF;
	unsigned short  BC;
	unsigned short  DE;
	unsigned short  HL;

}Pair_Reg_t;

typedef struct CPU_flags_s
{
	unsigned char RESERVED : 4;
	unsigned char C        : 1;
	unsigned char H        : 1;
	unsigned char N        : 1;
	unsigned char Z        : 1;

}CPU_flags_t;

typedef struct CPU_Register
{
 // Main CPU registers are defined here as a union:
	union CPU_Regs_u
	{
		Single_Reg_t SingleReg;
		Pair_Reg_t   PairReg;
	}CPU_Regs_t;

// CPU flags are defined here:
	union CPU_flags_u
	{
		CPU_flags_t flags;
		char        flag;  /*[7][6][5][4][3][2][1][0]*/
		   	   	   	   	   /*[Z][N][H][C][0][0][0][0]*/
	}CPU_flags_t;

	unsigned short int SP; /* stack pointer point to the current code on stack*/
	unsigned short int PC; /* program counter point to the next command to execute*/

	// Hold the CPU state
	bool_t CPU_is_On;
}CPU_Registers;

int RunCPU ();
int Fetch ();
int Decode(const char Opcode);
int Execute();
