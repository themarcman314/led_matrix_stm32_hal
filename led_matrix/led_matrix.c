/*
 * led_matrix.c
 *
 *  Created on: Apr 21, 2025
 *      Author: marcman
 */
#include <stdint.h>
#include "main.h"
#include <stdio.h>

#define LINE_SIZE 32


struct line
{
	uint32_t r;
};
struct frame
{
	struct line top[8];
	struct line bottom[8];
};

struct frame my_frame;

void set_line(uint32_t line_num);

void init_led(void)
{
	HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, GPIO_PIN_RESET);
	my_frame.top[0].r = 0x80000000;
	my_frame.bottom[0].r = 0x80000000;

}

void fill_line(uint32_t line_num)
{
	set_line(line_num);
	HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_SET); // deactivate led matrix
	for(uint32_t row_index =  0; row_index < LINE_SIZE; row_index++)
	{
		HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, my_frame.top[line_num].r>>(31-row_index)&1);
		//HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, my_frame.bottom[line_num].r>>31);
		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET);
		//HAL_Delay(1); // just go as fast as we can fk it
		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET);
		//HAL_Delay(1);
	}
	HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_RESET); // activate led matrix
	HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, GPIO_PIN_SET);
	HAL_Delay(1); // maintain latch for a little
	HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, GPIO_PIN_RESET);
}

void set_line(uint32_t line_num)
{
	// set c, b, and a bits
	HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, line_num & 0b1);
	HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, line_num & 0b10);
	HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, line_num & 0b100);
}

void fill_all_lines(void)
{
	for(uint32_t index = 0; index < 8; index++)
	{
		  fill_line(index);
	}
}
void led_crawler(void)
{
	static uint32_t line_index = 0;
	static uint32_t direction = 0;

	fill_all_lines();
	if(!direction && my_frame.top[line_index%8].r!=1)
		my_frame.top[line_index%8].r >>= 1;
	else if(direction && my_frame.top[line_index%8].r != 0x80000000)
		my_frame.top[line_index%8].r <<= 1;
	else
	{
		direction = !direction;
		my_frame.top[(line_index+1)%8].r = my_frame.top[line_index%8].r;
		line_index++;
	}

}
