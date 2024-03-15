#include <stdio.h>
#include <string.h> 
#include "sim4.h" 


// Single cycle CPU simulator


#include <stdio.h>
#include "sim4.h"

int fill_CPUControl(InstructionFields *fields, CPUControl *controlOut) {
    memset(controlOut, 0, sizeof(CPUControl)); // Clear the control signals

    switch(fields->opcode) {
        case 0x00: // R-type instructions
            controlOut->regDst = 1;
            controlOut->regWrite = 1;
            switch(fields->funct) {
                case 0x20: // ADD
                case 0x21: // ADDU
                    controlOut->ALU.op = 2; // Assume ALU.op = 2 is for ADD
                    break;
                case 0x22: // SUB
                case 0x23: // SUBU
                    controlOut->ALU.op = 2; // Assume ALU.op = 2 is for SUB
                    controlOut->ALU.bNegate = 1;
                    break;
                case 0x24: // AND
                    controlOut->ALU.op = 0; 
                    break;
                case 0x25: // OR
                    controlOut->ALU.op = 1; 
                    break;
                case 0x26: // XOR
                    controlOut->ALU.op = 4; // Setting ALU.op = 4 for XOR
                    break;
                case 0x2A: // SLT
                    controlOut->ALU.op = 3; 
                    controlOut->ALU.bNegate = 0; // For SLT, it's a comparison, not direct subtraction
                    break;
                default:
                    return 0; // Unrecognized funct code
            }
            break;
        case 0x08: // ADDI
            controlOut->ALUsrc = 1;
            controlOut->regWrite = 1;
            controlOut->ALU.op = 2; // ADD operation for immediate instructions
            break;
        case 0x09: // ADDIU
            controlOut->ALUsrc = 1;
            controlOut->regWrite = 1;
            controlOut->ALU.op = 2; // Same as ADDI, treating as ADD
            break;
        case 0x04: // BEQ
            controlOut->branch = 1;
            controlOut->ALU.op = 2; 
            controlOut->ALU.bNegate = 1; // To subtract and compare for equality
            break;
        case 0x0A: // SLTI
            controlOut->ALUsrc = 1;
            controlOut->regWrite = 1;
            controlOut->ALU.op = 3; // Using SLT operation with immediate
            break;
        case 0x02: // J
            controlOut->jump = 1;
            break;
        case 0x23: // LW
            controlOut->memRead = 1;
            controlOut->memToReg = 1;
            controlOut->regWrite = 1;
            controlOut->ALUsrc = 1;
            controlOut->ALU.op = 2; // ADD for address calculation
            break;
        case 0x2B: // SW
            controlOut->memWrite = 1;
            controlOut->ALUsrc = 1;
            controlOut->ALU.op = 2; // ADD for address calculation
            break;
        default:
            return 0; // Unrecognized opcode
    }

    return 1; // Successfully recognized and processed instruction
}

void extract_instructionFields(WORD instruction, InstructionFields *fieldsOut) {
    // Extract fields from the instruction
    fieldsOut->opcode  = (instruction >> 26) & 0x3F; // Top 6 bits
    fieldsOut->rs      = (instruction >> 21) & 0x1F; // Next 5 bits
    fieldsOut->rt      = (instruction >> 16) & 0x1F; // Next 5 bits
    fieldsOut->rd      = (instruction >> 11) & 0x1F; // Next 5 bits
    fieldsOut->shamt   = (instruction >> 6) & 0x1F;  // Next 5 bits
    fieldsOut->funct   = instruction & 0x3F;         // Bottom 6 bits
    fieldsOut->imm16   = instruction & 0xFFFF;       // Bottom 16 bits
    fieldsOut->imm32   = signExtend16to32(fieldsOut->imm16); // Sign-extend imm16
    fieldsOut->address = instruction & 0x3FFFFFF;    // Bottom 26 bits for J type
}
