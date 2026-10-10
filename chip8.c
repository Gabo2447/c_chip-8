#include "chip8.h"

static int RUNNING = false;

uint8_t memory[MEM_SIZE];
uint16_t stack[STACK_SIZE];

uint16_t PC;
uint16_t opcode;
uint8_t V[16];
uint16_t I;

uint8_t gfx[X_SIZE * Y_SIZE];
uint8_t keypad[16];

// Fontset CHIP-8
uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint16_t SP = 0;
uint8_t delay_timer = 0;
uint8_t sound_timer = 0;

void chip8_init() {
    memset(memory, 0, sizeof(memory));
    memset(stack, 0, sizeof(stack));
    memset(gfx, 0, sizeof(gfx));
    memset(keypad, 0, sizeof(keypad));
    RUNNING = true;

    PC = 0x200;
    opcode = 0;
    I = 0;
    SP = 0;
    delay_timer = 0;
    sound_timer = 0;

    for (int i = 0; i < 80; ++i) {
        memory[0x050 + i] = fontset[i];
    }
}

bool chip8_running() {
    return RUNNING;
}

void chip8_load(char* rom) {
    FILE *f = fopen(rom, "rb");
    if (f == NULL) {
        printf("Error opening file\n");
        exit(1);
    }

    fread(&memory[0x200], 1, MEM_SIZE - 0x200, f); // 0x200 -> 512
    fclose(f);
}

void chip8_cycle() {
    opcode = (memory[PC] << 8) | memory[PC + 1]; // 2 bytes = 2 * 8 bits
    printf("PC: 0x%04X | Opcode read: 0x%04X\n", PC, opcode);

    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) {
                printf("Cleaning screen...\n");
                memset(gfx, 0, sizeof(gfx));
            } else if (opcode == 0x00EE) {
                printf("Retornando de subrutina...\n");
                SP--;
                PC = stack[SP];
                break;
            }
            break;

        case 0x1000: {
            uint16_t nnn = opcode & 0x0FFF;
            printf("Saltando a la direccion: 0x%04X\n", nnn);
            PC = nnn;
            return;
        }
        case 0x2000: {
            uint16_t nnn = opcode & 0x0FFF;
            stack[SP] = PC;
            SP++;
            PC = nnn;
            return;
        }
        case 0x3000: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t kk = opcode & 0x00FF;

            if (V[x] == kk) {
                PC += 2;
            }
            break;
        }
        case 0x4000: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t kk = opcode & 0x00FF;

            if (V[x] != kk) {
                PC += 2;
            }
            break;
        }

        case 0x5000: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00FF) >> 4;

            if (V[x] == V[y]) {
                PC += 2;
            }

            break;
        }

        case 0x6000: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t kk = opcode & 0x00FF;
            V[x] = kk;
            break;
        }

        case 0x7000: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t kk = opcode & 0x00FF;
            V[x] += kk;
            break;
        }

        case 0x8000: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00FF) >> 4;
            uint8_t sub_op = opcode & 0x000F;

            switch (sub_op) {
                case 0x0: V[x] = V[y]; break;
                case 0x1: V[x] |= V[y]; break;
                case 0x2: V[x] &= V[y]; break;
                case 0x3: V[x] ^= V[y]; break;
                case 0x4:
                    if ((V[x] + V[y]) > 255) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[x] += V[y];
                    break;
                case 0x5:
                    V[0xF] = (V[x] >= V[y]) ? 1 : 0;
                    V[x] -= V[y];
                    break;
                case 0x6:
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    break;
                case 0x7:
                    V[0xF] = (V[y] >= V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    break;
                case 0xE:
                    V[0xF] = (V[x] & 0x80) >> 7;
                    V[x] <<= 1;
                    break;

                default:
                    printf("Sub-opcode 0x8xy%X not exists.\n", sub_op);
                    exit(1);
            }
            break;
        }
        case 0x9000: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00FF) >> 4;
            if (V[x] != V[y]) {
                PC += 2;
            }
            break;
        }
        case 0xA000: {
            I = opcode & 0x0FFF;
            break;
        }

        case 0xB000: {
            PC = (opcode & 0x0FFF) + V[0];
            break;
        }

        case 0xC000: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t kk = opcode & 0x00FF;
            V[x] = (rand() % 256) & kk;
            break;
        }
        case 0xD000: {
            uint8_t x = V[(opcode & 0x0F00) >> 8] % X_SIZE;
            uint8_t y = V[(opcode & 0x00FF) >> 4] % Y_SIZE;
            uint8_t height = opcode & 0x000F;

            V[0xF] = 0;

            for (int row = 0; row < height; row++) {
                uint8_t pixel_byte = memory[I + row];

                for (int col = 0; col < 8; col++) {
                    uint8_t pixel_bit = pixel_byte & (0x80 >> col);

                    if (pixel_bit == 0) {
                        continue;
                    }

                    int x_coord = (x + col) % X_SIZE;
                    int y_coord = (y + row) % Y_SIZE;
                    int i = x_coord + (y_coord * 64);

                    if (gfx[i] == 1) {
                        V[0xF] = 1;
                    }

                    gfx[i] ^= 1;
                }
            }
            break;
        }
        case 0xE000: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t key = V[x];
            uint8_t sub_op = opcode & 0x00FF;

            if (sub_op == 0x9E) {
                if (keypad[key] != 0) {
                    PC += 2;
                }
            } else if (sub_op == 0xA1) {
                if (keypad[key] == 0) {
                    PC += 2;
                }
            } else {
                printf("Opcode E desconocido: 0x%04X\n", opcode);
                exit(1);
            }
            break;
        }
        case 0xF000: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t sub_op = opcode & 0x00FF;

            switch (sub_op) {
                case 0x07:
                    V[x] = delay_timer;
                    break;

                case 0x0A: {
                    bool pressed = false;
                    for (int i = 0; i < 16; i++) {
                        if (keypad[i]) {
                            V[x] = i;
                            pressed = true;
                            break;
                        }
                    }
                    if (!pressed) {
                        PC -= 2;
                    }
                    break;
                }

                case 0x15:
                    delay_timer = V[x];
                    break;

                case 0x18:
                    sound_timer = V[x];
                    break;

                case 0x1E:
                    I += V[x];
                    break;

                case 0x29:
                    I = 0x050 + (V[x] * 5);
                    break;

                case 0x33:
                    memory[I]     = V[x] / 100;
                    memory[I + 1] = (V[x] / 10) % 10;
                    memory[I + 2] = V[x] % 10;
                    break;

                case 0x55:
                    for (int i = 0; i <= x; i++) {
                        memory[I + i] = V[i];
                    }
                    break;

                case 0x65:
                    for (int i = 0; i <= x; i++) {
                        V[i] = memory[I + i];
                    }
                    break;

                default:
                    printf("Opcode F unknown: 0x%04X\n", opcode);
                    exit(1);
            }
            break;
        }
        default:
            printf("Opcode unknown: 0x%04X\n", opcode);
            exit(1);
    }

    PC += 2;
}
