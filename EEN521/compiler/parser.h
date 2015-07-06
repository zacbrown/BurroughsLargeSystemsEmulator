// this is parser.h
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <fstream>

using namespace std;

struct node
{
    string tag, detail, ref_var;
    int value;
    vector<node*> part;

    node* add(node* n);
    void print(int indent = 0);
    void translateexpression(int reg, bool must_be_var = false);
    void translate_assembly(void);
    void translatejumpiffalse(int where, int reg);
    void translatejumpiftrue(int where, int reg);
    void translatestatement();
    void translate_top_level();
    void add_top_level_decl();
    void translate_program();
};

node* parse_block(reader& input);
node* parse(reader& input);
node* parse_const(reader& input);
node* parse_statement(reader& input);
node* parse_top_level(reader& input);
node* parse_else(reader& input);
node* parse_expression(reader& input);
node* N(string t, string d = "", int v = 0);
