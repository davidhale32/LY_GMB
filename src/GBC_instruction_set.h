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
	NOP       = 0x0,
	LD_BC_IMM = 0x1,
	HALT      = 0x76,
	PREF_CB   = 0xCB
}inst_t;

typedef enum instruction_cb_prefix_e
{
	RLC_B     = 0x0,
	RLC_A     = 0x1,
}inst_cb_prefix_t;

#endif /* GBC_INSTRUCTION_SET_H_ */
