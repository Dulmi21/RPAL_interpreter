#include <string>
#ifndef TOKEN_H_
#define TOKEN_H_

enum tokenType
{
    TOK_IDENTIFIER = 0,
    TOK_INTEGER = 1,
    TOK_OPERATOR = 2,
    TOK_STRING = 3,
    TOK_WHITESPACE = 4,
    TOK_DELETE = 5,
    TOK_COMMENT = 6,
    TOK_PUNCTION = 7,
    TOK_EOF = 8,
    TOK_ERROR = 9,
    TOK_ANY = 10
};

class token
{
public:
    token();
    virtual ~token();
    int tokType;
    std::string tokValue;
    int offset;
    std::string getValue();
    void setToken(int type, std::string value, int off);
};

#endif
