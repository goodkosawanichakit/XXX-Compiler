#pragma once

#include "ast.hpp"
#include "scanner.hpp"
#include "token.hpp"

namespace KIWI {
class Parser {
private:
  Scanner &scanner;
  const std::string &source;
  AST::Node *module;
  Token currentToken;
  Token previousToken;

  void advance();
  bool match(TokenType t);
  int getBindingPower(TokenType t);
  bool expect(TokenType t);
  bool expectSemi();
  void panic();
  AST::Type matchType(TokenType t);
  AST::Declr *parseDeclr();
  AST::Declr *parseVarDeclr();
  AST::Declr *parseFuncDeclr();
  std::vector<AST::Declr *> parseParams();
  AST::Declr *parseParam();
  AST::Block *parseBlock();
  AST::Expr *parseExpr(int b);
  AST::Expr *parseGroupExpr();
  AST::BinaryExpr *parseBinaryExpr();
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
