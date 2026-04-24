#include "scanner.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2)
    return 1;

  std::ifstream file(argv[1]);

  if (!file)
    return 2;

  std::string source;
  std::stringstream buffer;
  buffer << file.rdbuf();
  source = buffer.str();

  XX::Scanner scanner(source);
  for (;;) {
    XX::Token t = scanner.scanToken();
    std::cout << "Type: " << (int)t.type << " | Offset: " << t.offset
              << " | Length: " << t.length;
    std::cout << " | Lexeme: [" << source.substr(t.offset, t.length) << "]\n";

    if (t.type == XX::TokenType::TOKEN_EOF)
      break;
  }
  return 0;
}
