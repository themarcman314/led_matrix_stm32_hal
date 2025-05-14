/*
 * led_matrix.c
 *
 *  Created on: Apr 21, 2025
 *      Author: marcman
 */
#include <stdint.h>
#include "main.h"
#include <stdio.h>
#include "tim.h"
#include "led_bitmap.h"


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

typedef enum
{
  MATRIX_RDY       = 0x00U,
  MATRIX_BUSY,
  MATRIX_FILL_DATA
} state;

state current_state;

struct frame my_frame;

void set_line(uint32_t line_num);
void led_crawler(void);
void Display_time_set(uint8_t hour, uint8_t minute);

void init_led(void)
{
	current_state = MATRIX_RDY;
	HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, GPIO_PIN_RESET);
	my_frame.top[0].r = 0x80000000;
	// do not immediately go into interrupt
	htim2.Instance->SR = 0;
}

void fill_lines(uint32_t line_num)
{
	set_line(line_num);
	HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_SET); // deactivate led matrix
	for(uint32_t row_index =  0; row_index < LINE_SIZE; row_index++)
	{
		// set red data
		HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, my_frame.top[line_num].r>>(31-row_index)&1);
		HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, my_frame.bottom[line_num].r>>(31-row_index)&1);

		// "clock" for shift register
		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET);
		// just go as fast as we can fk it
		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_RESET); // activate led matrix

	HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, GPIO_PIN_SET);
	// maintain latch for a little
	current_state = MATRIX_BUSY;
	HAL_TIM_Base_Start_IT(&htim2);
}

void set_line(uint32_t line_num)
{
	// set c, b, and a bits
	HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, line_num & 0b1);
	HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, line_num & 0b10);
	HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, line_num & 0b100);
}

void fill_all_lines()
{
	static uint32_t line_idx = 0;
	if(line_idx != 8)
	{
		fill_lines(line_idx);
		line_idx++;
	}
	else
	{
		line_idx = 0;
		current_state = MATRIX_FILL_DATA;
	}
}

void led_loop()
{
	switch (current_state) {
		case MATRIX_RDY:
			fill_all_lines();
			break;

		case MATRIX_BUSY:
			// Do other stuff
			break;

		case MATRIX_FILL_DATA:
			Display_time_set(1,1);
			// update frame here
			//led_crawler();


			current_state = MATRIX_RDY;
			break;
	}

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{
		HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, GPIO_PIN_RESET);
		HAL_TIM_Base_Stop_IT(&htim2);
		htim2.Instance->CNT = 0;  // reset timer to 0
		current_state = MATRIX_RDY;
	}
}

void led_crawler(void)
{
	static uint32_t line_index = 0;
	static uint32_t direction = 0;

	struct line* line = my_frame.top; // start at top of display
	uint32_t index = line_index % 16; // run through 16 lines in loop

	// we can do this since top and bottom data is contiguous in memory
	if (!direction && line[index].r != 1)
		line[index].r >>= 1;
	else if (direction && line[index].r != 0x80000000)
		line[index].r <<= 1;
	else
	{
		line_index++;								// go to new line
		direction = !direction; 					// change direction
		line[(index + 1) % 16].r = line[index].r; 	// stay on same same x coordinate
		line[index].r = 0;							// turn off led on previous line
	}
}

// TODO: write implementation for this
void Display_time_set(uint8_t hour, uint8_t minute)
{
	switch (hour) {
		case 0:
			NUMBER_0
			break;
		case 1:
			NUMBER_1
			break;
		case 2:
			NUMBER_2
			break;
		case 3:
			NUMBER_3
			break;
		case 4:
			NUMBER_4
			break;
		case 5:
			NUMBER_5
			break;
		case 6:
			NUMBER_6
			break;
		case 7:
			NUMBER_7
			break;
		case 8:
			NUMBER_8
			break;
		case 9:
			NUMBER_9
			break;
		default:
			break;
	}
}
