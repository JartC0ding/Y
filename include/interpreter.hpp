#pragma once

#include <lexer.hpp>
#include <parser.hpp>

using namespace std;
using namespace lexer;
using namespace parser;

typedef vector<pair<string, FunctionNode> > applicative_variable_t;

namespace interpreter {
    class Interpreter {
        public:
            AST ast;

            Interpreter(AST p_ast);
        private:
            vector<string> functions;
            vector<string> results;
            int current_idx;
            int depth = 0;

            void exec(AST ast, applicative_variable_t vars);
            FunctionNode interpret_function(FunctionNode f, applicative_variable_t vars);
            string draw_function(FunctionNode f, applicative_variable_t vars);
    };
};
