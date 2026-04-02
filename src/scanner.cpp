#include "scanner.hpp"

static compiler::Scanner scanner;

bool compiler::Scanner::isAtEnd() { return current >= source.length(); }

void compiler::Scanner::initScanner(const std::string &source) {
  scanner.source = source;
  scanner.start = 0;
  scanner.current = 0;
  scanner.line = 1;
}

compiler::Token compiler::Scanner::scanToken() {
  scanner.start = scanner.current;

  if (isAtEnd())
    return Token{TokenType::TOKEN_EOF, "", line};

  // TODO: complete the scanToken() function

  return Token{TokenType::TOKEN_EOF, "", line};
}
