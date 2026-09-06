#pragma once

#include "ast.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include <memory>

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
  std::unique_ptr<AST::Declr> parseDeclr();
  std::unique_ptr<AST::Declr> parseVarDeclr();
  std::unique_ptr<AST::Declr> parseFuncDeclr();
  std::vector<std::unique_ptr<AST::Declr>> parseParams();
  std::unique_ptr<AST::Declr> parseParam();
  std::unique_ptr<AST::Block> parseBlock();
  std::unique_ptr<AST::Node> parseBlockItems();
  std::unique_ptr<AST::Stmt> parseRet();
  std::unique_ptr<AST::Stmt> parseAssign();
  std::unique_ptr<AST::Expr> parseExpr(int b);
  std::unique_ptr<AST::Expr> parseGroupExpr();
  std::unique_ptr<AST::Expr> parseUnaryExpr();
  std::unique_ptr<AST::Identifier> parseIdent();
  std::unique_ptr<AST::Expr> parseLiteral();
  std::unique_ptr<AST::IntLiteral> parseIntLiteral();
  std::unique_ptr<AST::FloatLiteral> parseFloatLiteral();

public:
  std::unique_ptr<AST::Forest> parse();
  Parser(Scanner &s, std::string &sc) : scanner(s), source(sc) { advance(); }
  ~Parser() {}
};
} // namespace KIWI
