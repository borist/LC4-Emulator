#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lc4.h"

signed short *get_reg(unsigned short in) {
	switch(in) { 
	case 0: return &R0;	break;
	case 1: return &R1;	break;
	case 2: return &R2;	break;
	case 3: return &R3;	break;
	case 4: return &R4;	break;
	case 5: return &R5;	break;
	case 6: return &R6;	break;
	case 7: return &R7;	break;
	default: 		break; }
	fprintf(stderr, "illegal register access : %d\n", in);
	exit(1);
}

void check(unsigned short in) {
	if (in >= 0x7FFF && in < 0xA000 && MSB != 0x1) {
		fprintf(stderr, "Tried to access OS code at 0x%04x without privilege!\n", in);
		exit(1);
	}

	else if ((in > 0x1FFF && in < 0x7FFF) || in >= 0xA000) {
		fprintf(stderr, "Illegal Access to memory at 0x%04x\n", in);
		exit(1);
		} 

}

void nzp(signed short in) {
	if (in < 0) 
		PSR = (PSR & 0x8000) | 0x4;
	else if (in == 0)
		PSR = (PSR & 0x8000) | 0x2;
	else if (in > 0)
		PSR = (PSR & 0x8001) | 0x1; 
}

void nzpu(unsigned short in) {
	if (in == 0)
		PSR = (PSR & 0x8000) | 0x2;
	else if (in > 0)
		PSR = (PSR & 0x8001) | 0x1; 
}

void nop() {
	op_code = MEM[PC];
	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	PC++;
}

void brn() {
	op_code = MEM[PC];
	signed short IMM9 = SEXT_9.x = (op_code & 0x1FF);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);

	if (NZP == 0x4)
		PC = PC + 1 + IMM9;
	else
		PC++;
	check(PC);
}

void brnz() {
	op_code = MEM[PC];
	signed short IMM9 = SEXT_9.x = (op_code & 0x1FF);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);

	if (NZP == 0x4 | NZP == 0x2)
		PC = PC + 1 + IMM9;
	else
		PC++;
	check(PC);
}

void brnp() {
	op_code = MEM[PC];
	signed short IMM9 = SEXT_9.x = (op_code & 0x1FF);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);

	if (NZP == 0x4 | NZP == 0x1)
		PC = PC + 1 + IMM9;
	else
		PC++;
	check(PC);
}

void brz() {
	op_code = MEM[PC];
	signed short IMM9 = SEXT_9.x = (op_code & 0x1FF);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);

	if(NZP == 0x2)
		PC = PC + 1 + IMM9;
	else
		PC++;
	check(PC);
}

void brzp() {
	op_code = MEM[PC];
	signed short IMM9 = SEXT_9.x = (op_code & 0x1FF);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);

	if (NZP == 0x2 | NZP == 0x1)
		PC = PC + 1 + IMM9;
	else
		PC++;
	check(PC);
}

void brp() {
	op_code = MEM[PC];
	signed short IMM9 = SEXT_9.x = (op_code & 0x1FF);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);

	if (NZP == 0x1)
		PC = PC + 1 + IMM9;
	else
		PC++;
	check(PC);
}

void brnzp() {
	op_code = MEM[PC];
	signed short IMM9 = SEXT_9.x = (op_code & 0x1FF);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);

	PC = PC + 1 + IMM9;
	check(PC);
}

void add() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	Rt = O_3_0(op_code);

	*get_reg(Rd) = (*get_reg(Rs)) + (*get_reg(Rt));

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;	
}

void mul() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	Rt = O_3_0(op_code);

	*get_reg(Rd) = (*get_reg(Rs)) * (*get_reg(Rt));

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void sub() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	Rt = O_3_0(op_code);

	*get_reg(Rd) = (*get_reg(Rs)) - (*get_reg(Rt));

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void divi() { 
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	Rt = O_3_0(op_code);
	if (*get_reg(Rt) < 0)
		*get_reg(Rd) = 0;
	else
		*get_reg(Rd) = ((unsigned int)*get_reg(Rs)) / ((unsigned int)*get_reg(Rt));

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void add_im() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	signed short IMM5 = SEXT_5.x = (op_code & 0x1F);//& with 0000000000011111
	
	*get_reg(Rd) = (*get_reg(Rs)) + IMM5;

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void cmp() {
	op_code = MEM[PC];
	Rs = O_11_9(op_code);
	Rt = O_3_0(op_code);

	nzp(*get_reg(Rs) - *get_reg(Rt));

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	PC++;
}

void cmpu() {
	op_code = MEM[PC];
	Rs = O_11_9(op_code);
	Rt = O_3_0(op_code);

	nzpu((unsigned short) (*get_reg(Rs) - *get_reg(Rt)));

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	PC++;
}

void cmpi() {
	op_code = MEM[PC];
	Rs = O_11_9(op_code);
	signed short IMM7 = SEXT_7.x = (op_code & 0x7F);

	nzp(*get_reg(Rs) - IMM7);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	PC++;
}

void cmpiu() {
	op_code = MEM[PC];
	Rs = O_11_9(op_code);
	unsigned short UIMM7 = SEXT_U7.x = (op_code & 0x7F);

	nzpu((unsigned short) (*get_reg(Rs) - UIMM7));
	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	PC++;
}

void jsr() {
	op_code = MEM[PC];
	signed short IMM11 = SEXT_11.x = (op_code & 0x7FF);

	
	R7 = PC + 1;
	nzp((unsigned short) R7);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	PC = (PC & 0x8000) | (IMM11 << 4);
	check(PC);
}

void jsrr() {
	op_code = MEM[PC];
	Rs = O_8_6(op_code);
	
	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	unsigned short retAdr = (unsigned short) *get_reg(Rs);

	R7 = PC + 1;
	nzpu((unsigned short) R7);
	PC = retAdr;
}

void and() {	
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	Rt = O_3_0(op_code);

	*get_reg(Rd) = (*get_reg(Rs)) & (*get_reg(Rt));

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void not() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);

	*get_reg(Rd) = ~(*get_reg(Rs));

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void or() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	Rt = O_3_0(op_code);

	*get_reg(Rd) = (*get_reg(Rs)) | (*get_reg(Rt));

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);	
	nzp(*get_reg(Rd));
	PC++;
}

void xor() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	Rt = O_3_0(op_code);

	*get_reg(Rd) = (*get_reg(Rs)) ^ (*get_reg(Rt));
	
	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void and_im() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	signed short IMM5 = SEXT_5.x = (op_code & 0x1F);

	*get_reg(Rd) = (*get_reg(Rs)) & IMM5;

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void cons() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	signed short IMM9 = SEXT_9.x = (op_code & 0x1FF);

	*get_reg(Rd) = IMM9;
	nzp(*get_reg(Rd));
	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile); 
	PC++;
}

void sll() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	unsigned short UIMM4 = SEXT_U4.x = (op_code & 0xF);

	*get_reg(Rd) = *get_reg(Rs) << UIMM4;
	

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void sra() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	unsigned short UIMM4 = SEXT_U4.x = (op_code & 0xF);

	*get_reg(Rd) = (signed short) (*get_reg(Rs) >> UIMM4);
	

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void srl() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	unsigned short UIMM4 = SEXT_U4.x = (op_code & 0xF);

	*get_reg(Rd) = (unsigned short) (*get_reg(Rs) >> UIMM4);
	
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void mod() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	Rt = O_3_0(op_code);

	*get_reg(Rd) = ((unsigned int)*get_reg(Rs)) % ((unsigned int)*get_reg(Rt));
	nzp(*get_reg(Rd));

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	PC++; 
}

void ldr() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	signed short IMM6 = SEXT_6.x = (op_code & 0x3F);
	unsigned short op = *get_reg(Rs) + IMM6;

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	
	if (op >= 0xA000 && MSB != 0x1) {
		fprintf(stderr, "Tried to access OS code at 0x%04x without privilege\n!", op);
		exit(1);
	}

	else if (op < 0x2000 || (op >= 0x7FFF && op < 0xA000)) {
		fprintf(stderr, "Illegal Access to memory at 0x%04x\n", op);
		exit(1);
		} 

	*get_reg(Rd) = MEM[op];
	nzp(*get_reg(Rd));
	PC++;
}

void str() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	Rs = O_8_6(op_code);
	signed short IMM6 = SEXT_6.x = (op_code & 0x3F);
	unsigned short op = *get_reg(Rs) + IMM6;

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);

	if (op >= 0xA000 && MSB != 0x1) {
		fprintf(stderr, "Tried to access OS code at 0x%04x without privilege!\n", op);
		exit(1);
	}

	else if (op < 0x2000 || (op >= 0x7FFF && op < 0xA000)) {
		fprintf(stderr, "Illegal Access to memory at 0x%04x\n", op);
		exit(1);
		} 


	MEM[op] = *get_reg(Rd);
	nzp(*get_reg(Rd));
	PC++;
}

void rti() {
	op_code = MEM[PC];	
	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	
	PC = (unsigned short) R7;
	PSR = PSR & ~(1 << 15);
}

void jmpr() {
	op_code = MEM[PC];
	Rs = O_8_6(op_code);
	
	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	PC = (unsigned short) *get_reg(Rs);
}

void jmp() {
	op_code = MEM[PC];
	signed short IMM11 = SEXT_11.x = (op_code & 0x7FF);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	PC = PC + 1 + IMM11;
	check(PC);
}

void hiconst() {
	op_code = MEM[PC];
	Rd = O_11_9(op_code);
	unsigned short UIMM8 = (op_code & 0xFF);
	
	*get_reg(Rd) = (*get_reg(Rd) & 0xFF) | (UIMM8 << 8);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);
	nzp(*get_reg(Rd));
	PC++;
}

void trap() {
	op_code = MEM[PC];
	unsigned short UIMM8 = SEXT_U8.x = (op_code & 0xFF);

	fwrite(&PC, 2, 1, outFile);
	fwrite(&op_code, 2, 1, outFile);

	R7 = PC + 1;
	nzpu((unsigned short) (R7));
	PC = (0x8000 | UIMM8);
	PSR = PSR | 1 << 15;

}

