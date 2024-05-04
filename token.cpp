#include "token.h"

token::token()
{
    tokType = 0;
    tokValue = "";
    offset = 0;
}

token::~token()
{
}

void token::setToken(int type, std::string value, int off)
{
    tokType = type;
    tokValue = value;
    offset = off;
}

std::string token::getValue()
{
    return tokValue;
}
