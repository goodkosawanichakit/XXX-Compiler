#pragma once

#include "ast.hpp"
#include "scanner.hpp"
#include "token.hpp"

namespace KIWI {
class Parser {
private:
  Scanner &scanner;
  const std::string &source;
  Token currentToken;
  Token previousToken;

  void advance();
  bool match(TokenType t);
  int getBindingPower(TokenType t);
  bool expect(TokenType t);
  void panic();
  AST::Type parseType(TokenType t);
  AST::Declr *parseDeclr();
  AST::Declr *parseVarDeclr();
  AST::Declr *parseFuncDeclr();
  std::vector<AST::Declr *> parseParams();
  AST::Declr *parseParam();
  AST::Block *parseBlock();
  AST::Node *parseBlockItems();
  AST::Expr *parseExpr(int b);
  AST::Expr *parseGroupExpr();
  AST::Expr *parseUnaryExpr();
  AST::Identifier *parseIdent();
  AST::Expr *parseLiteral();
  AST::IntLiteral *parseIntLiteral();
  AST::FloatLiteral *parseFloatLiteral();

public:
  AST::Forest *parse();
  Parser(Scanner &s, std::string &sc) : scanner(s), source(sc) { advance(); }
  ~Parser() {}
};
} // namespace KIWI
