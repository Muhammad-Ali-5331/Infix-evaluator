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
