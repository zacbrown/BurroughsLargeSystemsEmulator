#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string>
#include <vector>

struct symboltable
{ static const int size=1063;
  struct entry
  { string name;
    int value;
    entry * next;
    entry(string n, int v, entry * e); };
  entry * table[size];
  int hash(string s);
  bool lookup(string n, int & v);
  bool define(string n, int v);
  symboltable(); };

symboltable::entry::entry(string n, int v, entry * e)
{ name=n;
  value=v;
  next=e; }

symboltable::symboltable()
{ for (int i=0; i<size; i+=1)
    table[i]=NULL; }

int symboltable::hash(string s)
{ int h=1238912, n=s.length();
  for (int i=0; i<n; i+=1)
    h=h*169+s[i];
  if (h<0)
    h=-h;
  return h%size; }

bool symboltable::lookup(string n, int & v)
{ int h=hash(n);
  entry * p = table[h];
  while (p!=NULL && p->name!=n)
    p=p->next;
  if (p==NULL)
    return false;
  v=p->value;
  return true; }

bool symboltable::define(string n, int v)
{ int h=hash(n);
  entry * p = table[h];
  while (p!=NULL && p->name!=n)
    p=p->next;
  if (p==NULL)
  { table[h]=new entry(n, v, table[h]);
    return true; }
  p->value=v;
  return false; }

struct linker
{ vector <string> filenames;
  vector <FILE *> files;
  vector <int> lengths;
  vector <int> inodes;
  string exefilename;
  FILE * exefile;
  int * whole;
  bool list;
  symboltable symtab;
  linker();
  void link(); };

linker::linker()
{ list=false; }

int read(FILE * f)
{ int result;
  fread(&result, sizeof(result), 1, f);
  return result; }

string readstring(FILE * f)
{ string s="";
  int val, shift=24;
  while (true)
  { if (shift==24)
      val=read(f);
    char c = (char)((val>>shift)&0xFF);
    if (c==0)
      break;
    s+=c;
    shift-=8;
    if (shift<0)
      shift=24; }
  return s; }

void linker::link()
{ int totallength=0;
  bool ok=true;
  for (int i=0; i<filenames.size() && ok; i+=1)
  { FILE * f = files[i];
    int nlink=read(f);
    if (list && nlink!=0)
    { printf("input file '%s'\n", filenames[i].c_str());
      printf("  %d additional object files needed\n", nlink); }
    for (int j=0; j<nlink; j+=1)
    { string fname=readstring(f);
      bool hasdot=false;
      for (int h=0; h<fname.length(); h+=1)
      { if (fname[h]=='.')
        { hasdot=true;
          break; } }
      if (!hasdot)
        fname+=".obj";
      if (list)
        printf("    %s\n", fname.c_str());
      struct stat sb;
      stat(fname.c_str(), &sb);
      bool repeated=false;
      for (int j=0; j<inodes.size(); j+=1)
      { if (inodes[j]==sb.st_ino)
        { repeated=true;
          break; } }
      if (repeated)
        continue;
      filenames.push_back(fname);
      inodes.push_back(sb.st_ino);
      FILE * f = fopen(fname.c_str(), "rb");
      if (f==NULL)
      { fprintf(stderr, "linker: can not read '%s'\n", fname.c_str());
        ok=false;
        break; }
      files.push_back(f); } }
  for (int i=0; i<filenames.size() && ok; i+=1)
  { FILE * f = files[i];
    int nexp=read(f);
    if (list && nexp!=0)
    { printf("input file '%s'\n", filenames[i].c_str());
      printf("  %d exports\n", nexp); }
    for (int j=0; j<nexp; j+=1)
    { string ename=readstring(f);
      int value=read(f);
      bool ok=symtab.define(ename, totallength+value);
      if (!ok)
      { fprintf(stderr, "Multiple definitions for symbol '%s'\n", ename.c_str());
        break; }
      if (list)
        printf("    %s = %08X\n", ename.c_str(), totallength+value); }
    int len=read(f);
    lengths.push_back(len);
    totallength+=len; }
  whole=new int[totallength];
  int start=0;
  for (int i=0; i<filenames.size(); i+=1)
  { FILE * f = files[i];
    int len=lengths[i];
    if (list)
    { printf("input file '%s'\n", filenames[i].c_str());
      printf("  loading %d words\n", len); }
    for (int j=0; j<len; j+=1)
      whole[start+j]=read(f);
    start+=len; }
  start=0;
  for (int i=0; i<filenames.size(); i+=1)
  { FILE * f = files[i];
    int nimp=read(f);
    if (list && nimp!=0)
    { printf("input file '%s'\n", filenames[i].c_str());
      printf("  %d imports\n", nimp); }
    for (int j=0; j<nimp; j+=1)
    { string iname=readstring(f);
      int chain=read(f), value=0;
      ok=symtab.lookup(iname, value);
      if (!ok)
      { fprintf(stderr, "No definition found for symbol '%s'\n", iname.c_str());
        break; }
      if (list)
        printf("    %s = %08X\n", iname.c_str(), value);
      while (chain!=0)
      { int addr=start+chain;
        int orig=whole[addr];
        chain=orig&0xFFFF;
        whole[addr]=(orig&0xFFFF0000)|((value-addr-1)&0xFFFF); } }
    start+=lengths[i]; }
  if (list)
  { printf("output file '%s'\n", exefilename.c_str());
    printf("  writing %d words\n", totallength); }
  fwrite(whole, sizeof(whole[0]), totallength, exefile); }

void prepare_filename(string givenname, string inext, string & infilename, string & basefilename)
{ int dotpos=-1;
  for (int i=0; i<givenname.length(); i+=1)
  { if (givenname[i]=='/' || givenname[i]=='\\')
      dotpos=-1;
    else if (givenname[i]=='.')
      dotpos=i; }
  if (dotpos>=0)
  { infilename=givenname;
    basefilename=givenname.substr(0, dotpos); }
  else
  { infilename=givenname+"."+inext;
    basefilename=givenname; } }

void main(int argc, char * argv[])
{ linker L;
  string exefilename="", firstname="", x;
  bool ok=true;
  char nextis=0;
  for (int i=1; i<argc; i+=1)
  { if (argv[i][0]=='-')
    { string option=argv[i]+1;
      nextis=0;
      if (option=="o")
      { nextis='o'; }
      else if (option=="l" || option=="list")
        L.list=true;
      else
      { fprintf(stderr, "Bad option '%s' on command line\n", argv[i]);
        ok=false;
        break; } }
    else if (nextis=='o')
    { prepare_filename(argv[i], "exe", exefilename, x); }
    else
    { string ifname, basename;
      prepare_filename(argv[i], "obj", ifname, basename);
      if (firstname=="")
        firstname=basename;
      struct stat sb;
      stat(ifname.c_str(), &sb);
      bool repeated=false;
      for (int j=0; j<L.inodes.size(); j+=1)
      { if (L.inodes[j]==sb.st_ino)
        { repeated=true;
          break; } }
      if (repeated)
        continue;
      L.filenames.push_back(ifname);
      L.inodes.push_back(sb.st_ino);
      FILE * f = fopen(ifname.c_str(), "rb");
      if (f==NULL)
      { fprintf(stderr, "linker: can not read '%s'\n", ifname.c_str());
        ok=false;
        break; }
      L.files.push_back(f); } }
  if (ok && firstname=="")
  { fprintf(stderr, "linker: no input files given\n");
    ok=false; }
  if (exefilename=="")
    exefilename=firstname+".exe";
  L.exefilename=exefilename;
  L.exefile = fopen(exefilename.c_str(), "wb");
  if (L.exefile==NULL)
  { fprintf(stderr, "linker: can not create exe file '%s'\n", exefilename.c_str());
    ok=false; }
  if (ok)
  { L.link();
    for (int i=0; i<L.files.size(); i+=1)
      fclose(L.files[i]);
    fclose(L.exefile); }
  if (L.list)
    printf("all done\n"); }

