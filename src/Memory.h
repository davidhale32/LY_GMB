/*
 * Memory.h
 *
 *  Created on: 15 Dec 2020
 *      Author: dan
 */

#ifndef MEMORY_H_
#define MEMORY_H_


typedef struct interrupt_flags_s
{
	unsigned char V_Blank       : 1;
	unsigned char LCDC          : 1;
	unsigned char Timer         : 1;
	unsigned char Serial_I_O    : 1;
	unsigned char Joypad_P13    : 1;
	unsigned char Joypad_P12    : 1;
	unsigned char Joypad_P11    : 1;
	unsigned char Joypad_P10    : 1;

}interrupt_flags_t;


typedef struct MEMORY
{

	union interrupt_u
	{
		interrupt_flags_t IF_flags;
		unsigned char IF;
		                                 /*[7][6][5][4][3][2][1][0]*/
		   	   	   	   	                 /*[0][1][2][3][S][T][L][V]*/
	}interrupt_t;

	char I_ME_flag;

}MEMORY;

MEMORY Memory;


#endif /* MEMORY_H_ */
