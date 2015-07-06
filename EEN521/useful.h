// useful.h

#include <string>
using namespace std;

string int_to_string(int n);
int digit_value(char c);
bool string_to_int(string s, int& n);
int string_to_int(string s);
char backstroke_char(char c);
bool same(string a, string b);
string visible(char c);
string visible(string s);

bool kbd_any_ready();
bool kbd_line_ready();
char read_keyboard_char();
void clear_keyboard_buffer();
void interrupt_driven_keyboard(bool* ready_flag);
void normal_keyboard();
