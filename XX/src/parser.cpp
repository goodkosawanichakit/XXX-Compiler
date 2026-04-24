#include "parser.hpp"
#include "ast.hpp"
#include "scanner.hpp"

XX::Token XX::Parser::advance() { return scanner.scanToken(); }

bool XX::Parser::match(XX::TokenType t) {
  return (t == currentToken.type) ? true : false;
}

/*
XX::AST::Node *XX::Parser::parse() {
  module = new AST::Node();
  while (currentToken.type != TokenType::TOKEN_EOF) {
    switch (currentToken.type) {}
  }
  return module;
}
*/
