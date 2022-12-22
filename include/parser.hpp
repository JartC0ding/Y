#pragma once

#include <lexer.hpp>

using namespace std;
using namespace lexer;

#define EOT -1


namespace parser {
    class FunctionNode {
        public:
            bool is_num = false;
            bool is_var = false;
            int num = 0;
            string var_name = "";
            vector<string> args;
            vector<FunctionNode> body;
            vector<FunctionNode> applicatives;
    };

    typedef vector<pair<string, FunctionNode> > function_macro_t;
    /*class VariableNode {
        public:
            string name;
            Node n;
    };*/

    typedef vector<FunctionNode> AST;

    class Parser {
        public:
            vector<Token> tokens;
            Token current_token;
            int pos = -1;
            AST ast;
            function_macro_t functions;

            Parser(vector<Token> p_tokens);
            void advance();
            AST parse(int delim=-1);
            pair<FunctionNode, vector<string> > parse_function(vector<string> prev_expected_args);
            pair<vector<FunctionNode>, vector<string> > parse_second_class(int delim, vector<string> args);
    };
};
