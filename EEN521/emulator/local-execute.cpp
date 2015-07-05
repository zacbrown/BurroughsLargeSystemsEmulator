#include "codes.h"
#include "computer.h"
#include "useful.h"

void computer::local_execute()
{ int mainreg = current_instruction.mainreg;
  int operand = read_operand();
  if (interrupt_code!=0)                  
    return;   
  switch (current_instruction.opcode)
  {

    // insert your own cases here

    default:
    { interrupt_code=INTR_UNIMPOP;          
      interrupt_address=_registers[PC]-1;
      break; } } }

