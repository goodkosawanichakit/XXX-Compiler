#include <fstream>
#include <iostream>
#include <string>

int main() {
  std::ifstream file("test");
  std::string line;

  while (std::getline(file, line))
    std::cout << line << '\n';

  return 0;
}
