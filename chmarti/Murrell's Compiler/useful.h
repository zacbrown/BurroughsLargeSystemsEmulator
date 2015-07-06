// This is 'useful.h'

#ifndef __useful_included
#define __useful_included

#include <string>
using namespace std;

struct history
{ static const int size=20;
  string hist[size];
  int num, first, last;
  history();
  string see(int n);
  string take(int n);
  void add(string s); };

void printchar(FILE * f, string s);
void printchar(string s);
void printchar(char c);
void printstr(FILE * f, const char * s);
void printstr(const char * s);
void printstr(FILE * f, string s);
void printstr(string s);
void use_history(history & h);
string get_keyboard_line(int echo=1);
char * betterctime(time_t tt);
int limited_strcpy(char * dest, const char * src, int maxlen);
int char_to_int(char c, int base);
bool matchbeginning(string small, string big);
int findpos(char wanted, char * bigstring);
bool ends_with(string large, string small);
bool begins_with(string large, string small);
bool begins_with(const char * large, const char * small);
bool ends_with(const char * large, const char * small);
string afterdot(const char * s);

int posof(char c, const char * s);
int basefor(string str);
int valofstr(string str, int base=0);

#endif
