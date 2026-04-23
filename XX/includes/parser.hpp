#pragma once

#include "ast.hpp"
#include "scanner.hpp"

namespace XX {
class Parser {
private:
  Scanner &scanner;
  AST::Node *module;
  Token currentToken;
  Token previousToken;

  Token advance();
  bool match(TokenType t);

public:
  AST::Node *parse();

  Parser(Scanner &s) : scanner(s) { currentToken = advance(); }
  ~Parser() {}
};
} // namespace XX
