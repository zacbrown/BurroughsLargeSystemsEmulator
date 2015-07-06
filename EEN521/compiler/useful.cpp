// this is useful.cpp
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <fstream>

using namespace std;

string inttostring(int i)
 { char c[20];
   sprintf_s(c, "%d", i);
   return c; }

bool same(string a, string b)
{ return _stricmp(a.c_str(), b.c_str())==0; }

int digit_value(char c)
 { if (c>='0' && c<='9')
      return c-'0';
   if (c>='a' && c<='f')
      return c+10-'a';
   if (c>='A' && c<='F')
      return c+10-'A';
   return 99; }

bool string_to_int(string s, int & n)
 { int base=10, start=0, len=s.length(), sign=1;
   if (s[0]=='+')
      s=s.substr(1);
   else if (s[0]=='-')
    { sign=-1;
      s=s.substr(1); }
   if (s[0]=='0')
    { if (s[1]=='X' || s[1]=='x')
       { base=16;
         start=2; }
   else if (s[1]=='B' || s[1]=='b')
    { base=2;
      start=2; }
   else if (s[1]=='O' || s[1]=='o')
    { base=2;
      start=2; }
   else if (s[1]=='D' || s[1]=='d')
    { base=10;
      start=2; } }
   if (start>=len)
      return false;
   int val=0;
   for (int i=start; i<len; i+=1)
    { int d=digit_value(s[i]);
      if (d>=base)
      return false;
      val=val*base+d; }
   if (sign==-1)
      n=-val;
   else
      n=val;
   return true; }

int string_to_int(string s)
 { int n;
   string_to_int(s, n);
   return n; }

char backstroke_char(char c)
 { if (c>='A' && c<='Z')
      c+='a'-'A';
   if (c=='\\') return '\\';
   if (c=='\'') return '\'';
   if (c=='\"') return '\"';
   if (c=='0') return 0;
   if (c=='n') return '\n';
   if (c==' ') return ' ';
   if (c=='t') return '\t';
   if (c=='r') return '\r';
   if (c=='b') return '\b';
   return c; }

