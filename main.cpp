#include <bits/stdc++.h>
using namespace std;

unordered_set<char> OPS = {'+', '-', '*', '/'};

enum TokenType { TOK_NUMBER, TOK_VARIABLE, TOK_OPERATOR, TOK_LPAREN, TOK_RPAREN };
struct Token { TokenType type; string value; };

char matchingOpen(const char c) {
    if (c == ')') return '(';
    if (c == ']') return '[';
    if (c == '}') return '{';
    return 0;
}
bool isOpenBracket(const char c)  { return c == '(' || c == '[' || c == '{'; }

bool isCloseBracket(const char c) { return c == ')' || c == ']' || c == '}'; }

int precedence(const string &op) {
    if (op == "*" || op == "/") return 2;
    if (op == "+" || op == "-") return 1;
    return 0;
}

bool isUnaryMinus(const vector<Token> &tokens) {
    if (tokens.empty()) return true;
    TokenType last = tokens.back().type;
    return last == TOK_OPERATOR || last == TOK_LPAREN;
}

vector<Token> tokenize(const string &expr) {
    vector<Token> tokens;
    int i = 0, n = (int)expr.size();

    while (i < n) {
        if (isspace((unsigned char)expr[i])) { i++; continue; }

        // multi-digit integer
        if (isdigit((unsigned char)expr[i])) {
            string num;
            while (i < n && isdigit((unsigned char)expr[i]))
                num += expr[i++];
            tokens.push_back({TOK_NUMBER, num});
            continue;
        }
        if (expr[i] == '-' && isUnaryMinus(tokens)) {
            int j = i + 1;
            while (j < n && isspace((unsigned char)expr[j])) j++;
            if (j < n && isdigit((unsigned char)expr[j])) {
                string num = "-";
                i = j;
                while (i < n && isdigit((unsigned char)expr[i]))
                    num += expr[i++];
                tokens.push_back({TOK_NUMBER, num});
                continue;
            }
        }

        // C++ identifier
        if (isalpha((unsigned char)expr[i]) || expr[i] == '_') {
            string id;
            while (i < n && (isalnum((unsigned char)expr[i]) || expr[i] == '_'))
                id += expr[i++];
            // 'x' after an operand or closing bracket = multiplication sign
            if (id == "x" && !tokens.empty() &&
                (tokens.back().type == TOK_NUMBER ||
                 tokens.back().type == TOK_VARIABLE ||
                 tokens.back().type == TOK_RPAREN)) {
                tokens.push_back({TOK_OPERATOR, "*"});
            } else {
                tokens.push_back({TOK_VARIABLE, id});
            }
            continue;
        }

        if (isOpenBracket(expr[i])) {
            tokens.push_back({TOK_LPAREN, string(1, expr[i++])});
            continue;
        }
        if (isCloseBracket(expr[i])) {
            tokens.push_back({TOK_RPAREN, string(1, expr[i++])});
            continue;
        }

        // unicode × (U+00D7) → two bytes 0xC3 0x97 in UTF-8
        if ((unsigned char)expr[i] == 0xC3 && i + 1 < n && (unsigned char)expr[i+1] == 0x97) {
            tokens.push_back({TOK_OPERATOR, "*"});
            i += 2;
            continue;
        }

        if (OPS.contains(expr[i])) {
            tokens.push_back({TOK_OPERATOR, string(1, expr[i++])});
            continue;
        }

        cerr << "Syntax error: unexpected character '" << expr[i] << "'\n";
        exit(1);
    }

    return tokens;
}

void validateTokens(const vector<Token> &tokens) {
    if (tokens.empty()) {
        cerr << "Syntax error: empty expression\n";
        exit(1);
    }

    TokenType first = tokens.front().type;
    TokenType last  = tokens.back().type;

    if (first == TOK_OPERATOR) {
        cerr << "Syntax error: expression starts with operator '" << tokens.front().value << "'\n";
        exit(1);
    }
    if (last == TOK_OPERATOR) {
        cerr << "Syntax error: expression ends with operator '" << tokens.back().value << "'\n";
        exit(1);
    }
    if (last == TOK_LPAREN) {
        cerr << "Syntax error: expression ends with open bracket\n";
        exit(1);
    }
    if (first == TOK_RPAREN) {
        cerr << "Syntax error: expression starts with close bracket\n";
        exit(1);
    }

    for (int i = 0; i + 1 < (int)tokens.size(); i++) {
        TokenType cur  = tokens[i].type;
        TokenType next = tokens[i + 1].type;

        bool curOperand  = (cur  == TOK_NUMBER || cur  == TOK_VARIABLE);
        bool nextOperand = (next == TOK_NUMBER || next == TOK_VARIABLE);

        if (curOperand && nextOperand) {
            cerr << "Syntax error: missing operator between '" << tokens[i].value << "' and '" << tokens[i+1].value << "'\n";
            exit(1);
        }
        if (cur == TOK_OPERATOR && next == TOK_OPERATOR) {
            cerr << "Syntax error: consecutive operators '" << tokens[i].value << "' and '" << tokens[i+1].value << "'\n";
            exit(1);
        }
        if (curOperand && next == TOK_LPAREN) {
            cerr << "Syntax error: missing operator before '" << tokens[i+1].value << "'\n";
            exit(1);
        }
        if (cur == TOK_RPAREN && nextOperand) {
            cerr << "Syntax error: missing operator after '" << tokens[i].value << "'\n";
            exit(1);
        }
        if (cur == TOK_LPAREN && next == TOK_OPERATOR) {
            cerr << "Syntax error: operator '" << tokens[i+1].value << "' follows open bracket\n";
            exit(1);
        }
        if (cur == TOK_OPERATOR && next == TOK_RPAREN) {
            cerr << "Syntax error: operator '" << tokens[i].value << "' before close bracket\n";
            exit(1);
        }
        if (cur == TOK_LPAREN && next == TOK_RPAREN) {
            cerr << "Syntax error: empty brackets\n";
            exit(1);
        }
    }
}

vector<Token> toPostfix(const vector<Token> &tokens) {
    vector<Token> output;
    stack<Token> ops;

    for (const Token &tok : tokens) {
        if (tok.type == TOK_NUMBER || tok.type == TOK_VARIABLE) {
            output.push_back(tok);
        } else if (tok.type == TOK_OPERATOR) {
            while (!ops.empty() && ops.top().type == TOK_OPERATOR &&
                   precedence(ops.top().value) >= precedence(tok.value)) {
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(tok);
        } else if (tok.type == TOK_LPAREN) {
            ops.push(tok);
        } else if (tok.type == TOK_RPAREN) {
            bool matched = false;
            while (!ops.empty()) {
                if (ops.top().type == TOK_LPAREN) {
                    char open  = ops.top().value[0];
                    char close = tok.value[0];
                    if (open != matchingOpen(close)) {
                        cerr << "Syntax error: mismatched brackets '" << open << "' and '" << close << "'\n";
                        exit(1);
                    }
                    ops.pop();
                    matched = true;
                    break;
                }
                output.push_back(ops.top());
                ops.pop();
            }
            if (!matched) {
                cerr << "Syntax error: unmatched closing bracket '" << tok.value << "'\n";
                exit(1);
            }
        }
    }

    while (!ops.empty()) {
        if (ops.top().type == TOK_LPAREN) {
            cerr << "Syntax error: unmatched opening bracket '" << ops.top().value << "'\n";
            exit(1);
        }
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}

vector<string> collectVars(const vector<Token> &tokens) {
    vector<string> vars;
    map<string, bool> seen;
    for (const Token &t : tokens) {
        if (t.type == TOK_VARIABLE && !seen[t.value]) {
            seen[t.value] = true;
            vars.push_back(t.value);
        }
    }
    return vars;
}

double evaluatePostfix(const vector<Token> &postfix, const map<string, double> &varMap) {
    stack<double> stk;

    for (const Token &tok : postfix) {
        if (tok.type == TOK_NUMBER) {
            stk.push(stod(tok.value));
        } else if (tok.type == TOK_VARIABLE) {
            auto it = varMap.find(tok.value);
            if (it == varMap.end()) {
                cerr << "Runtime error: no value for variable '" << tok.value << "'\n";
                exit(2);
            }
            stk.push(it->second);
        } else if (tok.type == TOK_OPERATOR) {
            if (stk.size() < 2) {
                cerr << "Runtime error: insufficient operands for operator '" << tok.value << "'\n";
                exit(2);
            }
            double b = stk.top(); stk.pop();
            double a = stk.top(); stk.pop();
            if (tok.value == "+") stk.push(a + b);
            else if (tok.value == "-") stk.push(a - b);
            else if (tok.value == "*") stk.push(a * b);
            else if (tok.value == "/") {
                if (b == 0.0) {
                    cerr << "Logical error: division by zero\n";
                    exit(3);
                }
                stk.push(a / b);
            }
        }
    }
    if (stk.size() != 1) {
        cerr << "Runtime error: malformed expression\n";
        exit(2);
    }
    return stk.top();
}

string formatResult(double val) {
    long long iv = (long long)val;
    if ((double)iv == val) return to_string(iv);
    ostringstream oss;
    oss << val;
    return oss.str();
}

int main() {
    string line;
    if (!getline(cin, line)) {
        cerr << "Syntax error: no input provided\n";
        return 1;
    }

    // stage 1 – tokenize
    vector<Token> tokens = tokenize(line);

    // stage 2 – structural validation
    validateTokens(tokens);

    // stage 3 – convert to postfix
    vector<Token> postfix = toPostfix(tokens);

    string postfixStr;
    for (int i = 0; i < (int)postfix.size(); i++) {
        if (i) postfixStr += ' ';
        postfixStr += postfix[i].value;
    }

    // stage 4 – collect variables and prompt user
    vector<string> vars = collectVars(tokens);
    map<string, double> varMap;

    for (const string &var : vars) {
        cerr << "Enter value for " << var << ": ";
        double val;
        if (!(cin >> val)) {
            cerr << "Runtime error: invalid value for variable '" << var << "'\n";
            return 2;
        }
        varMap[var] = val;
    }

    // stage 5 – evaluate
    double result = evaluatePostfix(postfix, varMap);

    // output: postfix then result on stdout
    cout << postfixStr << "\n";
    cout << formatResult(result) << "\n";

    return 0;
}
