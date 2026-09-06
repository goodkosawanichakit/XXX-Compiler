#include "ast.hpp"
#include "astdump.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

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

  KIWI::Scanner scanner(source);
  KIWI::Parser parser(scanner, source);

  std::unique_ptr<KIWI::AST::Forest> module = parser.parse();

  KIWI::AST::Dumper dumper(scanner.getLineOffset());
  dumper.dump(module.get());
  std::cout << std::endl;

  return 0;
}
