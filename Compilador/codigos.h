#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
#include <string.h>
#include <ctype.h>
#ifndef CODIGOS_H_
#define CODIGOS_H_
#define TAM 50
#define TRUE 1
#define FALSE 0
#define PeV 10
#define VL_INT 51
#define VL_FLOAT 52
#define VL_CHAR 53
#define IDENTIFICADOR 54
#define PLR_MAIN 100
#define PLR_IF 101
#define PLR_ELSE 102
#define PLR_WHILE 103
#define PLR_DO 104
#define PLR_FOR 105
#define PLR_INT 106
#define PLR_FLOAT 107
#define PLR_CHAR 108
#define MAIORQ 109
#define MAIORIGUAL 110
#define MENORQ 111
#define MENORIGUAL 112
#define IGUALIGUAL 113
#define DIFERENTE 114
#define MAIS 115
#define MENOS 116
#define MULT 117
#define BARRA 118
#define IGUAL 119
#define ABREPARENTESE 120
#define FECHAPARENTESE 121
#define ABRECHAVE 122
#define FECHACHAVE 123
#define VIRGULA 124
#define COMENTARIO 125
#define ERROR_CHAR 200
#define ERROR_SEMANTICO 201
#define ERROR 404


typedef struct token {
	int cod;
	int tipo;
	char lexema[TAM];
} Token;

typedef struct symbol {
	char lexema[TAM];
	int tipo;
	int escopo;
	struct symbol *prox;
} Symbol;

#endif /* CODIGOS_H_ */
