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
#include <any>
#include "string"
#include "lexer.h"

using namespace std;
vector<pair<string, vector<string>>> ruleList;
//For making sure that the first and follow set is printed in the order appeared.
vector<string> addOrder;
vector<string> terminalOrder;

//first and follow set map.
map<string, vector<string>> firstSet;
map<string, vector<string>> followSet;

bool hasUseless = true;
//vectors for helping to track the grammar rule.
vector<string> rhs;
vector<string> lhs;
vector<string> terminals;
vector<string> nonTerminals;

//symbol map with every symbol and its corresponding added order.
map <string, int> symbols;
bool generateSymbols[100];
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
bool ifFind(vector<string> vec, const string& value);
void syntax_error();
bool isNonterminal(string var);
bool isTerminal(string var);

/**
 *
 * @param var -> string for checking if it is a nonterminal character.
 * @return {bool} -> if it is a non-terminal, return true, otherwise false.
 */
bool isNonterminal(string var){
    return ifFind(nonTerminals, var);
}

/**
 *
 * @param var -> string for checking if it is a terminal character.
 * @return {bool} -> if it is a terminal, return true, otherwise false.
 */
bool isTerminal(string var){
    return ifFind(terminals, var);
}

/**
 * Helper method to peek.
 * @return {Token} -> The token peeked.
 */
Token peek(){
    t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}

bool ifNotFind(vector<string> vec, const string& value){
    return (find(vec.begin(), vec.end(), value) == vec.end());
}

bool ifFind(vector<string> vec, const string& value){
    return (find(vec.begin(), vec.end(), value) != vec.end());
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


    //initialize symbol map for convenient purpose.
    symbols["#"] = 0;
    generateSymbols[0] = true;
    symbols["$"] = 1;
    symbolSize += 2;

    for(const auto & terminal : terminals){
        symbols[terminal] = symbolSize;
        generateSymbols[symbolSize++] = true;
    }

    for(auto &i : nonTerminals) {
        symbols[i] = symbolSize;
        generateSymbols[symbolSize++] = false;
    }
}


void printForTask1()
{
    string output;
    //Print all terminals.
    for(auto &i : terminals){
        output += i + " ";
    }
    output += " ";

    //Print all nonTerminals
    for(auto &i : nonTerminals) {
        output += i + " ";
    }

    cout << output << endl;
}

vector<pair<string, vector<string>>> ruleGen;
vector<pair<string, vector<string>>> usefulVec;

void getUseless(){
    bool isChanged = true;
    bool reachableSymbols[symbolSize];

    for (auto &item : symbols){
        generateSymbols[item.second] = isTerminal(item.first);
    }
    generateSymbols[0] = true;

    while (isChanged){
        isChanged = false;
        for (auto &item : ruleList){
            bool isGenerating = true;
            for (auto &rightRule : item.second){
                int tempIdx = symbols[rightRule];
                if (!generateSymbols[tempIdx]){
                    isGenerating = false;
                    break;
                }
            }

            //empty means epsilon.
            if (item.second.empty() || isGenerating){
                int tempIdx = symbols[item.first];
                if (!generateSymbols[tempIdx]){
                    generateSymbols[tempIdx] = true;
                    isChanged = true;
                }
            }
        }
    }

    bool isGenerating = false;
    for (auto &item : ruleList){
        vector<string> rightRules = item.second;
        if (!rightRules.empty()){
            for (auto &rightRule : rightRules){
                int tempIdx = symbols[rightRule];
                if (!rightRules.empty()){
                    isGenerating = generateSymbols[tempIdx];
                    if (!isGenerating){
                        hasUseless = false;
                        break;
                    }
                }
            }
        } else{
            isGenerating = true;
        }

        if (isGenerating){
            ruleGen.emplace_back(item.first, item.second);
        }
    }

    if (!ruleGen.empty() && (ruleGen[0].first == ruleList[0].first)){
        int tempIdx = symbols[ruleGen[0].first];
        //symbols => map<string, int>
        //string => content.
        //int => added order.
        for (auto &item : symbols){
            bool checker = (item.second == tempIdx);
            reachableSymbols[item.second] = checker;
        }

        isChanged = true;

        while (isChanged){
            isChanged = false;
            for (auto &item : ruleGen){
                int tempIdx2 = symbols[item.first];
                if (reachableSymbols[tempIdx2]){
                    for (auto &rightRule : item.second){
                        int tempIdx3 = symbols[rightRule];
                        if (!reachableSymbols[tempIdx3]){
                            reachableSymbols[tempIdx3] = true;
                            isChanged = true;
                        }
                    }
                }
            }
        }

        bool rea = false;
        for (auto &item : ruleGen){
            int tempIdx3 = symbols[item.first];
            if (!item.second.empty()){
                for (auto &rightRule : item.second){
                    rea = reachableSymbols[tempIdx3];
                    if (!rea){
                        hasUseless = false;
                        break;
                    }
                }
            } else{
                if (reachableSymbols[tempIdx3]){
                    rea = true;
                }
            }

            if (rea){
                usefulVec.emplace_back(item.first, item.second);
            }
        }
    }


}

// Task 2
void RemoveUselessSymbols() {
    //update useful
    getUseless();
    if(!usefulVec.empty()){
        string output;
        for(auto &item : usefulVec){
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
        for (const auto & j : terminalOrder){
            if (ifFind(set, j)){
                cout << j;
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
    bool isChanged, hasEpsilon;
    string firstSymbol = nonTerminals[0];
    followSet[firstSymbol].push_back("$");

    for (auto &item : ruleList){
        string left = item.first;
        vector<string> rights = item.second;
        for (size_t idx = 0; idx < rights.size(); idx++){
            if (isNonterminal(rights[idx])){
                for (size_t idx2 = idx + 1; idx2 < rights.size(); idx2++){
                    hasEpsilon = false;
                    if (ifFind(firstSet[rights[idx2]], "#")){
                        hasEpsilon = true;
                    }

                    for (auto & element : firstSet[rights[idx2]]){
                        if(ifNotFind(followSet[rights[idx]], element) && element != "#"){
                            followSet[rights[idx]].push_back(element);
                        }
                    }

                    if (!hasEpsilon){
                        break;
                    }
                }
            }
        }
    }

    do {
        isChanged = false;
        for (auto &item : ruleList){
            string left = item.first;
            vector<string> rights = item.second;

            for (size_t idx = 0; idx < rights.size(); idx++){
                hasEpsilon = false;
                if (isNonterminal(rights[idx])){
                    if(idx != rights.size() - 1){
                        for (size_t idx2 = idx + 1; idx2 < rights.size(); idx2++){
                            if (ifNotFind(firstSet[rights[idx2]], "#")){
                                hasEpsilon = true;
                                break;
                            }
                        }

                        if (!hasEpsilon){
                            for (auto &element : followSet[left]){
                                if (ifNotFind(followSet[rights[idx]], element)){
                                    followSet[rights[idx]].push_back(element);
                                    isChanged = true;
                                }
                            }
                        }

                    } else{
                        for (auto &element : followSet[left]){
                            if (ifNotFind(followSet[rights[idx]], element)){
                                followSet[rights[idx]].push_back(element);
                                isChanged = true;
                            }
                        }
                    }
                }
            }
        }

    } while (isChanged);
}

void printFollow(){
    getFirst();
    for (auto &item : terminals){
        firstSet[item].push_back(item);
    }
    firstSet["#"].push_back("#");

    getFollow();
    for (auto &item : addOrder){
        cout << "FOLLOW(" << item << ") = { ";
        vector<string> set = followSet[item];
        size_t i = 0;

        for (const auto & j : terminalOrder){
            if (ifFind(set, j)){
                cout << j;
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

vector<string> rhsFirst(const vector<string>& ruleBody) {
    vector<string> result;
    bool hasEpsilon;
    if (ruleBody.empty()) {
        result.emplace_back("#");
    }else{
        if (ruleBody.empty()){
            hasEpsilon = true;
        } else{
            for(auto &item : ruleBody){
                hasEpsilon = ifFind(firstSet[item], "#");

                for(auto &firstSetItem : firstSet[item]){
                    if(ifNotFind(result, firstSetItem) && firstSetItem != "#"){
                        result.push_back(firstSetItem);
                    }
                }

                if(!hasEpsilon){
                    break;
                }
            }
        }

        if(hasEpsilon){
            result.emplace_back("#");
        }
    }
    return result;
}

bool checkIfIntersec(const vector<string>& vec1, const vector<string>& vec2){
    for(auto &item : vec1){
        if(ifFind(vec2, item)){
            return true;
        }
    }
    return false;
}
// Task 5
void checkPP()
{
    string output;
    getUseless();
    firstSet["#"].push_back("#");
    if(!hasUseless){
        output = "NO";

    }else{
        getFirst();
        bool ifIntersec = false;

        for(auto &item : terminals){
            firstSet[item].push_back(item);
        }
        firstSet["#"].push_back("#");

       for(size_t idx = 0; idx < ruleList.size(); idx++){
            for(size_t idx2 = idx + 1; idx2 < ruleList.size(); idx2++){
                if(ruleList[idx].first == ruleList[idx2].first){
                    vector<string> first1 = rhsFirst(ruleList[idx].second);
                    vector<string> first2 = rhsFirst(ruleList[idx2].second);

                    ifIntersec = checkIfIntersec(first1, first2);
                    if(ifIntersec){
                        goto outLoop;
                    }
                }
            }
        }

        outLoop:

        bool ifIntersec2 = false;
        if(ifIntersec){
            output = "NO";
        }else{
            /*getFirst();
            for (auto &item : terminals){
                firstSet[item].push_back(item);
            }
            firstSet["#"].push_back("#");

            */
            getFollow();
            for(auto &item : nonTerminals){
                if(!(ifNotFind(firstSet[item], "#"))){
                    ifIntersec2 = checkIfIntersec(firstSet[item], followSet[item]);
                }
                if (ifIntersec2){
                    break;
                }
            }

            output = ifIntersec2 ? "NO" : "YES";
        }
    }
    cout << output;
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
            printFollow();
            break;

        case 5:
            checkPP();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }

    return 0;
}