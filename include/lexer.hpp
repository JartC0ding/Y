#pragma once

#include <iostream>
#include <vector>

using namespace std;

#define LEOF '\0'

namespace lexer {
    enum {
        ID,
        NUM,
        OPENING_PARENTHESIS,
        CLOSING_PARENTHESIS,
        LAMBDA,
        DOT,
        DOLLAR,
        EQUAL
    };

    class Token {
        public:
            Token(int8_t t, string sd, int ln);
            Token(int8_t t, int id, int ln);

            int8_t type;
            string string_data;
            int int_data;
            int line;
    };

    class Lexer {
        public:
            Lexer(string &text);
            vector<Token> lex();

        private:
            vector<Token> tokens;
            string text;
            int pos = -1;
            int line = 0;
            char current_char = 0;
            
            void advance();
            void lex_id();
            void lex_num();
    };
}
