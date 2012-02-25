/*
 * trace.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "lc4.h"

unsigned short MEM[MEMSLOTS];
unsigned int PC;
unsigned int endPC;
unsigned short PSR;
signed short R0, R1, R2, R3, R4, R5, R6, R7;
unsigned short Rd, Rs, Rt;
unsigned short op_code;
FILE *outFile;
char end = 0;

//reset/initialize the state of the simulator
void reset() {
	int i;
	end = 0;
	PSR = 0x2;
	PC = 0x0;
	R0 = R1 = R2 = R3 = R4 = R5 = R6 = R7 = 0;
	for (i = 0; i<MEMSLOTS; i++)
		MEM[i] = 0;
}

//function to load contents of file into memory
void load_into_mem(FILE *inFile) {
	int num, i;
	unsigned short x, addr, n, instr, line, index;
	
	while (!feof(inFile)) {	//ADD ERROR CHECKS! ILLEGAL MEM ACCESS, ETC!
		fread(&x, 2, 1, inFile);
		if (feof(inFile)) break;
		x = htons(x);

		//code and data headers
		if (x == 0xCADE || x == 0xDADA) {
			fread(&addr, 2, 1, inFile);
			fread(&n, 2, 1, inFile);
			addr = htons(addr);
			n = htons(n);
		
			for(i = 0; i < n; i++) {
				fread(&instr, 2, 1, inFile);
				instr = htons(instr);
				MEM[addr+i] = instr;
			}  
		}

		//symbol header
		else if (x == 0xC3B7) {
			fread(&addr, 2, 1, inFile);
			fread(&n, 2, 1, inFile);
			addr = htons(addr);
			n = htons(n);
			for(i = 0; i < n; i++) {
				fread(&instr, 1, 1, inFile);
			}
		}

		//file name header
		else if (x == 0xF17E) { 
			fread(&n, 2, 1, inFile);
			n = htons(n);
			for (i = 0; i < n; i++) {
				fread(&instr, 1, 1, inFile);
			}
		}

		//line number header
		else if (x == 0x715E) { 
			fread(&addr, 2, 1, inFile);
			fread(&line, 2, 1, inFile);
			fread(&index, 2, 1, inFile);
			addr = htons(addr);
			line = htons(line);
			index = htons(index);

		}

		else {
			fprintf(stderr, "Unrecognized format in section: %04x\n", x);
		}

	} 
	
	fclose (inFile);

}

	//execute method
	void execute() {
		unsigned short full;
		unsigned short op_code;

		while (!end) { 
			full = MEM[PC];
			op_code = OP(full);
			if (PC == endPC)
				end = 1;
	
			switch(op_code) {
				//branches
				case 0x0:
					if (O_11_9(full) == 0x0) nop();
					else if (O_11_9(full) == 0x4) brn();
					else if (O_11_9(full) == 0x6) brnz();
					else if (O_11_9(full) == 0x5) brnp();
					else if (O_11_9(full) == 0x2) brz();
					else if (O_11_9(full) == 0x3) brzp();
					else if (O_11_9(full) == 0x1) brp();
					else if (O_11_9(full) == 0x7) brnzp();
					break;

				//arithmetic
				case 0x1:
					if (O_5_3(full) == 0x0)	add();
					else if (O_5_3(full) == 0x1) mul();
					else if (O_5_3(full) == 0x2) sub();
					else if (O_5_3(full) == 0x3) divi();
					else if (((full >> 5) & 0x1) == 0x1) add_im();
					break;

				//compares
				case 0x2:
					if (comSUB(full) == 0x0) cmp();
					else if (comSUB(full) == 0x1) cmpu();
					else if (comSUB(full) == 0x2) cmpi();
					else if (comSUB(full) == 0x3) cmpiu();
					break;

				//jsr
				case 0x4:
					if (jsrSUB(full) == 0x1) jsr();
					else if (jsrSUB(full) == 0x0) jsrr();
					break;

				//logic
				case 0x5:
					if (O_5_3(full) == 0x0)	and();
					else if (O_5_3(full) == 0x1) not();
					else if (O_5_3(full) == 0x2) or();
					else if (O_5_3(full) == 0x3) xor();
					else if (((full >> 5) & 0x1) == 0x1) and_im();
					break;

				//ldr
				case 0x6: ldr();	break;

				//str
				case 0x7: str();	break;

				//rti
				case 0x8: rti();	break;

				//const
				case 0x9: cons();	break;

				//shifts
				case 0xA:
					if (sftSUB(full) == 0x0) sll();
					else if (sftSUB(full) == 0x1) sra();
					else if (sftSUB(full) == 0x2) srl();
					else if (sftSUB(full) == 0x3) mod();		
					break;	
		
				//jmp
				case 0xC:
					if (jsrSUB(full) == 0x0) jmpr();
					else if (jsrSUB(full) == 0x1) jmp();
					break;

				//hiconst
				case 0xD: hiconst();	break;

				//trap
				case 0xF: trap();	break;
				default:		break;
			}	
		}
	}

	//main method
	int main(int argc, char **argv) {
		FILE  *inFile;
		int i = 3;
	
		inFile = fopen (argv[3], "rb");
		outFile = fopen (argv[1], "wb");

		if (!inFile || !outFile) { //fix these up!
			if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL)
				fprintf (stderr, "Please enter an ending PC and an output file followed by .obj files as inputs\n");
			else if (!inFile)
				fprintf (stderr, "%s was not found\n", argv[3]);	

			exit(1);	
		}
	
		//get last_PC_to_execute
		if (sscanf (argv[2], "%d", &endPC) != 1)
			fprintf(stderr, "%d is not a valid positive integer as an ending PC\n", endPC);

		do {
		reset();
		load_into_mem(inFile);
		execute();
		i++;
		inFile = fopen (argv[i], "rb");
		} while(inFile != NULL);

		fclose(outFile);	
		return 0;
}
