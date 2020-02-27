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
vector<string> addOrder;
vector<string> terminalOrder;
map<string, vector<string>> firstSet;
map<string, vector<string>> followSet;

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
bool isNonterminal(string var);
bool isTerminal(string var);

bool isNonterminal(string var){
    return !ifNotFind(nonTerminals, var);
}

bool isTerminal(string var){
    return !ifNotFind(terminals, var);
}

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
    terminalOrder.emplace_back("#");
    terminalOrder.emplace_back("$");

    for(auto &i : ruleList){
        if(ifNotFind(nonTerminals, i.first)){
            nonTerminals.push_back(i.first);
            addOrder.push_back(i.first);
        }

        for(auto &item : i.second){
            if(ifNotFind(lhs, item)){
                if(ifNotFind(terminals, item)){
                    terminals.push_back(item);
                    terminalOrder.push_back(item);
                }

            }else{
                if(ifNotFind(nonTerminals, item)){
                    nonTerminals.push_back(item);
                    addOrder.push_back(item);
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


bool isGenerating = false;

void isGenerate(bool *useless){
    bool isChanged;
    do {
        isChanged = false;
        for (auto &item : ruleList) {
            string left = item.first;
            vector<string> rightRules = item.second;
            for (auto &rightRule : rightRules) {
                //check is there any element not true in usefulSymbol
                int index = distance(symbols, find(symbols, symbols + symbolSize, rightRule));
                isGenerating = useless[index];
                if (!isGenerating) {
                   break;
                }
            }

            //check if all generating or empty (empty means there is only one epsilon) and sign to true in usefulSymbol
            if (item.second.empty() || isGenerating) {
                int index = distance(symbols, find(symbols, symbols + symbolSize, left));
                if(!useless[index]){
                    useless[index] = true;
                    isChanged = true;
                }
            }
        }
    }while(isChanged);

}

bool rea = true;
void isReachable(bool *reachable, vector<pair<string, vector<string>>> ruleGenRules) {
    bool isChanged;
    do{
        isChanged = false;
        for (auto &item : ruleGenRules) {
            string left = item.first;
            vector<string> rightRules = item.second;

            int index = distance(symbols, find(symbols, symbols + symbolSize, left));
            if (reachable[index]) {
                for (auto &rightRule : rightRules) {
                    int tempIdx = distance(symbols, find(symbols, symbols + symbolSize, rightRule));
                    if(!reachable[tempIdx]){
                        reachable[tempIdx] = true;
                        isChanged = true;
                    }
                }
            }
        }

    }while(isChanged);
}

vector<pair<string, vector<string>>> ruleGen;
vector<pair<string, vector<string>>> useful;
void getUseless(){
    bool generateSymbols[symbolSize];
    bool reachableSymbols[symbolSize];
    for (int i = 1; i < symbolSize; i++) {
        generateSymbols[i] = isTerminal(symbols[i]);
    }

    generateSymbols[0] = true;

    //get generate array
    isGenerate(generateSymbols);
    for(auto &item : ruleList){
        string left = item.first;
        vector<string> rightRules = item.second;

        for(auto &rightRule : rightRules){
            int idx = distance(symbols, find(symbols, symbols + symbolSize, rightRule));
            if(generateSymbols[idx]){
                isGenerating = true;
            }else{
                //one ungenerating element means whole rule ungenerating
                isGenerating = false;
                break;
            }
        }

        if(isGenerating){
            ruleGen.emplace_back(item.first, item.second);
        }
    }

    if(!ruleGen.empty()){
        int index = distance(symbols, find(symbols, symbols + symbolSize, ruleList[0].first));
        for(int i = 0; i < symbolSize; i++){
            bool reachable = (i == index);
            reachableSymbols[i] = reachable;
        }

        //get reachable array
        isReachable(reachableSymbols, ruleGen);
        for(auto &i : ruleGen){
            for(auto &j : i.second){
                int idx = distance(symbols, find(symbols, symbols + symbolSize, j));
                rea = reachableSymbols[idx];
                if(!rea){
                    break;
                }
            }

            if(rea){
                //all reachable rules from ruleGen is useful
                useful.emplace_back(i.first, i.second);
            }
        }
    }
}

// Task 2
void RemoveUselessSymbols() {
    //update useful
    getUseless();
    if(!useful.empty()){
        string output;
        for(auto &item : useful){
            output += item.first + " -> ";
            if(!item.second.empty()){
                for(auto &rightRule : item.second){
                    output += rightRule + " ";
                }
            }else{
                output += "#";
            }
            output += "\n";
        }
        cout << output;

    }else{
        cout << "";
    }
}

void getFirst(){
    bool isChanged;
    do{
        isChanged = false;
        //bool isEpsilon = true;

        for (auto &item : ruleList){
            string left = item.first;
            vector<string> rightStatement = item.second;
            if (firstSet.count(left) == 0){
                firstSet[left] = vector<string>();
            }

            if (rightStatement.empty()){
                if (ifNotFind(firstSet[left], "#")){
                    firstSet[left].push_back("#");
                    isChanged = true;
                }

            } else{
                bool hasEpsilon;
                for (size_t i = 0; i < rightStatement.size(); i++){

                    if (isTerminal(rightStatement[i])){
                        if (ifNotFind(firstSet[left], rightStatement[i])){
                            firstSet[left].push_back(rightStatement[i]);
                            isChanged = true;
                        }
                        break;

                    } else if (isNonterminal(rightStatement[i])){
                        hasEpsilon = false;
                        if (!firstSet[rightStatement[i]].empty()) {
                            vector<string> set = firstSet[rightStatement[i]];
                            for (auto &setItem : set) {
                                if (ifNotFind(firstSet[left], setItem) && setItem != "#") {
                                    firstSet[left].push_back(setItem);
                                    isChanged = true;
                                }
                                if (setItem == "#") {
                                    hasEpsilon = true;
                                }
                            }

                            if (!hasEpsilon) {
                                break;

                            } else if (i + 1 == rightStatement.size()){
                                if (ifNotFind(firstSet[left], "#")){
                                    firstSet[left].push_back("#");
                                    isChanged = true;
                                }
                            }

                        } else{
                            break;
                        }
                    }
                }
            }

        }

    } while(isChanged);

}

void printFirst(){
    for (auto &item : addOrder){
        cout << "FIRST(" << item << ") = { ";
        vector<string> set = firstSet[item];
        size_t i = 0;
        for (size_t j = 0; j < terminalOrder.size(); j++){
            if (!ifNotFind(set, terminalOrder[j])){
                cout << terminalOrder[j];
                if (i + 1 != set.size()){
                    cout << ", ";
                    i++;
                } else{
                    break;
                }
            }
        }

        cout << " }" << endl;
    }
}

void getFollow() {
    bool isChanged;
    string firstSymbol = nonTerminals[0];
    followSet[firstSymbol].push_back("$");

    do {
        isChanged = false;
        for (auto &item : ruleList) {
            for(size_t i = 0; i < item.second.size(); i++){
                if (isTerminal(item.second[i])){
                    continue;
                }

                string left = item.first;
                vector<string> restRight = vector<string>(item.second.begin() + i + 1, item.second.end());
                vector<string> tempFollow = followSet[item.second[i]];
                size_t sizeChange = tempFollow.size();
                bool hasEpsilon = false;

                if (!restRight.empty()){
                    for (size_t idx = 0; idx < restRight.size(); idx++){
                        if (isNonterminal(restRight[idx])){
                            for (auto &innerElement : firstSet[restRight[idx]]){
                                if (ifNotFind(tempFollow, innerElement) && innerElement != "#"){
                                   tempFollow.push_back(innerElement);
                                } else if (innerElement == "#"){
                                    hasEpsilon = true;
                                }
                            }

                            if (hasEpsilon){
                                if (idx + 1 != restRight.size()){
                                    continue;
                                }

                                vector<string> tempFollow2 = followSet[left];
                                for (auto &temp : tempFollow2){
                                    if (ifNotFind(tempFollow, temp)){
                                        tempFollow.push_back(temp);
                                    }
                                }

                            } else{
                                break;
                            }

                        //if is terminal
                        //if (isNonterminal(restRight[idx]))
                        } else{
                            if (ifNotFind(tempFollow, restRight[idx])){
                                tempFollow.push_back(restRight[idx]);
                            }
                            break;
                        }
                    }

                //only have one stuff.
                //if (!restRight.empty())
                } else{
                    for (auto &element : followSet[left]){
                        if (ifNotFind(tempFollow, element)){
                            tempFollow.push_back(element);
                        }
                    }
                }

                if (sizeChange != tempFollow.size()){
                    for (auto &element : tempFollow){
                        if (ifNotFind(followSet[item.second[i]], element)){
                            followSet[item.second[i]].push_back(element);
                            isChanged = true;
                        }
                    }
                }
            }
        }

    } while (isChanged);
}

void printFollow(){
    for (auto &item : addOrder){
        cout << "FOLLOW(" << item << ") = { ";
        vector<string> set = followSet[item];
        size_t i = 0;

        for (size_t j = 0; j < terminalOrder.size(); j++){
            if (!ifNotFind(set, terminalOrder[j])){
                cout << terminalOrder[j];
                if (i + 1 != set.size()){
                    cout << ", ";
                    i++;
                } else{
                    break;
                }
            }
        }

        cout << " }" << endl;
    }
}

int main (int argc, char* argv[])
{
    int task;
    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        //return 1;
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

        case 2:
            RemoveUselessSymbols();
            break;

        case 3:
            getFirst();
            printFirst();
            break;

        case 4:
            getFirst();
            getFollow();
            printFollow();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }

    return 0;
}

