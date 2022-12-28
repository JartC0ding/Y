#include <interpreter.hpp>

using namespace interpreter;

Interpreter::Interpreter(AST p_ast) {
    results = vector<string>(p_ast.size());
    for (int i = 0; i < p_ast.size(); i++) {
        this->current_idx = i;
        this->functions.push_back(this->draw_function(p_ast[i], applicative_variable_t(0)));
    }

    for (int i = 0; i < p_ast.size(); i++) {
        this->current_idx = i;
        this->depth = 0;
        this->interpret_function(p_ast[i], applicative_variable_t(0));
    }

    for (int i = 0; i < this->functions.size(); i++) {
        cout << this->functions[i] << " -> " << this->results[i] << endl;
    }
}

int find_var(string n, applicative_variable_t vars) {
    for (int i = 0; i < vars.size(); i++) {
        if (!n.compare(vars[i].first)) return i;
    }
    return -1;
}

void Interpreter::exec(AST ast, applicative_variable_t vars) {
    for (int i = 0; i < ast.size(); i++) {
        this->interpret_function(ast[i], vars);
    }
}

string Interpreter::draw_function(FunctionNode f, applicative_variable_t vars) {
    if (f.is_num) return to_string(f.num);

    // try to replace if var
    if (f.is_var) {
        if (find_var(f.var_name, vars) != -1) {
            return draw_function(vars[find_var(f.var_name, vars)].second, vars);
        } else {
            return f.var_name;
        }
    }

    string i = "(\u03BB";
    for (auto j : f.args) {
        i += j + " ";
    }
    i.erase(i.end()-1);
    i+=". ";

    for (auto j : f.body) {
        i += this->draw_function(this->interpret_function(j, vars), vars) + " ";
    }
    i.erase(i.end()-1);
    i += ")";

    if (f.applicatives.size() > 0) {
        i += "(";

        for (auto j : f.applicatives) {
            i += this->draw_function(this->interpret_function(j, vars), vars) + " ";
        }

        i.erase(i.end()-1);
        i += ")";
    }

    return i;
}

FunctionNode Interpreter::interpret_function(FunctionNode f, applicative_variable_t vars) {
    // draw if no application has to be done
    if (f.is_var) {
        if (find_var(f.var_name, vars) != -1) {
            auto v = vars[find_var(f.var_name, vars)].second;
            
            // solution is validated by searching for higher predecence
            try {
                stoi(this->results[current_idx]);
            } catch (exception e) {
                this->results[this->current_idx] = draw_function(v, vars); // fucks with actual results
            }
            
            return v;
        } else {
            return f;
        }
    }
    if (f.is_num) {
        this->results[this->current_idx] = this->draw_function(f, vars);
        return f;
    }

    if (f.applicatives.size() == 0) {
        this->results[this->current_idx] = this->draw_function(f, vars);
        return f;
    } else {
        // first try to replace vars in applicatives, if a applicative is a function that is not a constant, evaluate it
        for (int i = 0; i < f.applicatives.size(); i++) {
            if (!f.applicatives[i].is_num && !f.applicatives[i].is_var) {
                // function, eval it

                f.applicatives[i] = this->interpret_function(f.applicatives[i], vars);
            }
            if (f.applicatives[i].is_var && find_var(f.applicatives[i].var_name, vars) != -1) f.applicatives[i] = vars[find_var(f.applicatives[i].var_name, vars)].second;
        }
 
        // append applicatives to allready existing applicatives
        for (int i = 0; i < f.applicatives.size(); i++) {
            vars.push_back(pair<string, FunctionNode>(f.args[i], f.applicatives[i]));
        }
        // try to replace all vars in body, if a var is a function that is not a constant, evaluate this function as well
        this->exec(f.body, vars);
        
        return f;
    }
}

