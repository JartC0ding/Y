#include <parser.hpp>
#include <algorithm>
#include <vector>

using namespace std;
using namespace parser;

int find_func_macro(string n, vector<function_macro_t> f) {
    for (int i = 0; i < f.size(); i++) {
        if (!n.compare(f[i].first)) return i;
    }
    return -1;
}

bool Parser::verify_line() {
    return this->current_line == this->current_token.line;
}

Parser::Parser(vector<Token> p_tokens) : current_token(EOT, 0, 0) {
    this->tokens = p_tokens;
    this->advance();
}

void Parser::advance() {
    this->pos++;
    if (this->pos >= this->tokens.size()) {
        this->current_token = Token(EOT, 0, 0);
    } else {
        this->current_token = this->tokens[this->pos];
    }
}

AST Parser::parse(int delim) {
    while (current_token.type != EOT && current_token.type != delim) {
        this->current_line = current_token.line;
        // has to start with a () for function. Each executed function will print it's result
        if (current_token.type == OPENING_PARENTHESIS) {
            advance();
            this->ast.push_back(parse_function(vector<string>(0)).first);
        } else if (current_token.type == DOLLAR) {
            // variable found
            advance();
            
            if (current_token.type == ID && find_func_macro(current_token.string_data, this->functions) == -1) {
                string name = current_token.string_data;
                advance();

                if (current_token.type == EQUAL) {
                    advance();

                    if (current_token.type == OPENING_PARENTHESIS && this->verify_line()) {
                        advance();
                        FunctionNode fn = parse_function(vector<string>(0)).first;

                        this->functions.push_back(function_macro_t(name, fn));
                    } else if (current_token.type == NUM) {
                        FunctionNode fn;
                        fn.is_num = true;
                        fn.num = current_token.int_data;
                        advance();

                        this->functions.push_back(function_macro_t(name, fn));
                    } else {
                        cout << "Expected function in macro definition" << endl;
                        abort();
                    }
                } else {
                    cout << "Expected assignment" << endl;
                    abort();
                }
            } else if (current_token.type == ID && find_func_macro(current_token.string_data, this->functions) != -1 && this->verify_line()) {
                // a function exists
                string name = current_token.string_data;
                advance();

                if (current_token.type == OPENING_PARENTHESIS && this->verify_line()) {
                    // called
                    advance();
                    FunctionNode f = this->functions[find_func_macro(name, this->functions)].second;
                    auto parse_ret = parse_second_class(CLOSING_PARENTHESIS, vector<string>(0)); 
                    f.applicatives = parse_ret.first;
                    advance();

                    if (f.applicatives.size() < f.args.size()) {
                        cout << "Too few arguments to consume" << endl;
                        abort();
                    } else if (f.applicatives.size() > f.args.size()) {
                        cout << "Too many arguments to consume" << endl;
                        abort();
                    }
                   
                    this->ast.push_back(f);
                } else {
                    // just referenced
                    this->ast.push_back(this->functions[find_func_macro(name, this->functions)].second);
                }
            } else {
                cout << "Expected macro name" << endl;
                abort();
            }
        } else {
            cout << "Expected a parenthesis as start of lambda function" << endl;
            abort();
        }
    }

    return this->ast;
}
pair<vector<FunctionNode>, vector<string> > Parser::parse_second_class(int delim, vector<string> args) {
    vector<FunctionNode> f(0);
    vector<string> vars(0);

    while (current_token.type != EOT && current_token.type != delim) {
        // has to start with a () for function. Each executed function will print it's result
        if (current_token.type == OPENING_PARENTHESIS) {
            advance();
            auto parsed = parse_function(args);
            f.push_back(parsed.first);
            for (auto i : parsed.second) {
                vars.push_back(i);
            }
        } else if (current_token.type == NUM) {
            FunctionNode ff;
            ff.is_num = true;
            ff.num = current_token.int_data;
            this->advance();
            f.push_back(ff);
        } else if (current_token.type == ID) {
            //TODO: check if var is a variable thats expected to be consumed
            // not in args
            if (find(args.begin(), args.end(), current_token.string_data) == args.end()) {
                // invalid variable
                cout << "Invalid var" << endl;
                abort();
            }

            FunctionNode ff;
            ff.is_var = true;
            ff.var_name = current_token.string_data;
            this->advance();
            f.push_back(ff);
            vars.push_back(ff.var_name);
        } else if (current_token.type == DOLLAR) {
            // variable
            advance();
            if (current_token.type == ID && find_func_macro(current_token.string_data, this->functions) != -1) {
                // a function exists
                string name = current_token.string_data;
                advance();

                if (current_token.type == OPENING_PARENTHESIS && this->verify_line()) {
                    // called
                    advance();
                    FunctionNode ff = this->functions[find_func_macro(name, this->functions)].second;
                    auto parse_ret = parse_second_class(CLOSING_PARENTHESIS, vector<string>(0)); 
                    ff.applicatives = parse_ret.first;
                    advance();

                    if (ff.applicatives.size() < ff.args.size()) {
                        cout << "Too few arguments to consume" << endl;
                        abort();
                    } else if (ff.applicatives.size() > ff.args.size()) {
                        cout << "Too many arguments to consume" << endl;
                        abort();
                    }
                   
                    f.push_back(ff);
                } else {
                    // just referenced
                    f.push_back(this->functions[find_func_macro(name, this->functions)].second);
                }
            } else {
                cout << "Expected valid macro name" << endl;
                abort();
            }

        } else {
            cout << "Expected a function or church numeral" << endl;
            abort();
        }
    }

    return pair<vector<FunctionNode>, vector<string> >(f, vars);
}


pair<FunctionNode, vector<string> > Parser::parse_function(vector<string> prev_expected_args) {
    FunctionNode f;
    vector<string> args_to_consume;

    if (current_token.type == LAMBDA) {
        this->advance();
    } else {
        cout << "Expected lambda in function " << endl;
        abort();
    }

    // parse args
    while (current_token.type == ID) {
        f.args.push_back(current_token.string_data);
        this->advance();
    }
    args_to_consume = f.args;

    // args end in .
    if (current_token.type == DOT) {
        this->advance();
        
        // parse body
        pair<vector<FunctionNode>, vector<string> > ret = parse_second_class(CLOSING_PARENTHESIS, f.args);
        f.body = ret.first;
        this->advance();

        // check if consumed
        int cnt = 0;
        for (auto i : f.args) {
        // check if arg appears in ret.second. Count all the occurences and if the occurences are smaller than args, than there was an onconsumed arg
            if (find(ret.second.begin(), ret.second.end(), i) != ret.second.end()) {
                cnt++;
            }
        }

        if (cnt < f.args.size()) {
            cout << "Unconsumed arguments! (" << f.args.size() - cnt << ")" << endl;
            abort();
        }
        // parse args
        if (current_token.type == OPENING_PARENTHESIS && this->verify_line()) {
            advance();
            auto parse_ret = parse_second_class(CLOSING_PARENTHESIS, prev_expected_args); 
            f.applicatives = parse_ret.first;
            advance();

            if (f.applicatives.size() < f.args.size()) {
                cout << "Too few arguments to consume" << endl;
                abort();
            } else if (f.applicatives.size() > f.args.size()) {
                cout << "Too many arguments to consume" << endl;
                abort();
            }
           
            return pair<FunctionNode, vector<string> > (f, parse_ret.second);
        } else {
            f.applicatives = vector<FunctionNode> (0);
            return pair<FunctionNode, vector<string> > (f, vector<string>(0));
        }
    } else {
        cout << "Expected . at the end of args" << endl;
        abort();
    }
}
