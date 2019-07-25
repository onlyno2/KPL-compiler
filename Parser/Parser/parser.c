/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {  // PROGRAM identifier ; block .
  //assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  //assert("Program parsed!");
}

void compileBlock(void) { // CONST ident = constant; ...
  //assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  else compileBlock2();
  //assert("Block parsed!");
}

void compileBlock2(void) { // TYPE iden = type; ...
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) { // VAR ident : type; ...
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) { // ctring con
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) { // BEGIN statement END
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void) { // ident = constant; compileConstDecl();
  // TODO
  if(lookAhead->tokenType == TK_IDENT) {
    compileConstDecl();
    compileConstDecls();
  }
  else {  }
}

void compileConstDecl(void) { // ident = constant or NULL
  // TODO
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

void compileTypeDecls(void) { // ident = type; compileTypeDecl();
  // TODO
  if(lookAhead->tokenType == TK_IDENT) {
    compileTypeDecl();
    compileTypeDecls();
  } else {  }
}

void compileTypeDecl(void) { // ident = type or NULL
  // TODO
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

void compileVarDecls(void) { // ident : type; compilteVarDecls();
  // TODO
  if(lookAhead->tokenType == TK_IDENT) {
    compileVarDecl();
    compileVarDecls();
  } else {  }
}

void compileVarDecl(void) { // ident : type or NULL
  // TODO
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}

void compileSubDecls(void) { // FUNCTION or 
  //assert("Parsing subtoutines ....");
  // TODO
  if(lookAhead->tokenType == KW_PROCEDURE) {
    eat(KW_PROCEDURE);
    compileProcDecl();
    compileSubDecls();  
  } else if(lookAhead->tokenType == KW_FUNCTION) {
    eat(KW_FUNCTION);
    compileFuncDecl();
    compileSubDecls();
  }
  //assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {
  //assert("Parsing a function ....");
  // TODO
  eat(TK_IDENT);
  if(lookAhead->tokenType == SB_LPAR) {
    compileParams();
  }
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  //assert("Function parsed ....");
}

void compileProcDecl(void) {
  //assert("Parsing a procedure ....");
  // TODO
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  //assert("Procedure parsed ....");
}

void compileUnsignedConstant(void) {
  // TODO
  if(lookAhead->tokenType == TK_NUMBER) {
    eat(TK_NUMBER);
  } else if(lookAhead->tokenType == TK_CHAR) {
    eat(TK_CHAR);
  } else if(lookAhead->tokenType == TK_IDENT) {
    eat(TK_IDENT);
  }
  else {
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileConstant(void) {
  // TODO
  if(lookAhead->tokenType == SB_MINUS) {
    eat(SB_MINUS);
    compileConstant2();
  } else if(lookAhead->tokenType == SB_PLUS) {
    eat(SB_PLUS);
    compileConstant2();
  } else if(lookAhead->tokenType == TK_CHAR) {
    eat(TK_CHAR);
  }
  else {
    compileConstant2();
  }
}

void compileConstant2(void) {
  // TODO
  if(lookAhead->tokenType == TK_IDENT) {
    eat(TK_IDENT);
  } else if(lookAhead->tokenType == TK_NUMBER) {
    eat(TK_NUMBER);
  } else {
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileType(void) {
  // TODO
  if(lookAhead->tokenType == KW_INTEGER) {
    eat(KW_INTEGER);
  } else if(lookAhead->tokenType == KW_CHAR) {
    eat(KW_CHAR);
  } else if(lookAhead->tokenType == TK_IDENT) {
    eat(TK_IDENT);
  } else if(lookAhead->tokenType == KW_ARRAY) {
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);
    eat(SB_RSEL);
    eat(KW_OF);
    compileType();
  } else {
    error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileBasicType(void) {
  // TODO
  if(lookAhead->tokenType == KW_INTEGER) {
    eat(KW_INTEGER);
  } else if(lookAhead->tokenType == KW_CHAR) {
    eat(KW_CHAR);
  } else {
    error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileParams(void) {
  // TODO
  if(lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    eat(SB_RPAR);
  } else if(lookAhead->tokenType == SB_SEMICOLON || lookAhead->tokenType == SB_COLON) {

  } else {
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileParams2(void) {
  // TODO
  switch (lookAhead->tokenType) {
    case SB_SEMICOLON:
      eat(SB_SEMICOLON);
      compileParam();
      compileParams2();
      break;
    case SB_RPAR:
      break;
    default:
      error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileParam(void) {
  // TODO 
  if(lookAhead->tokenType == TK_IDENT) {
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  } else if(lookAhead->tokenType == KW_VAR){
    eat(KW_VAR);
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  }
}

void compileStatements(void) {
  // TODO
  compileStatement();
  compileStatements2();
}

void compileStatements2(void) {
  // TODO
  if(lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
  } else if(lookAhead->tokenType == KW_END){ 
    
  } else {
    eat(SB_SEMICOLON);
  }
}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileAssignSt(void) {
  //assert("Parsing an assign statement ....");
  // TODO
  eat(TK_IDENT);
  if(lookAhead->tokenType == SB_LSEL) {
    compileIndexes();
  }
  eat(SB_ASSIGN);
  compileExpression();
  //assert("Assign statement parsed ....");
}

void compileCallSt(void) {
  //assert("Parsing a CALL statement ....");
  // TODO
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
  //assert("CALL statement parsed ....");
}

void compileGroupSt(void) {
  //assert("Parsing a group statement ....");
  // TODO
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  //assert("Group statement parsed ....");
}

void compileIfSt(void) {
  //assert("Parsing an IF statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
  //assert("IF statement parsed ....");
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  //assert("Parsing a WHILE statement ....");
  // TODO
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  //assert("WHILE statement pased ....");
}

void compileForSt(void) {
  //assert("Parsing a FOR statement ....");
  // TODO
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  //assert("FOR statement parsed ....");
}

void compileArguments(void) {
  // TODO
  switch (lookAhead->tokenType) {
    case SB_LPAR:
      eat(SB_LPAR);
      compileExpression();
      compileArguments2();
      eat(SB_RPAR);
      break;
  // Follow - same as call statement as statement:
    case SB_SEMICOLON:    // foo();
    case KW_END:          // foo() end.
    case KW_ELSE:         // if foo() else 
  // Follow - term2
    case SB_TIMES:        // foo() *
    case SB_SLASH:        // foo() /
  // Follow - expression3
  // Follow (For statement)
    case KW_TO:
    case KW_DO:
  // Follow (arguments2)
    case SB_COMMA:   
  // Follow (condition2)
    case SB_EQ:           // foo() = 0
    case SB_NEQ:          // foo() != 0
    case SB_LE:           // foo() <= 0
    case SB_LT:           // foo() < 0
    case SB_GE:           // foo() >= 0
    case SB_GT:           // foo() > 0
  // Follow (factor)
    case SB_RPAR:         // A(.foo().)
  // Follow (indexes)
    case SB_RSEL:         // bar(foo())
  // Follow (if statement)
    case KW_THEN:
      break;
  // Error
    default:
      error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileArguments2(void) {
  // TODO
  if(lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    compileExpression();
    compileArguments2();
  } else if(lookAhead->tokenType == SB_RPAR) {

  } else {
    error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void) {
  // TODO
  compileExpression();
  compileCondition2();
}

void compileCondition2(void) {
  // TODO
  switch(lookAhead->tokenType)
  {
    case SB_EQ:
      eat(SB_EQ);
      compileExpression();
      break;
    case SB_NEQ:
      eat(SB_NEQ);
      compileExpression();
      break;
    case SB_LE:
      eat(SB_LE);
      compileExpression();
      break;
    case SB_GE:
      eat(SB_GE);
      compileExpression();
      break;
    case SB_LT:
      eat(SB_LT);
      compileExpression();
      break;
    case SB_GT:
      eat(SB_GT);
      compileExpression();
      break;
    default: error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileExpression(void) {
  //assert("Parsing an expression");
  // TODO
  if(lookAhead->tokenType == SB_PLUS) {
    eat(SB_PLUS);
    compileExpression2();
  } else if(lookAhead->tokenType == SB_MINUS) {
    eat(SB_MINUS);
    compileExpression2();
  } else {
    compileExpression2();
  }
  //assert("Expression parsed");
}

void compileExpression2(void) {
  // TODO
  compileTerm();
  compileExpression3();
}


void compileExpression3(void) {
  // TODO
  if(lookAhead->tokenType == SB_PLUS) {
    eat(SB_PLUS);
    compileTerm();
    compileExpression3();
  } else if(lookAhead->tokenType == SB_MINUS) {
    eat(SB_MINUS);
    compileTerm();
    compileExpression3();
  } else {  }
}

void compileTerm(void) {
  // TODO
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  // TODO
  switch (lookAhead->tokenType) 
  {
    case SB_TIMES:
      eat(SB_TIMES);
      compileFactor();
      compileTerm2();
      break;
    case SB_SLASH:
      eat(SB_SLASH);
      compileFactor();
      compileTerm2();
      break;
    // Follow - same as expression3
    case SB_PLUS:
    case SB_MINUS:
    // Follow (statement)
    case SB_SEMICOLON:
    case KW_END:
    case KW_ELSE:
    // Follow (For statement)
    case KW_TO:
    case KW_DO:
    // Follow (arguments2)
    case SB_COMMA:
    // Follow (condition2)
    case SB_EQ:
    case SB_NEQ:
    case SB_LE:
    case SB_LT:
    case SB_GE:
    case SB_GT:
    // Follow (factor)
    case SB_RPAR:
    // Follow (indexes)
    case SB_RSEL:
    // Follow (if statement)
    case KW_THEN:
      break;
    default:
      error(ERR_INVALIDTERM, lookAhead->lineNo, lookAhead->colNo);
      break;
  }
}

void compileFactor(void) {
  // TODO
  if(lookAhead->tokenType == TK_NUMBER || lookAhead->tokenType == TK_CHAR) {
    compileUnsignedConstant();
  } else if(lookAhead->tokenType == TK_IDENT) {
    eat(TK_IDENT);
    if(lookAhead->tokenType == SB_LSEL) {
      compileIndexes();
    } else if(lookAhead->tokenType == SB_LPAR) {
      compileArguments();
    } else {  }
  } else if(lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileExpression();
    eat(SB_RPAR);
  } else { 
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);  
  }
}

void compileIndexes(void) {
  // TODO
  if (lookAhead->tokenType == SB_LSEL) {
      eat(SB_LSEL);
      compileExpression();
      eat(SB_RSEL);
      compileIndexes();
  }
}

int compile(char *FileName) {
  if (openInputStream(FileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
