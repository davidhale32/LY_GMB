/*
 * GBC_instruction_set.h
 *
 *  Created on: 6 Dec 2020
 *      Author: dan
 */

#ifndef GBC_INSTRUCTION_SET_H_
#define GBC_INSTRUCTION_SET_H_

typedef enum instruction_e
{
	NOP               = 0x0,
	LD_BC_IMM         = 0x1,
	LD_BC_A           = 0x2,
	INC_BC            = 0x3,
	INC_B             = 0x4,
	DEC_B             = 0x5,
	LD_B_IMM          = 0x6,
	RLCA              = 0x7,
	LD_SP_IMM_ADDRESS = 0x8,
	ADD_HL_BC         = 0x9,
	LD_A_BC           = 0x0A,
	DEC_BC            = 0x0B,
	INC_C             = 0x0C,
	DEC_C             = 0x0D,
	LD_C_IMM          = 0x0E,
	RRCA              = 0x0F,
	STOP              = 0x10,
	LD_DE_IMM         = 0x11,
	LD_DE_A           = 0x12,
	INC_DE            = 0x13,
	INC_D             = 0x14,
	DEC_D             = 0x15,
	LD_D_IMM          = 0x16,
	RLA               = 0x17,
	JR_r8             = 0x18,
	ADD_HL_DE         = 0x19,
	LD_A_DE           = 0x1A,
	DEC_DE            = 0x1B,
	INC_E             = 0x1C,
	DEC_E             = 0x1D,
	LD_E_IMM          = 0x1E,
	RRA               = 0x1F,
	JR_NZ_r8          = 0x20,
	LD_HL_IMM         = 0x21,
	LD_HL_plus_A      = 0x22,
	INC_HL            = 0x23,
	INC_H             = 0x24,
	DEC_H             = 0x25,
	LD_H_IMM          = 0x26,
	DAA               = 0x27,
	JR_Z_r8           = 0x28,
	ADD_HL_HL         = 0x29,
	LD_A_HL_plus      = 0x2A,
	DEC_HL            = 0x2B,
	INC_L             = 0x2C,
	DEC_L             = 0x2D,
	LD_L_IMM          = 0x2E,
	CPL               = 0x2F,
	JR_NC_r8          = 0x30,
	LD_SP_IMM         = 0x31,
	LD_HL_minus_A     = 0x32,
	INC_SP            = 0x33,
	INC_hl            = 0x34,
	DEC_hl            = 0x35,
	LD_HL_d8          = 0x36,
	HALT              = 0x76,
	PREF_CB           = 0xCB
}inst_t;

typedef enum instruction_cb_prefix_e
{
	RLC_B     = 0x0,
	RLC_A     = 0x1,
}inst_cb_prefix_t;

#endif /* GBC_INSTRUCTION_SET_H_ */
