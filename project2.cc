/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <map>
#include "string"
#include "lexer.h"

using namespace std;
vector<pair<string, vector<string>>> ruleList;

vector<string> rhs;
vector<string> lhs;
vector<string> terminals;
vector<string> nonTerminals;
string symbols[1000];
int symbolSize = 0;
LexicalAnalyzer lexer;
Token t;

//Parsing
void parse_rule_list();
void parse_rule();
void parse_RHS();
void parse_id_list();

//Utility
Token peek();
bool ifNotFind(vector<string> vec, const string& value);
void syntax_error();

Token peek(){
    t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}

bool ifNotFind(vector<string> vec, const string& value){
    return (find(vec.begin(), vec.end(), value) == vec.end());
}

void syntax_error(){
    cout << "SYNTAX ERROR !!!" << endl;
    exit(1);
}

void parse_rule_list(){
    t = lexer.GetToken();

    if(t.token_type == ID){
        string ruleName = t.lexeme;
        lexer.UngetToken(t);
        parse_rule();

        ruleList.emplace_back(ruleName, rhs);
        rhs.clear();

        t = peek();
        if(t.token_type == ID){
            parse_rule_list();

        }else if(t.token_type == DOUBLEHASH) {
            return;

        } else{
            syntax_error();
        }

    } else{
        syntax_error();
    }
}

void parse_rule(){
    t = lexer.GetToken();
    if(t.token_type == ID) {
        lhs.push_back(t.lexeme);
        t = lexer.GetToken();

        if (t.token_type == ARROW) {
            parse_RHS();
            t = lexer.GetToken();
            if (t.token_type != HASH) {
                syntax_error();
            }

        } else{
            syntax_error();
        }

    } else{
        syntax_error();
    }
}

void parse_RHS() {
    t = lexer.GetToken();
    if(t.token_type == ID) {
        rhs.push_back(t.lexeme);

        t = peek();
        if (t.token_type == ID) {
            parse_id_list();
        }

    }else if(t.token_type == HASH){
        lexer.UngetToken(t);

    }else{
        syntax_error();
    }
}

void parse_id_list() {
    t = lexer.GetToken();
    if(t.token_type == ID) {
        rhs.push_back(t.lexeme);

        t = peek();
        if (t.token_type == ID) {
            parse_id_list();
        }
    } else{
        syntax_error();
    }
}


void ReadGrammar()
{
    parse_rule_list();

    for(auto &i : ruleList){
        if(ifNotFind(nonTerminals, i.first)){
            nonTerminals.push_back(i.first);
        }

        for(auto &item : i.second){
            if(ifNotFind(lhs, item)){
                if(ifNotFind(terminals, item)){
                    terminals.push_back(item);
                }

            }else{
                if(ifNotFind(nonTerminals, item)){
                    nonTerminals.push_back(item);
                }
            }
        }
    }

    symbols[0] = "#";
    symbols[1] = "$";
    symbolSize += 2;
    
    for(const auto & terminal : terminals){
        symbols[symbolSize++] = terminal;
    }

    for(auto &i : nonTerminals) {
        symbols[symbolSize++] = i;
    }
}

// Task 1
void printForTask1()
{
    string output;
    for(auto &i : terminals){
        output += i + " ";
    }
    output += " ";

    for(auto &i : nonTerminals) {
        output += i + " ";
    }

    cout << output << endl;
}

/*

bool gen = false;
void isGenerate(vector<bool> useless){
    //save current one
    vector<bool> prev;
    for(int i = 0; i < symbolSize; i++){
        prev[i] = useless[i];
    }

    //setGenerate(useless);

    for(auto &i : ruleList){
        //go through rule body vector
        for(auto &j : i.second){
            //check is there any element not true in usefulSymbol
            int index = distance(symbols, find(symbols, symbols + symbolSize, j));
            if(useless[index]){
                gen = true;
            }else{
                //one ungenerating element means whole rule ungenerating
                gen = false;
                break;
            }
        }

        //check if all generating or empty (empty means there is only one epsilon) and sign to true in usefulSymbol
        if(i.second.empty()|| gen){
            int index = distance(symbols, find(symbols, symbols + symbolSize, i.first));
            useless[index] = true;
        }
    }

    if(prev != useless){
        isGenerate(useless);
    }
}*/

void getFirst(){
    vector<pair<string, vector<string>>> firstSet;

    for (auto &item : ruleList){
        string left = item.first;
        vector<string> right = item.second;

        bool isChanged = true;
        do{
            
        }
    }
}

int main (int argc, char* argv[])
{
    int task;
    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the firstSet argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);


    ReadGrammar();  // Reads the input grammar from standard input
    // and represent it internally in data structures
    // ad described in project 2 presentation file

    switch (task) {
        case 1: printForTask1();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }

    return 0;
}

