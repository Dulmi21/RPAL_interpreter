#include <iostream>
#include "parser.h"
#include <string>
#include <stack>
#include <vector>

int main()
{
    // Input string to tokenize
    std::string input = "let Sum(A) = Psum (A,Order A ) where rec Psum(T, N) = N eq 0->0 | Psum(T, N - 1) + T N in Print(Sum(1, 2, 3, 4, 5)) ";

    // Create a lexer instance
    lexer lexer;

    // Tokenize the input string
    lexer.tokenize(input);

    // Get the vector of tokens
    std::vector<token> tokens = lexer.getTokens();

    // Print out the tokens
    for (const token &t : tokens)
    {
        std::cout << "Type: " << t.tokType << ", Value: " << t.tokValue << ", Offset: " << t.offset << std::endl;
    }

    // Create a parser instance with the lexer
    parser parser(&lexer);

    // Parse the input tokens
    parser.parse();

    // Print the abstract syntax tree (AST)
    std::cout << "Abstract Syntax Tree (AST):" << std::endl;

    parser.printAST(); // Change this line

    // Print the tree structure
    // parser.print_string_in_treestack(treeStack);

    return 0;
}
