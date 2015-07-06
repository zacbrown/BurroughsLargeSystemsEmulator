#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../useful.h"
#include "../codes.h"

#define strtof(A,B) atof(A)

const char end_of_file_char = (char)26;
const string end_of_file = "end of file";
const string end_of_line = "end of line";
const int max_line_length = 1000;

const char *a_number = "number", *a_name = "name", *an_opcode = "opcode",
    *a_command = "command", *a_register = "register", *the_end = "end",
    *a_comma = "comma", *an_open_bracket = "open", *a_close_bracket = "close",
    *a_plus_sign = "plus", *a_minus_sign = "minus", *a_string = "string",
    *an_equals_sign = "equals", *a_colon = "colon", *something_else = "other",
    *a_bad_number = "bad number", *a_label = "label", *an_import = "imported name",
    *a_constant = "named constant", *a_cond_code = "condition code", *a_float = "float";

struct symbol
{
    string name;
    const char* kind;
    int value;
    symbol* next_same_hash;
    symbol(string n, symbol* nsh = NULL, const char* k = a_name, int v = 0);
};

symbol::symbol(string n, symbol* nsh, const char* k, int v)
{
    name = n;
    kind = k;
    value = v;
    next_same_hash = nsh;
}

struct symboltable
{
    static const int tabsize = 2013;
    vector<symbol *> import_list, export_list;
    symboltable();
    symbol* table[tabsize];
    int hash(string s);
    symbol* find(string s);
    bool define(symbol* e, const char* k, int v);
    void insert(string s, const char* k, int v);
    int num_imports();
    void note_import(symbol* e);
    symbol* get_import(int i);
    int num_exports();
    void note_export(symbol* e);
    symbol* get_export(int i);
};

symboltable::symboltable()
{
    for (int i = 0; i < tabsize; i += 1)
        table[i] = NULL;
    for (int i = 0; i < 128; i += 1)
        insert(instruction_name[i], an_opcode, i);
    for (int i = 0; i < num_fakecodes; i += 1)
        insert(fakecodes[i], a_number, i);
    for (int i = 0; i < num_iocommands; i += 1)
        insert(iocommands[i], a_number, i);
    for (int i = 0; i < num_condcodes; i += 1)
        insert(condcodes[i], a_cond_code, i);
    for (int i = 0; i < num_flags; i += 1)
        insert(flags[i], a_number, i);
    for (int i = 0; i < num_interrupts; i += 1)
        insert(short_ints[i], a_number, i);
    for (int i = 0; i < num_pageprots; i += 1)
        insert(pageprots[i], a_number, 1 << i);
    for (int i = 0; i < num_specregs; i += 1)
        insert(specregs[i], a_number, i);
    for (int i = 0; i < 16; i += 1)
        insert(register_name[i], a_register, i);
}

int symboltable::hash(string s)
{
    int h = 1321, len = s.length();
    for (int i = 0; i < len; i += 1)
        h = h * 69 + s[i];
    if (h < 0) h = -h;
    return h % tabsize;
}

symbol* symboltable::find(string s)
{
    int pos = hash(s);
    symbol* e = table[pos];
    while (e != NULL && e->name != s)
        e = e->next_same_hash;
    if (e == NULL)
    {
        e = new symbol(s, table[pos]);
        table[pos] = e;
    }
    return e;
}

bool symboltable::define(symbol* e, const char* k, int v)
{
    bool ok = e->kind == a_name;
    if (e->kind != an_import && k != an_import)
        e->value = v;
    e->kind = k;
    if (k == an_import)
        import_list.push_back(e);
    return ok;
}

void symboltable::insert(string s, const char* k, int v)
{
    symbol* e = find(s);
    e->kind = k;
    e->value = v;
}

int symboltable::num_imports()
{
    return import_list.size();
}

void symboltable::note_import(symbol* e)
{
    import_list.push_back(e);
}

symbol* symboltable::get_import(int i)
{
    if (i >= import_list.size())
        return NULL;
    return import_list[i];
}

int symboltable::num_exports()
{
    return export_list.size();
}

void symboltable::note_export(symbol* e)
{
    export_list.push_back(e);
}

symbol* symboltable::get_export(int i)
{
    if (i >= export_list.size())
        return NULL;
    return export_list[i];
}

struct filereader
{
    string filename;
    FILE* fi;
    int linenumber, linelength, charnumber, skipstart;
    filereader* previous;
    bool ok;
    char current[max_line_length + 2];
    filereader();
    bool push(string filename);
    bool pop();
    void open(string fname);
    void rewind();
    char read();
    char readstringchar();
    bool get_next_line();
    void back();
    bool finished();
    bool end_line();
    string skipped_text();
    ~filereader();
};

filereader::filereader()
{
    ok = false;
    linenumber = 0;
    linelength = 0;
    charnumber = 0;
    skipstart = 0;
    current[0] = 0;
    previous = NULL;
}

bool filereader::push(string newfile)
{
    filereader* old = new filereader;
    old->filename = filename;
    old->fi = fi;
    old->linenumber = linenumber;
    old->previous = previous;
    previous = old;
    int dotpos = -1;
    for (int i = newfile.length() - 1; i > 0; i -= 1)
        if (newfile[i] == '.')
        {
            dotpos = i;
            break;
        }
    if (dotpos == -1)
        newfile += ".ash";
    open(newfile);
    if (!ok)
    {
        filename = old->filename;
        fi = old->fi;
        linenumber = old->linenumber;
        previous = old->previous;
        old->previous = NULL;
        delete old;
    }
    return ok;
}

bool filereader::pop()
{
    if (previous == NULL)
        return false;
    filereader* old = previous;
    filename = old->filename;
    fi = old->fi;
    linenumber = old->linenumber;
    previous = old->previous;
    old->previous = NULL;
    delete old;
    linelength = 0;
    charnumber = 0;
    skipstart = 0;
    current[0] = 0;
    ok = true;
    return true;
}

filereader::~filereader()
{
    delete previous;
    if (ok)
        fclose(fi);
    ok = false;
}

void filereader::open(string fname)
{
    filename = fname;
    fi = fopen(fname.c_str(), "r");
    ok = (fi != NULL);
    linenumber = 0;
    linelength = 0;
    charnumber = 0;
    skipstart = 0;
    current[0] = 0;
}

void filereader::rewind()
{
    ::rewind(fi);
    linenumber = 0;
    linelength = 0;
    charnumber = 0;
    current[0] = 0;
}

bool filereader::end_line()
{
    int start = charnumber;
    charnumber = linelength;
    for (int i = start; i < linelength; i += 1)
        if (current[i] > ' ')
        {
            skipstart = i;
            return false;
        }
    skipstart = linelength - 1;
    return true;
}

string filereader::skipped_text()
{
    current[linelength - 1] = 0;
    return current + skipstart;
}

bool filereader::finished()
{
    return current[0] == end_of_file_char;
}

bool filereader::get_next_line()
{
    char* s = fgets(current, max_line_length, fi);
    charnumber = 0;
    skipstart = 0;
    if (s == NULL)
    {
        current[0] = end_of_file_char;
        current[1] = 0;
        linelength = 1;
        return false;
    }
    else
    {
        linelength = strlen(current);
        if (current[linelength - 1] != '\n')
        {
            current[linelength] = '\n';
            linelength += 1;
            current[linelength] = 0;
        }
        linenumber += 1;
        return true;
    }
}

char filereader::read()
{
    if (charnumber < 0)
    {
        charnumber += 1;
        return ' ';
    }
    if (charnumber >= linelength)
        return '\n';
    char c = current[charnumber];
    charnumber += 1;
    if (c == '\t' || c == '\r')
        c = ' ';
    return c;
}

char filereader::readstringchar()
{
    char c = read();
    if (c == '\\')
    {
        c = read();
        if (c < ' ')
            return c;
        back();
        c = '\\';
    }
    return c;
}

void filereader::back()
{
    charnumber -= 1;
}

struct reader
{
    filereader fr;
    string previous, current;
    bool backed;
    reader();
    void open(string fname);
    bool push(string filename);
    bool pop();
    bool including();
    string read();
    bool ok();
    void back();
    void rewind();
    bool get_next_line();
    bool finished();
    bool end_line();
    string skipped_text();
    int getlinenumber();
    string getwholeline();
};

reader::reader()
{
    previous = "";
    current = "";
    backed = false;
}

bool reader::push(string newfile)
{
    return fr.push(newfile);
}

bool reader::pop()
{
    return fr.pop();
}

bool reader::including()
{
    return fr.previous != NULL;
}

void reader::open(string fname)
{
    fr.open(fname);
}

bool reader::ok()
{
    return fr.ok;
}

void reader::rewind()
{
    fr.rewind();
    previous = "";
    current = "";
    backed = false;
}

bool reader::get_next_line()
{
    backed = false;
    previous = "";
    current = "";
    return fr.get_next_line();
}

bool reader::end_line()
{
    bool answer = fr.end_line();
    if (backed)
        return current == end_of_line;
    return answer;
}

string reader::skipped_text()
{
    return fr.skipped_text();
}

bool reader::finished()
{
    return fr.finished();
}

void reader::back()
{
    backed = true;
}

string reader::read()
{
    if (backed)
    {
        backed = false;
        return current;
    }
    previous = current;
    current = "";
    char c = ' ';
    while (c == ' ')
        c = fr.read();
    if (c == '\n')
    {
        current = end_of_line;
        return current;
    }
    if (c == end_of_file_char)
    {
        current = end_of_file;
        return current;
    }
    if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_' || c == '%' || c == '.' ||
        c == '$' || c == '@' || c == '#')
    {
        while (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' ||
            c == '_' || c == '%' || c == '.' || c == '$' || c == '@' || c == '#')
        {
            if (c >= 'a' && c <= 'z')
                c += 'A' - 'a';
            current += c;
            c = fr.read();
        }
        fr.back();
        return current;
    }
    if (c >= '0' && c <= '9')
    {
        bool alldec = true;
        while (c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z')
        {
            if (c < '0' || c > '9')
                alldec = false;
            current += (char)(toupper(c));
            c = fr.read();
        }
        if (!alldec || c != '.')
        {
            fr.back();
            return current;
        }
        current += '.';
        c = fr.read();
        {
            while (c >= '0' && c <= '9')
            {
                current += c;
                c = fr.read();
            }
        }
        if (c == 'e' || c == 'E')
        {
            current += 'e';
            c = fr.read();
            if (c == '+' || c == '-')
            {
                current += c;
                c = fr.read();
                while (c >= '0' && c <= '9')
                {
                    current += c;
                    c = fr.read();
                }
            }
        }
        if (c == 'h' || c == 'H')
            current += 'H';
        else if (c == 'l' || c == 'L')
            current += 'L';
        else
            fr.back();
        string dot = ".";
        current = dot + current;
        return current;
    }
    if (c == '\"' || c == '\'')
    {
        char end = c, prev = c;
        current = c;
        c = fr.readstringchar();
        while (!(c == end && prev != '\\' || c == '\n'))
        {
            current += c;
            prev = c;
            c = fr.readstringchar();
        }
        if (c == '\n')
            fr.back();
        return current;
    }
    if (c == '/')
    {
        c = fr.read();
        if (c != '/')
        {
            fr.back();
            current = "/";
            return current;
        }
        while (c != '\n')
            c = fr.read();
        current = end_of_line;
        return current;
    }
    current = c;
    return current;
}

int reader::getlinenumber()
{
    return fr.linenumber;
}

string reader::getwholeline()
{
    if (fr.current[0] == end_of_file_char)
        return "\n";
    return fr.current;
}

void prepare_filenames(string givenname, string inext, string& infilename, string& basefilename)
{
    int dotpos = -1;
    for (int i = 0; i < givenname.length(); i += 1)
    {
        if (givenname[i] == '/' || givenname[i] == '\\')
            dotpos = -1;
        else if (givenname[i] == '.')
            dotpos = i;
    }
    if (dotpos >= 0)
    {
        infilename = givenname;
        basefilename = givenname.substr(0, dotpos);
    }
    else
    {
        infilename = givenname + "." + inext;
        basefilename = givenname;
    }
}

static symbol dummy("???", NULL, something_else);

struct assembler
{
    symboltable symtab;
    reader rdr;
    FILE* objectfile;
    int pass, location, error_count, output_type, produce_listing;
    bool error_on_line;
    assembler();
    void start_pass(int n);
    symbol* identify(string s, symbol& dflt = dummy);
    symbol* read(symbol& dflt = dummy);
    vector<string> links;
    void note_link(string s);
    bool get_number(int& value, bool canbefloat = false);
    void assemble_one_line();
    bool assemble(string fname);
    void deposit(int v);
    void deposit(string s);
    void produce(int v, bool hidelow = false);
    void error(const char* format, ...);
};

assembler::assembler()
{
    error_count = 0;
    error_on_line = false;
    output_type = 0;
    produce_listing = 0;
}

void assembler::start_pass(int n)
{
    pass = n;
    if (pass > 1)
        rdr.rewind();
    location = 0;
    if (output_type == 0)
        output_type = 'O';
}

void assembler::error(const char* format, ...)
{
    if (error_on_line)
        return;
    fprintf(stderr, "line %d: %s\n", rdr.getlinenumber(), rdr.getwholeline().c_str());
    va_list args;
    va_start(args, format);
    fprintf(stderr, "**** bad! ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    error_on_line = true;
    error_count += 1;
}

void assembler::note_link(string s)
{
    links.push_back(s);
}

symbol* assembler::identify(string s, symbol& ans)
{
    ans.name = s;
    ans.value = 0;
    char c = s[0];
    if (s == end_of_line)
        ans.kind = the_end;
    else if (c == '\"')
        ans.kind = a_string;
    else if (c == ',')
        ans.kind = a_comma;
    else if (c == ':')
        ans.kind = a_colon;
    else if (c == '=')
        ans.kind = an_equals_sign;
    else if (c == '+')
        ans.kind = a_plus_sign;
    else if (c == '-')
        ans.kind = a_minus_sign;
    else if (c == '[')
        ans.kind = an_open_bracket;
    else if (c == ']')
        ans.kind = a_close_bracket;
    else if (c == '.')
    {
        c = s[1];
        if (c >= '0' && c <= '9')
        {
            s = s.substr(1);
            ans.name = s;
            int last = s.length() - 1;
            if (last > 0 && (s[last] == 'H' || s[last] == 'L'))
                s = s.substr(0, last);
            union
            {
                float fv;
                int iv;
            } converter;
            converter.fv = strtof(s.c_str(), NULL);
            ans.value = converter.iv;
            ans.kind = a_float;
        }
        else
            ans.kind = a_command;
    }
    else if (c == '\'')
    {
        int len = s.length();
        ans.value = 0;
        for (int i = 1; i < len; i += 1)
        {
            char c = s[i];
            if (c == '\\')
            {
                i += 1;
                c = backstroke_char(s[i]);
            }
            ans.value = ans.value * 256 + c;
        }
        ans.kind = a_number;
    }
    else if (c >= '0' && c <= '9')
    {
        int last = s.length() - 1;
        if (last > 0 && (s[last] == 'H' || s[last] == 'L'))
            s = s.substr(0, last);
        bool ok = string_to_int(s, ans.value);
        if (ok)
            ans.kind = a_number;
        else
            ans.kind = a_bad_number;
    }
    else if (c == '#')
    {
        s = s.substr(1);
        symbol* e = symtab.find(s);
        if (pass == 2 && e->kind == a_name)
            error("There is nothing matching '%s' in the symbol table\n", s.c_str());
        ans.kind = a_number;
        ans.value = e->value;
    }
    else if (c >= 'A' && c <= 'Z' || c == '_' || c == '%' || c == '$' || c == '@')
    {
        symbol* e = symtab.find(s);
        return e;
    }
    else
        ans.kind = something_else;
    return & ans;
}

symbol* assembler::read(symbol& ans)
{
    string s = rdr.read();
    return identify(s, ans);
}

bool assembler::get_number(int& val, bool canbefloat)
{
    int sign = 1;
    symbol* item = read();
    if (item->kind == a_plus_sign || item->kind == a_minus_sign)
    {
        if (item->kind == a_minus_sign)
            sign = -1;
        item = read();
    }
    if (item->kind == a_label || item->kind == an_import)
    {
        error("'%s' is a label, it represents an unknown location and can not act as a number\n", item->name.c_str());
        return false;
    }
    if (item->kind == a_name && pass != 1)
    {
        error("The name '%s' is never defined\n", item->name.c_str());
        return false;
    }
    if (item->kind == a_number || item->kind == a_name || item->kind == a_constant || item->kind == an_opcode)
    {
        val = sign * item->value;
        char last = item->name[item->name.length() - 1];
        if (last == 'L')
            val = val & 0xFFFF;
        else if (last == 'H')
            val = (val >> 16) & 0xFFFF;
        return true;
    }
    if (item->kind == a_float)
    {
        if (!canbefloat)
        {
            error("floating point numbers are not acceptable here");
            return false;
        }
        if (sign < 0)
        {
            union
            {
                float fv;
                int iv;
            } converter;
            converter.iv = item->value;
            converter.fv *= -1;
            val = converter.iv;
        }
        else
            val = item->value;
        char last = item->name[item->name.length() - 1];
        if (last == 'L')
            val = val & 0xFFFF;
        else if (last == 'H')
            val = (val >> 16) & 0xFFFF;
        return true;
    }
    error("This %s '%s' does not make a proper value\n", item->kind, item->name.c_str());
    return false;
}

void assembler::deposit(int v)
{
    fwrite(&v, sizeof(v), 1, objectfile);
}

void assembler::deposit(string s)
{
    int val = 0, len = s.length(), shift = 24;
    for (int i = 0; i <= len; i += 1)
    {
        unsigned char c = s[i];
        val |= c << shift;
        shift -= 8;
        if (shift < 0)
        {
            deposit(val);
            shift = 24;
            val = 0;
        }
    }
    if (shift != 24)
        deposit(val);
}

void assembler::produce(int v, bool hidelow)
{
    if (pass == 1)
    {
        location += 1;
        return;
    }
    if (produce_listing)
    {
        if (hidelow)
        {
            printf("   %04X: %04X---- ", location, (v >> 16) & 0x0000FFFF);
            if (produce_listing == 1)
            {
                for (int shift = 31; shift >= 16; shift -= 1)
                {
                    printf("%d", ((v >> shift) & 1));
                    if ((shift & 7) == 0)
                        printf(" ");
                }
                printf("-------- -------- ");
            }
        }
        else
        {
            printf("   %04X: %08X ", location, v);
            if (produce_listing == 1)
            {
                for (int shift = 31; shift >= 0; shift -= 1)
                {
                    printf("%d", ((v >> shift) & 1));
                    if ((shift & 7) == 0)
                        printf(" ");
                }
            }
        }
        printf("\n");
    }
    deposit(v);
    location += 1;
}

void process_command(assembler& a, reader& r, string name)
{
    if (name == ".MAKEEXE")
    {
        a.output_type = 'E';
        if (a.symtab.num_imports() != 0 || a.symtab.num_exports() != 0)
            a.error("A simple EXE file can not have exports or imports\n");
    }
    else if (name == ".MAKEOBJ")
        a.output_type = 'O';
    else if (name == ".DATA")
    {
        if (a.rdr.including())
        {
            a.error(".INCLUDEd definition files may not contribute data\n");
            return;
        }
        while (true)
        {
            int num;
            bool ok = a.get_number(num, true);
            if (!ok)
                return;
            a.produce(num);
            symbol* item = a.read();
            if (item->kind == the_end)
                break;
            if (item->kind != a_comma)
            {
                a.error("stray symbol '%s'\n", item->name.c_str());
                return;
            }
        }
    }
    else if (name == ".SPACE")
    {
        if (a.rdr.including())
        {
            a.error(".INCLUDEd definition files may not contribute data\n");
            return;
        }
        int num;
        bool ok = a.get_number(num);
        if (!ok)
            return;
        if (a.pass == 1)
        {
            a.location += num;
            return;
        }
        for (int i = 0; i < num; i += 1)
        {
            a.deposit(0);
            a.location += 1;
        }
    }
    else if (name == ".ALIGN")
    {
        if (a.rdr.including())
        {
            a.error(".INCLUDEd definition files may not contribute data\n");
            return;
        }
        int num;
        bool ok = a.get_number(num);
        if (!ok)
            return;
        int over = a.location % num;
        if (over == 0)
            return;
        int needed = num - over;
        if (a.pass == 1)
        {
            a.location += needed;
            return;
        }
        for (int i = 0; i < needed; i += 1)
        {
            a.deposit(0);
            a.location += 1;
        }
    }
    else if (name == ".INCLUDE")
    {
        symbol* item = a.read();
        string s;
        if (item->kind == a_string)
            s = item->name.substr(1);
        else if (item->kind == a_label || item->kind == a_name || item->kind == a_constant ||
            item->kind == an_import || item->kind == an_opcode)
            s = item->name;
        else
        {
            a.error("This %s '%s' does not work as a file name\n", item->kind, item->name.c_str());
            return;
        }
        bool ok = a.rdr.push(s);
        if (!ok)
        {
            a.error("The file '%s' could not be accessed\n", s.c_str());
            return;
        }
    }
    else if (name == ".STRING")
    {
        if (a.rdr.including())
        {
            a.error(".INCLUDEd definition files may not contribute data\n");
            return;
        }
        symbol* item = a.read();
        if (item->kind != a_string)
        {
            a.error("This %s '%s' is not a string\n", item->kind, item->name.c_str());
            return;
        }
        string s = item->name;
        int value = 0, shift = 0, length = s.length();
        for (int i = 1; i <= length; i += 1)
        {
            unsigned char c = s[i];
            if (c == '\\')
            {
                i += 1;
                c = backstroke_char(s[i]);
            }
            value |= c << shift;
            if (shift == 24)
            {
                a.produce(value);
                value = 0;
                shift = 0;
            }
            else
                shift += 8;
        }
        if (shift != 0)
            a.produce(value);
    }
    else if (name == ".ADDRESS")
    {
        if (a.rdr.including())
        {
            a.error(".INCLUDEd definition files may not contribute data\n");
            return;
        }
        symbol* item = a.read();
        if (item->kind == a_label)
        {
            a.produce(item->value - a.location - 1);
        }
        else if (item->kind == an_import)
        {
            int n = item->value;
            item->value = a.location;
            a.produce(n, true);
        }
        else if (item->kind == an_import || item->kind == a_name)
        {
            int n = item->value;
            item->value = a.location;
            a.produce(n, true);
            if (a.pass == 2)
                a.error("The label '%s' is never defined\n", item->name.c_str());
        }
    }
    else if (name == ".IMPORT")
    {
        while (true)
        {
            symbol* item = a.read();
            if (item->kind == a_label || item->kind == a_constant)
            {
                a.error("The name '%s' has been defined, it can not also be imported\n", item->name.c_str());
                return;
            }
            else if (item->kind == a_name)
            {
                if (a.pass == 1)
                {
                    a.symtab.note_import(item);
                    item->kind = an_import;
                    item->value = 0;
                }
            }
            else if (item->kind != an_import)
            {
                a.error("This %s '%s' is not a valid label for importing\n", item->kind, item->name.c_str());
                return;
            }
            item = a.read();
            if (item->kind == the_end)
                break;
            if (item->kind != a_comma)
            {
                a.error("stray symbol '%s'\n", item->name.c_str());
                return;
            }
        }
    }
    else if (name == ".EXPORT")
    {
        while (true)
        {
            symbol* item = a.read();
            if (item->kind == a_label || item->kind == a_constant)
            {
                if (a.pass == 1)
                    a.symtab.note_export(item);
            }
            else if (item->kind == a_name)
            {
                if (a.pass == 1)
                    a.symtab.note_export(item);
                else
                {
                    a.error("The label '%s' is never defined\n", item->name.c_str());
                    return;
                }
            }
            else
            {
                a.error("This %s '%s' is not a valid label name for exporting\n", item->kind, item->name.c_str());
                return;
            }
            item = a.read();
            if (item->kind == the_end)
                break;
            if (item->kind != a_comma)
            {
                a.error("stray symbol '%s'\n", item->name.c_str());
                return;
            }
        }
    }
    else if (name == ".LINK")
    {
        symbol* item = a.read();
        if (item->kind != a_string)
        {
            a.error("File names after .LINK must be quoted \"strings\"\n");
            return;
        }
        if (a.pass == 1)
            a.note_link(item->name);
        item = a.read();
        if (item->kind != the_end)
        {
            a.error("stray symbol '%s'\n", item->name.c_str());
            return;
        }
    }
    else
        a.error("Unrecognised command directive '%s'\n", name.c_str());
}

void assembler::assemble_one_line()
{
    error_on_line = false;
    while (true)
    {
        rdr.get_next_line();
        if (rdr.finished())
        {
            bool ok = rdr.pop();
            if (!ok)
                return;
        }
        else
            break;
    }
    if (produce_listing && pass == 2 && !rdr.including())
        printf("   %6d: %s", rdr.getlinenumber(), rdr.getwholeline().c_str());
    symbol* item = NULL;
    while (true)
    {
        item = read();
        if (item->name == end_of_file || item->kind == the_end)
            return;
        if (item->kind == a_name || item->kind == a_label || item->kind == an_import || item->kind == a_constant)
        {
            symbol* sym = item;
            item = read();
            if (item->kind == a_colon)
            {
                if (sym->kind == an_import)
                {
                    error("Label %s is imported, it can not also be defined here\n", sym->name.c_str());
                    return;
                }
                bool ok = symtab.define(sym, a_label, location);
                if (pass == 1 && !ok)
                {
                    error("Label %s has already been defined\n", sym->name.c_str());
                    return;
                }
                if (produce_listing && pass == 1)
                    printf("   %s = %08X, relative\n", sym->name.c_str(), location);
                continue;
            }
            else if (item->kind == an_equals_sign)
            {
                if (sym->kind == an_import)
                {
                    error("Label %s is imported, it can not also be defined here\n", sym->name.c_str());
                    return;
                }
                int val = 0;
                bool ok = get_number(val, true);
                if (!rdr.end_line())
                {
                    error("error on line: unexpected text '%s' after apparent end of definition\n", rdr.skipped_text().c_str());
                    ok = false;
                }
                if (ok)
                {
                    ok = symtab.define(sym, a_constant, val);
                    if (pass == 1 && !ok)
                    {
                        error("Constant %s has already been defined\n", sym->name.c_str());
                        return;
                    }
                    if (produce_listing && pass == 1)
                        printf("   %s = %08X, absolute\n", sym->name.c_str(), val);
                }
                return;
            }
            else
            {
                error("What is this %s '%s' doing here? It is not an opcode\n", item->kind, item->name.c_str());
                return;
            }
        }
        else
            break;
    }
    if (item->kind == a_command)
    {
        process_command(*this, rdr, item->name);
    }
    else if (item->kind == an_opcode)
    {
        int opcode = item->value, mainreg = 0, indexreg = 0, indirect = 0, number = 0;
        bool used_import = false;
        item = read();
        if (item->kind == a_register || item->kind == a_cond_code)
        {
            int regnum = item->value;
            item = read();
            if (item->kind == a_comma)
            {
                mainreg = regnum;
                item = read();
            }
            else
            {
                rdr.back();
                item->kind = a_register;
                item->value = regnum;
            }
        }
        if (item->kind == an_open_bracket)
        {
            indirect = 1;
            item = read();
        }
        if (item->kind == a_register)
        {
            indexreg = item->value;
            if (indexreg == 0)
                error("R0 can not be used as the secondary or index register\n");
            item = read();
            rdr.back();
            if (item->kind == a_plus_sign || item->kind == a_minus_sign)
            {
                get_number(number);
            }
            else if (item->kind == a_name || item->kind == a_number || item->kind == a_label ||
                item->kind == an_import || item->kind == a_constant)
            {
                error("+ or - required between a register and an offset\n");
            }
            else if (item->kind != a_close_bracket && item->kind != the_end)
            {
                error("In an operand, register may only be followed by +N or -N\n");
            }
        }
        else if (item->kind == a_label)
        {
            indexreg = PC;
            number = item->value - location - 1;
        }
        else if (item->kind == a_name)
        {
            if (pass == 2)
                error("The symbol '%s' was never defined\n", item->name.c_str());
            else
            {
                indexreg = PC;
                number = item->value;
                item->value = location;
            }
        }
        else if (item->kind == an_import)
        {
            indexreg = PC;
            number = item->value;
            item->value = location;
            used_import = true;
        }
        else if (item->kind == the_end)
        {
        }
        else
        {
            rdr.back();
            get_number(number, true);
        }
        item = read();
        if (item->kind == a_plus_sign || item->kind == a_minus_sign)
        {
            bool neg = item->kind == a_minus_sign;
            int extra;
            get_number(extra);
            number += extra;
        }
        else
            rdr.back();
        if (indirect)
        {
            item = read();
            if (item->kind != a_close_bracket)
            {
                error("improper operand, close bracket missing\n");
            }
            else
            {
                item = read();
            }
        }
        if (rdr.including())
        {
            error(".INCLUDEd definition files may not contribute code\n");
            return;
        }
        int instr = (opcode << 25) | (indirect << 24) | (mainreg << 20) | (indexreg << 16) | (number & 0xFFFF);
        produce(instr, used_import);
    }
    else
    {
        error("why would a line begin with with %s '%s'?\n", item->kind, item->name.c_str());
        return;
    }
    if (!rdr.end_line())
        error("error on line: unexpected text '%s' after apparent end\n", rdr.skipped_text().c_str());
}

bool assembler::assemble(string fname)
{
    string infilename, basefilename;
    prepare_filenames(fname, "ass", infilename, basefilename);
    rdr.open(infilename);
    if (!rdr.ok())
    {
        fprintf(stderr, "Assembler: Can't open file '%s'\n", infilename.c_str());
        return false;
    }
    start_pass(1);
    while (!rdr.finished())
        assemble_one_line();
    if (error_count > 0)
    {
        fprintf(stderr, "Error%s detected, assembly not completed\n", error_count > 1 ? "s" : "");
        return false;
    }
    string ext = ".exe";
    if (output_type == 'O')
        ext = ".obj";
    string objectfilename = basefilename + ext;
    objectfile = fopen(objectfilename.c_str(), "wb");
    if (objectfile == NULL)
    {
        fprintf(stderr, "Assembler: Can't create file '%s'\n", objectfilename.c_str());
        return false;
    }
    if (output_type == 'O')
    {
        int n = links.size();
        deposit(n);
        for (int i = 0; i < n; i += 1)
        {
            string s = links[i];
            if (s[0] == '\"')
                s = s.substr(1);
            deposit(s);
        }
        n = symtab.num_exports();
        deposit(n);
        for (int i = 0; i < n; i += 1)
        {
            symbol* sym = symtab.get_export(i);
            deposit(sym->name);
            deposit(sym->value);
        }
        n = symtab.num_imports();
        for (int i = 0; i < n; i += 1)
        {
            symbol* sym = symtab.get_import(i);
            sym->value = 0;
        }
    }
    if (output_type == 'O')
        deposit(location);
    start_pass(2);
    while (!rdr.finished())
        assemble_one_line();
    if (error_count > 0)
    {
        fprintf(stderr, "Error%s detected, assembly not completed\n", error_count > 1 ? "s" : "");
        fclose(objectfile);
        return false;
    }
    if (output_type == 'O')
    {
        int n = symtab.num_imports();
        deposit(n);
        for (int i = 0; i < n; i += 1)
        {
            symbol* sym = symtab.get_import(i);
            deposit(sym->name);
            deposit(sym->value);
        }
    }
    fclose(objectfile);
    return true;
}

void main(int argc, char* argv[])
{
    assembler a;
    bool ok = true;
    string filename = "";
    for (int i = 1; i < argc; i += 1)
    {
        if (argv[i][0] == '-')
        {
            string option = argv[i] + 1;
            if (option == "exe")
            {
                a.output_type = 'E';
            }
            else if (option == "obj")
            {
                a.output_type = 'O';
            }
            else if (option == "list" || option == "l")
            {
                a.produce_listing = 16;
            }
            else if (option == "listbinary" || option == "lb")
            {
                a.produce_listing = 1;
            }
            else
            {
                fprintf(stderr, "Bad option '%s' on command line\n", argv[i]);
                ok = false;
                break;
            }
        }
        else if (filename == "")
            filename = argv[i];
        else
        {
            fprintf(stderr, "Multiple file names on command line: '%s' and '%s'\n", filename.c_str(), argv[i]);
            ok = false;
            break;
        }
    }
    if (filename == "")
    {
        fprintf(stderr, "No file name on command line\n");
        ok = false;
    }
    if (ok)
        a.assemble(filename);
}
