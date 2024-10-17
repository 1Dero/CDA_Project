#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    // printf("ALUControl = %d\n", ALUControl);
    if(ALUControl == 0b000) { // This uses xor on ALUControl and a binary number "000" (0b is a prefix to a binary representation)
        // 000 
        *ALUresult = A + B;
    }
    else if(ALUControl == 0b001) {
        // 001
        *ALUresult = A - B;
        // printf("Subtraction result: %d - %d = %d\n", A, B, *ALUresult);
    }
    else if(ALUControl == 0b010) {
        // 010
        // printf("slt\n");
        *ALUresult = ((int)A < (int)B)? 1 : 0;
    }
    else if(ALUControl == 0b011) {
        // 011
        // printf("sltu\n");
        *ALUresult = (A < B)? 1 : 0;
    }
    else if(ALUControl == 0b100) {
        // 100
        *ALUresult = A & B;
    }
    else if(ALUControl == 0b101) {
        // 101 
        *ALUresult = A | B;
    }
    else if(ALUControl == 0b110) {
        // 110
        *ALUresult = B << 16;
    }
    else if(ALUControl == 0b111) {
        // 111
        *ALUresult = ~A;
    }

    // Makes Zero 1 if ALUresult is zero, else makes it 0.
    if(*ALUresult == 0) *Zero = 1;
    else *Zero = 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Halt Condition: PC counter does not represent a valid address in Mem
    if(PC % 4 != 0) return 1;

    *instruction = Mem[PC >> 2];
    return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    *op = (instruction >> 26) & (0b111111); // [31-26]
    *r1 = (instruction >> 21) & (0b11111); // [25-21]
    *r2 = (instruction >> 16) & (0b11111); // [20-16]
    *r3 = (instruction >> 11) & (0b11111); // [15-11]
    *funct = instruction & (0b111111); // [5-0]
    *offset = instruction & (0b1111111111111111); // [15-0]
    *jsec = instruction & (0b11111111111111111111111111); // [25-0]
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    //printf("%d\n", op);
    if(op == 0b000000) {
        // R-type
        // printf("R-type Instruction\n");
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0b111; // R-type instruction
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 1;
        return 0;
    }
    else if(op == 0b000010) {
        // Jump
        // printf("Jump Instruction\n");
        controls->RegDst = 2;
        controls->Jump = 1;
        controls->Branch = 2;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->ALUOp = 0b000; // don't care
        controls->MemWrite = 0;
        controls->ALUSrc = 2;
        controls->RegWrite = 0;
        return 0;
    }
    else {
        // printf("I-type Instruction: ");
        // I-type
        if(op == 0b000100) {
            // beq
            // printf("beq\n");
            controls->RegDst = 2;
            controls->Jump = 0;
            controls->Branch = 1;
            controls->MemRead = 0;
            controls->MemtoReg = 2;
            controls->ALUOp = 0b001; // substraction to test for equality
            controls->MemWrite = 0;
            controls->ALUSrc = 2;
            controls->RegWrite = 0;
            return 0;
        }
        else if(op == 0b100011) {
            // lw
            // printf("lw\n");
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUOp = 0b000; // addition
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            return 0;
        }
        else if(op == 0b101011) {
            // sw
            // printf("sw\n");
            controls->RegDst = 2;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 2;
            controls->ALUOp = 0b000; // addition
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
            controls->RegWrite = 0;
            return 0;
        }
        else if(((op >> 3) == 0b001)) {
            op = op & 0b000111;
            // immediate operation
            if(op == 0b111) {
                // lui
                // printf("lui\n");
                controls->RegDst = 0;
                controls->Jump = 0;
                controls->Branch = 0;
                controls->MemRead = 0;
                controls->MemtoReg = 0;
                controls->ALUOp = 0b110; // shift left 16 bits
                controls->MemWrite = 0;
                controls->ALUSrc = 1;
                controls->RegWrite = 1;
            }
            else {
                // arithmetic
                controls->RegDst = 0;
                controls->Jump = 0;
                controls->Branch = 0;
                controls->MemRead = 0;
                controls->MemtoReg = 0;

                if(op == 0b000) {
                    // addi
                    // printf("addi\n");
                    controls->ALUOp = 0b000; // addition 
                }
                else if(op == 0b010) {
                    // slti
                    // printf("slti\n");
                    controls->ALUOp = 0b010; // set less than
                }
                else if(op == 0b011) {
                    // sltiu
                    // printf("sltiu\n");
                    controls->ALUOp = 0b011; // set less than unsigned
                }
                else {
                    return 1;
                }
                

                controls->MemWrite = 0;
                controls->ALUSrc = 1;
                controls->RegWrite = 1;
            }
            return 0;
        }
    }
    return 1;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    if((offset >> 15) == 1) {
        *extended_value = 0xffff0000 | offset;
    }
    else {
        *extended_value = 0x0000ffff & offset;
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    if(ALUSrc == 1) {
        // immediate field used
        ALU(data1, extended_value, ALUOp, ALUresult, Zero);
    }
    else {
        // no immediate
        if(ALUOp == 0b111) {
            // R-type instruction
            // printf("R-type: ");
            if(funct == 0b100000) {
                // add
                // printf("add\n");
                ALU(data1, data2, 0b000, ALUresult, Zero);
            }
            else if(funct == 0b100010) {
                // sub
                // printf("sub\n");
                ALU(data1, data2, 0b001, ALUresult, Zero);
            }
            else if(funct == 0b101010) {
                // slt
                // printf("slt ");
                ALU(data1, data2, 0b010, ALUresult, Zero);
                // printf("%d\n", *ALUresult);
            }
            else if(funct == 0b101011) {
                // sltu
                // printf("sltu\n");
                ALU(data1, data2, 0b011, ALUresult, Zero);
            }
            else if(funct == 0b100100) {
                // and
                // printf("and\n");
                ALU(data1, data2, 0b100, ALUresult, Zero);
            }
            else if(funct == 0b100101) {
                // or
                // printf("or\n");
                ALU(data1, data2, 0b101, ALUresult, Zero);
            }
            else if(funct == 0b000000) {
                // shift left 16 bits
                // printf("shift\n");
                ALU(data1, data2, 0b110, ALUresult, Zero);
            }
            else if(funct == 0b100111) {
                // not
                // printf("not\n");
                ALU(data1, data2, 0b111, ALUresult, Zero);
            }
            else {
                return 1;
            }
        }
        else {
            // printf("ALUop = %d\n", ALUOp);
            ALU(data1, data2, ALUOp, ALUresult, Zero);
        }
    }
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if(MemRead == 1) {
        // lw
        if(ALUresult % 4 != 0 || ALUresult >= 65536) return 1;
        *memdata = Mem[ALUresult >> 2];
    }
    else if(MemWrite == 1) {
        // sw
        if(ALUresult % 4 != 0 || ALUresult >= 65536) return 1;
        Mem[ALUresult >> 2] = data2;
    }
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if(RegWrite == 0) return;

    if(RegDst == 0) {
        // Use r2
        if(MemtoReg == 0) {
            // Use ALUresult
            Reg[r2] = ALUresult;
        }
        else {
            // Use memdata
            Reg[r2] = memdata;
        }
    }
    else if(RegDst == 1) {
        // Use r3
        if(MemtoReg == 0) {
            // Use ALUresult
            Reg[r3] = ALUresult;
        }
        else {
            // Use memdata
            Reg[r3] = memdata;
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    *PC += 4;
    // printf("Zero = %d\n", Zero);
    if(Branch == 1 && Zero == 1) {
        *PC += (extended_value << 2);
    }
    if(Jump == 1) {
        *PC = (*PC & 0xf0000000) | (jsec << 2);
    }
}

