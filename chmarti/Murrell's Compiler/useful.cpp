// this is useful.cpp

#include "everything.h"

string opcodes[128];
opcodeentry opdata[128];
opcodeentry opload[] = { opdefs };
opcodeentry dollars[] = { dolldefs };
char * condname[] = { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" };

void printchar(FILE * f, char c)
{ if (c=='\n') fprintf(f, "\\n");
  else if (c=='\a') fprintf(f, "\\a");
  else if (c=='\b') fprintf(f, "\\b");
  else if (c=='\t') fprintf(f, "\\t");  
  else if (c=='\r') fprintf(f, "\\r");  
  else if (c=='\'') fprintf(f, "\\\'");  
  else if (c=='\"') fprintf(f, "\\\"");  
  else if (c=='\\') fprintf(f, "\\\\");  
  else if (c=='\0') fprintf(f, "\\0");  
  else if (c<' ' || c>=127) fprintf(f, "\\[%d]", c & 0xFF);  
  else fprintf(f, "%c", c); }

void printchar(char c)
{ printchar(stdout, c); }

void printstr(const char * s)
{ for (int i=0; s[i]!=0; i+=1)        
    printchar(stdout, s[i]); }

void printstr(FILE * f, const char * s)
{ for (int i=0; s[i]!=0; i+=1)        
    printchar(f, s[i]); }

void printstr(string s)
{ printstr(stdout, s.c_str()); }

void printstr(FILE * f, string s)
{ printstr(f, s.c_str()); }

history::history()
{ num=0; first=0; last=0; }

void history::add(string s)
{ if (num==size)
  { hist[first]=s;
    first+=1;
    if (first==size)
      first=0;
    last+=1;
    if (last==size)
      last=0; }
  else
  { last+=1;
    if (last==size)
      last=0;
    hist[last]=s;
    num+=1; } }

string history::see(int n)
{ if (n>num || n<=0)
    return "";
  int pos=last+1-n;
  if (pos<0)
    pos+=size;
  return hist[pos]; }

string history::take(int n)
{ if (n>num || n<=0)
    return "";
  int pos=last+1-n;
  if (pos<0)
    pos+=size;
  string it=hist[pos];
  for (int i=n; i>1; i-=1)
  { int p=last+1-i;
    if (p<0)
      p+=size;
    int q=last+2-i;
    if (q<0)
      q+=size;
    hist[p]=hist[q]; }
  hist[last]=it;
  return it; }

static history * thishist=NULL;
static history * dflthist=NULL;

void use_history(history & h)
{ thishist = & h; }

string get_keyboard_line(int echo)
{ char line[300];
  if (thishist==NULL)
  { if (dflthist==NULL)
      dflthist=new history();
    thishist=dflthist; }
  history & hist = * thishist;
  int len=0, esc=0, pos=0, histpos=0;
  while (1)
  { char c = get_keyboard_char_wait();
    if (c==3)
    { line[0]=3;
      line[1]=0;
      return line; }
    if (c==27)
    { if (esc==0)
        esc=1;
      continue; }
    else if (esc==1 && c==91)
    { esc=2;
      continue; }
    else if (esc==2)
    { char x[4];
      x[0]=27;
      x[1]=91;
      x[2]=c;
      x[3]=0;
      esc=0;
      if (c=='B')                   // down
      { int oldlen=len;
        write(1, "\r  ", 3);
        for (int i=0; i<=len; i+=1)
          write(1, " ", 1);
        if (histpos>0)
          histpos-=1;
        strcpy(line, hist.see(histpos).c_str());
        len=strlen(line);
        pos=len;
        printf("\r< %s", line);
        fflush(stdout); }
      if (c=='A')                  // up
      { int oldlen=len;
        write(1, "\r  ", 3);
        for (int i=0; i<=len; i+=1)
          write(1, " ", 1);
        if (histpos<=hist.num)
          histpos+=1;
        strcpy(line, hist.see(histpos).c_str());
        len=strlen(line);
        pos=len;
        printf("\r< %s", line);
        fflush(stdout); }
      if (c=='C')                  // right
      { if (pos<len)
        { write(1, x, 4);
          pos+=1; } }
      if (c=='D')                  // left
      { if (pos>0)
        { write(1, x, 4);
          pos-=1; } }
      continue; }
    esc=0;
    if (c==8)
    { if (pos>0)
      { pos-=1;
        len-=1;
        write(1, "\b \b", 3);
        for (int i=pos; i<len; i+=1)
        { line[i]=line[i+1];
          write(1, &line[i], 1); }
        line[len]=0;
        for (int i=pos; i<len; i+=1)
          write(1, "\b", 1); } }
    else if (c==10 || len>298)
    { line[len]=0;
      write(1, "\n", 1);
      string r = line;
      if (histpos==0)
        hist.add(line);
      else
        r=hist.take(histpos);
      return r; }
    else
    { if (pos==len)
      { line[len]=c;
        write(1, &c, 1);
        pos+=1;
        len+=1;
        line[len]=0; }
      else
      { for (int i=len; i>pos; i-=1)
          line[i]=line[i-1];
        line[pos]=c;
        write(1, line+pos, len-pos+1);
        for (int i=pos; i<len; i+=1)
          write(1, "\b", 1);
        len+=1;
        pos+=1;
        line[len]=0; } } } }

void init_opcodes()
{ for (int i=0; i<128; i+=1)
  { opcodes[i]="ERROR";
    opdata[i].name="ERROR";
    opdata[i].opcode=0; }
  for (int i=0; 1; i+=1)
  { if (opload[i].name==NULL)
      break;
    if (opload[i].name[0]!='.')
    { int p=opload[i].opcode;
      opcodes[p]=opload[i].name;
      opdata[p].name=opload[i].name;
      opdata[p].opcode=opload[i].opcode;
      opdata[p].kind=opload[i].kind; } }
  for (int i=0; 1; i+=1)
  { if (dollars[i].name==NULL)
      break;
    if (dollars[i].kind==OT_COND)
      condname[dollars[i].opcode]=dollars[i].name; } }

int asslook(string s)
{ for (int i=0; 1; i+=1)
  { if (opload[i].name==NULL)
      return -1;
    if (opload[i].name==s)
      return opload[i].opcode; } }

bool ends_with(string large, string small)
{ return large.substr(large.length()-small.length())==small; }

bool ends_with(const char * whole, const char * part)
{ int lw=strlen(whole), lp=strlen(part);
  if (lp>lw)
    return 0;
  if (strcasecmp(whole+lw-lp, part)==0)
    return 1;
  return 0; }

bool begins_with(const char * whole, const char * part)
{ int lw=strlen(whole), lp=strlen(part);
  if (lp>lw)
    return 0;
  for (int i=0; i<lp; i+=1)
  { if (toupper(whole[i])!=toupper(part[i]))
      return 0; }
  return 1; }

bool begins_with(string large, string small)
{ return large.substr(0, small.length())==small; }

int limited_strcpy(char * dest, const char * src, int maxlen)
{ for (int i=0; i<maxlen; i+=1)
  { char c = src[i];
    dest[i]=c;
    if (c==0) return i; }
  dest[maxlen]=0;
  return maxlen; }

int char_to_int(char c, int base)
{ int val=-1;
  if (c>='0' && c<='9')
    val=c-'0';
  else if (c>='A' && c<='F')
    val=c-'A'+10;
  else if (c>='a' && c<='f')
    val=c-'a'+10;
  if (val>=base)
    return -1;
  return val; }

bool matchbeginning(string small, string big)
{ int smalllen=small.length(), biglen=big.length();
  if (biglen<smalllen)
    return false;
  for (int i=0; i<smalllen; i+=1)
  { if (toupper(small[i]) != toupper(big[i]))
      return false; }
  return true; }

int findpos(char wanted, char * bigstring)
{ for (int i=0; 1; i+=1)
  { char c=bigstring[i];
    if (c==0) break;
    if (c==wanted) return i; }
  return -1; }

int posof(char c, const char * s)
{ for (int i=0; 1; i+=1)
  { if (s[i]==0) return -1;
    if (s[i]==c) return i; } }

string afterdot(const char * s)
{ for (int i=0; 1; i+=1)
  { if (s[i]==0) return "";
    if (s[i]=='.') return s+i+1; } }

static const char * possdigits = "0123456789ABCDEF";

int basefor(string str)
{ int base=10;
  const char * s = str.c_str();
  if (s[0]=='0')
  { char s1=s[1];
    if (s1>='a' && s1<='z') s1-='a'-'A';
    if (s1=='B')
    { base=2;
      s+=2; }
    else if (s1=='D')
      s+=2;
    else if (s1=='X')
    { base=16;
      s+=2; }
    else if (s1=='O')
    { base=8;
      s+=2; }
    else if (s1==0)
      return 10;
    else
      s+=1; }
  if (s[0]==0)
    return 0;
  for (int i=0; 1; i+=1)
  { char si = s[i];
    if (si==0)
      return base;
    if (si>='a' && si<='z') si-='a'-'A';
    int pos=posof(si, possdigits);
    if (pos<0)
    { if (si=='H' && s[i+1]==0) return base;
      if (si=='L' && s[i+1]==0) return base;
      return 0; }
    if (pos>=base)
      return 0; } }

int valofstr(string str, int base)
{ const char * s = str.c_str();
  if (base==0)
    base=basefor(str);
  char s1=s[1];
  if (s1>='a' && s1<='z') s1-='a'-'A';
  if (s[0]=='0' && (s1=='B' || s1=='D' || s1=='O' || s1=='X')) s+=2;
  int val=0;
  for (int i=0; 1; i+=1)
  { char si=s[i];
    if (si==0) return val;
    if (si=='H' || si=='h') return (val>>16)&0xFFFF;
    if (si=='L' || si=='l') return val&0xFFFF;
    val = val * base + posof(si, possdigits); }
  return val; }

static const char * dow[] = { "sun", "mon", "tue", "wed", "thu", "fri", "sat" },
                  * moy[] = { "jan", "feb", "mar", "apr", "may", "jun",
                              "jul", "aug", "sep", "oct", "nov", "dec" };
                              
char * newfilename(const char * ext)
{ static char name[100];
  time_t tt=time(NULL);
  struct tm * t = localtime(&tt);
  sprintf(name, "%02d%02d%s%02d%s%04d.%s",
          t->tm_hour, t->tm_min, dow[t->tm_wday], t->tm_mday, moy[t->tm_mon], t->tm_year+1900, ext);
  return name; }
                              
char * betterctime(time_t tt)
{ static char name[100];
  struct tm * t = localtime(&tt);
  sprintf(name, "%02d:%02d %s %2d %s %d",
                t->tm_hour, t->tm_min, dow[t->tm_wday], t->tm_mday, moy[t->tm_mon], t->tm_year+1900);
  return name; }

