// this is everything.h

#ifndef __everything_included
#define __everything_included

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
//#include "compat.h"
#include "useful.h"

extern char * regname[], * condname[];

const int FL_RUN=0, FL_ZERO=1, FL_NEG=2, FL_INTR=3;

const int MFL_RUN=1<<FL_RUN, MFL_ZERO=1<<FL_ZERO, MFL_NEG=1<<FL_NEG, MFL_INTR=1<<FL_INTR;

const int R_PC=15, R_SP=14, R_FP=13;

const int OP_BAD=0, OP_LOAD=1, OP_LOADH=2, OP_STORE=3, OP_ZERO=4, OP_ADD=5, OP_SUB=6, OP_MUL=7, OP_DIV=8,
          OP_MOD=9, OP_RSUB=10, OP_RDIV=11, OP_RMOD=12, OP_INC=13, OP_DEC=14, OP_CMP=15, OP_RCMP=16,
          OP_CMPZ=17, OP_JUMP=18, OP_JCOND=19, OP_JCNDF=20, OP_PUSH=21, OP_POP=22, OP_PUSHA=23, OP_POPA=24,
          OP_CALL=25, OP_RET=26, OP_AND=27, OP_OR=28, OP_XOR=29, OP_NOT=30, OP_NEG=31, OP_LDFLS=32, OP_STFLS=33,
          OP_ANDTF=34, OP_STOREH=35, OP_GTBOF=36, OP_GTBFR=37, OP_PTBOF=38, OP_PTBFR=39,
          OP_SHL=40, OP_ASHL=41, OP_ROTL=42, OP_SHR=43, OP_ASHR=44, OP_ROTR=45, OP_MKSEL=46, OP_CMBSL=47,
          OP_MIN=48, OP_MAX=49,
          OP_OUTN=120, OP_OUTCH=121, OP_OUTS=122, OP_INN=123, OP_INCH=124, OP_OUT=125,
          OP_HALT=127;

const int OT_NOREG=1, OT_NOOPD=2, OT_COND=4, OT_DEST=8, OT_JUMP=16, OT_ISREG=32, OT_OPT=64;

#define opdefs { "BAD",    OP_BAD   , OT_OPT },  \
               { "LOAD",   OP_LOAD  , 0 },  \
               { "LOADH",  OP_LOADH , 0 },  \
               { "STOREH", OP_STOREH, OT_DEST },  \
               { "STORE",  OP_STORE , OT_DEST },  \
               { "ZERO",   OP_ZERO  , OT_NOREG | OT_DEST },  \
               { "ADD",    OP_ADD   , 0 },  \
               { "SUB",    OP_SUB   , 0 },  \
               { "MUL",    OP_MUL   , 0 },  \
               { "DIV",    OP_DIV   , 0 },  \
               { "MOD",    OP_MOD   , 0 },  \
               { "RSUB",   OP_RSUB  , 0 },  \
               { "RDIV",   OP_RDIV  , 0 },  \
               { "RMOD",   OP_RMOD  , 0 },  \
               { "INC",    OP_INC   , OT_NOREG | OT_DEST },  \
               { "DEC",    OP_DEC   , OT_NOREG | OT_DEST },  \
               { "CMP",    OP_CMP   , 0 },  \
               { "RCMP",   OP_RCMP  , 0 },  \
               { "CMPZ",   OP_CMPZ  , OT_NOREG },  \
               { "JUMP",   OP_JUMP  , OT_NOREG | OT_JUMP },  \
               { "JCOND",  OP_JCOND , OT_COND | OT_JUMP },  \
               { "JCNDF",  OP_JCNDF , OT_COND | OT_JUMP },  \
               { "PUSH",   OP_PUSH  , OT_NOREG },  \
               { "POP",    OP_POP   , OT_NOREG },  \
               { "PUSHA",  OP_PUSHA , OT_NOREG | OT_NOOPD },  \
               { "POPA",   OP_POPA  , OT_NOREG | OT_NOOPD },  \
               { "CALL",   OP_CALL  , OT_NOREG | OT_JUMP },  \
               { "RET",    OP_RET   , OT_NOREG | OT_NOOPD },  \
               { "AND",    OP_AND   , 0 },  \
               { "OR",     OP_OR    , 0 },  \
               { "XOR",    OP_XOR   , 0 },  \
               { "NOT",    OP_NOT   , OT_NOREG },  \
               { "NEG",    OP_NEG   , OT_NOREG },  \
               { "LDFLS",  OP_LDFLS , OT_NOREG },  \
               { "STFLS",  OP_STFLS , OT_NOREG | OT_DEST },  \
               { "ANDTF",  OP_ANDTF , 0 },  \
               { "GTBOF",  OP_GTBOF , 0 },  \
               { "PTBOF",  OP_PTBOF , OT_DEST },  \
               { "GTBFR",  OP_GTBFR , OT_DEST },  \
               { "PTBFR",  OP_PTBFR , OT_DEST },  \
               { "CMBSL",  OP_CMBSL,  0 },  \
               { "MKSEL",  OP_MKSEL,  0 },  \
               { "SHL",    OP_SHL   , 0 },    \
               { "ASHL",   OP_ASHL  , 0 },   \
               { "ROTL",   OP_ROTL  , 0 },   \
               { "SHR",    OP_SHR   , 0 },    \
               { "ASHR",   OP_ASHR  , 0 },   \
               { "ROTR",   OP_ROTR  , 0 },   \
               { "OUTN",   OP_OUTN  , OT_NOREG },  \
               { "OUTCH",  OP_OUTCH , OT_NOREG },  \
               { "OUTS",   OP_OUTS  , OT_NOREG },  \
               { "OUT",    OP_OUT   , OT_NOREG },  \
               { "IN",     OP_INN   , OT_NOREG | OT_DEST },  \
               { "INCH",   OP_INCH  , OT_NOREG | OT_DEST },  \
               { "HALT",   OP_HALT  , OT_NOREG | OT_OPT },  \
               { "MIN",    OP_MIN   , 0 },   \
               { "MAX",    OP_MAX   , 0 },   \
               { NULL,     0        , OT_NOREG | OT_NOOPD }

const int CN_Z=0, CN_NZ=1, CN_LT=2, CN_LEQ=3, CN_GT=4, CN_GEQ=5, CN_INTR=6;

#define dolldefs { "PC",   R_PC    , OT_ISREG },  \
                 { "SP",   R_SP    , OT_ISREG },  \
                 { "FP",   R_FP    , OT_ISREG },  \
                 { "Z",    CN_Z    , OT_COND },  \
                 { "NZ",   CN_NZ   , OT_COND },  \
                 { "EQ",   CN_Z    , 0 },  \
                 { "NE",   CN_NZ   , 0 },  \
                 { "LT",   CN_LT   , OT_COND },  \
                 { "LE",   CN_LEQ  , OT_COND },  \
                 { "GT",   CN_GT   , OT_COND },  \
                 { "GE" ,  CN_GEQ  , OT_COND },  \
                 { "POS",  CN_GEQ  , 0 },  \
                 { "NEG",  CN_LT   , 0 },  \
                 { "INTR", CN_INTR , OT_COND },  \
                 { NULL,   0       , 0 }

struct opcodeentry
{ char * name;
  int opcode;
  int kind; };

extern string opcodes[];
extern opcodeentry opdata[];
extern opcodeentry dollars[];

struct initblock
{ char optionkind[128];    // '-' = absent, 'n' = numeric, 's' = string
  int optionval[128];
  string optionstr[128];
  int errorcode;
  string inputext, outputext;
  string basename, inputname, outputname, copyname;
  char infiletype, outfiletype, copyfiletype; // '-'=unwanted, 'f'= FILE *, 'i' = int descriptor
  int infd, outfd;
  FILE * infile, * outfile, * copyfile;
  initblock();
  ~initblock();
  bool performinit(vector <string> & args); };

void init_opcodes();
int asslook(string opc);
void assemble_line(string line, int exefile, int pass);
void assemble(initblock & ib);
void compile(initblock & ib);
void assemble(initblock & ib);
void disassemble(initblock & ib);
char * disassemble(int instr);
void emulate(initblock & ib);
char * newfilename(const char * ext);

int findpos(char wanted, char * bigstring);

#endif
