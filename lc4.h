

#define MEMSLOTS 1<<16

#define USER_CODE 0x0000
#define USER_DATA 0x4000
#define OS_CODE 0x8000
#define OS_DATA 0xA000

#define OP(I) ((I) >> 12) 
#define O_3_0(I) ((I) & 0x7)
#define O_5_3(I) (((I) >> 3) & 0x7)
#define O_8_6(I) (((I) >> 6) & 0x7)
#define O_11_9(I) (((I) >> 9) & 0x7)
#define comSUB(I) (((I) >> 7) & 0x3)
#define jsrSUB(I) (((I) >> 11) & 0x1)
#define sftSUB(I) (((I) >> 4) & 0x3)
#define MSB (((PSR) >> 15) & 0x1)
#define NZP ((PSR) & 0x7) 
#define FLIP(I) (((I) << 8) | ((I) >> 8))

extern unsigned short MEM[MEMSLOTS];
extern unsigned int PC;
extern unsigned int endPC;
extern unsigned short PSR;
extern signed short R0, R1, R2, R3, R4, R5, R6, R7;
extern unsigned short Rd, Rs, Rt;
extern unsigned short op_code;
extern FILE *outFile;

struct { unsigned short x:4; } SEXT_U4;
struct { signed short x:5; }   SEXT_5;
struct { signed short x:6; }   SEXT_6;
struct { signed short x:7; }    SEXT_7;
struct { unsigned short x:7; } SEXT_U7;
struct { unsigned short x:8; } SEXT_U8;
struct { signed short x:9; }   SEXT_9;
struct { signed short x:11; }  SEXT_11;

void nzp(signed short);

void nop();

void brn();
void brnz();
void brnp();
void brz();
void brzp();
void brp();
void brnzp();

void add();
void sub();
void mul();
void divi();
void add_im();
void mod();

void cmp();
void cmpu();
void cmpi();
void cmpiu();

void jsr();
void jsrr();

void and();
void not();
void or();
void xor();
void and_im();

void ldr();
void str();

void rti();

void cons();
void sll();
void sra();
void srl();

void jmpr();
void jmp();

void hiconst();

void trap();
