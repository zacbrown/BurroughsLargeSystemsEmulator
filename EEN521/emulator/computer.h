#include <vector>
#include <string>
  
struct decoded_instruction
{ int whole_value;
  int opcode, mainreg, indirect, indexreg, number, notes;
  decoded_instruction();
  decoded_instruction(int memval);
  decoded_instruction(int op, int mr, int ind, int ir, int n);
  void decode(int memval);
  string printable(); };

struct computer
{ static const int maximum_memory=1024*1024;
  int memory_in_use;
  int _registers[16], effective_address, interrupted, oldinterrupt;
  int flr, flz, fln, flerr, flsys, flip, flvm;
  int sr_pdbr, sr_intvec, sr_cgbr, sr_cglen, sr_debug, sr_timer, sr_syssp, sr_sysfp;
  int * memory[0x10000];
  bool stepping, runwild, reallyhalted;
  decoded_instruction current_instruction;
  int interrupt_code, interrupt_address, interrupt_detail, countdown;
  vector <int> discs, discsize;

  computer();
  ~computer();
  void reset();

  int & registers(int r);
  int & sysregisters(int r);
  int & usrregisters(int r);
  int get_flag(int f);
  void set_flag(int f, int v);
  bool is_user_flagmask(int fm);
  bool is_user_flag(int f);
  int get_special_register(int r);
  void set_special_register(int r, int v);
  const char * special_register_name(int r);
  void physical_memory_create(int address);
  int physical_memory_read(int address);
  void physical_memory_write(int address, int value);
  int translate(int address);
  int memory_read(int address);
  void memory_write(int address, int value);
  string string_from_memory(int addr, int maxlen, bool zeroterminated);
  int read_operand();
  void write_operand(int value);
  void fetch();
  void execute();
  int combine_flags();
  void decombine_flags(int f);
  void local_execute();
  bool loadfile(string fname, int address);
  bool get_parameter_block(int addr, int size, int params[]);
  void put_parameter(int addr, int which, int value);
  int perform_io_operation(int operation, int addr);
  void fake_function_call(int fn, int regnum);
  void prepare_interrupt(int ic);
  bool debugextract(string line, unsigned int & origloc1, unsigned int & loc1,
                    unsigned int & loc2, string & base, int & val, bool & wasoff);
  void debugshow(string s, bool physonly=false);
  void debugset(string s, bool physonly=false);
  void vm_show(int virtaddr);
  void printinterrupt(int level, bool forceit=false); };

