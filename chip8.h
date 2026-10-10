/**
 * @file chip8.h
 *
 * @author Gabriel
 */
#ifndef __CHIP8_H_
#define __CHIP8_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0
#define bool int

#define MEM_SIZE 4096
#define STACK_SIZE 16

#define X_SIZE 64
#define Y_SIZE 32

extern uint8_t gfx[X_SIZE * Y_SIZE];
extern uint8_t keypad[16];

void chip8_init();
void chip8_load(char* rom);
bool chip8_running();
void chip8_cycle();

#endif //__CHIP8_H_
