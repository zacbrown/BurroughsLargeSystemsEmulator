// useful.cpp

#include "useful.h"
#include <stdio.h>
#include <string.h>

#define strcasecmp _stricmp

bool same(string a, string b)
{
    return strcasecmp(a.c_str(), b.c_str()) == 0;
}

string int_to_string(int n)
{
    char s[20];
    sprintf(s, "%d", n);
    return s;
}

string visible(char c)
{
    string r = "";
    if (c == '\n')
        r += "\\n";
    else if (c < ' ')
        r = r + "^" + (char)(c + 64);
    else if (c <= '~')
        r = r + c;
    else
    {
        char s[20];
        sprintf(s, "\\%03o", c);
        r = r + s;
    }
    return r;
}

string visible(string s)
{
    string r = "";
    for (int i = 0; i < s.length(); i += 1)
    {
        char c = s[i];
        if (c == '\n')
            r += "\\n";
        else if (c < ' ')
            r = r + "^" + (char)(c + 64);
        else if (c <= '~')
            r = r + c;
        else
        {
            char s[20];
            sprintf(s, "\\%03o", c);
            r = r + s;
        }
    }
    return r;
}

int digit_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c + 10 - 'a';
    if (c >= 'A' && c <= 'F')
        return c + 10 - 'A';
    return 99;
}

bool string_to_int(string s, int& n)
{
    int base = 10, start = 0, len = s.length(), sign = 1;
    if (s[0] == '+')
        s = s.substr(1);
    else if (s[0] == '-')
    {
        sign = -1;
        s = s.substr(1);
    }
    if (s[0] == '0')
    {
        if (s[1] == 'X' || s[1] == 'x')
        {
            base = 16;
            start = 2;
        }
        else if (s[1] == 'B' || s[1] == 'b')
        {
            base = 2;
            start = 2;
        }
        else if (s[1] == 'O' || s[1] == 'o')
        {
            base = 2;
            start = 2;
        }
        else if (s[1] == 'D' || s[1] == 'd')
        {
            base = 10;
            start = 2;
        }
    }
    if (start >= len)
        return false;
    int val = 0;
    for (int i = start; i < len; i += 1)
    {
        int d = digit_value(s[i]);
        if (d >= base)
            return false;
        val = val * base + d;
    }
    if (sign == -1)
        n = -val;
    else
        n = val;
    return true;
}

int string_to_int(string s)
{
    int n;
    string_to_int(s, n);
    return n;
}

char backstroke_char(char c)
{
    if (c >= 'A' && c <= 'Z')
        c += 'a' - 'A';
    if (c == '\\') return '\\';
    if (c == '\'') return '\'';
    if (c == '\"') return '\"';
    if (c == '0') return 0;
    if (c == 'n') return '\n';
    if (c == ' ') return ' ';
    if (c == 't') return '\t';
    if (c == 'r') return '\r';
    if (c == 'b') return '\b';
    return c;
}

#ifdef unix

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

static termios saved_termios;
static int savedflags=0;
static bool atexitted=false, odd_keyboard=false;
static void (* character_handler)(char)=NULL;
static FILE * oldstdout=NULL;

static void sigio(int x)
{ char c;
  int n=read(0, &c, 1);
  if (n>0 && character_handler!=NULL) 
    character_handler(c); }

int stdwrite(void * x, const char * s, int n)
{ int total=0;
  while (total<n)
  { int amt=write(1, s, n-total);
    if (amt<0)
    { usleep(10000);
      continue; }
    total+=amt;
    s+=amt;
    if (total<n)
      usleep(10000); }
  return total; }

static void tty_atexit();

bool keyboard_immediate(bool echo, void (* character_taker)(char))
{ termios buf;
  if (tcgetattr(0, &saved_termios)<0)
    return false;
  buf=saved_termios;
  buf.c_lflag &= ~ICANON;
  if (!echo)
    buf.c_lflag &= ~ECHO;
  buf.c_cc[VMIN]=1;
  buf.c_cc[VTIME]=0;
  if (tcsetattr(0, TCSAFLUSH, &buf)<0)
    return false;
  character_handler = character_taker;
  if (!atexitted)
  { atexit(tty_atexit);
    atexitted=true; }
  odd_keyboard=true;
  signal(SIGIO, sigio);
  int pid=getpid();
  int flags=fcntl(0, F_GETFL, 0);
  fcntl(0, F_SETFL, flags | O_ASYNC | O_NONBLOCK);
  fcntl(0, F_SETOWN, pid);
  if (oldstdout==NULL)
    oldstdout=stdout;
  stdout=fwopen(NULL, stdwrite);
  return true; }

bool keyboard_normal()
{ if (!odd_keyboard)
    return true;
  stdout=oldstdout;
  if (tcsetattr(0, TCSAFLUSH, &saved_termios)<0)
    return false;
  odd_keyboard=false;
  signal(SIGIO, SIG_IGN);
  fcntl(0, F_SETFL, savedflags);
  return true; }

static void tty_atexit(void)
{ if (odd_keyboard)
    keyboard_normal(); }

static const int kbb_size=260;
static char keyboard_buffer[kbb_size];
static int kbb_b=0, kbb_e=0, kbb_n=0, kbb_l=0;
static bool * kb_ready_flag = NULL;

static void kb_getter(char c)
{ char lostch = keyboard_buffer[kbb_e];
  keyboard_buffer[kbb_e]=c;
  kbb_e+=1;
  if (kbb_e>=kbb_size)
    kbb_e=0;
  if (kbb_n<kbb_size-1)
    kbb_n+=1;
  else
  { kbb_b+=1;
    if (kbb_b>=kbb_size)
      kbb_b=0;
    if (lostch=='\n')
      kbb_l-=1; }
  if (c=='\n')
    kbb_l+=1;
  if (kbb_n>0 && kb_ready_flag!=NULL)
    *kb_ready_flag=true; }

bool kbd_line_ready()
{ return kbb_l>0; }

bool kbd_any_ready()
{ return kbb_n>0; }

char read_keyboard_char()
{ if (kbb_n<=0)
    return 0;
  char c = keyboard_buffer[kbb_b];
  kbb_b+=1;
  if (kbb_b>=kbb_size)
    kbb_b=0;
  kbb_n-=1;
  if (kbb_n<=0 && kb_ready_flag!=NULL)
    *kb_ready_flag=false;
  if (c=='\n')
    kbb_l-=1;
  return c; }

void interrupt_driven_keyboard(bool * ready_flag)
{ kb_ready_flag = ready_flag;
  *kb_ready_flag = false;
  kbb_b=0;
  kbb_e=0;
  kbb_n=0;
  kbb_l=0;
  keyboard_immediate(false, kb_getter); }

void normal_keyboard()
{ kb_ready_flag = NULL;
  keyboard_normal(); }

void clear_keyboard_buffer()
{ if (kb_ready_flag!=NULL)
    *kb_ready_flag = false;
  kbb_b=0;
  kbb_e=0;
  kbb_n=0;
  kbb_l=0; }

#else

bool kbd_line_ready()
{
    return false;
}

char read_keyboard_char()
{
    return 0;
}

void interrupt_driven_keyboard(bool* ready_flag)
{
}

void normal_keyboard()
{
}

#endif
