#include "parser.hpp"
#include "ast.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include <cstdint>
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

KIWI::AST::Forest *KIWI::Parser::parse() {
  AST::Forest *module = new AST::Forest();
  while (!match(TokenType::TOKEN_EOF)) {
    module->vec.push_back(parseDeclr());
  }
  return module;
}

KIWI::AST::Declr *KIWI::Parser::parseDeclr() {
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
    return new AST::ErrorDeclr(
        previousToken.offset, previousToken.length,
        "Unknown keyword at -> " +
            source.substr(previousToken.offset, currentToken.offset +
                                                    currentToken.length -
                                                    previousToken.offset));
  }
}

KIWI::AST::Declr *KIWI::Parser::parseFuncDeclr() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;
  advance();

  AST::Identifier *ident = parseIdent();

  if (!expect(TokenType::LEFT_PAREN)) {
    return nullptr;
  }

  std::vector<AST::Declr *> params;
  params = parseParams();

  if (!expect(TokenType::RIGHT_PAREN)) {
    return nullptr;
  }

  if (!expect(TokenType::RETURN_TYPE)) {
    return nullptr;
  }

  AST::Type retType = parseType(currentToken.type);

  AST::Block *block = parseBlock();

  return new AST::FuncDeclr(o, l, ident, params, retType, block);
}

std::vector<KIWI::AST::Declr *> KIWI::Parser::parseParams() {
  std::vector<AST::Declr *> params;
  while (!match(TokenType::RIGHT_PAREN) && !match(TokenType::TOKEN_EOF)) {
    params.push_back(parseParam());
    if (!match(TokenType::COMMA))
      break;
    advance();
  }
  return params;
}

KIWI::AST::Declr *KIWI::Parser::parseParam() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;

  AST::Type t = parseType(currentToken.type);

  AST::Identifier *ident = parseIdent();

  return new AST::ParamDeclr(o, l, t, ident);
}

KIWI::AST::Block *KIWI::Parser::parseBlock() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;

  if (!expect(TokenType::LEFT_BRACE)) {
    return nullptr;
  }

  std::vector<AST::Node *> stmts;

  while (!match(TokenType::RIGHT_BRACE) && !match(TokenType::TOKEN_EOF)) {
    stmts.push_back(parseBlockItems());
  }

  if (!expect(TokenType::RIGHT_BRACE)) {
    // TODO: free items in stmts;
    return nullptr;
  }

  return new AST::Block(o, l, stmts);
}

KIWI::AST::Node *KIWI::Parser::parseBlockItems() {
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
    // parseAssign:
  case TokenType::KW_RETURN:
    return parseRet();
  default:
    advance();
    return new AST::ErrorDeclr(
        previousToken.offset, previousToken.length,
        "Unknown keyword at -> " +
            source.substr(previousToken.offset, currentToken.offset +
                                                    currentToken.length -
                                                    previousToken.offset));
  }
}

KIWI::AST::Stmt *KIWI::Parser::parseRet() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;
  advance();

  AST::Expr *e = parseExpr(0);

  if (!expect(TokenType::SEMICOLON))
    return nullptr;

  return new AST::ReturnStmt(o, l, e);
}

// VarDeclr = type identifiers "="  (Expr | BinaryExpr) ";"
KIWI::AST::Declr *KIWI::Parser::parseVarDeclr() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;
  AST::Type t = parseType(currentToken.type);

  AST::Identifier *ident = parseIdent();

  if (!match(TokenType::EQUAL)) {
    if (!expect(TokenType::SEMICOLON)) {
      return new AST::ErrorDeclr(
          o, l,
          "You forget to add ';' at the end of this -> " +
              source.substr(o,
                            previousToken.offset + previousToken.length - o));
    }
    return new AST::VarDeclr(o, l, t, ident, nullptr);
  }
  advance();

  KIWI::AST::Expr *value = parseExpr(0);

  if (value->getKind() == AST::Kind::ERROR_EXPR)
    return new AST::VarDeclr(o, l, t, ident, value);

  if (!expect(TokenType::SEMICOLON)) {
    return new AST::ErrorDeclr(
        o, l,
        "You forget to add ';' at the end of this -> " +
            source.substr(o, previousToken.offset + previousToken.length - o));
  }

  return new AST::VarDeclr(o, l, t, ident, value);
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
KIWI::AST::Expr *KIWI::Parser::parseExpr(int b) {
  AST::Expr *left = parseLiteral();
  while (b < getBindingPower(currentToken.type)) {
    uint32_t o = currentToken.offset;
    uint16_t l = currentToken.length;
    std::string op = source.substr(currentToken.offset, currentToken.length);
    int currB = getBindingPower(currentToken.type);
    advance();
    AST::Expr *right = parseExpr(currB);
    if (right->getKind() == AST::Kind::ERROR_EXPR)
      return right;
    left = new AST::BinaryExpr(o, l, op, left, right);
  }
  return left;
}

KIWI::AST::Expr *KIWI::Parser::parseGroupExpr() {
  advance();
  AST::Expr *e = parseExpr(0);

  if (e->getKind() == AST::Kind::ERROR_EXPR)
    return e;

  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;

  if (!expect(TokenType::RIGHT_PAREN)) {
    return new AST::ErrorExpr(
        o, l,
        "Expected ')' at -> " +
            source.substr(o, previousToken.offset + previousToken.length - o));
  }

  return e;
}

KIWI::AST::Expr *KIWI::Parser::parseUnaryExpr() {
  uint32_t o = currentToken.offset;
  uint16_t l = currentToken.length;
  std::string op = source.substr(currentToken.offset, currentToken.length);
  advance();
  AST::Expr *expr = parseExpr(50);
  if (expr->getKind() == AST::Kind::ERROR_EXPR)
    return expr;
  return new AST::UnaryExpr(o, l, op, expr);
}

KIWI::AST::Expr *KIWI::Parser::parseLiteral() {
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
    return new AST::ErrorExpr(
        o, l,
        "Expected Expression at -> " +
            source.substr(o, previousToken.offset + previousToken.length - o));
  }
}

KIWI::AST::Identifier *KIWI::Parser::parseIdent() {
  advance();
  return new AST::Identifier(
      previousToken.offset, previousToken.length,
      source.substr(previousToken.offset, previousToken.length));
}

KIWI::AST::IntLiteral *KIWI::Parser::parseIntLiteral() {
  advance();
  return new AST::IntLiteral(
      previousToken.offset, previousToken.length,
      std::stoll(source.substr(previousToken.offset, previousToken.length)));
}

KIWI::AST::FloatLiteral *KIWI::Parser::parseFloatLiteral() {
  advance();
  return new AST::FloatLiteral(
      previousToken.offset, previousToken.length,
      std::stod(source.substr(previousToken.offset, previousToken.length)));
}
