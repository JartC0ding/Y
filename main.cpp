#include <iostream>
#include <sstream>
#include <fstream>
#include <lexer.hpp>
#include <parser.hpp>
#include <interpreter.hpp>

using namespace std;
using namespace lexer;
using namespace parser;
using namespace interpreter;

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Too few arguments! (Expected 1 got 0)" << endl;
        return 1;
    }
    if (argc > 2) {
        cout << "Too many arguments! (Expected 1 got " << to_string(argc) << ")" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    string str((istreambuf_iterator<char>(file)),
                 istreambuf_iterator<char>());

    Lexer l = Lexer(str);
    vector<Token> v = l.lex();

    Parser p(v);
    AST a = p.parse();

    Interpreter i(a);
}

