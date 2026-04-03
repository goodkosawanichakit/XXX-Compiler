#include "scanner.hpp"

compiler::Scanner::Scanner(const std::string &source) : source(source) {}

bool compiler::Scanner::isAtEnd() { return current >= source.length(); }

bool compiler::Scanner::match(char c) {
  if (isAtEnd())
    return false;
  if (source[current] != c)
    return false;
  current++;
  return true;
}

char compiler::Scanner::advance() { return source[current++]; }

char compiler::Scanner::peek() { return source[current]; }

compiler::Token compiler::Scanner::scanToken() {

  // TODO: complete the scanToken() function
  // - implement Basic Keyword, Integer Literal

  if (isAtEnd())
    return Token{TokenType::TOKEN_EOF, "", line};

  char c = advance();

  switch (c) {
  case '+':
    return Token{TokenType::PLUS, "+", line};
  case '-':
    return Token{TokenType::MINUS, "-", line};
  case '*':
    return Token{TokenType::STAR, "*", line};
  case '/':
    return Token{TokenType::SLASH, "/", line};
  case '(':
    return Token{TokenType::LEFT_PAREN, "(", line};
  case ')':
    return Token{TokenType::RIGHT_PAREN, ")", line};
  case '{':
    return Token{TokenType::LEFT_BRACE, "{", line};
  case '}':
    return Token{TokenType::RIGHT_BRACE, "}", line};
  case ';':
    return Token{TokenType::SEMICOLON, ";", line};
  case ':':
    return Token{TokenType::COLON, ":", line};
  case '.':
    if (match('.'))
      return Token{TokenType::DOT_DOT, "..", line};
    return Token{TokenType::DOT, ".", line};
  case '=':
    if (match('='))
      return Token{TokenType::EQUAL_EQUAL, "==", line};
    return Token{TokenType::EQUAL, "=", line};
  case '!':
    if (match('='))
      return Token{TokenType::NOT_EQUAL, "!=", line};
    return Token{TokenType::NOT, "!", line};
  case '<':
    if (match('='))
      return Token{TokenType::LESS_EQUAL, "<=", line};
    return Token{TokenType::LESS, "<", line};
  case '>':
    if (match('='))
      return Token{TokenType::GREATER_EQUAL, ">=", line};
    return Token{TokenType::GREATER, ">", line};
  }

  return Token{TokenType::TOKEN_EOF, "", line};
}
