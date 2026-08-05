#include "astdump.hpp"
#include "ast.hpp"
#include <iostream>

// TBH these two match function is llm generated cause I'm lazy.
std::string matchEnumKind(KIWI::AST::Kind k) {
  switch (k) {
  case KIWI::AST::Kind::EXPR:
    return "EXPR";
  case KIWI::AST::Kind::BLOCK:
    return "BLOCK";
  case KIWI::AST::Kind::BINARY_EXPR:
    return "BINARY_EXPR";
  case KIWI::AST::Kind::UNARY_EXPR:
    return "UNARY_EXPR";
  case KIWI::AST::Kind::INT_LITERAL:
    return "INT_LITERAL";
  case KIWI::AST::Kind::FLOAT_LITERAL:
    return "FLOAT_LITERAL";
  case KIWI::AST::Kind::RETURN_STMT:
    return "RETURN_STMT";
  case KIWI::AST::Kind::ASSIGN_STMT:
    return "ASSIGN_STMT";
  case KIWI::AST::Kind::VAR_DECLR:
    return "VAR_DECLR";
  case KIWI::AST::Kind::PARAM_DECLR:
    return "PARAM_DECLR";
  case KIWI::AST::Kind::FUNC_DECLR:
    return "FUNC_DECLR";
  case KIWI::AST::Kind::IDENTIFIER:
    return "IDENTIFIER";
  case KIWI::AST::Kind::ERROR_STMT:
  case KIWI::AST::Kind::ERROR_EXPR:
  case KIWI::AST::Kind::ERROR_DECLR:
    return "ERROR";
  }
  return "UNKNOWN_KIND";
}

std::string matchEnumType(KIWI::AST::Type t) {
  switch (t) {
  case KIWI::AST::Type::INT8:
    return "INT8";
  case KIWI::AST::Type::INT16:
    return "INT16";
  case KIWI::AST::Type::INT32:
    return "INT32";
  case KIWI::AST::Type::INT64:
    return "INT64";
  case KIWI::AST::Type::FLOAT8:
    return "FLOAT8";
  case KIWI::AST::Type::FLOAT16:
    return "FLOAT16";
  case KIWI::AST::Type::FLOAT32:
    return "FLOAT32";
  case KIWI::AST::Type::FLOAT64:
    return "FLOAT64";
  }
  return "UNKNOWN_TYPE";
}

uint32_t KIWI::AST::Dumper::getLine(uint32_t of) {
  if (lineOffset.empty())
    return 1;
  int l = 0;
  int r = lineOffset.size() - 1;
  uint32_t target = lineOffset.size();
  while (l <= r) {
    int m = l + (r - l) / 2;

    if (lineOffset[m] > of) {
      target = m;
      r = m - 1;
    } else {
      l = m + 1;
    }
  }

  return target;
}

void KIWI::AST::Dumper::dump(KIWI::AST::Forest *module) {
  if (!module)
    return;
  for (KIWI::AST::Node *node : module->vec) {
    if (!node) {
      std::cout << "TS ERROR na" << std::endl;
      continue;
    }

    switch (node->getKind()) {
    case Kind::VAR_DECLR:
      dumpVarDeclr((VarDeclr *)node, 0);
      break;
    case Kind::FUNC_DECLR:
      dumpFuncDeclr((FuncDeclr *)node, 0);
      break;
    default:
      // TODO: IDK what I need to handle in this deafult section.
      // so todo is I need to think what I'm gonna do
      dumpErrorStmt((ErrorStmt *)node, 0);
    }
  }
}

// Name first, ReturnType after -> reads like the source: fn foo(...) -> INT32
// (also folded the identifier straight into this line instead of recursing
// into dumpIdent as a separate child, same as VarDeclr already did)
void KIWI::AST::Dumper::dumpFuncDeclr(FuncDeclr *node, int d) {
  if (!node)
    return;

  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Name: " << node->getName()->getName()
            << " ReturnType: " << matchEnumType(node->getReturnType())
            << std::endl;

  for (Declr *param : node->getParams())
    dumpParamDeclr((ParamDeclr *)param, d + 1);

  dumpBlock(node->getBlock(), d + 1);
}

// Prob work fine I think
void KIWI::AST::Dumper::dumpVarDeclr(VarDeclr *node, int d) {
  if (!node)
    return;
  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Name: " << node->getIdentifier()->getName()
            << " Type: " << matchEnumType(node->getType()) << std::endl;
  dumpExpr(node->getExpr(), d + 1);
}

void KIWI::AST::Dumper::dumpParamDeclr(ParamDeclr *node, int d) {
  if (!node)
    return;

  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Name: " << node->getIdentifier()->getName()
            << " Type: " << matchEnumType(node->getType()) << std::endl;
}

void KIWI::AST::Dumper::dumpBlock(Block *node, int d) {
  if (!node)
    return;
  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << std::endl;

  for (auto i : node->stmts)
    dumpBlockItems(i, d + 1);
}

void KIWI::AST::Dumper::dumpBlockItems(Node *node, int d) {
  if (!node)
    return;

  switch (node->getKind()) {
  case Kind::VAR_DECLR:
    dumpVarDeclr((VarDeclr *)node, d);
    break;
  case Kind::RETURN_STMT:
    dumpRet((ReturnStmt *)node, d);
    break;
  case Kind::ASSIGN_STMT:
    dumpAssign((AssignStmt *)node, d);
    break;
  default:
    dumpErrorStmt((ErrorStmt *)node, d);
  }
}

void KIWI::AST::Dumper::dumpRet(ReturnStmt *node, int d) {
  if (!node)
    return;
  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Value: " << std::endl;
  dumpExpr(node->getExpr(), d + 1);
}

void KIWI::AST::Dumper::dumpAssign(AssignStmt *node, int d) {
  if (!node)
    return;

  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind()) << ' ';
  dumpIdent(node->getIdent(), d);
  dumpExpr(node->getExpr(), d + 1);
}

void KIWI::AST::Dumper::dumpExpr(Expr *node, int d) {
  if (!node)
    return;
  switch (node->getKind()) {
  case Kind::INT_LITERAL:
    return dumpIntLiteral((IntLiteral *)node, d);
  case Kind::FLOAT_LITERAL:
    return dumpFloatLiteral((FloatLiteral *)node, d);
  case Kind::BINARY_EXPR:
    return dumpBinaryExpr((BinaryExpr *)node, d);
  case Kind::UNARY_EXPR:
    return dumpUnaryExpr((UnaryExpr *)node, d);
  case Kind::IDENTIFIER:
    return dumpIdent((Identifier *)node, d);
  case Kind::ERROR_STMT:
    return dumpErrorStmt((ErrorStmt *)node, d);
  case Kind::ERROR_EXPR:
    return dumpErrorExpr((ErrorExpr *)node, d);
  case Kind::ERROR_DECLR:
    return dumpErrorDeclr((ErrorDeclr *)node, d);
  default:
    std::cout << "How did you get here." << std::endl;
  }
}

void KIWI::AST::Dumper::dumpErrorStmt(ErrorStmt *node, int d) {
  if (!node)
    return;
  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Error: " << node->getMessage() << std::endl;
}

void KIWI::AST::Dumper::dumpErrorExpr(ErrorExpr *node, int d) {
  if (!node)
    return;
  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Error: " << node->getMessage() << std::endl;
}

void KIWI::AST::Dumper::dumpErrorDeclr(ErrorDeclr *node, int d) {
  if (!node)
    return;
  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Error: " << node->getMessage() << std::endl;
}

void KIWI::AST::Dumper::dumpIdent(Identifier *node, int d) {
  if (!node)
    return;
  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Name: " << node->getName() << std::endl;
}

void KIWI::AST::Dumper::dumpBinaryExpr(BinaryExpr *node, int d) {
  if (!node)
    return;
  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Operator: " << node->getOP() << std::endl;
  dumpExpr(node->getLExpr(), d + 1);
  dumpExpr(node->getRExpr(), d + 1);
}

void KIWI::AST::Dumper::dumpUnaryExpr(UnaryExpr *node, int d) {
  if (!node)
    return;

  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Operator: " << node->getOP() << std::endl;

  dumpExpr(node->getExpr(), d + 1);
}

void KIWI::AST::Dumper::dumpIntLiteral(IntLiteral *node, int d) {
  if (!node)
    return;
  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Value: " << node->getValue() << std::endl;
}

void KIWI::AST::Dumper::dumpFloatLiteral(FloatLiteral *node, int d) {
  if (!node)
    return;
  std::cout << std::string(d * 2, ' ') << matchEnumKind(node->getKind())
            << " Value: " << node->getValue() << std::endl;
}
