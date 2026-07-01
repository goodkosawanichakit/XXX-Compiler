#pragma once

#include "ast.hpp"
#include <vector>

namespace KIWI::AST {
class Dumper {
private:
  const std::vector<uint32_t> &lineOffset;
  void dumpErrorStmt(ErrorStmt *node, int d);
  void dumpErrorExpr(ErrorExpr *node, int d);
  void dumpErrorDeclr(ErrorDeclr *node, int d);
  void dumpIdent(Identifier *node, int d);
  void dumpFloatLiteral(FloatLiteral *node, int d);
  void dumpIntLiteral(IntLiteral *node, int d);
  void dumpBinaryExpr(BinaryExpr *node, int d);
  void dumpUnaryExpr(UnaryExpr *node, int d);
  void dumpExpr(Expr *node, int d);
  void dumpVarDeclr(VarDeclr *node, int d);
  void dumpFuncDeclr(FuncDeclr *node, int d);
  void dumpParamDeclr(ParamDeclr *node, int d);
  void dumpBlock(Block *node, int d);
  void dumpBlockItems(Node *node, int d);
  uint32_t getLine(uint32_t of);

public:
  void dump(Forest *module);
  Dumper(const std::vector<uint32_t> &l) : lineOffset(l) {}
  ~Dumper() {}
};
}; // namespace KIWI::AST
