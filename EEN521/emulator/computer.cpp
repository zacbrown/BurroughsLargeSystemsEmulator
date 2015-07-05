// computer.cpp

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>
#include "useful.h"
#include "codes.h"
#include "computer.h"

bool quiet=false;

decoded_instruction::decoded_instruction()
{ decode(0); }

decoded_instruction::decoded_instruction(int memval)
{ decode(memval); }

void decoded_instruction::decode(int memval)
{ whole_value = memval;
  opcode = (memval>>25) & 0x7F;
  indirect = (memval>>24) & 0x01;
  mainreg = (memval>>20) & 0x0F;
  indexreg = (memval>>16) & 0x0F;
  number = memval & 0xFFFF;
  notes = instruction_notes[opcode];
  if (number & 0x8000)
    number |= 0xFFFF0000; }

decoded_instruction::decoded_instruction(int op, int mr, int ind, int ir, int n)
{ opcode = op;
  mainreg = mr;
  indirect = ind;
  indexreg = ir;
  number = n;
  whole_value = ((opcode & 0x7F)<<25) |
                ((indirect & 0x01)<<24) | 
                ((mainreg & 0x0F)<<20) |
                ((indexreg & 0x0F)<<16) |
                (number & 0xFFFF); }

string decoded_instruction::printable()
{ string answer=instruction_name[opcode];
  int note = instruction_notes[opcode];
  answer+=' ';
  while (answer.length()<8)
    answer+=' ';
  if (!(note&OPCODENOTE_NOREG))
  { if (note&OPCODENOTE_CONDCODE)
      answer+=condcodes[mainreg];
    else
      answer+=register_name[mainreg];
    answer+=", "; }
  if (indirect)
    answer+="[";
  if (indexreg!=0)
  { answer+=register_name[indexreg];
    if (number>0)
      answer+='+'; }
  if (note&OPCODENOTE_FAKECODE)
  { if (number>=0 && number<num_fakecodes)
      answer+=fakecodes[number];
    else
      answer+="???"; }
  else if (note&OPCODENOTE_IOCOMMAND)
  { if (number>=0 && number<num_iocommands)
      answer+=iocommands[number];
    else
      answer+="???"; }
  else if (note&OPCODENOTE_SPECREG)
  { if (number>=0 && number<num_specregs)
      answer+=specregs[number];
    else
      answer+="???"; }
  else if (note&OPCODENOTE_FLAG)
  { if (number>=0 && number<num_flags)
      answer+=flags[number];
    else
      answer+="???"; }
  else if (number!=0 || indexreg==0)
    answer+=int_to_string(number);
  if (indirect)
    answer+=']';
  return answer; }

computer::computer()
{ for (int i=0; i<0x10000; i+=1)
    memory[i]=NULL;
  discs.push_back(0);
  discsize.push_back(0);
  reset(); }

computer::~computer()
{ for (int i=0; i<0x10000; i+=1)
    if (memory[i]!=NULL)
      delete[] memory[i]; }

bool kbd_rdy=false, control_c=false;
const int ci_max=300;
char ci_buffer[ci_max+2];
int ci_n=0, ci_b=0, ci_e=0;

void clear_console_input()
{ ci_n=0;
  ci_b=0;
  ci_e=0; }

bool console_ready()
{ return ci_n>0; }

char get_console_char()
{ if (ci_n<=0)
    return 0;
  int c=ci_buffer[ci_b];
  ci_b+=1;
  if (ci_b>=ci_max)
    ci_b=0;
  ci_n-=1;
  return c; }

void remove_console_char()
{ if (ci_n<=0)
    return;
  ci_e-=1;
  if (ci_e<0)
    ci_e=ci_max-1;
  ci_n-=1; }

void add_console_char(char c)
{ if (ci_n>=ci_max-2)
    return;
  ci_buffer[ci_e]=c;
  ci_e+=1;
  if (ci_e>=ci_max)
    ci_e=0;
  ci_n+=1; }

static int max_mem=0, first_free=0, num_discs=0;

void computer::reset()
{ for (int i=0; i<0x10000; i+=1)
  { if (memory[i]!=NULL)
      delete[] memory[i];
    memory[i]=NULL; }
  for (int i=0; i<14; i+=1)
    _registers[i]=0;
  stepping=false;
  runwild=false;
  flr=0; flz=0; fln=0; flerr=0; flsys=1; flip=1; flvm=0;
  sr_pdbr=0; sr_intvec=0; sr_cgbr=0; sr_cglen=0; sr_debug=0;
  sr_timer=0; sr_syssp=0; sr_sysfp=0;
  memory_in_use=0;
  interrupt_code=0;
  interrupt_address=0;
  interrupt_detail=0;
  effective_address=0;
  interrupted=0;
  oldinterrupt=0;
  reallyhalted=false;
  countdown=0;
  current_instruction.decode(0);
  clear_console_input(); }

int & computer::registers(int r)
{ if (flsys)
  { if (r==SP)
      return sr_syssp;
    if (r==FP)
      return sr_sysfp;
    return _registers[r]; }
  else
    return _registers[r]; }

int & computer::sysregisters(int r)
{ if (r==SP)
    return sr_syssp;
  if (r==FP)
    return sr_sysfp;
  return _registers[r]; }

int & computer::usrregisters(int r)
{ return _registers[r]; }

int computer::combine_flags()
{ int r=0;
  if (flr) r|=FLAGMASK_R;
  if (flz) r|=FLAGMASK_Z;
  if (fln) r|=FLAGMASK_N;
  if (flerr) r|=FLAGMASK_ERR;
  if (flsys) r|=FLAGMASK_SYS;
  if (flvm) r|=FLAGMASK_VM;
  if (flip) r|=FLAGMASK_IP;
  return r; }

void computer::decombine_flags(int f)
{ flr=0; if (f&FLAGMASK_R) flr=1;
  flz=0; if (f&FLAGMASK_Z) flz=1;
  fln=0; if (f&FLAGMASK_N) fln=1;
  flerr=0; if (f&FLAGMASK_ERR) flerr=1;
  flsys=0; if (f&FLAGMASK_SYS) flsys=1;
  flvm=0; if (f&FLAGMASK_VM) flvm=1;
  flip=0; if (f&FLAGMASK_IP) flip=1; }

int computer::get_flag(int f)
{ if (f==FLAG_R) return flr;
  if (f==FLAG_Z) return flz;
  if (f==FLAG_N) return fln;
  if (f==FLAG_ERR) return flerr;
  if (f==FLAG_SYS) return flsys;
  if (f==FLAG_VM) return flvm;
  if (f==FLAG_IP) return flip;
  return 0; }

bool computer::is_user_flagmask(int f)
{ if (f==FLAGMASK_Z) return true;
  if (f==FLAGMASK_N) return true;
  if (f==FLAGMASK_ERR) return true;
  return false; }

bool computer::is_user_flag(int f)
{ return is_user_flagmask(1<<f); }

void computer::set_flag(int f, int v)
{ if (f==FLAG_R) flr=v;
  else if (f==FLAG_Z) flz=v;
  else if (f==FLAG_N) fln=v;
  else if (f==FLAG_ERR) flerr=v;
  else if (f==FLAG_SYS) flsys=v;
  else if (f==FLAG_VM) flvm=v;
  else if (f==FLAG_IP) flip=v; }

const char * special_register_name(int i)
{ if (i<0 || i>=num_specregs)
    return "???";
  return specregs[i]; }

int computer::get_special_register(int i)
{ if (i==SR_FLAGS)
    return combine_flags();
  else if (i==SR_PDBR)
    return sr_pdbr;
  else if (i==SR_INTVEC)
    return sr_intvec;
  else if (i==SR_CGBR)
    return sr_cgbr;
  else if (i==SR_CGLEN)
    return sr_cglen;
  else if (i==SR_DEBUG)
    return sr_debug;
  else if (i==SR_TIMER)
    return sr_timer;
  else if (i==SR_SYSSP)
    return sr_syssp;
  else if (i==SR_SYSFP)
    return sr_sysfp;
  else
    return 0; }

void computer::set_special_register(int i, int v)
{ if (i==SR_FLAGS)
    decombine_flags(v);
  else if (i==SR_PDBR)
    sr_pdbr=v;
  else if (i==SR_INTVEC)
    sr_intvec=v;
  else if (i==SR_CGBR)
    sr_cgbr=v;
  else if (i==SR_CGLEN)
    sr_cglen=v;
  else if (i==SR_DEBUG)
    sr_debug=v;
  else if (i==SR_TIMER)
    sr_timer=v;
  else if (i==SR_SYSSP)
    sr_syssp=v;
  else if (i==SR_SYSFP)
    sr_sysfp=v; }

void computer::vm_show(int address)
{ printf("\n");
  if (!flvm)
    printf("  Virtual Memory is OFF\n");
  printf("  PDBR Page Directory Base Register = 0x%08X\n", sr_pdbr);
  fflush(stdout);
  if (sr_pdbr==0)
    return;
  int dir=(address>>22)&0x3FF;
  int pg=(address>>11)&0x7FF;
  int offs=address&0x7FF;
  printf("  Virtual Address 0x%08X = table %d, page %d, word %d\n", address, dir, pg, offs);
  int ptaddr=physical_memory_read(sr_pdbr+dir);
  printf("  [%08X+%d = %08X] = %08X = page table address\n", sr_pdbr, dir, sr_pdbr+dir, ptaddr);
  if (ptaddr==0)
  { printf("  No page table, translation can not proceed\n");
    fflush(stdout);
    return; }
  ptaddr&=0xFFFFF800;
  int pgaddr=physical_memory_read(ptaddr+pg);
  printf("  [%08X+%d = %08X] = %08X = page table entry\n", ptaddr, pg, ptaddr+pg, pgaddr);
  printf("  %08X = %06X00 + %03X :", pgaddr, (pgaddr&0xFFFFF800)>>8, pgaddr&0x7FF);
  if (pgaddr&PAGE_RES)
    printf(" RESIDENT");
  else
  { printf(" page NOT RESIDENT\n");
    fflush(stdout);
    return; }
  if (pgaddr&PAGE_SYS)
    printf(", SYSTEM mode access only\n");
  else
    printf(", USER and SYSTEM mode access\n");
  int physaddr = (pgaddr&0xFFFFF800)+offs;
  int content = physical_memory_read(physaddr);
  printf("  Physical Address = %08X, content = %08X = %d\n", physaddr, content, content);
  fflush(stdout); }

int computer::translate(int address)
{ if (!flvm)
    return address;
  int dir=(address>>22)&0x3FF;
  int pg=(address>>11)&0x7FF;
  int offs=address&0x7FF;
  int ptaddr=physical_memory_read(sr_pdbr+dir);
  if (ptaddr==0)
  { interrupt_code=INTR_PAGEFAULT;
    interrupt_address=address;
    return 0; }
  int pgaddr=physical_memory_read((ptaddr&0xFFFFF800)+pg);
  if ((pgaddr&PAGE_RES)==0)
  { interrupt_code=INTR_PAGEFAULT;
    interrupt_address=address;
    return 0; }
  else if ((pgaddr&PAGE_SYS)!=0 && !flsys)
  { interrupt_code=INTR_PAGEPRIV;
    interrupt_address=address;
    return 0; }
  return (pgaddr&0xFFFFF800)+offs; }

int computer::memory_read(int address)
{ if (flvm)
  { address=translate(address);
    if (interrupt_code!=0)
      return 0; }
  return physical_memory_read(address); }

void computer::memory_write(int address, int value)
{ if (flvm)
  { address=translate(address);
    if (interrupt_code!=0)
      return; }
  physical_memory_write(address, value); }

void computer::physical_memory_create(int baseaddress)
{ int blocknum = (baseaddress>>16) & 0xFFFF;
  if (memory_in_use>=maximum_memory)
  { fprintf(stderr, "You're being too greedy with memory!\n");
    interrupt_code=INTR_MEMORY;
    interrupt_address=baseaddress;
    return; }
  if (memory[blocknum]==NULL)
    memory[blocknum]=new int[0x10000]; }

int computer::physical_memory_read(int address)
{ int blocknum = (address>>16) & 0xFFFF, offset = address&0xFFFF;
  if (memory[blocknum]==NULL)
    return 0;
  return memory[blocknum][offset]; }

void computer::physical_memory_write(int address, int value)
{ int blocknum = (address>>16) & 0xFFFF, offset = address&0xFFFF;
  if (memory[blocknum]==NULL)
  { interrupt_code = INTR_MEMORY;
    interrupt_address = address;
    return; }
  memory[blocknum][offset] = value; }

void computer::fetch()
{ if (countdown>0)
  { countdown-=1;
    if (countdown==0 && runwild)
    { runwild=false;
      stepping=true;
      printf("\n"); } }
  interrupt_code=0;
  interrupt_address=0;
  int & pc = registers(PC);
  int addr=pc;
  if (addr==sr_debug && !flip)
  { interrupt_address=addr;
    interrupt_detail=0;
    prepare_interrupt(INTR_DEBUG);
    return; }
  int code = memory_read(addr);
  if (interrupt_code!=0)
  { prepare_interrupt(interrupt_code);
    return; }
  pc+=1;
  current_instruction.decode(code);
  effective_address = current_instruction.number;
  if (current_instruction.indexreg!=0)  
    effective_address += registers(current_instruction.indexreg);
  if (current_instruction.indirect)
    memory_read(effective_address);
  pc-=1;
  if (interrupt_code!=0)
    prepare_interrupt(interrupt_code); }

int computer::read_operand()
{ if (current_instruction.indirect)
  { int v = memory_read(effective_address);
    if (interrupt_code!=0)
      v=0;
    return v; }
  return effective_address; }

void computer::write_operand(int value)
{ if (!current_instruction.indirect)
  { if (current_instruction.number==0)
    { registers(current_instruction.indexreg)=value;
      return; }
    interrupt_code=INTR_UNWROP;
    interrupt_address=registers(PC)-1;
    return; }
  memory_write(effective_address, value); }

string computer::string_from_memory(int address, int maxlen, bool zeroterminated)
{ string s = "";
  for (int j=0; j<maxlen || maxlen==0; j+=1)
  { int v=memory_read(address+(j>>2));
    if (interrupt_code!=0)
      return "";
    int shft=(j&3)*8;
    char c=(v>>shft)&0xFF;
    if (c==0 && zeroterminated)
      break;
    s+=c; }
  return s; }

bool computer::get_parameter_block(int addr, int size, int params[])
{ for (int i=0; i<size; i+=1)
  { params[i]=memory_read(addr+i);
    if (interrupt_code!=0)
      return false; }
  return true; }

void computer::put_parameter(int addr, int which, int value)
{ memory_write(addr+which, value); }

int computer::perform_io_operation(int operation, int paramaddr)
{ if (!flsys)
  { interrupt_code=INTR_PRIVOP;
    return -1; }
  int params[16];
  int buffer[128];
  static int mtfile=-1;
  flerr=1;
  switch (operation)
  { case IO_READDISC:
    { bool ok=get_parameter_block(paramaddr, 4, params);
      if (!ok)
        return -2;
      int unit=params[0], numblk=params[1], blockpos=params[2]*512, address=params[3];
      if (unit<1 || unit>discs.size())
        return -3;
      int n = lseek(discs[unit], blockpos, SEEK_SET);
      if (n!=blockpos || numblk<0 || blockpos+numblk*512>discsize[unit])
        return -4;
      for (int b=0; b<numblk; b+=1)
      { n = read(discs[unit], buffer, 512);
        if (n==0)
          return b;
        if (n!=512)
          return -4;
        for (int i=0; i<128; i+=1)
        { memory_write(address+i, buffer[i]);
          if (interrupt_code!=0)
            return -2; }
        address+=128; }
      flerr=0;
      return numblk; }
    case IO_WRITEDISC:
    { bool ok=get_parameter_block(paramaddr, 4, params);
      if (!ok)
        return -2;
      int unit=params[0], numblk=params[1], blockpos=params[2]*512, address=params[3];
      if (unit<1 || unit>discs.size())
        return -3;
      int n = lseek(discs[unit], blockpos, SEEK_SET);
      if (n!=blockpos || numblk<0 || blockpos+numblk*512>discsize[unit])
        return -4;
      for (int b=0; b<numblk; b+=1)
      { for (int i=0; i<128; i+=1)
        { buffer[i] = memory_read(address+i);
          if (interrupt_code!=0)
            return -2; }
        n = write(discs[unit], buffer, 512);
        if (n==0)
          return b;
        if (n!=512)
          return -4;
        address+=128; }
      flerr=0;
      return numblk; }
    case IO_SIZEDISC:
    { bool ok=get_parameter_block(paramaddr, 2, params);
      if (!ok)
        return -2;
      int unit=params[0];
      if (unit<1 || unit>discs.size())
        return -3;
      int blks=discsize[unit]/512;
      put_parameter(paramaddr, 1, blks);
      flerr=0;
      return blks; }
    case IO_TERMINC:
    { bool ok=get_parameter_block(paramaddr, 2, params);
      if (!ok)
        return -2;
      int maxnum=params[0], address=params[1], num=0;
      while (num<maxnum)
      { int c=0;
        if (kbd_any_ready())
          c=read_keyboard_char();
        int i=num>>2, j=num&3;
        int kmask=~(0xFF<<(j*8));
        int v=memory_read(address+i)&kmask;
        v|=(c&0xFF)<<(j*8);
        memory_write(address+i, v);
        if (interrupt_code!=0)
          return -2;
        if (c==0) break;
        num+=1; }
      flerr=0;
      return num; }
    case IO_TERMINW:
    { bool ok=get_parameter_block(paramaddr, 2, params);
      if (!ok)
        return -2;
      int maxnum=params[0], address=params[1], num=0;
      while (num<maxnum)
      { if (!kbd_any_ready())
          break;
        memory_write(address+num, read_keyboard_char());
        if (interrupt_code!=0)
          return -2;
        num+=1; }
      flerr=0;
      return num; }
    case IO_TERMOUTC:
    { bool ok=get_parameter_block(paramaddr, 2, params);
      if (!ok)
        return -2;
      int maxnum=params[0], address=params[1];
      string s = string_from_memory(address, maxnum, maxnum==0);
      if (interrupt_code!=0)
        return -2;
      if (stepping)
        printf("OUTPUT: '%s'\n\n", visible(s).c_str());
      else
        printf("%s", s.c_str());
      fflush(stdout);
      flerr=0;
      if (maxnum==0)
        return s.length();
      return maxnum; }
    case IO_TERMOUTW:
    { bool ok=get_parameter_block(paramaddr, 2, params);
      if (!ok)
        return -2;
      int maxnum=params[0], address=params[1], actual;
      if (stepping)
        printf("OUTPUT: '");
      for (int i=0; i<maxnum || maxnum==0; i+=1)
      { int v=memory_read(address+i)&0xFF;
        if (v==0 && maxnum==0)
          break;
        if (interrupt_code!=0)
          return -2;
        if (stepping)
          printf("%s", visible(v).c_str());
        else
          putchar(v);
        actual+=1; }
      if (stepping)
        printf("'\n\n");
      fflush(stdout);
      flerr=0;
      return actual; }
    case IO_SECONDS:
    { time_t t = time(NULL);
      flerr=0;
      return t-946702800; }
    case IO_DATETIME:
    { bool ok=get_parameter_block(paramaddr, 8, params);
      if (!ok)
        return -2;
      time_t t = params[0]+946702800;
      flerr=0;
      tm *now=localtime(&t);
      put_parameter(paramaddr, 1, 1900+now->tm_year);
      put_parameter(paramaddr, 2, 1+now->tm_mon);
      put_parameter(paramaddr, 3, now->tm_mday);
      put_parameter(paramaddr, 4, now->tm_wday);
      put_parameter(paramaddr, 5, now->tm_hour);
      put_parameter(paramaddr, 6, now->tm_min);
      put_parameter(paramaddr, 7, now->tm_sec);
      return 1; }
    case IO_MTLOAD:
    { bool ok=get_parameter_block(paramaddr, 2, params);
      if (!ok)
        return -2;
      if (mtfile>=0)
      { close(mtfile);
        mtfile=-1; }
      int writable=params[0], address=params[1];
      string fname = string_from_memory(address, 500, true);
      if (interrupt_code!=0)
        return -2;
      if (writable)
        mtfile=open(fname.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0644);
      else
        mtfile=open(fname.c_str(), O_RDONLY | O_BINARY);
      if (stepping)
      { printf("MAGNETIC TAPE WITH LABEL '%s'", visible(fname).c_str());
        if (mtfile>=0)
        { printf(" LOADED SUCCESSFULLY, ");
          if (writable)
            printf("WRITE ONLY\n\n");
          else
            printf("READ ONLY\n\n"); }
        else
          printf(" NOT FOUND\n\n");
        fflush(stdout); }
      if (mtfile<0)
        return -5;
      flerr=0;
      return 1; }
    case IO_MTUNLOAD:
    { if (mtfile<0)
        return -3;
      close(mtfile);
      mtfile=-1;
      if (stepping)
      { printf("MAGNETIC TAPE SAFELY UNLOADED\n\n");
        fflush(stdout); }
      flerr=0;
      return 1; }
    case IO_MTREAD:
    { if (mtfile<0)
        return -3;
      bool ok=get_parameter_block(paramaddr, 2, params);
      if (!ok)
        return -2;
      int numblk=params[0], address=params[1];
      for (int b=0; b<numblk; b+=1)
      { int n = read(mtfile, buffer, 512);
        if (n==0)
          return b;
        else if (n<0)
          return -5;
        else if (n!=512)
        { char * bp = (char *)buffer;
          for (int i=n; i<512; i+=1)
            bp[i]=0; }
        for (int i=0; i<128; i+=1)
        { memory_write(address+i, buffer[i]);
          if (interrupt_code!=0)
            return -2; }
        address+=128; }
      flerr=0;
      return numblk; }
    case IO_MTWRITE:
    { if (mtfile<0)
        return -3;
      bool ok=get_parameter_block(paramaddr, 2, params);
      if (!ok)
        return -2;
      int numblk=params[0], address=params[1];
      for (int b=0; b<numblk; b+=1)
      { for (int i=0; i<128; i+=1)
        { buffer[i] = memory_read(address+i);
          if (interrupt_code!=0)
            return -2; }
        int n = write(mtfile, buffer, 512);
        if (n==0)
          return b;
        if (n!=512)
          return -5;
        address+=128; }
      flerr=0;
      return numblk; }
    default:
      return -1; } }

float decode_float(int n)
{ union { float fv; int iv; } converter;
  converter.iv=n;
  return converter.fv; }

int encode_float(float n)
{ union { float fv; int iv; } converter;
  converter.fv=(float)n;
  return converter.iv; }

void computer::fake_function_call(int fn, int regnum)
{ flerr=0;
  switch (fn)
  { case FAKE_PRINTCHAR:
    { if (stepping)
        printf("OUTPUT: '%s'\n\n", visible(registers(regnum)&0xFF).c_str());
      else
        printf("%c", registers(regnum)&0xFF);
      fflush(stdout);
      break; }
    case FAKE_PRINTINT:
    { if (stepping)
        printf("OUTPUT: '%d'\n\n", registers(regnum));
      else
        printf("%d", registers(regnum));
      fflush(stdout);
      break; }
    case FAKE_PRINTFLOAT:
    { if (stepping)
        printf("OUTPUT: '%e'\n\n", decode_float(registers(regnum)));
      else
        printf("%e", decode_float(registers(regnum)));
      fflush(stdout);
      break; }
    case FAKE_PRINTHEX:
    { if (stepping)
        printf("OUTPUT: '%08X'\n\n", registers(regnum));
      else
        printf("%08X", registers(regnum));
      fflush(stdout);
      break; }
    case FAKE_PRINT4CHAR:
    { int v=registers(regnum);
      if (stepping)
        printf("OUTPUT: '");
      for (int s=0; s<=24; s+=8)
      { char c=(v>>s)&0xFF;
        if (stepping)
          printf("%s", visible(c).c_str());
        else
          putchar(c); }
      if (stepping)
        printf("'\n\n");
      fflush(stdout);
      break; }
    case FAKE_PRINTSTR:
    { int str=registers(regnum);
      bool going=true;
      if (stepping)
        printf("OUTPUT: '");
      for (int i=0; going; i+=1)
      { int v=memory_read(str+i);
        if (interrupt_code!=0)
          break;
        for (int s=0; s<=24; s+=8)
        { char c=(v>>s)&0xFF;
          if (c==0)
          { going=false;
            break; }
          if (stepping)
            printf("%s", visible(c).c_str());
          else
            putchar(c); } }
      if (stepping)
        printf("'\n\n");
      fflush(stdout);
      break; }
    case FAKE_READCHAR:
    { if (!console_ready())
      { printf("USER INPUT REQUIRED: ");
        fflush(stdout);
        int linelen=0;
        while (1)
        { if (!kbd_rdy && !control_c)
          { sleep(1);
            continue; }
          if (control_c)
          { printf("[control-c]\n");
            return; }
          char c = read_keyboard_char();
          if (c=='\n')
          { putchar(c);
            add_console_char(c);
            break; }
          else if (c==27)
          { putchar('\n');
            break; }
          else if (c=='\b')
          { if (linelen>0)
            { printf("\b \b");
              linelen-=1;
              fflush(stdout); }
            remove_console_char(); }
          else if (c>=' ')
          { putchar(c);
            fflush(stdout);
            linelen+=1;
            add_console_char(c); } }
        printf("\n");
        fflush(stdout); }
      registers(regnum)=get_console_char();
      break; }
    default:
    { flerr=1; } } }

int execount=0;

void computer::printinterrupt(int ic, bool forceit)
{ if (!stepping && !forceit)
    return;
  if (ic<0 || ic>=num_interrupts)
    printf("MYSTERY INTERRUPT NUMBER %d\n\n", ic);
  else
    printf("INTERRUPT %s\n\n", interrupt_name[ic]);
  fflush(stdout); }

void computer::prepare_interrupt(int ic)
{ oldinterrupt=0;
  interrupt_code=0;
  printinterrupt(ic);
  if (ic==INTR_TIMER)
    sr_timer=0;
  if (sr_intvec==0)
  { oldinterrupt=ic;
    interrupted=INTR_INTRFAULT;
    interrupt_detail=ic;
    if (!stepping)
      printinterrupt(ic, true);
    printinterrupt(INTR_INTRFAULT, true);
    flr=0;
    return; }
  int dest=memory_read(sr_intvec+ic);
  for (int i=0; i<16 && interrupt_code==0; i+=1)
    memory_write(sr_syssp-i-1, registers(i));
  if (interrupt_code==0)
    memory_write(sr_syssp-17, interrupt_detail);
  if (interrupt_code==0)
    memory_write(sr_syssp-18, interrupt_address);
  if (interrupt_code==0)
    memory_write(sr_syssp-19, interrupt_code);
  if (interrupt_code==0)
    memory_write(sr_syssp-20, combine_flags());
  if (interrupt_code!=0 || dest==0)
  { interrupt_code=0;
    oldinterrupt=ic;
    interrupted=INTR_INTRFAULT;
    interrupt_detail=ic;
    printinterrupt(INTR_INTRFAULT);
    dest=memory_read(sr_intvec+INTR_INTRFAULT);
    if (interrupt_code!=0 || dest==0)
    { interrupt_code=0;
      if (!stepping)
        printinterrupt(ic, true);
      printinterrupt(INTR_INTRFAULT, true);
      flr=0;
      return; } }
  sr_syssp-=20;
  flsys=1;
  flip=1;
  _registers[PC]=dest; }

void computer::execute()
{ interrupted=0;
  int & pc = _registers[PC];
  if (sr_timer>1)
    sr_timer-=1;
  if (interrupt_code==0 && sr_timer==1)
    interrupt_code=INTR_TIMER;
  if (interrupt_code!=0)
  { if (flip)
    { if (interrupt_code!=INTR_TIMER && interrupt_code!=INTR_KEYBD)
      { stepping=false;
        runwild=false;
        printinterrupt(interrupt_code, true); } }
    else
    { prepare_interrupt(interrupt_code);
      return; } }
  int opcode = current_instruction.opcode;
  int mainregnum = current_instruction.mainreg;
  int & mainreg = registers(mainregnum);
  int operand = read_operand();
  if (interrupt_code!=0)
  { if (opcode!=OP_PHLOAD && opcode!=OP_PHSTORE)
    { prepare_interrupt(interrupt_code);
      return; } }
  pc+=1;
  execount+=1;
  switch (opcode)
  { case OP_HALT:
    { if (flsys)
      { flr=0;
        reallyhalted=true; }
      else
      { interrupt_code=INTR_HALT;
        interrupt_address=pc-1; }
      break; }
    case OP_LOAD:
    { mainreg=operand;
      break; }
    case OP_LOADH:
    { mainreg &= 0xFFFF;
      mainreg |= operand<<16;
      break; }
    case OP_STORE:
    { write_operand(mainreg);
      break; }
    case OP_INC:
    { write_operand(operand+1);
      break; }
    case OP_DEC:
    { write_operand(operand-1);
      break; }
    case OP_ADD:
    { mainreg+=operand;
      break; }
    case OP_SUB:
    { mainreg-=operand;
      break; }
    case OP_MUL:
    { mainreg*=operand;
      break; }
    case OP_DIV:
    { if (operand==0)
      { interrupt_code=INTR_DIVZERO;
        interrupt_address=pc-1;
        break; }
      mainreg/=operand;
      break; }
    case OP_MOD:
    { if (operand==0)
      { interrupt_code=INTR_DIVZERO;
        interrupt_address=pc-1;
        break; }
      mainreg%=operand;
      break; }
    case OP_RSUB:
    { mainreg=operand-mainreg;
      break; }
    case OP_RDIV:
    { if (mainreg==0)
      { interrupt_code=INTR_DIVZERO;
        interrupt_address=pc-1;
        break; }
      mainreg=operand/mainreg;
      break; }
    case OP_RMOD:
    { if (mainreg==0)
      { interrupt_code=INTR_DIVZERO;
        interrupt_address=pc-1;
        break; }
      mainreg=operand%mainreg;
      break; }
    case OP_AND:
    { mainreg&=operand;
      break; }
    case OP_OR:
    { mainreg|=operand;
      break; }
    case OP_XOR:
    { mainreg|=operand;
      break; }
    case OP_NOT:
    { mainreg=~operand;
      break; }
    case OP_SHL:
    { int bits=(mainreg>>(32-operand))&((1<<operand)-1);
      mainreg<<=operand;
      if (bits==0)
        flz=1;
      else
        flz=0;
      break; }
    case OP_SHR:
    { int bits=mainreg&((1<<operand)-1);
      unsigned int r=mainreg;
      mainreg=r>>operand;
      if (bits==0)
        flz=1;
      else
        flz=0;
      break; }
    case OP_COMP:
    { flz=0;
      fln=0;
      if (mainreg==operand)
        flz=1;
      else if (mainreg<operand)
        fln=1;
      break; }
    case OP_COMPZ:
    { flz=0;
      fln=0;
      if (operand==0)
        flz=1;
      else if (operand<0)
        fln=1;
      break; }
    case OP_TBIT:
    { int mask=1<<operand;
      if ((mainreg&mask)==0)
        flz=0;
      else
        flz=1;
      break; }
    case OP_SBIT:
    { int mask=1<<operand;
      mainreg|=mask;
      break; }
    case OP_CBIT:
    { int mask=1<<operand;
      mainreg&=~mask;
      break; }
    case OP_JUMP:
    { if (pc==operand)
        usleep(10000);
      pc=operand;
      break; }
    case OP_JZER:
    { if (mainreg==0)
      { if (pc==operand)
          usleep(10000);
        pc=operand; }
      break; }
    case OP_JPOS:
    { if (mainreg>=0)
      { if (pc==operand)
          usleep(10000);
        pc=operand; }
      break; }
    case OP_JNEG:
    { if (mainreg<0)
      { if (pc==operand)
          usleep(10000);
        pc=operand; }
      break; }
    case OP_JCOND:
    { if (mainregnum==COND_EQL)
      { if (flz)
          pc=operand; }
      else if (mainregnum==COND_NEQ)
      { if (!flz)
          pc=operand; }
      else if (mainregnum==COND_LSS)
      { if (fln)
          pc=operand; }
      else if (mainregnum==COND_LEQ)
      { if (flz || fln)
          pc=operand; }
      else if (mainregnum==COND_GTR)
      { if (!flz && !fln)
          pc=operand; }
      else if (mainregnum==COND_GEQ)
      { if (!fln)
          pc=operand; }
      else if (mainregnum==COND_ERR)
      { if (flerr)
          pc=operand; }
      else
      { interrupt_code=INTR_UNIMPOP;
        interrupt_address=pc-1; }
      break; }
    case OP_GETFL:
    { mainreg=get_flag(operand);
      flz=mainreg;
      break; }
    case OP_SETFL:
    { int val=0;
      if (mainreg!=0) val=1;
      if (is_user_flag(operand) || flsys)
        set_flag(operand, val);
      else
      { interrupt_code=INTR_PRIVOP;
        interrupt_address=pc-1; }
      break; }
    case OP_GETSR:
    { mainreg=get_special_register(operand);
      break; }
    case OP_SETSR:
    { if (flsys)
        set_special_register(operand, mainreg);
      else
      { interrupt_code=INTR_PRIVOP;
        interrupt_address=pc-1; }
      break; }
    case OP_PUSH:
    { int & sp = registers(SP);
      memory_write(sp-1, operand);
      if (interrupt_code==0)
        sp-=1;
      break; }
    case OP_POP:
    { int & sp = registers(SP);
      int value = memory_read(sp);
      if (interrupt_code==0)
        write_operand(value);
      if (interrupt_code==0)
        sp+=1;
      break; }
    case OP_CALL:
    { int & sp = registers(SP);
      memory_write(sp-1, pc);
      if (interrupt_code==0)
      { sp-=1;
        pc=operand; }
      break; }
    case OP_RET:
    { int & sp = registers(SP);
      int addr = memory_read(sp);
      if (interrupt_code==0)
      { sp+=1;
        pc=addr; }
      break; }
    case OP_LDCH:
    { int charnum=mainreg;
      int wordnum=charnum/4;
      charnum-=4*wordnum;
      int shift=charnum*8;
      int word=memory_read(operand+wordnum);
      if (interrupt_code==0)
        mainreg=(word>>shift)&0xFF;
      break; }
    case OP_STCH:
    { int charnum=mainreg;
      int wordnum=charnum/4;
      charnum-=4*wordnum;
      int shift=charnum*8;
      int word=memory_read(operand+wordnum);
      word &=~ (0xFF<<shift);
      word |= (_registers[0]&0xFF)<<shift;
      if (interrupt_code==0)
        memory_write(operand+wordnum, word);
      break; }
    case OP_DOIO:
    { int operation = memory_read(operand);
      if (interrupt_code!=0)
        break;
      int result = perform_io_operation(operation, operand+1);
      if (interrupt_code==0)
        mainreg=result;
      break; }
    case OP_FAKEIT:
    { fake_function_call(operand, mainregnum);
      break; }
    case OP_FLAGSJ:
    { if (flsys)
      { decombine_flags(mainreg);
        pc=operand; }
      else
      { interrupt_code=INTR_PRIVOP;
        interrupt_address=pc-1; }
      break; }
    case OP_WAIT:
    { usleep(50000);
      if (sr_timer>0)
      { sr_timer-=50000;
        if (sr_timer<2)
          sr_timer=2; }
      pc-=1;
      break; }
    case OP_PAUSE:
    { usleep(50000);
      if (sr_timer>0)
      { sr_timer-=50000;
        if (sr_timer<2)
          sr_timer=2; }
      break; }
    case OP_BREAK:
    { runwild=false;
      if (!stepping || countdown>0)
      { pc-=1;
        if (operand==0)
          printf("\n\nBREAK-POINT REACHED\n");
        else
          printf("\n\nBREAK-POINT %d REACHED\n", operand);
        fflush(stdout);
        stepping=true; }
      countdown=0;
      break; }
    case OP_IRET:
    { if (!flsys)
      { interrupt_code=INTR_PRIVOP;
        interrupt_address=pc-1;
        break; }
      int flags, addr, tosys=0, r[16], oldsp=sr_syssp;
      flags = memory_read(sr_syssp);
      if (flags&FLAGMASK_SYS)
        tosys=1;
      for (int i=0; i<16 && interrupt_code==0; i+=1)
        r[i]=memory_read(sr_syssp+19-i);
      if (interrupt_code==0)
      { if (tosys)
        { for (int i=0; i<16; i+=1)
            sysregisters(i)=r[i]; }
        else
        { for (int i=0; i<16; i+=1)
            usrregisters(i)=r[i]; }
        sr_syssp=oldsp+20;
        decombine_flags(flags); }
      break; }
    case OP_SYSCALL:
    { if (operand<0 || operand>=sr_cglen || sr_cgbr==0)
      { interrupt_code=INTR_BADCALL;
        interrupt_address=pc-1;
        interrupt_detail=operand;
        break; }
      int dest=memory_read(sr_cgbr+operand);
      for (int i=0; i<16 && interrupt_code==0; i+=1)
        memory_write(sr_syssp-i-1, registers(i));
      if (interrupt_code==0)
        memory_write(sr_syssp-17, mainreg);
      if (interrupt_code==0)
        memory_write(sr_syssp-18, mainregnum);
      if (interrupt_code==0)
        memory_write(sr_syssp-19, operand);
      if (interrupt_code==0)
        memory_write(sr_syssp-20, combine_flags());
      if (interrupt_code==0)
      { sr_syssp-=20;
        flsys=1;
        pc=dest; }
      break; }
    case OP_ATAS:
    { int v = operand;
      write_operand(1);
      if (interrupt_code==0)
        mainreg=v;
      if (v==1)
        usleep(10000);
      break; }
    case OP_PHLOAD:
    { if (flsys==0)
      { interrupt_code=INTR_PRIVOP;
        interrupt_address=pc-1;
        break; }
      mainreg = physical_memory_read(effective_address);
      break; }
    case OP_PHSTORE:
    { if (flsys==0)
      { interrupt_code=INTR_PRIVOP;
        interrupt_address=pc-1;
        break; }
      physical_memory_write(effective_address, mainreg);
      break; }
    case OP_VTRAN:
    { mainreg=translate(operand);
      break; }
    case OP_MOVE:
    { int num=registers(0);
      int src=mainreg;
      int dst=operand;
      if (current_instruction.indirect || current_instruction.number)
      { interrupt_code=INTR_UNIMPOP;
        interrupt_address=pc-1;
        break; }
      for (int i=0; num>0; i+=1)
      { if (interrupt_code!=0)
          break;
        if (i>=50)
        { if (sr_timer==2)
            break;
          else if (sr_timer>2)
            sr_timer-=1;
          i=0; }
        int v=memory_read(src);
        if (interrupt_code!=0)
          break;
        memory_write(dst, v);
        if (interrupt_code!=0)
          break;
        src+=1;
        dst+=1;
        num-=1; }
      registers(0)=num;
      mainreg=src;
      write_operand(dst);
      if (num>0)
        pc-=1;
      break; }
    case OP_FADD:
    { mainreg=encode_float(decode_float(mainreg)+decode_float(operand));
      break; }
    case OP_FSUB:
    { mainreg=encode_float(decode_float(mainreg)-decode_float(operand));
      break; }
    case OP_FMUL:
    { mainreg=encode_float(decode_float(mainreg)*decode_float(operand));
      break; }
    case OP_FDIV:
    { float fop = decode_float(operand);
      if (fop==0)
      { interrupt_code=INTR_DIVZERO;
        interrupt_address=pc-1;
        break; }
      mainreg=encode_float(decode_float(mainreg)/fop);
      break; }
    case OP_FCOMP:
    { flz=0;
      fln=0;
      float fm=decode_float(mainreg), fo=decode_float(operand);
      if (fm==fo)
        flz=1;
      else if (fm<fo)
        fln=1;
      break; }
    case OP_FIX:
    { mainreg=(int)decode_float(operand);
      break; }
    case OP_FRND:
    { mainreg=encode_float(round(decode_float(operand)));
      break; }
    case OP_FLOAT:
    { mainreg=encode_float(operand);
      break; }
    case OP_FLOG:
    { float f = decode_float(mainreg);
      if (f<=0.0)
        f=0.0;
      else if (operand==0)
        f=log(f);
      else if (operand==10)
        f=log10(f);
      else
        f=log(f)/log(operand);
      mainreg=encode_float(f);
      break; }
    case OP_FEXP:
    { float f = decode_float(mainreg);
      if (operand==0)
        f=exp(f);
      else if (operand==2)
        f=exp2(f);
      else
        f=exp(f*log(operand));
      mainreg=encode_float(f);
      break; }
    case OP_FFO:
    { unsigned int mask=0x80000000;
      for (int i=31; i>=0; i-=1)
      { if ((operand&mask)!=0)
        { mainreg=i;
          flz=0;
          fln=0;
          break; }
        mask>>=1; }
      mainreg=-1;
      flz=1;
      fln=1;
      break; }
    case OP_FLZ:
    { unsigned int mask=1;
      for (int i=0; i<32; i+=1)
      { if ((operand&mask)==0)
        { mainreg=i;
          flz=0;
          fln=0;
          break; }
        mask<<=1; }
      mainreg=-1;
      flz=1;
      fln=1;
      break; }
    case OP_RAND:
    { mainreg=random()&0x7FFFFFFF;
      break; }
    default:
    { local_execute(); } } }

bool computer::loadfile(string fname, int addr)
{ FILE * fi = fopen(fname.c_str(), "rb");
  if (fi==NULL)
  { fprintf(stderr, "can't read '%s'\n", fname.c_str());
    return false; }
  int block[1024], total=0;
  while (true)
  { int nr = fread(block, 4, 1024, fi);
    if (nr<=0) break;
    for (int i=0; i<nr; i+=1)
    { memory_write(addr, block[i]);
      if (interrupt_code!=0)
      { if (interrupt_code>=0 && interrupt_code<num_interrupts)
          printf("loading failed '%s' at 0x%08X\n", interrupt_name[interrupt_code], interrupt_address);
        else
          printf("loading failed at 0x%08X\n", interrupt_address);
        break; }
      addr+=1; }
    total+=nr;
    if (nr!=1024)
      break; }
  if (addr>first_free)
    first_free=addr+1;
  fflush(stdout);
  fclose(fi);
  return true; }

void debinfo(computer & c)
{ for (int i=0; i<16; i+=1)
  { int j=((i&3)<<2)|((i>>2)&3);
    int v=c.registers(j);
    if (j>12 || v<-9999 || v>9999)
      printf("   %-3s= 0x%08X", register_name[j], v);
    else
      printf("   %-3s= %-10d", register_name[j], v);
    if (j==SP)
    { if (c.current_instruction.notes & OPCODENOTE_USESSSP)
      { if (c.flsys)
          printf("   USRSP = 0x%08X", c._registers[SP]);
        else
          printf("   SYSSP = 0x%08X", c.sr_syssp); } }
    else if (j==FP)
    { if (c.current_instruction.notes & OPCODENOTE_USESSSP)
      { if (c.flsys)
          printf("   USRFP = 0x%08X", c._registers[FP]);
        else
          printf("   SYSFP = 0x%08X", c.sr_sysfp); } }
    if (i%4==3) printf("\n"); }
  if (c.current_instruction.notes & OPCODENOTE_SPECREG)
    printf("   %s = 0x%08X\n", special_register_name(c.effective_address), c.get_special_register(c.effective_address));
  int cf=c.combine_flags();
  printf("   FLAGS = 0x%08X: ", cf);
  for (int i=0; i<num_flags; i+=1)
  { if (!(cf&(1<<i)))
      printf("~");
    if (flags[i][0]=='$')
      printf("%s ", flags[i]+1);
    else
      printf("%s ", flags[i]); }
  printf("\n");
  if (c.current_instruction.indexreg!=0 && c.current_instruction.number!=0)
  { int v = c.registers(c.current_instruction.indexreg)+c.current_instruction.number;
    if (c.current_instruction.indexreg==PC)
      v+=1;
    printf("   %s%+0d = 0x%08X = %d\n", register_name[c.current_instruction.indexreg], c.current_instruction.number, v, v); }
  if (c.current_instruction.notes & OPCODENOTE_FLOATO)
  { if (c.current_instruction.indirect)
    { int v = c.memory_read(c.effective_address);
      printf("   [%08X=%d] = %d = %e\n", c.effective_address, c.effective_address, v, decode_float(v)); }
    else
      printf("   operand = %e\n", decode_float(c.effective_address)); }
  else if (c.current_instruction.indirect)
    printf("   [%08X=%d] = %d\n", c.effective_address, c.effective_address, c.memory_read(c.effective_address));
  if (c.current_instruction.notes & OPCODENOTE_FLOATR)
    printf("   %s = %f\n", register_name[c.current_instruction.mainreg], 
                decode_float(c.registers(c.current_instruction.mainreg)));
  printf("\n%08X: (%08X)   %s   ", c._registers[PC],
         c.current_instruction.whole_value, c.current_instruction.printable().c_str());
  fflush(stdout); }

void postdebinfo(computer & c)
{ if (c.current_instruction.notes & OPCODENOTE_FLOATR)
    printf("   %s = %e\n", register_name[c.current_instruction.mainreg], 
                decode_float(c.registers(c.current_instruction.mainreg)));
  if (c.current_instruction.notes & OPCODENOTE_SPECREG)
    printf("   %s = 0x%08X\n", special_register_name(c.effective_address), c.get_special_register(c.effective_address));
  if (c.current_instruction.indirect)
    printf("   [%08X=%d] = %d\n", c.effective_address, c.effective_address, c.memory_read(c.effective_address));
  fflush(stdout); }

string prepare_filename(string givenname, string inext)
{ int dotpos=-1;
  for (int i=0; i<givenname.length(); i+=1)
  { if (givenname[i]=='/' || givenname[i]=='\\')
      dotpos=-1;
    else if (givenname[i]=='.')
      dotpos=i; }
  if (dotpos>=0)
    return givenname;
  else
    return givenname+"."+inext; }

int prepare_disc(int num, string name, int size)
{ string fname = prepare_filename(name, "disc");
  unsigned char block[512];
  for (int i=0; i<512; i+=1)
    block[i]=0;
  int dotposition=-1;
  for (int i=fname.length()-1; i>0; i-=1)
  { if (fname[i]=='.')
    { dotposition=i;
      break; } }
  if (dotposition==-1)
    fname+=".disc";
  int df = open(fname.c_str(), O_RDWR | O_CREAT | O_BINARY, 0600);
  if (df<0) return df;
  int currsize=lseek(df, 0, SEEK_END)/512;
  if (currsize==0 && size==0)
    size=2000;
  if (currsize<size && size>40000)
  { fprintf(stderr, "Disc file '%s', requested size too big\n", fname.c_str());
    close(df);
    return -1; }
  for (int i=currsize; i<size; i+=1)
    write(df, block, 512);
  size=lseek(df, 0, SEEK_END)/512;
  if (!quiet)
    printf("disc %d from file '%s', %d blocks\n", num, fname.c_str(), size);
  return df; }

bool read_initialisation(computer & c)
{ ifstream f("system.setup");
  if (f.fail())
  { fprintf(stderr, "Can not open file 'system.setup'\n");
    return false; }
  string line, item;
  bool successful=true;
  for (int lnnum=1; true; lnnum+=1)
  { getline(f, line);
    if (f.fail()) 
      break;
    istringstream iss(line);
    iss >> item;
    if (iss.fail())
      continue;
    if (item[0]=='/' && item[1]=='/')
      continue;
    successful=false;
    if (same(item, "quiet"))
      quiet=true;
    else if (same(item, "memory"))
    { int from, to;
      iss >> item;
      if (iss.fail())
      { fprintf(stderr, "system.setup line %d, no address after 'memory'\n", lnnum);
        break; }
      bool ok=string_to_int(item, from);
      if (!ok)
      { fprintf(stderr, "system.setup line %d, '%s' not a number\n", lnnum, item.c_str());
        break; }
      iss >> item;
      if (iss.fail())
        to=from;
      else if (same(item, "to"))
      { iss >> item;
        if (iss.fail())
        { fprintf(stderr, "system.setup line %d, no address after 'to'\n", lnnum);
          break; }
        bool ok=string_to_int(item, to);
        if (!ok)
        { fprintf(stderr, "system.setup line %d, '%s' not a number\n", lnnum, item.c_str());
          break; } }
      if (ok)
      { iss >> item;
        if (!iss.fail())
          ok=false; }
      if (!ok)
      { fprintf(stderr, "system.setup line %d, memory parameter confused '%s'\n", lnnum, item.c_str());
        break; }
      unsigned int a1 = from&0xFFFF0000, a2 = to&0xFFFF0000;
      for (unsigned int a=a1; a<=a2; a+=0x10000)
      { if (a<a1)
          break;
        c.physical_memory_create(a);
        if (c.interrupt_code!=0)
          break; }
      if (a1==0)
        max_mem=(a2|0xFFFF)+1; }
    else if (same(item, "load"))
    { int address;
      iss >> item;
      if (iss.fail())
      { fprintf(stderr, "system.setup line %d, no file name after 'load'\n", lnnum);
        break; }
      string name = prepare_filename(item, "exe");
      iss >> item;
      if (iss.fail() || !same(item, "at"))
      { fprintf(stderr, "system.setup line %d, no 'at' after 'load %s'\n", lnnum, name.c_str());
        break; }
      iss >> item;
      if (iss.fail())
      { fprintf(stderr, "system.setup line %d, no address after 'at'\n", lnnum);
        break; }
      bool ok=string_to_int(item, address);
      if (!ok)
      { fprintf(stderr, "system.setup line %d, '%s' not a number\n", lnnum, item.c_str());
        break; }
      iss >> item;
      if (!iss.fail())
      { fprintf(stderr, "system.setup line %d, load parameter confused '%s'\n", lnnum, item.c_str());
        break; }
      if (!quiet)
        printf("load file '%s' starting at 0x%08X\n", name.c_str(), address);
      ok=c.loadfile(name, address);
      if (!ok)
        break; }
    else if (same(item, "sp") || same(item, "pc") || same(item, "fp"))
    { int regnum=-1;
      for (int i=0; i<16; i+=1)
      { if (same(item, register_name[i]))
        { regnum=i;
          break; } }
      if (regnum!=-1)
      { int value;
        iss >> item;
        if (iss.fail())
        { fprintf(stderr, "system.setup line %d, no value after '%s'\n", lnnum, register_name[regnum]);
          break; }
        bool ok=string_to_int(item, value);
        if (!ok)
        { fprintf(stderr, "system.setup line %d, '%s' not a number\n", lnnum, item.c_str());
          break; }
        iss >> item;
        if (!iss.fail())
        { fprintf(stderr, "system.setup line %d, %s parameter confused '%s'\n", 
                  lnnum, register_name[regnum], item.c_str());
          break; }
        if (!quiet)
          printf("set %s to 0x%08X\n", register_name[regnum], value);
        c.usrregisters(regnum)=value;
        c.sysregisters(regnum)=value; }
      else
      { fprintf(stderr, "system.setup line %d, '%s' unknown parameter\n", lnnum, item.c_str());
        break; } }
    else if (same(item, "disc"))
    { string name;
      iss >> name;
      if (iss.fail())
      { fprintf(stderr, "system.setup line %d, no name after 'disc'\n", lnnum);
        break; }
      int size;
      iss >> size;
      if (iss.fail())
        size=0;
      int dfile = prepare_disc(c.discs.size(), name, size);
      if (dfile<0)
      { fprintf(stderr, "system.setup line %d, disc file '%s' is not accessible\n", lnnum, name.c_str());
        break; }
      int realsize = lseek(dfile, 0, SEEK_END);
      num_discs+=1;
      c.discs.push_back(dfile);
      c.discsize.push_back(realsize); }
    successful=true; }
  f.close();
  return successful; }

void sigint(int a)
{ if (control_c)
    exit(1);
  control_c=true; }

bool computer::debugextract(string line, unsigned int & origloc1, unsigned int & loc1,
                            unsigned int & loc2, string & base, int & val, bool & wasoff)
{ int start=0, len=0;
  loc1=0;
  loc2=0;
  origloc1=0;
  val=0;
  base="";
  wasoff=false;
  string first="", second="", third="";
  char c=line[0];
  bool gotloc2=false;
  bool debug = line[0] == '?';
  if (debug)
    start=1;
  while (c==' ')
  { start+=1;
    c=line[start]; }
  while (c>' ' && c!='-' && c!='+' && c!=':' && c!='=')
  { len+=1;
    c=line[start+len]; }
  if (len>0)
    first=line.substr(start, len);
  start+=len;
  len=0;
  if (c==' ')
  { while (c==' ')
    { start+=1;
      c=line[start]; } }
  else if (c!=0)
  { char oc=c;
    start+=1;
    while (line[start]==' ')
      start+=1;
    start-=1;
    line[start]=oc; }
  while (c>' ')
  { if (len>0 && (c=='+' || c=='-' || c==':' || c=='='))
      break;
    len+=1;
    c=line[start+len]; }
  if (len>0)
    second=line.substr(start, len);
  start+=len;
  len=0;
  if (c==' ')
  { while (c==' ')
    { start+=1;
      c=line[start]; } }
  else
  { char oc=c;
    start+=1;
    while (line[start]==' ')
      start+=1;
    start-=1;
    line[start]=oc; }
  while (c>' ')
  { len+=1;
    c=line[start+len]; }
  if (len>0)
    third=line.substr(start, len);
  if (debug)
    printf("first='%s', second='%s', third='%s'\n", first.c_str(), second.c_str(), third.c_str());
  bool found=false;
  const char * f = first.c_str();
  for (int i=0; i<16; i+=1)
  { if (strcasecmp(f, register_name[i])==0)
    { loc1=registers(i);
      found=true;
      break; } }
  if (!found)
  { for (int i=0; i<num_specregs; i+=1)
    { if (strcasecmp(f, specregs[i]+1)==0)
      { loc1=get_special_register(i);
        found=true;
        break; } } }
  if (!found && strcasecmp(f, "usrsp")==0)
  { loc1=_registers[SP];
    found=true; }
  if (!found && strcasecmp(f, "usrfp")==0)
  { loc1=_registers[FP];
    found=true; }
  if (found)
    base=first;
  if (!found)
  { string x="0x";
    x+=first;
    int a;
    found=string_to_int(x, a);
    loc1=a; }
  if (!found)
  { printf("Can't understand '%s' as a memory location\n", f);
    return false; }
  origloc1=loc1;
  if (debug)
    printf("1: origloc1=%d, loc1=%d, loc2=%d, val=%d, wasoff=%d\n", origloc1, loc1, loc2, val, wasoff);
  found=false;
  if (second=="")
  { loc2=loc1;
    gotloc2=true;
    found=true; }
  if (second[0]==':' || second[0]>='0' && second[0]<='9')
  { int len=0;
    if (second[0]==':')
    { found=string_to_int(second.substr(1), len);
      if (found)
      { loc2=loc1+len-1;
        gotloc2=true; } }
    else
    { string x="0x";
      x+=second;
      found=string_to_int(x, len);
      if (found)
      { loc2=len;
        gotloc2=true; } } }
  else if (second[0]=='=')
  { found=string_to_int(second.substr(1), len);
    if (found) val=len; }
  else if (second[0]=='-' || second[0]=='+')
  { bool neg=second[0]=='-';
    wasoff=true;
    int offset;
    found=string_to_int(second.substr(1), offset);
    if (found)
    { if (neg)
        loc1-=offset;
      else
        loc1+=offset; } }
  if (debug)
    printf("2: origloc1=%d, loc1=%d, loc2=%d, val=%d, wasoff=%d\n", origloc1, loc1, loc2, val, wasoff);
  if (!found)
  { printf("Can't understand '%s' as an offset or number\n", second.c_str());
    return false; }
  found=false;
  if (third!="")
  { if (third[0]==':' || third[0]=='=' || third[0]>='0' && third[0]<='9')
    { int len=0;
      if (third[0]==':')
      { found=string_to_int(third.substr(1), len);
        if (found)
        { loc2=loc1+len-1;
          gotloc2=true; } }
      else if (third[0]=='=')
      { found=string_to_int(third.substr(1), len);
        if (found) val=len; }
      else
      { found=string_to_int(third, len);
        if (found) 
        { loc2=len;
          gotloc2=true; } } }
    if (third[0]=='-' || third[0]=='+')
    { bool neg=third[0]=='-';
      wasoff=true;
      int offset;
      found=string_to_int(third.substr(1), offset);
      if (found)
      { if (neg)
          loc2=origloc1-offset;
        else
          loc2=origloc1+offset;
        gotloc2=true; } }
    if (debug)
      printf("3: origloc1=%d, loc1=%d, loc2=%d, val=%d, wasoff=%d\n", origloc1, loc1, loc2, val, wasoff);
    if (!found)
    { printf("Can't understand '%s' as an offset or number\n", third.c_str());
      return false; } }
  if (!gotloc2)
    loc2=loc1;
  return true; }

void computer::debugshow(string line, bool physonly)
{ unsigned int origloc1, loc1, loc2;
  int val;
  string base;
  bool wasoff, done=false;
  bool ok=debugextract(line, origloc1, loc1, loc2, base, val, wasoff);
  if (line[0]=='?')
    printf("origloc1=%d, loc1=%d, loc2=%d, base='%s', val=%d, wasoff=%d\n",
           origloc1, loc1, loc2, base.c_str(), val, wasoff);
  if (!ok)
    return;
  if (base!="" && !wasoff && loc1==loc2)
  { printf("\n  %s  =  %d  =", base.c_str(), loc1);
    done=true; }
  else
    printf("\n");
  unsigned int i=loc1, count=0;
  while (true)
  { if (base!="" && !done)
    { printf("  %s", base.c_str());
      if (i<origloc1)
        printf("-%-4d", origloc1-i);
      else if (i==origloc1)
        printf("     ");
      else
        printf("+%-4d", i-origloc1); }
    int v = 0;
    if (physonly)
      v=physical_memory_read(i);
    else
      v=memory_read(i);
    printf("  %08X: %08X = %d\n", i, v, v);
    if (loc1<=loc2)
    { i+=1;
      if (i>loc2) break; }
    else
    { i-=1;
      if (i<loc2) break; }
    count+=1;
    if (count>64)
    { printf("64 words maximum at one time\n");
      break; } }
  printf("\n"); }

void computer::debugset(string line, bool physonly)
{ unsigned int origloc1, loc1, loc2;
  int val;
  string base;
  bool wasoff;
  bool ok=debugextract(line, origloc1, loc1, loc2, base, val, wasoff);
  if (!ok)
    return;
  if (base!="" && !wasoff)
  { const char * f = base.c_str();
    for (int i=0; i<16; i+=1)
    { if (strcasecmp(f, register_name[i])==0)
      { registers(i)=val;
        return; } }
    for (int i=0; i<num_specregs; i+=1)
    { if (strcasecmp(f, specregs[i]+1)==0)
      { set_special_register(i, val);
        return; } }
    if (strcasecmp(f, "usrsp")==0)
    { _registers[SP]=val;
      return; }
    if (strcasecmp(f, "usrfp")==0)
    { _registers[FP]=val;
      return; } }
  else
  { if (physonly)
      physical_memory_write(loc1, val);
    else
      memory_write(loc1, val);
    if (base!="")
    { printf("  %s", base.c_str());
      if (loc1<origloc1)
        printf("-%-4d", origloc1-loc1);
      else if (loc1==origloc1)
        printf("     ");
      else
        printf("+%-4d", loc1-origloc1); }
    int v=0;
    if (physonly)
      v=physical_memory_read(loc1);
    else
      v=memory_read(loc1);
    printf("  %08X: %08X = %d\n", loc1, v, v); } }

void main(int argc, char * argv[])
{ computer c;
  bool ok=true, autorun=false, autoexit=false;
  string filename="";
  for (int i=1; i<argc; i+=1)
  { if (argv[i][0]=='-')
    { if (argv[i][1]=='r')
      { autorun=true;
        if (argv[i][2]=='s')
          autoexit=false;
        else
          autoexit=true; }
      else
      { fprintf(stderr, "Bad option '%s' on command line\n", argv[i]);
        ok=false;
        break; } }
    else if (filename=="")
      filename = prepare_filename(argv[i], "exe");
    else
    { fprintf(stderr, "Multiple file names on command line: '%s' and '%s'\n", filename.c_str(), argv[i]);
      ok=false;
      break; } }
  if (ok)
    ok=read_initialisation(c);
  srandomdev();
  if (ok && filename!="")
  { if (!quiet)
      printf("load file '%s' starting at 0x%08X\n", filename.c_str(), 0x400);
    ok=c.loadfile(filename, 0x400); }
  if (!ok)
  { fprintf(stderr, "Initialisation failed\n");
    exit(1); }
  c.physical_memory_write(0x100, max_mem);
  c.physical_memory_write(0x101, first_free);
  c.physical_memory_write(0x102, num_discs);
  interrupt_driven_keyboard(&kbd_rdy);
  signal(SIGINT, sigint);
  char command[1000];
  int linelen=0, controlccount=0;
  c.runwild=false;
  if (autorun)
  { c.runwild=true;
    c.flr=true; }
  while (true)
  { if (c.flr==false || control_c)
    { if (autorun && autoexit && c.reallyhalted)
        break;
      if (c.runwild || c.stepping)
      { printf("\nPROCESSOR HALTED with PC = 0x%08X\n", c._registers[PC]);
        fflush(stdout); }
      clear_keyboard_buffer();
      control_c=false;
      c.runwild=false;
      c.stepping=false; }
    if (c.runwild)
    { c.fetch();
      if (control_c)
      { c.runwild=false;
        c.stepping=true;
        continue; }
      if (kbd_rdy && c.interrupt_code==0 && !c.flip)
        c.interrupt_code=INTR_KEYBD;
      c.execute();
      if (c.interrupt_code!=0 && c.flip && c.interrupt_code!=INTR_KEYBD && c.interrupt_code!=INTR_TIMER)
      { c.runwild=false;
        c.stepping=true;
        c.prepare_interrupt(c.interrupt_code);
        continue; } }
    else
    { if (c.stepping)
      { if (c.interrupt_code!=0)
        { if (c.flip)
          { if (c.stepping)
            { printf("IGNORING: ");
              c.printinterrupt(c.interrupt_code, true); } }
          else
          { c.prepare_interrupt(c.interrupt_code);
            continue; } }
        c.fetch();
        if (c.interrupted)
        { int oc=c.interrupted;
          c.interrupted=0;
          c.fetch();
          if (c.interrupted)
            c.oldinterrupt=oc; }
        debinfo(c); }
      bool normalend=true;
      if (c.stepping && c.countdown>0 && !control_c)
        strcpy(command, "step");
      else
      { printf("> ");
        fflush(stdout);
        c.countdown=0;
        while (1)
        { if (!kbd_rdy && !control_c)
          { sleep(1);
            continue; }
          if (control_c)
          { clear_keyboard_buffer();
            printf("[control-c]\n");
            if (c.stepping)
              strcpy(command, "x");
            else
              strcpy(command, "");
            break; }
          char ch = read_keyboard_char();
          if (ch>=' ' || ch=='\n')
            putchar(ch);
          else if (ch=='\b')
          { if (linelen>0)
            { printf("\b \b");
              linelen-=1; }
            fflush(stdout);
            continue; }
          else if (ch=='X'-64)
          { if (linelen>0)
            { putchar('\r');
              for (int i=0; i<linelen+3; i+=1)
                putchar(' ');
              printf("\r> ");
              linelen=0; }
            fflush(stdout);
            continue; }
          else if (ch==27 && c.stepping)
          { printf("\n\n");
            if (linelen==0)
              strcpy(command, "x");
            else
            { command[linelen]=0;
              normalend=false;
              linelen=0; }
            break; }
          else
            printf("^%c", ch+64);
          fflush(stdout);
          if (ch=='\n')
          { if (linelen==0)
            { if (c.stepping)
                putchar('\n');
              strcpy(command, "step");
              break; } 
            else
            { command[linelen]=0;
              linelen=0;
              break; } }
          else if (c.stepping && linelen==0 && ch==' ')
          { putchar('\n');
            putchar('\n');
            strcpy(command, "step");
            break; }
          if (linelen<997)
          { command[linelen]=ch;
            linelen+=1; } } }
      if (control_c)
      { control_c=false;
        controlccount+=1;
        if (controlccount>=2)
          break; }
      int len=strlen(command);
      while (len>0 && command[len-1]<=' ')
        len-=1;
      command[len]=0;
      char * s=command;
      while (*s<=' ' && *s!=0)
        s+=1;
      if (*s==0)
        continue;
      char init[20];
      int pos=0;
      while (true)
      { char c=s[pos];
        if (c<=' ')
        { init[pos]=0;
          break; }
        else if (pos<19)
          init[pos]=tolower(c);
        pos+=1; }
      init[19]=0;
      if (s[pos]!=0 && s[pos]<=' ')
        pos+=1;
      char * rest = s+pos;
      controlccount=0;
      if (strcmp(init, "step")==0 || strcmp(init, "s")==0)
      { int num=0;
        if (rest[0]!=0)
        { bool ok = string_to_int(rest, num);
          if (!ok)
          { printf("Incomprehensible\n");
            continue; }
          else
            c.countdown=num; }
        if (!c.stepping)
        { c.flr=true;
          c.stepping=true;
          continue; }
        if (num>0 && c.current_instruction.opcode==OP_BREAK)
        { c._registers[PC]+=1;
          continue; }
        c.execute();
        postdebinfo(c); }
      else if (strcmp(init, "run")==0 || strcmp(init, "r")==0)
      { if (rest[0]!=0)
        { int num;
          bool ok = string_to_int(rest, num);
          if (!ok)
          { printf("Incomprehensible\n");
            continue; }
          else
            c.countdown=num; }
        c.flr=true;
        c.runwild=true;
        c.stepping=false;
        if (c.current_instruction.opcode==OP_BREAK)
          c._registers[PC]+=1;
        continue; }
      else if (strcmp(init, "input")==0)
      { for (int i=0; 1; i+=1)
        { if (rest[i]==0)
            break;
          add_console_char(rest[i]); }
        if (normalend)
          add_console_char('\n');
        continue; }
      else if (init[0]=='?')
      { c.debugshow(s+1);
        continue; }
      else if (init[0]=='p' && init[1]=='?')
      { c.debugshow(s+2, true);
        continue; }
      else if (strcmp(init, "set")==0)
      { c.debugset(rest);
        continue; }
      else if (strcmp(init, "pset")==0)
      { c.debugset(rest, true);
        continue; }
      else if (strcmp(init, "vm")==0)
      { string s="0x";
        int addr;
        string_to_int(s+rest, addr);
        c.vm_show(addr);
        printf("\n");
        continue; }
      else if (strcmp(init, "pause")==0 || strcmp(init, "p")==0)
      { c.stepping=false;
        continue; }
      else if (strcmp(init, "x")==0)
      { if (c.stepping)
        { c.stepping=false;
          continue; }
        else
          break; }
      else if (strcmp(init, "exit")==0 || strcmp(init, "quit")==0 || strcmp(init, "q")==0)
        break;
      else
      { printf("\nunrecognised command '%s'\n\n", visible(init).c_str());
        fflush(stdout);
        continue; } } } }

