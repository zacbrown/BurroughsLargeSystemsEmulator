// codes.h

const int OPCODENOTE_NOREG = 1;
const int OPCODENOTE_CONDCODE = 2;
const int OPCODENOTE_FAKECODE = 4;
const int OPCODENOTE_IOCOMMAND = 8;
const int OPCODENOTE_FLAG = 16;
const int OPCODENOTE_SPECREG = 32;
const int OPCODENOTE_USESSSP = 64;
const int OPCODENOTE_FLOATO = 128;
const int OPCODENOTE_FLOATR = 256;

const int OP_HALT = 0;
const int OP_LOAD = 1;
const int OP_LOADH = 2;
const int OP_STORE = 3;
const int OP_INC = 4;
const int OP_DEC = 5;
const int OP_ADD = 6;
const int OP_SUB = 7;
const int OP_MUL = 8;
const int OP_DIV = 9;
const int OP_MOD = 10;
const int OP_RSUB = 11;
const int OP_RDIV = 12;
const int OP_RMOD = 13;
const int OP_AND = 14;
const int OP_OR = 15;
const int OP_XOR = 16;
const int OP_NOT = 17;
const int OP_SHL = 18;
const int OP_SHR = 19;
const int OP_COMP = 20;
const int OP_COMPZ = 21;
const int OP_TBIT = 22;
const int OP_SBIT = 23;
const int OP_CBIT = 24;
const int OP_JUMP = 25;
const int OP_JZER = 26;
const int OP_JPOS = 27;
const int OP_JNEG = 28;
const int OP_JCOND = 29;
const int OP_GETFL = 30;
const int OP_SETFL = 31;
const int OP_GETSR = 32;
const int OP_SETSR = 33;
const int OP_PUSH = 34;
const int OP_POP = 35;
const int OP_CALL = 36;
const int OP_RET = 37;
const int OP_LDCH = 38;
const int OP_STCH = 39;
const int OP_DOIO = 40;
const int OP_FAKEIT = 41;
const int OP_FLAGSJ = 42;
const int OP_WAIT = 43;
const int OP_PAUSE = 44;
const int OP_BREAK = 45;
const int OP_IRET = 46;
const int OP_SYSCALL = 47;
const int OP_ATAS = 48;
const int OP_PHLOAD = 49;
const int OP_PHSTORE = 50;
const int OP_VTRAN = 51;
const int OP_MOVE = 52;
const int OP_FADD = 53;
const int OP_FSUB = 54;
const int OP_FMUL = 55;
const int OP_FDIV = 56;
const int OP_FCOMP = 57;
const int OP_FIX = 58;
const int OP_FRND = 59;
const int OP_FLOAT = 60;
const int OP_FLOG = 61;
const int OP_FEXP = 62;
const int OP_FFO = 63;
const int OP_FLZ = 64;
const int OP_RAND = 65;
const int OP_OP66 = 66;
const int OP_OP67 = 67;
const int OP_OP68 = 68;
const int OP_OP69 = 69;
const int OP_OP70 = 70;
const int OP_OP71 = 71;
const int OP_OP72 = 72;
const int OP_OP73 = 73;
const int OP_OP74 = 74;
const int OP_OP75 = 75;
const int OP_OP76 = 76;
const int OP_OP77 = 77;
const int OP_OP78 = 78;
const int OP_OP79 = 79;
const int OP_OP80 = 80;
const int OP_OP81 = 81;
const int OP_OP82 = 82;
const int OP_OP83 = 83;
const int OP_OP84 = 84;
const int OP_OP85 = 85;
const int OP_OP86 = 86;
const int OP_OP87 = 87;
const int OP_OP88 = 88;
const int OP_OP89 = 89;
const int OP_OP90 = 90;
const int OP_OP91 = 91;
const int OP_OP92 = 92;
const int OP_OP93 = 93;
const int OP_OP94 = 94;
const int OP_OP95 = 95;
const int OP_OP96 = 96;
const int OP_OP97 = 97;
const int OP_OP98 = 98;
const int OP_OP99 = 99;
const int OP_OP100 = 100;
const int OP_OP101 = 101;
const int OP_OP102 = 102;
const int OP_OP103 = 103;
const int OP_OP104 = 104;
const int OP_OP105 = 105;
const int OP_OP106 = 106;
const int OP_OP107 = 107;
const int OP_OP108 = 108;
const int OP_OP109 = 109;
const int OP_OP110 = 110;
const int OP_OP111 = 111;
const int OP_OP112 = 112;
const int OP_OP113 = 113;
const int OP_OP114 = 114;
const int OP_OP115 = 115;
const int OP_OP116 = 116;
const int OP_OP117 = 117;
const int OP_OP118 = 118;
const int OP_OP119 = 119;
const int OP_OP120 = 120;
const int OP_OP121 = 121;
const int OP_OP122 = 122;
const int OP_OP123 = 123;
const int OP_OP124 = 124;
const int OP_OP125 = 125;
const int OP_OP126 = 126;
const int OP_OP127 = 127;
const char* const instruction_name[] =
{"HALT", "LOAD", "LOADH", "STORE", "INC", "DEC",
    "ADD", "SUB", "MUL", "DIV", "MOD", "RSUB",
    "RDIV", "RMOD", "AND", "OR", "XOR", "NOT",
    "SHL", "SHR", "COMP", "COMPZ", "TBIT", "SBIT",
    "CBIT", "JUMP", "JZER", "JPOS", "JNEG", "JCOND",
    "GETFL", "SETFL", "GETSR", "SETSR", "PUSH", "POP",
    "CALL", "RET", "LDCH", "STCH", "DOIO", "FAKEIT",
    "FLAGSJ", "WAIT", "PAUSE", "BREAK", "IRET", "SYSCALL",
    "ATAS", "PHLOAD", "PHSTORE", "VTRAN", "MOVE", "FADD",
    "FSUB", "FMUL", "FDIV", "FCOMP", "FIX", "FRND",
    "FLOAT", "FLOG", "FEXP", "FFO", "FLZ", "RAND",
    "OP66", "OP67", "OP68", "OP69", "OP70", "OP71",
    "OP72", "OP73", "OP74", "OP75", "OP76", "OP77",
    "OP78", "OP79", "OP80", "OP81", "OP82", "OP83",
    "OP84", "OP85", "OP86", "OP87", "OP88", "OP89",
    "OP90", "OP91", "OP92", "OP93", "OP94", "OP95",
    "OP96", "OP97", "OP98", "OP99", "OP100", "OP101",
    "OP102", "OP103", "OP104", "OP105", "OP106", "OP107",
    "OP108", "OP109", "OP110", "OP111", "OP112", "OP113",
    "OP114", "OP115", "OP116", "OP117", "OP118", "OP119",
    "OP120", "OP121", "OP122", "OP123", "OP124", "OP125",
    "OP126", "OP127"};
const int instruction_notes[] =
{1, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 1, 0, 0, 0, 2,
    16, 16, 32, 32, 1, 1, 1, 1, 0, 0,
    0, 4, 0, 0, 0, 0, 65, 65, 0, 0,
    0, 0, 0, 384, 384, 384, 384, 384, 128, 384,
    256, 384, 384, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0};
const int num_opcodes = 128;

const int FAKE_PRINTCHAR = 0;
const int FAKE_PRINT4CHAR = 1;
const int FAKE_PRINTINT = 2;
const int FAKE_PRINTHEX = 3;
const int FAKE_PRINTSTR = 4;
const int FAKE_READCHAR = 5;
const int FAKE_PRINTFLOAT = 6;
const char* const fakecodes[] =
{"$PRINTCHAR", "$PRINT4CHAR", "$PRINTINT", "$PRINTHEX", "$PRINTSTR", "$READCHAR",
    "$PRINTFLOAT"};
const int num_fakecodes = 7;

const int IO_READDISC = 0;
const int IO_WRITEDISC = 1;
const int IO_SIZEDISC = 2;
const int IO_TERMINC = 3;
const int IO_TERMINW = 4;
const int IO_TERMOUTC = 5;
const int IO_TERMOUTW = 6;
const int IO_SECONDS = 7;
const int IO_DATETIME = 8;
const int IO_MTLOAD = 9;
const int IO_MTUNLOAD = 10;
const int IO_MTREAD = 11;
const int IO_MTWRITE = 12;
const char* const iocommands[] =
{"$READDISC", "$WRITEDISC", "$SIZEDISC", "$TERMINC", "$TERMINW", "$TERMOUTC",
    "$TERMOUTW", "$SECONDS", "$DATETIME", "$MTLOAD", "$MTUNLOAD", "$MTREAD",
    "$MTWRITE"};
const int num_iocommands = 13;

const int COND_EQL = 0;
const int COND_NEQ = 1;
const int COND_LSS = 2;
const int COND_LEQ = 3;
const int COND_GTR = 4;
const int COND_GEQ = 5;
const int COND_ERR = 6;
const char* const condcodes[] =
{"EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ",
    "ERR", "???", "???", "???", "???", "???",
    "???", "???", "???", "???"};
const int num_condcodes = 7;

const int FLAG_R = 0;
const int FLAG_Z = 1;
const int FLAG_N = 2;
const int FLAG_ERR = 3;
const int FLAG_SYS = 4;
const int FLAG_IP = 5;
const int FLAG_VM = 6;
const int FLAGMASK_R = 1;
const int FLAGMASK_Z = 2;
const int FLAGMASK_N = 4;
const int FLAGMASK_ERR = 8;
const int FLAGMASK_SYS = 16;
const int FLAGMASK_IP = 32;
const int FLAGMASK_VM = 64;
const char* const flags[] =
{"$R", "$Z", "$N", "$ERR", "$SYS", "$IP",
    "$VM"};
const int num_flags = 7;

const int SR_FLAGS = 0;
const int SR_PDBR = 1;
const int SR_INTVEC = 2;
const int SR_CGBR = 3;
const int SR_CGLEN = 4;
const int SR_DEBUG = 5;
const int SR_TIMER = 6;
const int SR_SYSSP = 7;
const int SR_SYSFP = 8;
const char* const specregs[] =
{"$FLAGS", "$PDBR", "$INTVEC", "$CGBR", "$CGLEN", "$DEBUG",
    "$TIMER", "$SYSSP", "$SYSFP", "???", "???", "???",
    "???", "???", "???", "???"};
const int num_specregs = 9;

const int PAGE_RES = 1;
const int PAGE_SYS = 2;
const char* const pageprots[] =
{"PAGE$RES", "PAGE$SYS"};
const int num_pageprots = 2;

const char* const short_ints[] =
{"IV$NONE", "IV$MEMORY", "IV$PAGEFAULT", "IV$UNIMPOP", "IV$HALT", "IV$DIVZERO",
    "IV$UNWROP", "IV$TIMER", "IV$PRIVOP", "IV$KEYBD", "IV$BADCALL", "IV$PAGEPRIV",
    "IV$DEBUG", "IV$INTRFAULT"};
const int INTR_NONE = 0;
const int INTR_MEMORY = 1;
const int INTR_PAGEFAULT = 2;
const int INTR_UNIMPOP = 3;
const int INTR_HALT = 4;
const int INTR_DIVZERO = 5;
const int INTR_UNWROP = 6;
const int INTR_TIMER = 7;
const int INTR_PRIVOP = 8;
const int INTR_KEYBD = 9;
const int INTR_BADCALL = 10;
const int INTR_PAGEPRIV = 11;
const int INTR_DEBUG = 12;
const int INTR_INTRFAULT = 13;
const char* const interrupt_name[] =
{"NONE: no interrupt",
    "MEMORY: Physical memory access failed",
    "PAGEFAULT: Page fault",
    "UNIMPOP: Unimplemented operation code",
    "HALT: HALT instruction executed",
    "DIVZERO: Division by zero",
    "UNWROP: Unwritable instruction operand",
    "TIMER: Countdown timer reached zero",
    "PRIVOP: Privileged operation in user mode",
    "KEYBD: Keyboard character ready",
    "BADCALL: Bad SYSCALL index",
    "PAGEPRIV: User mode access to system mode page",
    "DEBUG: PC=$DEBUG trap",
    "INTRFAULT: Failure to process interrupt"};
const int num_interrupts = 14;

const char* const register_name[] =
{"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
    "R8", "R9", "R10", "R11", "R12", "SP", "FP", "PC"};
const int SP = 13, FP = 14, PC = 15;
