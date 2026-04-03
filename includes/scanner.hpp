#pragma once

#include <string>

namespace compiler {

enum class TokenType {
  // Single-character tokens
  PLUS,
  MINUS,
  STAR,
  SLASH,
  SEMICOLON,
  COLON,
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  DOT,

  // Two-character tokens
  DOT_DOT,
  NOT,
  NOT_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals
  IDENTIFIER,
  STRING,
  NUMBER_INT,
  NUMBER_FLOAT,

  // Keywords
  KW_INT,
  KW_FLOAT,
  KW_BOOL,
  KW_IF,
  KW_ELSE,
  KW_LOOP,
  KW_IN,
  KW_STEP,
  KW_FN,
  KW_RETURN,
  KW_TRUE,
  KW_FALSE,

  TOKEN_EOF
};

struct Token {
  TokenType type;
  std::string lexeme;
  int line;
};

class Scanner {
private:
  std::string source;
  size_t start = 0;
  size_t current = 0;
  int line = 1;

  bool isAtEnd();
  bool match(char c);
  char advance();
  char peek();

public:
  Scanner(const std::string &source);
  Token scanToken();
};

} // namespace compiler
