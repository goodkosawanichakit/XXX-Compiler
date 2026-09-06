#include "parser.hpp"
#include "ast.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include <cstdint>
#include <memory>
#include <string>

void KIWI::Parser::advance() {
  previousToken = currentToken;
  currentToken = scanner.scanToken();
}

bool KIWI::Parser::match(KIWI::TokenType t) {
  return (t == currentToken.type) ? true : false;
}

bool KIWI::Parser::expect(KIWI::TokenType t) {
  if (match(t)) {
    advance();
    return true;
  }

  panic();
  return false;
}

KIWI::AST::Type KIWI::Parser::parseType(KIWI::TokenType t) {
  advance();
  switch (t) {
  case KIWI::TokenType::KW_INT8:
    return AST::Type::INT8;
  case KIWI::TokenType::KW_INT16:
    return AST::Type::INT16;
  case KIWI::TokenType::KW_INT32:
    return AST::Type::INT32;
  case KIWI::TokenType::KW_INT64:
    return AST::Type::INT64;
  case KIWI::TokenType::KW_FLOAT8:
    return AST::Type::FLOAT8;
  case KIWI::TokenType::KW_FLOAT16:
    return AST::Type::FLOAT16;
  case KIWI::TokenType::KW_FLOAT32:
    return AST::Type::FLOAT32;
  case KIWI::TokenType::KW_FLOAT64:
    return AST::Type::FLOAT64;
  default:
    // TODO: It's unreachable what do I do (in todo yes)
    return AST::Type::INT8;
  }
}

void KIWI::Parser::panic() {
  while (!match(TokenType::TOKEN_EOF) && !match(TokenType::SEMICOLON) &&
         !currentToken.isReservedWord())
    advance();

  if (match(TokenType::SEMICOLON))
    advance();
}

std::unique_ptr<KIWI::AST::Forest> KIWI::Parser::parse() {
  std::unique_ptr<AST::Forest> module = std::make_unique<AST::Forest>();
  while (!match(TokenType::TOKEN_EOF)) {
    module->vec.push_back(parseDeclr());
  }
  return module;
}

std::unique_ptr<KIWI::AST::Declr> KIWI::Parser::parseDeclr() {
  switch (currentToken.type) {
  case TokenType::KW_INT8:
  case TokenType::KW_INT16:
  case TokenType::KW_INT32:
  case TokenType::KW_INT64:
  case TokenType::KW_FLOAT8:
  case TokenType::KW_FLOAT16:
  case TokenType::KW_FLOAT32:
  case TokenType::KW_FLOAT64:
    return parseVarDeclr();
  case TokenType::KW_FN:
    return parseFuncDeclr();
  default:
    advance();
    return std::make_unique<AST::ErrorDeclr>(
        previousToken.offset, previousToken.length,
        "Unknown keyword at -> " +
            source.substr(previousToken.offset, currentToken.offset +
                                                    currentToken.length -
                                                    previousToken.offset));
  }
}

std::unique_ptr<KIWI::AST::Declr> KIWI::Parser::parseFuncDeclr() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;
  advance();

  std::unique_ptr<AST::Identifier> ident = parseIdent();

  // TODO: Return a proper Error message
  if (!expect(TokenType::LEFT_PAREN)) {
    return nullptr;
  }

  std::vector<std::unique_ptr<AST::Declr>> params;
  params = parseParams();

  if (!expect(TokenType::RIGHT_PAREN)) {
    return nullptr;
  }

  if (!expect(TokenType::RETURN_TYPE)) {
    return nullptr;
  }

  AST::Type retType = parseType(currentToken.type);

  std::unique_ptr<AST::Block> block = parseBlock();

  return std::make_unique<AST::FuncDeclr>(
      o, l, std::move(ident), std::move(params), retType, std::move(block));
}

std::vector<std::unique_ptr<KIWI::AST::Declr>> KIWI::Parser::parseParams() {
  std::vector<std::unique_ptr<KIWI::AST::Declr>> params;
  while (!match(TokenType::RIGHT_PAREN) && !match(TokenType::TOKEN_EOF)) {
    params.push_back(parseParam());
    if (!match(TokenType::COMMA))
      break;
    advance();
  }
  return params;
}

std::unique_ptr<KIWI::AST::Declr> KIWI::Parser::parseParam() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;

  AST::Type t = parseType(currentToken.type);

  std::unique_ptr<AST::Identifier> ident = parseIdent();

  return std::make_unique<AST::ParamDeclr>(o, l, t, std::move(ident));
}

std::unique_ptr<KIWI::AST::Block> KIWI::Parser::parseBlock() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;

  if (!expect(TokenType::LEFT_BRACE)) {
    return nullptr;
  }

  std::vector<std::unique_ptr<AST::Node>> stmts;

  while (!match(TokenType::RIGHT_BRACE) && !match(TokenType::TOKEN_EOF)) {
    stmts.push_back(parseBlockItems());
  }

  if (!expect(TokenType::RIGHT_BRACE)) {
    // TODO: free items in stmts;
    return nullptr;
  }

  return std::make_unique<AST::Block>(o, l, std::move(stmts));
}

std::unique_ptr<KIWI::AST::Node> KIWI::Parser::parseBlockItems() {
  switch (currentToken.type) {
  case TokenType::KW_INT8:
  case TokenType::KW_INT16:
  case TokenType::KW_INT32:
  case TokenType::KW_INT64:
  case TokenType::KW_FLOAT8:
  case TokenType::KW_FLOAT16:
  case TokenType::KW_FLOAT32:
  case TokenType::KW_FLOAT64:
    return parseVarDeclr();
  case TokenType::IDENTIFIER:
    return parseAssign();
  case TokenType::KW_RETURN:
    return parseRet();
  default:
    advance();
    return std::make_unique<AST::ErrorDeclr>(
        previousToken.offset, previousToken.length,
        "Unknown keyword at -> " +
            source.substr(previousToken.offset, currentToken.offset +
                                                    currentToken.length -
                                                    previousToken.offset));
  }
}

std::unique_ptr<KIWI::AST::Stmt> KIWI::Parser::parseRet() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;
  advance();

  std::unique_ptr<AST::Expr> e = parseExpr(0);

  if (!expect(TokenType::SEMICOLON))
    return nullptr;

  return std::make_unique<AST::ReturnStmt>(o, l, std::move(e));
}

std::unique_ptr<KIWI::AST::Stmt> KIWI::Parser::parseAssign() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;

  std::unique_ptr<AST::Identifier> ident = parseIdent();

  if (!expect(TokenType::EQUAL))
    return nullptr;

  std::unique_ptr<AST::Expr> expr = parseExpr(0);

  if (!expect(TokenType::SEMICOLON))
    return nullptr;

  return std::make_unique<AST::AssignStmt>(o, l, std::move(ident),
                                           std::move(expr));
}

// VarDeclr = type identifiers "="  (Expr | BinaryExpr) ";"
std::unique_ptr<KIWI::AST::Declr> KIWI::Parser::parseVarDeclr() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;
  AST::Type t = parseType(currentToken.type);

  std::unique_ptr<AST::Identifier> ident = parseIdent();

  if (!match(TokenType::EQUAL)) {
    if (!expect(TokenType::SEMICOLON)) {
      return std::make_unique<AST::ErrorDeclr>(
          o, l,
          "You forget to add ';' at the end of this -> " +
              source.substr(o,
                            previousToken.offset + previousToken.length - o));
    }
    return std::make_unique<AST::VarDeclr>(o, l, t, std::move(ident), nullptr);
  }
  advance();

  std::unique_ptr<AST::Expr> value = parseExpr(0);

  if (value->getKind() == AST::Kind::ERROR_EXPR)
    return std::make_unique<AST::VarDeclr>(o, l, t, std::move(ident),
                                           std::move(value));

  if (!expect(TokenType::SEMICOLON)) {
    return std::make_unique<AST::ErrorDeclr>(
        o, l,
        "You forget to add ';' at the end of this -> " +
            source.substr(o, previousToken.offset + previousToken.length - o));
  }

  return std::make_unique<AST::VarDeclr>(o, l, t, std::move(ident),
                                         std::move(value));
}

int KIWI::Parser::getBindingPower(KIWI::TokenType t) {
  switch (t) {
  case TokenType::PLUS:
  case TokenType::MINUS:
    return 10;
  case TokenType::STAR:
  case TokenType::SLASH:
    return 20;
  default:
    return 0;
  }
}

// What's the Expression?
// I don't have any idea either, JK
// BinaryExpr = Expr "OP" Expr
// Expr = IntLiteral | FloatLiteral | BinaryExpr | ...
// b is for binding power in case I forget it.
std::unique_ptr<KIWI::AST::Expr> KIWI::Parser::parseExpr(int b) {
  std::unique_ptr<AST::Expr> left = parseLiteral();
  while (b < getBindingPower(currentToken.type)) {
    uint32_t o = currentToken.offset;
    uint16_t l = currentToken.length;
    std::string op = source.substr(currentToken.offset, currentToken.length);
    int currB = getBindingPower(currentToken.type);
    advance();
    std::unique_ptr<AST::Expr> right = parseExpr(currB);
    if (right->getKind() == AST::Kind::ERROR_EXPR)
      return right;
    left = std::make_unique<AST::BinaryExpr>(o, l, op, std::move(left),
                                             std::move(right));
  }
  return left;
}

std::unique_ptr<KIWI::AST::Expr> KIWI::Parser::parseGroupExpr() {
  advance();
  std::unique_ptr<AST::Expr> e = parseExpr(0);

  if (e->getKind() == AST::Kind::ERROR_EXPR)
    return e;

  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;

  if (!expect(TokenType::RIGHT_PAREN))
    return std::make_unique<AST::ErrorExpr>(
        o, l,
        "Expected ')' at -> " +
            source.substr(o, previousToken.offset + previousToken.length - o));

  return e;
}

std::unique_ptr<KIWI::AST::Expr> KIWI::Parser::parseUnaryExpr() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;
  std::string op = source.substr(currentToken.offset, currentToken.length);
  advance();
  std::unique_ptr<AST::Expr> expr = parseExpr(50);
  if (expr->getKind() == AST::Kind::ERROR_EXPR)
    return expr;
  return std::make_unique<AST::UnaryExpr>(o, l, op, std::move(expr));
}

std::unique_ptr<KIWI::AST::Expr> KIWI::Parser::parseLiteral() {
  switch (currentToken.type) {
  case TokenType::MINUS:
    return parseUnaryExpr();
  case TokenType::NUMBER_INT:
    return parseIntLiteral();
  case TokenType::NUMBER_FLOAT:
    return parseFloatLiteral();
  case TokenType::IDENTIFIER:
    return parseIdent();
  case TokenType::LEFT_PAREN:
    return parseGroupExpr();
  default:
    uint32_t o = currentToken.offset;
    uint16_t l = currentToken.length;
    panic();
    return std::make_unique<AST::ErrorExpr>(
        o, l,
        "Expected Expression at -> " +
            source.substr(o, previousToken.offset + previousToken.length - o));
  }
}

std::unique_ptr<KIWI::AST::Identifier> KIWI::Parser::parseIdent() {
  advance();
  return std::make_unique<AST::Identifier>(
      previousToken.offset, previousToken.length,
      source.substr(previousToken.offset, previousToken.length));
}

std::unique_ptr<KIWI::AST::IntLiteral> KIWI::Parser::parseIntLiteral() {
  advance();
  return std::make_unique<AST::IntLiteral>(
      previousToken.offset, previousToken.length,
      std::stoll(source.substr(previousToken.offset, previousToken.length)));
}

std::unique_ptr<KIWI::AST::FloatLiteral> KIWI::Parser::parseFloatLiteral() {
  advance();
  return std::make_unique<AST::FloatLiteral>(
      previousToken.offset, previousToken.length,
      std::stod(source.substr(previousToken.offset, previousToken.length)));
}
