/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 Da thuc hien: 1 + 2 + 3 + 4 + 5('\n', '\t') + 6 + 7
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

/* truoc la dau blank */
void skipBlank() {
  // TODO
  if(currentChar != EOF) {
    readChar();
  }
}

/* truoc la (* */
void skipComment() {
  // TODO
  //int flag = 0;
  do {
    readChar();
  } while(currentChar != EOF && charCodes[currentChar] != CHAR_TIMES);

  if(currentChar == EOF) {
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
  }
  readChar();
  if(charCodes[currentChar] == CHAR_RPAR) {
    readChar();
  } else {
    skipComment();
  }
}

void skipLineComment() {
  do {
    readChar();
  } while(currentChar != EOF && currentChar != '\n');
}

/* ki tu dau tien */
Token* readIdentKeyword(void) {
  // TODO
  Token *token = makeToken(TK_NONE, lineNo, colNo);
  int counter = 0;

  // doc cac chu cho den cuoi file hoac gap ki tu khong thoa man
  do {    
    if(counter < 10) {
      token->string[counter++] = (char)currentChar;
    }
    readChar();
  } while((charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] ==CHAR_DIGIT) && currentChar != EOF);

  // check do dai cua token

  token->string[counter] = '\0';
  token->tokenType = checkKeyword(token->string); // kiem tra xem token co phu hop khong
  if(token->tokenType == TK_NONE) { // kiem tra loi (TK_NONE la khong phai key)
    token->tokenType = TK_IDENT;
  }

  return token;
}

Token* readConstString(void) {
  Token *token = makeToken(TK_STRING, lineNo, colNo);
  int counter = 0;
  do {
    readChar();
    if(currentChar == '\n' || currentChar == EOF) error(ERR_INVALIDSYMBOL, lineNo, colNo);
    token->string[counter++] = currentChar;
  } while(charCodes[currentChar] != CHAR_QUOTES);
  readChar();
  return token;
}

/* so dau tien */
Token* readNumber(void) {
  // TODO
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int counter = 0;

  while(charCodes[currentChar] == CHAR_DIGIT) {
    token->string[counter++] = currentChar;
    readChar();
  }
  
  token->string[counter] = '\0';
  token->value = atoi(token->string);

  return token;
}

/* truoc la dau ' */
Token* readConstChar(void) {
  // TODO
  Token *token = makeToken(TK_CHAR, lineNo, colNo);
  
  
  // get char trong '
  readChar();

  if(currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
    return token;
  }

  else if(charCodes[currentChar] == CHAR_BACKSLASH) {
    token->string[0] = currentChar;
    readChar();
    if(currentChar != 'n' && currentChar != 't' && charCodes[currentChar] != CHAR_SINGLEQUOTE) {
      error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
    }
    token->string[1] = currentChar;
    token->string[2] = '\0';
    readChar(); 
    readChar();
    return token;
  }

  else if(charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
  }
  else {
    token->string[0] = currentChar; 
    token->string[1] = '\0';

    // doc ki tu sau ki tu get duoc
    readChar();
    // het file
    if(currentChar == EOF) {
      token->tokenType = TK_NONE;
      error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
      return token;
    }
  }

  if(charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    readChar();
    return token;
  } else {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);  
    return token;
  }
}

Token* getToken(void) {
  Token *token;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);
  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();

  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_QUOTES:
    return readConstString();
  case CHAR_DIGIT: return readNumber();
  // cac dau + - * /
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
    // ....
    // TODO
    // ....
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SLASH:
    readChar();
    if(charCodes[currentChar] == CHAR_SPACE) {
      token = makeToken(SB_SLASH, lineNo, colNo);
      return token;
    } else if(charCodes[currentChar] == CHAR_SLASH) {
      skipLineComment();
    }
    else {
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
    }
  case CHAR_LPAR:
    token = makeToken(TK_NONE, lineNo, colNo);
    readChar();
    if(charCodes[currentChar] == CHAR_TIMES) {
      skipComment();
      return getToken();
    }
    if(charCodes[currentChar] == CHAR_SPACE) {
      readChar();
      return getToken();
    }
    if(charCodes[currentChar] == CHAR_PERIOD) {
      token->tokenType = SB_LSEL;
      readChar();
      return token;
    } 
    else {
      token->tokenType = SB_LPAR;
      return token;
    }
    break;
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA: // ,
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SEMICOLON: // ;
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;   
  case CHAR_PERIOD: // .
    token = makeToken(SB_PERIOD, lineNo, colNo);
    readChar();
    if(charCodes[currentChar] == CHAR_RPAR && currentChar != EOF) {
      token->tokenType = SB_RSEL;
      readChar();
      return token;
    }
    return token;
  case CHAR_COLON: // :
    token = makeToken(SB_COLON, lineNo, colNo);
    // doc ki tu tiep theo kiem tra. Neu la dau = thi la phep gan khong phai chi la dinh nghia ?
    readChar();
    if(charCodes[currentChar] == CHAR_EQ) {
      token->tokenType = SB_ASSIGN;
      readChar();
    }
    return token;
  // const char case
  case CHAR_SINGLEQUOTE: return readConstChar();
  // logic case
  case CHAR_EQ: 
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_EXCLAIMATION: // !
    token = makeToken(TK_NONE, lineNo, colNo);
    readChar();
    if(charCodes[currentChar] != CHAR_EQ) {
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
    } else {
      token->tokenType = SB_NEQ;
    }
    return token;
  case CHAR_GT: // >
    token = makeToken(SB_GT, lineNo, colNo);
    // check >=
    readChar();
    if(charCodes[currentChar] == CHAR_EQ) {
      token->tokenType = SB_GE;
      readChar();
    }
    return token;
  case CHAR_LT: // <
    token = makeToken(SB_LT, lineNo, colNo);
    // check <=
    readChar();
    if(charCodes[currentChar] == CHAR_EQ) {
      token->tokenType = SB_LE;
      readChar();
    } 
    else if(charCodes[currentChar] == CHAR_GT) {
      token->tokenType = SB_NEQ;
    }
    return token;
  case CHAR_LSR:
    token = makeToken(SB_LSR, lineNo, colNo);
    readChar();
    return token;
  case CHAR_RSR:
    token = makeToken(SB_RSR, lineNo, colNo);
    readChar();
    return token;
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  case SB_LSR: printf("SB_LSR\n"); break;
  case SB_RSR: printf("SB_RSR\n"); break;
  case TK_STRING: printf("TK_STRING\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }  

  return 0;
} 



