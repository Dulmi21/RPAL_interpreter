#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <vector>
#include "token.h"

class lexer
{
public:
    lexer();
    ~lexer();

    void tokenize(const std::string &input);
    std::vector<token> getTokens() const;
    token *getNextToken();
    std::vector<token> tokens;

private:
    

    // Helper methods for tokenization
    bool isIdentifier(char c);
    bool isDigit(char c);
    bool isOperator(char c);
    bool isWhitespace(char c);
    bool isPunctuation(char c);
    bool isComment(const std::string &input, size_t &index);
    bool isStringLiteral(const std::string &input, size_t &index);
    bool isEOL(char c);
    bool isEOF(char c);
    bool isKeyword(const std::string &input, size_t &index);

    void addToken(tokenType type, const std::string &value, int offset);
};

#endif /* LEXER_H_ */
