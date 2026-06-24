#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cctype>
#include <fstream>

using namespace std;

enum TokenType {
    START, SESH, KEYWORD, IDENTIFIER, NUMBER, STRING, ASSIGN, SEMICOLON, PRINT, UNKNOWN
};

struct Token {
    TokenType type;
    string value;
};

// Lexer: Tokenizes the input source code
vector<Token> tokenize(const string& source) {
    vector<Token> tokens;
    stringstream ss(source);
    string word;

    while (ss >> word) {
        if (word == "bhai") {
            string next;
            if (ss >> next) {
                if (next == "start") tokens.push_back({START, "bhai start"});
                else if (next == "sesh") tokens.push_back({SESH, "bhai sesh"});
                else {
                    tokens.push_back({KEYWORD, "bhai"});
                    tokens.push_back({IDENTIFIER, next});
                }
            }
        } 
        else if (word == "=") {
            tokens.push_back({ASSIGN, "="});
        } 
        else if (word.back() == ';') {
            string val = word.substr(0, word.length() - 1);
            if (isdigit(val[0])) {
                tokens.push_back({NUMBER, val});
            } else {
                tokens.push_back({IDENTIFIER, val});
            }
            tokens.push_back({SEMICOLON, ";"});
        } 
        else if (word.find("bol(") == 0 && word.back() == ';') {
            size_t open_p = word.find('(');
            size_t close_p = word.find(')');
            string content = word.substr(open_p + 1, close_p - open_p - 1);
            
            tokens.push_back({PRINT, content});
            tokens.push_back({SEMICOLON, ";"});
        }
    }
    return tokens;
}

// Parser & Interpreter: Executes tokens
void interpret(const vector<Token>& tokens) {
    map<string, int> symbolTable;
    bool hasStarted = false;

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].type == START) {
            hasStarted = true;
            continue;
        }
        if (!hasStarted) {
            cout << "Error: Program must begin with 'bhai start'!" << endl;
            return;
        }
        if (tokens[i].type == SESH) {
            break;
        }

        if (tokens[i].type == KEYWORD && tokens[i].value == "bhai") {
            if (i + 3 < tokens.size() && tokens[i+1].type == IDENTIFIER && tokens[i+2].type == ASSIGN && tokens[i+3].type == NUMBER) {
                string varName = tokens[i+1].value;
                int varVal = stoi(tokens[i+3].value);
                symbolTable[varName] = varVal;
                i += 4;
            }
        }
        
        if (tokens[i].type == PRINT) {
            string content = tokens[i].value;
            if (content.front() == '"' && content.back() == '"') {
                cout << content.substr(1, content.length() - 2) << endl;
            } 
            else if (symbolTable.find(content) != symbolTable.end()) {
                cout << symbolTable[content] << endl;
            } else {
                cout << "Error: Variable '" << content << "' is not defined!" << endl;
            }
        }
    }
}

int main() {
    // Reads code from the external file
    ifstream file("program.dost"); 
    if (!file.is_open()) {
        cout << "Error: Could not open 'program.dost' file!" << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string sourceCode = buffer.str();
    file.close();

    cout << "--- Dost Lang Output ---" << endl;
    vector<Token> tokens = tokenize(sourceCode);
    interpret(tokens);
    
    return 0;
}
