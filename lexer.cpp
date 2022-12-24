#include "include/lexer.hpp"
#include <regex>

using namespace std;
using namespace lexer;

Token::Token(int8_t t, string sd, int ln) {
    this->type = t;
    this->string_data = sd;
    this->int_data = 0;
    this->line = ln;
}

Token::Token(int8_t t, int id, int ln) {
    this->type = t;
    this->string_data = " ";
    this->int_data = id;
    this->line = ln;
}

Lexer::Lexer(string &text) {
    this->text = regex_replace(text, regex("\u03BB"), "\\");
    this->tokens = vector<Token>();
    this->advance();
}

void Lexer::advance() {
    // going out of bounds
    if ((this->pos+1) >= this->text.length()) {
        this->current_char = LEOF;
    } else {
        this->current_char = this->text[++pos];
    }
}

void Lexer::lex_num() {
    string i = "";
    while (current_char != LEOF && ('0' <= current_char && current_char <= '9')) {
        i += current_char;
        this->advance();
    }

    tokens.push_back( Token(NUM, stoi(i), this->line) );
    current_char = this->text[--this->pos];
}

void Lexer::lex_id() {
    string i = "";
    while (current_char != LEOF && (('a' <= current_char && current_char <= 'z') || ('A' <= current_char && current_char <= 'Z') || ('0' <= current_char && current_char <= '9'))) {
        i += current_char;
        advance();
    }

    tokens.push_back( Token(ID, i, this->line) );
    current_char = this->text[--this->pos];
}

vector<Token> Lexer::lex() {
    while (current_char != LEOF) {
       switch (current_char) {
            case ' ':  break;
            case '\t': break;
            case '\n':
                {
                    this->line++;
                }break;
            case '/':
                {
                    advance();
                    if (current_char == '/') {
                        advance();
                        while (current_char != LEOF && current_char != '\n') advance();
                    } else {
                        cout << "Expected '/' in comment!" << endl;
                        abort();
                    }
                }break;
            case '\\':
                {
                    tokens.push_back( Token(LAMBDA, "\\", this->line) );
                }break;
            case '.':
                {
                    tokens.push_back( Token(DOT, ".", this->line) );
                }break;
            case '$':
                {
                    tokens.push_back( Token(DOLLAR, "$", this->line) );
                }break;
            case '=':
                {
                    tokens.push_back( Token(EQUAL, "=", this->line) );
                }break;
            case '(':
                {
                    tokens.push_back( Token(OPENING_PARENTHESIS, "(", this->line) );
                }break;
            case ')':
                {
                    tokens.push_back( Token(CLOSING_PARENTHESIS, ")", this->line) );
                }break;
            default:
                {
                    // number
                    if ('0' <= current_char && current_char <= '9') {
                       this->lex_num();
                    } else if (('a' <= current_char && current_char <= 'z') || ('A' <= current_char && current_char <= 'Z')) {
                        this->lex_id();
                    } else {
                        // error
                        cout << "Unknown Token: " << current_char << endl;
                        abort();
                    }
                }break;
        }

        this->advance();
    }

    return this->tokens;
}
