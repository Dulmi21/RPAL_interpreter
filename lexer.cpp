#include "lexer.h"
#include <cctype>    // For std::isdigit
#include <algorithm> // For std::find
#include <iostream>

lexer::lexer() {}

lexer::~lexer() {}

void lexer::tokenize(const std::string &input)
{
    tokens.clear(); // Clear previous tokens

    int offset = 0; // Initialize offset

    size_t index = 0;
    while (index < input.size())
    {
        char c = input[index];

        if (isWhitespace(c))
        {
            // Skip whitespace
            index++;
            offset++;
        }
        else if (isIdentifier(c))
        {
            // Identify identifiers
            size_t start = index;
            while (index < input.size() && (isIdentifier(input[index]) || isDigit(input[index])))
            {
                index++;
                offset++;
            }
            std::string value = input.substr(start, index - start);
            addToken(TOK_IDENTIFIER, value, start);
        }
        else if (isDigit(c))
        {
            // Identify integers
            size_t start = index;
            while (index < input.size() && isDigit(input[index]))
            {
                index++;
                offset++;
            }
            std::string value = input.substr(start, index - start);
            addToken(TOK_INTEGER, value, start);
        }
        else if (isOperator(c))
        {
            // Identify operators
            size_t start = index;
            while (index < input.size() && isOperator(input[index]))
            {
                index++;
                offset++;
            }
            std::string value = input.substr(start, index - start);
            addToken(TOK_OPERATOR, value, start);
        }
        else if (c == '\'')
        {
            // Identify string literals
            size_t start = index;
            bool escaped = false;
            index++; // Skip opening quote
            offset++;
            while (index < input.size() && (input[index] != '\'' || escaped))
            {
                if (input[index] == '\\' && !escaped)
                {
                    escaped = true;
                }
                else
                {
                    escaped = false;
                }
                index++;
                offset++;
            }
            if (index < input.size())
            {
                index++; // Skip closing quote
                offset++;
            }
            std::string value = input.substr(start, index - start);
            addToken(TOK_STRING, value, start);
        }
        else if (isPunctuation(c))
        {
            // Identify punctuation
            addToken(TOK_PUNCTION, std::string(1, c), offset);
            index++;
            offset++;
        }
        else if (isEOL(c))
        {
            // Handle End-Of-Line
            index++;
            offset++;
        }
        else
        {
            // Invalid character, skip
            index++;
            offset++;
        }
    }
}

std::vector<token> lexer::getTokens() const
{
    return tokens;
}

bool lexer::isIdentifier(char c)
{
    return std::isalpha(c) || c == '_';
}

bool lexer::isDigit(char c)
{
    return std::isdigit(c);
}

bool lexer::isOperator(char c)
{
    // Define the list of valid operator symbols
    const std::string operators = "+-*/<>&.@/:=˜|$!#%^_[],{}\"‘?";
    return operators.find(c) != std::string::npos;
}

bool lexer::isWhitespace(char c)
{
    return c == ' ' || c == '\t'; // Add more whitespace characters as needed
}

bool lexer::isPunctuation(char c)
{
    // Check for valid punctuation characters
    return c == '(' || c == ')' || c == ';' || c == ',';
}

bool lexer::isEOL(char c)
{
    // Check for End-Of-Line characters
    return c == '\n' || c == '\r';
}

void lexer::addToken(tokenType type, const std::string &value, int offset)
{
    // Add a token to the token vector
    token t;
    t.tokType = type;
    t.tokValue = value;
    t.offset = offset;
    tokens.push_back(t);
}

token *lexer::getNextToken()
{
    // Check if there are tokens available
    if (!tokens.empty())
    {
        // Get the next token
        token *nextToken = new token(tokens.front());
        // Remove the token from the vector
        tokens.erase(tokens.begin());
        return nextToken;
    }
    else
    {
        // Return nullptr if there are no more tokens
        token *emptyToken = new token();
        // Set its properties
        emptyToken->setToken(8, "EOF", tokens.size()); // Assuming 8 represents EOF type and 0 represents an empty offset
        return emptyToken;
    }
}
