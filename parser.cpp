#include "parser.h"
#include <stack>
#include <string>
#include <iostream>
#include <algorithm>

parser::parser(lexer *lex)
{
    this->lex = lex;
    nextToken = NULL;
    std::vector<token> tokens = lex->getTokens();
    std::cout << "Parser initialized" << std::endl;
}

parser::~parser()
{
}

void parser::read(const std::string &tokenStr)
{
    std::vector<token> tokens = lex->getTokens();
    std::cout << "Reading token: " << tokenStr << std::endl;
    if (nextToken == NULL)
    {
        nextToken = lex->getNextToken();
    }

    if (nextToken->getValue() == tokenStr)
    {
        delete nextToken;
        nextToken = lex->getNextToken();
    }
    else
    {
        std::cerr << "Error: Expected " << tokenStr << " but got " << nextToken->getValue() << std::endl;
        exit(1);
    }
}

void parser::buildTree(const std::string &nodeStr, int numChildNodes, int type)
{
    std::cout << "Building tree with node string: " << nodeStr << std::endl;
    treeNode *newNode = new treeNode(nodeStr, type);
    if (numChildNodes > 0)
    {
        for (int i = 0; i < numChildNodes; ++i)
        {
            if (treeStack.empty())
            {
                std::cerr << "Error: Stack is empty" << std::endl;
                exit(1);
            }
            treeNode *child = treeStack.top();
            treeStack.pop();

            // Add the child node to the new node's children vector
            newNode->children.push_back(child);
        }
        std::reverse(newNode->children.begin(), newNode->children.end());

        // for (int i = numChildNodes - 1; i >= 0; --i)
        // {
        //     newNode->children.push_back(newNode->children[i]);
        // }
    }

    // Push the new node onto the stack
    treeStack.push(newNode);

    std::cout << "Children of the new node:" << std::endl;
    for (const auto &child : newNode->children)
    {
        std::cout << child->nodeString << std::endl;
    }
}

bool parser::isKeyword(const std::string &val)
{
    std::cout << "Checking if " << val << " is a keyword" << std::endl;
    if (val == "in" || val == "where" || val == "." || val == "aug" || val == "and" || val == "or" || val == "&" || val == "not" || val == "gr" || val == "ge" || val == "ls" || val == "le" || val == "eq" || val == "ne" || val == "+" || val == "-" || val == "*" || val == "/" || val == "**" || val == "@" || val == "within" || val == "rec" || val == "let" || val == "fn")
        return true;
    else
        return false;
}

void parser::parse()
{
    std::vector<token> tokens = lex->getTokens();
    std::cout << "Parsing started" << std::endl;

    do
    {
        nextToken = lex->getNextToken();
        std::cout << "Next token: " << nextToken->getValue() << std::endl;
    } while (nextToken->tokType == TOK_DELETE);
    E();

    if (!treeStack.empty() && treeStack.size() != 1)
    {
        printf("Error: Stack not empty at the end of parsing\n");
        exit(0);
    }
    else if (lex->getNextToken()->tokType != TOK_EOF)
    {
        printf("Error: Parsing finished input still remaining\n");
        exit(0);
    }
    printf("Parsing Finished. AST Generated\n");
}

/* E    -> ’let’ D ’in’ E                          => ’let’
     -> ’fn’ Vb+ ’.’ E                          => ’lambda’
     ->  Ew;
*/
void parser::E()
{
    std::cout << "Parsing E" << std::endl;
    if (nextToken->tokValue == "let")
    {
        read("let");
        D();
        read("in");
        E();
        buildTree("let", 2, treeNode::LET);
    }
    else if (nextToken->tokValue == "fn")
    {
        read("fn");
        int n = 0;
        do
        {
            Vb();
            n++;
        } while (nextToken->tokValue == "(" || nextToken->tokType == TOK_IDENTIFIER);
        read(".");
        E();
        buildTree("lambda", n + 1, treeNode::LAMBDA);
    }
    else
    {
        Ew();
        std::cout << "came out of Ew" << std::endl;
    }
}

/* Ew   -> T ’where’ Dr                            => ’where’
   -> T;*/
void parser::Ew()
{
    std::cout << "Parsing Ew" << std::endl;
    T();
    if (nextToken->tokValue == "where")
    {
        read("where");
        Dr();
        buildTree("where", 2, treeNode::WHERE);
    }
}

/* T    -> Ta ( ’,’ Ta )+                          => ’tau’
    -> Ta ;
 */
void parser::T()
{
    std::cout << "Parsing T" << std::endl;
    int n = 0;
    Ta();
    if (nextToken->tokValue == ",")
    {
        do
        {
            read(",");
            Ta();
            n++;
        } while (nextToken->tokValue == ",");
        buildTree("tau", n + 1, treeNode::TAU);
    }
}

/* Ta   -> Ta ’aug’ Tc                             => ’aug’
    -> Tc ;
 */
void parser::Ta()
{
    std::cout << "Parsing Ta" << std::endl;
    Tc();
    while (nextToken->tokValue == "aug")
    {
        read("aug");
        Tc();
        buildTree("aug", 2, treeNode::AUG);
    }
}

/* Tc   -> B ’->’ Tc ’|’ Tc                      => '->'
    -> B ;
 */
void parser::Tc()
{
    std::cout << "Parsing Tc" << std::endl;
    B();
    if (nextToken->tokValue == "->")
    {
        read("->");
        Tc();
        read("|");
        Tc();
        buildTree("->", 3, treeNode::TERNARY);
    }
}

/* B    -> B ’or’ Bt                               => ’or’
     -> Bt ;
  */
void parser::B()
{
    std::cout << "Parsing B" << std::endl;
    Bt();
    while (nextToken->tokValue == "or")
    {
        read("or");
        Bt();
        buildTree("or", 2, treeNode::OR);
    }
}

/*  Bt   -> Bt ’&’ Bs                               => ’&’
     -> Bs ;
  */
void parser::Bt()
{
    std::cout << "Parsing Bt" << std::endl;
    Bs();
    while (nextToken->tokValue == "&")
    {
        read("&");
        Bs();
        buildTree("&", 2, treeNode::AND_LOGICAL);
    }
}

/* Bs   -> ’not’ Bp                                => ’not’
     -> Bp ;
  */
void parser::Bs()
{
    std::cout << "Parsing Bs" << std::endl;
    if (nextToken->tokValue == "not")
    {
        read("not");
        Bp();
        buildTree("not", 1, treeNode::NOT);
    }
    else
    {
        Bp();
    }
}

/* Bp   -> A (’gr’ | ’>’ ) A                       => ’gr’
     -> A (’ge’ | ’>=’) A                       => ’ge’
     -> A (’ls’ | ’<’ ) A                       => ’ls’
     -> A (’le’ | ’<=’) A                       => ’le’
     -> A ’eq’ A                                => ’eq’
     -> A ’ne’ A                                => ’ne’
     -> A ;
  */
void parser::Bp()
{
    std::cout << "Parsing Bp" << std::endl;
    A();
    if (nextToken->tokValue == "gr" || nextToken->tokValue == ">")
    {
        if (nextToken->tokValue == "gr")
            read("gr");
        else if (nextToken->tokValue == ">")
            read(">");
        A();
        buildTree("gr", 2, treeNode::GR);
    }
    else if (nextToken->tokValue == "ge" || nextToken->tokValue == ">=")
    {
        if (nextToken->tokValue == "ge")
            read("ge");
        else if (nextToken->tokValue == ">=")
            read(">=");
        A();
        buildTree("ge", 2, treeNode::GE);
    }
    else if (nextToken->tokValue == "ls" || nextToken->tokValue == "<")
    {
        if (nextToken->tokValue == "ls")
            read("ls");
        else if (nextToken->tokValue == "<")
            read("<");
        A();
        buildTree("ls", 2, treeNode::LS);
    }
    else if (nextToken->tokValue == "le" || nextToken->tokValue == "<=")
    {
        if (nextToken->tokValue == "le")
            read("le");
        else if (nextToken->tokValue == "<=")
            read("<=");
        A();
        buildTree("le", 2, treeNode::LE);
    }
    else if (nextToken->tokValue == "eq")
    {
        read("eq");
        A();
        buildTree("eq", 2, treeNode::EQ);
    }
    else if (nextToken->tokValue == "ne")
    {
        read("ne");
        A();
        buildTree("ne", 2, treeNode::NE);
    }
}

/* A    -> A ’+’ At                                => ’+’
     -> A ’-’ At                                => ’-’
     ->   ’+’ At
     ->   ’-’ At                                => ’neg’
     -> At ;
  */
void parser::A()
{
    std::cout << "Parsing A" << std::endl;
    std::string treeStr;
    if (nextToken->tokValue == "+")
    {
        read("+");
        At();
    }
    else if (nextToken->tokValue == "-")
    {
        read("-");
        At();
        buildTree("neg", 1, treeNode::NEG);
    }
    else
    {
        At();
    }
    while (nextToken->tokValue == "+" || nextToken->tokValue == "-")
    {
        if (nextToken->tokValue == "+")
        {
            read("+");
            treeStr = "+";
        }
        else
        {
            read("-");
            treeStr = "-";
        }
        At();
        buildTree(treeStr, 2, treeStr == "+" ? treeNode::ADD : treeNode::SUBTRACT);
    }
}

/* At   -> At ’*’ Af                               => ’*’
     -> At ’/’ Af                               => ’/’
     -> Af ;
  */
void parser::At()
{
    std::cout << "Parsing At" << std::endl;
    std::string treeStr;
    Af();
    while (nextToken->tokValue == "*" || nextToken->tokValue == "/")
    {
        if (nextToken->tokValue == "*")
        {
            read("*");
            treeStr = "*";
        }
        else
        {
            read("/");
            treeStr = "/";
        }
        Af();
        buildTree(treeStr, 2, treeStr == "*" ? treeNode::MULTIPLY : treeNode::DIVIDE);
    }
}

/* Af   -> Ap ’**’ Af                              => ’**’
     -> Ap ;
  */
void parser::Af()
{
    std::cout << "Parsing Af" << std::endl;
    Ap();
    if (nextToken->tokValue == "**")
    {
        read("**");
        Af();
        buildTree("**", 2, treeNode::EXPONENTIAL);
    }
    std::cout << "leaving Af" << std::endl;
}

/* Ap   -> Ap ’@’ ’<IDENTIFIER>’ R                 => ’@’
     -> R ;
  */
void parser::Ap()
{
    std::cout << "Parsing Ap" << std::endl;
    R();
    std::cout << "just executed R" << std::endl;
    while (nextToken->tokValue == "@")
    {
        read("@");
        buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
        read(nextToken->tokValue);
        R();
        buildTree("@", 3, treeNode::AT);
    }

    std::cout << "leaving Ap" << std::endl;
}

/* R    -> R Rn                                    => ’gamma’
     -> Rn ;
*/
void parser::R()
{
    std::cout << "Parsing R" << std::endl;
    Rn();
    while ((TOK_IDENTIFIER == nextToken->tokType || TOK_INTEGER == nextToken->tokType || TOK_STRING == nextToken->tokType || "(" == nextToken->tokValue || "false" == nextToken->tokValue || "true" == nextToken->tokValue || "nil" == nextToken->tokValue || "dummy" == nextToken->tokValue) && !isKeyword(nextToken->tokValue))
    {
        Rn();
        buildTree("gamma", 2, treeNode::GAMMA);
        // std::cout << "Next token: " << nextToken->getValue() << std::endl;
    }
    std::cout << "leaving R" << std::endl;
}

/* Rn   -> ’<IDENTIFIER>’
     -> ’<INTEGER>’
     -> ’<STRING>’
     -> ’true’                                  => ’true’
     -> ’false’                                 => ’false’
     -> ’nil’                                   => ’nil’
     -> ’(’ E ’)’
     -> ’dummy’                                 => ’dummy’ ;
*/
void parser::Rn()
{
    std::cout << "Parsing Rn" << std::endl;
    if ("(" == nextToken->tokValue)
    {
        read("(");
        E();
        std::cout << "came out of E in RN " << std::endl;
        read(")");
        std::cout << "read the last token )" << std::endl;
    }
    else if (TOK_IDENTIFIER == nextToken->tokType || TOK_INTEGER == nextToken->tokType || TOK_STRING == nextToken->tokType)
    {
        if ("true" == nextToken->tokValue)
        {
            read("true");
            buildTree("<true>", 0, treeNode::TRUE);
        }
        else if ("false" == nextToken->tokValue)
        {
            read("false");
            buildTree("<false>", 0, treeNode::FALSE);
        }
        else if ("nil" == nextToken->tokValue)
        {
            read("nil");
            buildTree("<nil>", 0, treeNode::NIL);
        }
        else if ("dummy" == nextToken->tokValue)
        {
            read("dummy");
            buildTree("<dummy>", 0, treeNode::DUMMY);
        }
        else if (TOK_IDENTIFIER == nextToken->tokType)
        {
            buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
            read(nextToken->tokValue);
            std::cout << "Line " << __LINE__ << ": " << std::endl;
        }
        else if (TOK_STRING == nextToken->tokType)
        {
            buildTree(nextToken->tokValue, 0, treeNode::STRING);
            read(nextToken->tokValue);
        }
        else if (TOK_INTEGER == nextToken->tokType)
        {
            buildTree(nextToken->tokValue, 0, treeNode::INTEGER);
            read(nextToken->tokValue);
            std::cout << "Next token: " << nextToken->getValue() << std::endl;
        }
    }
}

/* D    -> Da ’within’ D                           => ’within’
     -> Da ;
  */
void parser::D()
{
    std::cout << "Parsing D" << std::endl;
    std::cout << "Next token: " << nextToken->getValue() << std::endl;
    Da();

    if (nextToken->tokValue == "within")
    {
        read("within");
        D();
        buildTree("within", 2, treeNode::WITHIN);
    }
}
/*     Da   -> Dr ( ’and’ Dr )+                        => ’and’
         -> Dr ;
  */
void parser::Da()
{
    std::cout << "Parsing Da" << std::endl;
    int n = 0;
    Dr();
    while (nextToken->tokValue == "and")
    {
        read("and");
        Dr();
        n++;
    }
    if (n > 0)
        buildTree("and", n + 1, treeNode::AND);
}

/*      Dr   -> ’rec’ Db                                => ’rec’
         -> Db ;
  */
void parser::Dr()
{
    std::cout << "Parsing Dr" << std::endl;
    if (nextToken->tokValue == "rec")
    {
        read("rec");
        Db();
        buildTree("rec", 1, treeNode::REC);
    }
    else
    {
        Db();
    }
}

/* Db   -> Vl ’=’ E                              => ’=’
     -> ’<IDENTIFIER>’ Vb+ ’=’ E              => ’fcn_form’
     -> ’(’ D ’)’ ;
  */
void parser::Db()
{
    std::cout << "Parsing Db" << std::endl;
    std::cout << "Next token: " << nextToken->getValue() << std::endl;
    if (nextToken->tokValue == "(")
    {
        read("(");
        D();
        read(")");
    }
    else if (nextToken->tokType == TOK_IDENTIFIER)
    {
        // Since identifier type is common here, read it here now and consider it for build tree later.
        buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
        std::cout << "Identifier: " << nextToken->tokValue << std::endl;
        read(nextToken->tokValue);
        std::cout << "Next token: " << nextToken->getValue() << std::endl;
        std::cout << "Line " << __LINE__ << ": " << std::endl;
        if (nextToken->tokValue == "," || nextToken->tokValue == "=")
        {
            Vl();
            read("=");
            E();
            buildTree("=", 2, treeNode::BINDING);
            std::cout << "Next token: " << nextToken->getValue() << std::endl;
            std::cout << "Line " << __LINE__ << ": " << std::endl;
        }
        else
        {
            int n = 0;
            do
            {
                Vb();
                n++;
            } while (nextToken->tokValue == "(" || nextToken->tokType == TOK_IDENTIFIER);
            read("=");
            E();
            buildTree("function_form", n + 2, treeNode::FCN_FORM); // The identifier at the start of this function is included here as n + 2
        }
    }
}

/*    Vb   -> ’<IDENTIFIER>’
         -> ’(’ Vl ’)’
         -> ’(’ ’)’                                 => ’()’;
  */
void parser::Vb()
{
    if (nextToken->tokType == TOK_IDENTIFIER)
    {
        buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
        read(nextToken->tokValue);
    }
    else if (nextToken->tokValue == "(")
    {
        read("(");
        if (nextToken->tokValue == ")")
        {
            read(")");
            buildTree("()", 0, treeNode::PARANTHESES);
        }
        else if (nextToken->tokType == TOK_IDENTIFIER)
        {
            // Before getting into Vl, an identifier must be read
            // Vl expects its caller to do this.
            buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
            read(nextToken->tokValue);
            Vl();
            read(")");
        }
    }
}

//    Vl   -> ’<IDENTIFIER>’ list ’,’                 => ’,’?;
void parser::Vl()
{
    int n = 0;
    while (nextToken->tokValue == ",")
    {
        read(",");
        if (nextToken->tokType == TOK_IDENTIFIER)
        {
            buildTree(nextToken->tokValue, 0, treeNode::IDENTIFIER);
            read(nextToken->tokValue);
            n++;
        }
        else
        {
            printf("ERROR In Vl()\n");
        }
    }
    // n+1 for the identifier that was read before Vl was called.
    if (n > 0)
        buildTree(",", n + 1, treeNode::COMMA);
}

std::stack<treeNode *> parser::getTreeStack() const
{
    return treeStack;
}

void parser::preOrderTraverse(treeNode *node, int depth)
{
    // Print current node's data with appropriate indentation

    for (int i = 0; i < depth; ++i)
    {
        std::cout << ".";
    }

    if (node->type == treeNode::IDENTIFIER)
    {
        std::cout << "<ID:" << node->nodeString << ">" << std::endl;
    }
    else if (node->type == treeNode::INTEGER)
    {
        std::cout << "<INT:" << node->nodeString << ">" << std::endl;
    }

    else{
        std::cout << node->nodeString << std::endl;
    }
    

    // Recursively traverse each child node

    for (auto childNode : node->children)
    {
        // std::cout << "childNode: " << childNode->nodeString << std::endl;
        preOrderTraverse(childNode, depth + 1);
    }
}

void parser::printAST()
{
    std::stack<treeNode *> treeStack = getTreeStack();
    if (!treeStack.empty())
    {
        preOrderTraverse(treeStack.top(), 0);
    }
}
