﻿#include "codigos.h"

int linha;
int coluna;
int error;
int chamada = 0;
int opR_aux, expR_check = 0;
int cont, label_cont = 0, tmp_cont = 0, atr_cont = 0;
int dec_var;
int op_atual;
int flag_print = 0;
int fator_check = 0, fator_cont = 0, cont_it = 0, termo_check = 0,
		termo_int = 0;
int escopo = 0;
char tipos_vtr[3][6] = { "Int", "Float", "Char" }; //PARA PRINTAR O TIPO DO ID OU VAR
char tipos_op[4][6] = { "+", "-", "*", "/" }; //PARA PRINTAR O TIPO DO ID OU VAR
char tipos_relacionail[6][6] = { "<", ">", "<=", ">=", "==", "!=" }; //PARA PRINTAR O TIPO DO ID OU VAR
Token tk, g_tmp_var, aux_g;
Symbol *tabela;

FILE * AbrirArquivo(char nome[]) {
	FILE * arq;
	arq = fopen(nome, "r+b");
	return arq;
}
enum tipos_de_variavel {
	Int, Float, Char
} tiposVar; //PARA USAR EM tipos_vtr
enum tipos_de_operador {
	mais, menos, mult, barra
} tiposOp;
enum tipos_de_op_relacional {
	menor, maior, menorigual, maiorigual, igualigual, diferente
} tiposOpEr;
//------------------ DECLARACOES DAS FUNCOES --------------------------------
int verifica_reservadas(char lex[]);
int verifica_char(char ch);
int verifica_EOF(char ch);
int verifica_quebra_tab(char ch);
int verifica_quebra(char ch);
int verifica_tab(char ch);
int verifica_fator(int fator);
int verifica_tipo(int tipo);
int verifica_iteracao(int tipo);
int verifica_atribuicao(int tipo);
int verifica_comando(int tipo);
int verifica_comando_basico(int tipo);
int verifica_op_relacional(int tipo);
int verifica_bloco(int tipo);
int verifica_expr_arit(int check);
Token verifica_tipo_resultante(Token a1, Token a2, Token * tk_tmp);
int verifica_tipo_atr(Token a1, Token a2);
Symbol * fim_lista();
void preencher_tabela(Token t);
void destruir_escopo();
void destruir_tabela();
void covert_to_float(Token *tken);
char * gerador_de_label();
char * gerador_de_var_tmp();
int busca_na_tabela(Token *aux);
int declara_variavel(FILE * comp);
int iteracao(FILE * comp);
int atribuicao(FILE * comp);
Token expr_relacional(FILE * comp);
Token expr_arit(FILE * comp, Token *tk1);
Token expr_arit2(FILE * comp, Token * tk1);
Token termo(FILE * comp, Token * tk1);
Token fator(FILE * comp, Token *tk1);
int comando_basico(FILE * comp);
int comando(FILE * comp);
int bloco(FILE * comp);
Token Scanner(FILE * comp);
Token Parser(FILE * comp);
//------------------------------------------------------------------------
int main(int argc, char * argv[]) { // MAIN
	FILE * comp;
	Token token;
	token.cod = 0;
	token.lexema[0] = '\0';
	comp = AbrirArquivo(argv[1]); //argv[1]
	if (comp == NULL)
		printf("Erro ao tentar criar/abrir arquivo \n");
	else {
		fseek(comp, 0, 0);
		linha = 1;
		coluna = 1;
		error = 0;
		do {
			if (error == 1) {
				fclose(comp);
				return 0;
				break;
			} else {
				token = Parser(comp);

			}
		} while (token.cod != EOF);
		printf("\nFIM DE ARQUIVO.");
	}
	printf("\n");
	fclose(comp);
	return 0;
}
//---------------------- FUNCOES DE VERIFICACAO ----------------------
int verifica_reservadas(char lex[]) {
	if (strcmp(lex, "main") == 0) {
		return PLR_MAIN;
	} else if (strcmp(lex, "if") == 0) {
		return PLR_IF;
	} else if (strcmp(lex, "else") == 0) {
		return PLR_ELSE;
	} else if (strcmp(lex, "while") == 0) {
		return PLR_WHILE;
	} else if (strcmp(lex, "do") == 0) {
		return PLR_DO;
	} else if (strcmp(lex, "for") == 0) {
		return PLR_FOR;
	} else if (strcmp(lex, "int") == 0) {
		return PLR_INT;
	} else if (strcmp(lex, "float") == 0) {
		return PLR_FLOAT;
	} else if (strcmp(lex, "char") == 0) {
		return PLR_CHAR;
	} else
		return FALSE;
}

int verifica_char(char ch) {
	if (ch == '_') {
		return TRUE;
	} else if (isalpha(ch)) {
		return TRUE;
	} else if (isdigit(ch)) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_EOF(char ch) {
	if (ch == EOF) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_quebra_tab(char ch) {
	if (ch == '\t') {
		return TRUE;
	} else if (ch == 13) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_quebra(char ch) {

	if (ch == '\n') {
		coluna = 0;
		linha = linha + 1;
		return TRUE;
	} else {
		return FALSE;
	}
}

int verifica_tab(char ch) {
	if (ch == '\t') {
		coluna = coluna + 4;
		return TRUE;
	} else {
		return FALSE;
	}
}

int verifica_fator(int fator) { // VERIFICA O TIPO
	if (fator == ABREPARENTESE || fator == IDENTIFICADOR || fator == VL_INT
			|| fator == VL_FLOAT || fator == VL_CHAR) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_tipo(int tipo) { // VERIFICA O TIPO
	if (tipo == PLR_INT || tipo == PLR_FLOAT || tipo == PLR_CHAR) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_iteracao(int tipo) { // VERIFICA O TIPO
	if (tipo == PLR_WHILE || tipo == PLR_DO) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_atribuicao(int tipo) { // VERIFICA O TIPO
	if (tipo == IDENTIFICADOR) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_comando(int tipo) { // VERIFICA O TIPO
	if (tipo == PLR_IF || tipo == PLR_WHILE || tipo == PLR_DO
			|| tipo == IDENTIFICADOR || tipo == ABRECHAVE) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_comando_basico(int tipo) { // VERIFICA O TIPO
	if (tipo == IDENTIFICADOR || tipo == ABRECHAVE) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_op_relacional(int tipo) { // VERIFICA O TIPO
	if (tipo == IGUALIGUAL || tipo == DIFERENTE || tipo == MENORQ
			|| tipo == MAIORQ || tipo == MENORIGUAL || tipo == MAIORIGUAL) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_bloco(int tipo) { // VERIFICA O TIPO
	if (tipo == ABRECHAVE) {
		return TRUE;
	} else
		return FALSE;
}

int verifica_expr_arit(int check) {  // VERIFICA EXPRESSAO ARITIMETICA

	if (check == MAIS) {
		return TRUE;
	} else if (check == MENOS) {
		return TRUE;
	} else if (check == MULT) {
		return TRUE;
	} else if (check == BARRA) {
		return TRUE;
	} else if (check == IGUAL) {
		return TRUE;
	} else {
		return FALSE;
	}
}

Token verifica_tipo_resultante(Token a1, Token a2, Token * tk_tmp) {
	Token tk_aux;
	char * var_tmp;
	if (a1.cod == IDENTIFICADOR) {
		if (a2.cod == IDENTIFICADOR) {
			if (a1.tipo != a2.tipo) {
				if (a1.tipo == VL_CHAR || a2.tipo == VL_CHAR) {
					printf(
							"\nERRO na linha %i, coluna %i: Erro de semantica. 1\n",
							linha, coluna - 1);
					error = 1;
					tk_aux.cod = ERROR;
				} else {
					if (a1.cod == VL_FLOAT) {
						a2.tipo = VL_FLOAT;
						if (tk_tmp->tipo != 3) {
							var_tmp = gerador_de_var_tmp();
							printf("%s = int to float %s\n", var_tmp,
									a2.lexema);
							strcpy(tk_tmp->lexema, var_tmp);
							tk_tmp->cod = TMP;
							tk_tmp->tipo = 2;
							free(var_tmp);
						}
						tk_aux = a2;
					} else {

						a1.tipo = VL_FLOAT;
						if (tk_tmp->tipo != 3) {
							var_tmp = gerador_de_var_tmp();
							printf("%s = int to float %s\n", var_tmp,
									a1.lexema);
							strcpy(tk_tmp->lexema, var_tmp);
							tk_tmp->cod = TMP;
							tk_tmp->tipo = 1;
							free(var_tmp);
						}
						tk_aux = a1;
					}
				}
			} else {
				tk_aux = a1;
			}
		} else if (a2.cod == VL_CHAR) {
			if (a1.tipo != VL_CHAR) {
				printf("\nERRO na linha %i, coluna %i: Erro de semantica.2\n",
						linha, coluna - 1);
				error = 1;
				tk_aux.cod = ERROR;
			} else {
				tk_aux = a2;
			}
		} else if (a2.cod == VL_FLOAT) {
			if (a1.tipo == VL_CHAR) {
				printf("\nERRO na linha %i, coluna %i: Erro de semantica.3\n",
						linha, coluna - 1);
				error = 1;
				tk_aux.cod = ERROR;
			} else {
				if (a1.tipo == VL_INT) {
					a1.tipo = VL_FLOAT;
					if (tk_tmp->tipo != 3) {
						var_tmp = gerador_de_var_tmp();
						printf("%s = int to float %s\n", var_tmp, a1.lexema);
						strcpy(tk_tmp->lexema, var_tmp);
						tk_tmp->cod = TMP;
						tk_tmp->tipo = 1;
						free(var_tmp);
					}
					tk_aux = a1;
				} else {
					tk_aux = a2;
				}
			}

		} else {
			if (a1.tipo == VL_CHAR) {
				printf("\nERRO na linha %i, coluna %i: Erro de semantica.4\n",
						linha, coluna - 1);
				error = 1;
				tk_aux.cod = ERROR;
			} else {
				if (a1.tipo == VL_FLOAT) {
					covert_to_float(&a2);
					if (tk_tmp->tipo != 3) {
						var_tmp = gerador_de_var_tmp();
						printf("%s = int to float %s\n", var_tmp, a2.lexema);
						strcpy(tk_tmp->lexema, var_tmp);
						tk_tmp->cod = TMP;
						tk_tmp->tipo = 2;
						free(var_tmp);
					}
					tk_aux = a2;
				} else
					tk_aux = a2;
			}
		}
	} else if (a1.cod == VL_CHAR) {
		if (a2.cod == IDENTIFICADOR) {
			if (a2.tipo != VL_CHAR) {
				printf("\nERRO na linha %i, coluna %i: Erro de semantica.5\n",
						linha, coluna - 1);
				error = 1;
				tk_aux.cod = ERROR;
			} else {
				tk_aux = a1;
			}
		} else if (a2.cod != VL_CHAR) {
			printf("\nERRO na linha %i, coluna %i: Erro de semantica.6\n",
					linha, coluna - 1);
			error = 1;
			tk_aux.cod = ERROR;
		} else {
			tk_aux = a1;
		}
	} else if (a1.cod == VL_FLOAT) {
		if (a2.cod == IDENTIFICADOR) {
			if (a2.tipo == VL_CHAR) {
				printf("\nERRO na linha %i, coluna %i: Erro de semantica.\n",
						linha, coluna - 1);
				error = 1;
				tk_aux.cod = ERROR;
			} else if (a2.tipo == VL_INT) {
				a2.tipo = VL_FLOAT;
				if (tk_tmp->tipo != 3) {
					var_tmp = gerador_de_var_tmp();
					printf("%s = int to float %s\n", var_tmp, a2.lexema);
					strcpy(tk_tmp->lexema, var_tmp);
					tk_tmp->cod = TMP;
					tk_tmp->tipo = 2;
					free(var_tmp);
				}
				tk_aux = a2;
			} else {
				tk_aux = a1;
			}
		} else {
			if (a2.cod == VL_CHAR) {
				printf("\nERRO na linha %i, coluna %i: Erro de semantica.7\n",
						linha, coluna - 1);
				error = 1;
				tk_aux.cod = ERROR;
			} else if (a2.cod == VL_INT) {
				covert_to_float(&a2);
				if (tk_tmp->tipo != 3) {
					var_tmp = gerador_de_var_tmp();
					printf("%s = int to float %s\n", var_tmp, a2.lexema);
					strcpy(tk_tmp->lexema, var_tmp);
					tk_tmp->cod = TMP;
					tk_tmp->tipo = 2;
					free(var_tmp);
				}
				tk_aux = a1;
			} else {
				tk_aux = a1;
			}
		}
	} else if (a1.cod == VL_INT) {
		if (a2.cod == IDENTIFICADOR) {
			if (a2.tipo == VL_CHAR) {
				printf("\nERRO na linha %i, coluna %i: Erro de semantica.8\n",
						linha, coluna - 1);
				error = 1;
				tk_aux.cod = ERROR;
			} else if (a2.tipo == VL_FLOAT) {
				covert_to_float(&a1);  //USADO
				if (tk_tmp->tipo != 3) {
					var_tmp = gerador_de_var_tmp();
					printf("%s = int to float %s\n", var_tmp, a1.lexema);
					strcpy(tk_tmp->lexema, var_tmp);
					tk_tmp->cod = TMP;
					tk_tmp->tipo = 1;
					free(var_tmp);
				}
				tk_aux = a1;
			} else {
				tk_aux = a1;
			}
		} else {
			if (a2.cod == VL_CHAR) {
				printf("\nERRO na linha %i, coluna %i: Erro de semantica.9\n",
						linha, coluna - 1);
				error = 1;
				tk_aux.cod = ERROR;
			} else if (a2.cod == VL_FLOAT) {
				covert_to_float(&a1);  //USADO
				if (tk_tmp->tipo != 3) {
					var_tmp = gerador_de_var_tmp();
					printf("%s = int to float %s\n", var_tmp, a1.lexema);
					strcpy(tk_tmp->lexema, var_tmp);
					tk_tmp->cod = TMP;
					tk_tmp->tipo = 1;
					free(var_tmp);
				}
				tk_aux = a1;
			} else {
				tk_aux = a1;
			}
		}
	} else {
		tk_aux = a1;
	}
	return tk_aux;
}

int verifica_tipo_atr(Token a1, Token a2) {
	int op;
	if (a1.tipo == VL_INT) {
		if (a2.cod == IDENTIFICADOR) {
			if (a2.tipo != VL_INT) {
				op = FALSE;
			} else {
				op = TRUE;
			}

		} else if (a2.cod != VL_INT) {
			op = FALSE;
		} else {
			op = TRUE;
		}

	} else if (a1.tipo == VL_CHAR) {
		if (a2.cod == IDENTIFICADOR) {
			if (a2.tipo != VL_CHAR) {
				op = FALSE;
			} else {
				op = TRUE;
			}

		} else if (a2.cod != VL_CHAR) {
			op = FALSE;
		} else {
			op = TRUE;
		}

	} else {
		if (a2.cod == IDENTIFICADOR) {
			if (a2.tipo == VL_CHAR) {
				op = FALSE;
			} else {
				op = TRUE;
			}

		} else if (a2.cod == VL_CHAR) {
			op = FALSE;
		} else {
			op = TRUE;
		}
	}
	return op;
}

//------------------------------------------------------------------------
void covert_to_float(Token *tken) {
	tken->cod = VL_FLOAT;
}

char * gerador_de_label() {
	char buf[5];
	char * labbel;
	labbel = (char*) calloc(5, sizeof(char));
	sprintf(buf, "%i", label_cont);
	strcpy(labbel, "L");
	strcat(labbel, buf);
	label_cont++;
	return labbel;
}

char * gerador_de_var_tmp() {
	char buf[5];
	char * tmp_var;
	tmp_var = (char*) calloc(5, sizeof(char));
	sprintf(buf, "%i", tmp_cont);
	strcpy(tmp_var, "T");
	strcat(tmp_var, buf);
	tmp_cont++;
	return tmp_var;
}

Symbol * fim_lista() {
	Symbol * p;
	p = tabela;
	while (p->prox != NULL) {
		p = p->prox;
	}
	return p;
}

void preencher_tabela(Token t) {

	if (tabela == NULL) {
		tabela = (Symbol *) malloc(sizeof(Symbol));
		tabela->prox = NULL;
		strcpy(tabela->lexema, t.lexema);
		tabela->tipo = t.tipo;
		tabela->escopo = escopo;
	} else if (busca_na_tabela(&t) == TRUE) {
		printf(
				"\nERRO na linha %i, coluna %i lexema %s: Erro de semantica, variavel com multiplas declaracoes. 10\n",
				linha, coluna - 1, tk.lexema);
		error = 1;
		destruir_tabela();
	} else {
		Symbol *nova, *p;
		p = fim_lista();
		nova = (Symbol *) malloc(sizeof(Symbol));
		nova->tipo = t.tipo;
		strcpy(nova->lexema, t.lexema);
		nova->escopo = escopo;
		nova->prox = p->prox;
		p->prox = nova;
	}
}

void destruir_escopo() {
	Symbol *aux1, *aux2, *nova, *p;
	aux1 = NULL;
	p = tabela;
	while (p != NULL) {
		if (p->escopo != escopo) {
			if (aux1 == NULL) {
				nova = (Symbol *) malloc(sizeof(Symbol));
				nova->tipo = p->tipo;
				strcpy(nova->lexema, p->lexema);
				nova->escopo = p->escopo;
				nova->prox = NULL;
				aux1 = nova;
				p = p->prox;

			} else if (aux1->prox == NULL) {
				nova = (Symbol *) malloc(sizeof(Symbol));
				nova->tipo = p->tipo;
				strcpy(nova->lexema, p->lexema);
				nova->escopo = p->escopo;
				nova->prox = NULL;
				aux1->prox = nova;
				aux2 = aux1->prox;
				p = p->prox;

			} else {
				nova = (Symbol *) malloc(sizeof(Symbol));
				nova->tipo = p->tipo;
				strcpy(nova->lexema, p->lexema);
				nova->escopo = p->escopo;
				nova->prox = NULL;
				aux2->prox = nova;
				aux2 = aux2->prox;
				p = p->prox;

			}
		} else {
			p = p->prox;
		}
	}
	destruir_tabela();
	tabela = aux1;
}

void destruir_tabela() {
	Symbol *proxNo;
	while (tabela != NULL) {
		proxNo = tabela->prox;
		free(tabela);
		tabela = proxNo;
	}
}

int busca_na_tabela(Token *aux) {

	Symbol *p;
	int op = TRUE, achou = FALSE, escopo_aux;
	escopo_aux = escopo;
	again: p = tabela;
	if (escopo_aux == 0) {
		op = FALSE;
		goto fim_busca;
	}
	do {
		if (strcmp(aux->lexema, p->lexema) == 0) {
			if (p->escopo == escopo_aux) {
				achou = TRUE;
				if (aux->tipo == 0) {
					aux->tipo = p->tipo;
				}
			} else if (p->prox == NULL) {
				op = FALSE;
			} else {
				p = p->prox;
			}
		} else if (p->prox == NULL) {
			op = FALSE;
			break;
		} else {
			p = p->prox;
		}

	} while (p != NULL && achou == FALSE && op == TRUE);

	if (achou == FALSE && escopo_aux != 0 && dec_var == 0) {
		escopo_aux--;
		op = TRUE;
		goto again;
	}
	if (achou == TRUE) {
		op = TRUE;
	}
	fim_busca: return op;
}

int declara_variavel(FILE * comp) {
	Token tkaux;
	int tipo;
	if (verifica_tipo(tk.cod) == TRUE) {
		if (tk.cod == PLR_CHAR) {
			tipo = VL_CHAR;
		} else if (tk.cod == PLR_INT) {
			tipo = VL_INT;
		} else {
			tipo = VL_FLOAT;
		}
		tkaux = Scanner(comp);
		id: if (tkaux.cod == IDENTIFICADOR) {
			tkaux.tipo = tipo;
			preencher_tabela(tkaux);

			if (error == 1) {
				return FALSE;
			}
			tkaux = Scanner(comp);
			if (tkaux.cod == PeV) {
				return TRUE;
			} else if (tkaux.cod == VIRGULA) {
				tkaux = Scanner(comp);
				goto id;
			} else {
				goto dv_erro;
			}
		} else {
			goto dv_erro;
		}
	} else {
		dv_erro: if (error == 1) {
			return FALSE;
		}
		printf(
				"\nERRO: Declaracao de variavel mal formada na linha %i, coluna %i.\n",
				linha, coluna - 1);
		error = 1;
		return FALSE;
	}
}

int iteracao(FILE * comp) { // while "("<expr_relacional>")" <comando> | do <comando> while "("<expr_relacional>")"";"

	char * labbel;
	char labbel_aux1[3], labbel_aux2[3];
	Token tk_it;
	cont = 0;
	if (verifica_iteracao(tk.cod) == TRUE) {
		if (tk.cod == PLR_WHILE) {
			labbel = gerador_de_label();
			strcpy(labbel_aux1, labbel);
			free(labbel);
			printf("%s: \n", labbel_aux1);
			tk = Scanner(comp);
			if (tk.cod == ABREPARENTESE) {
				tk = Scanner(comp);
				tk_it = expr_relacional(comp);

				if (tk_it.cod != ERROR) {
					labbel = gerador_de_label();
					strcpy(labbel_aux2, labbel);
					free(labbel);
					printf("if %s == 0 goto %s\n", tk_it.lexema, labbel_aux2);

					tk = Scanner(comp);
					if (tk.cod == FECHAPARENTESE) {
						tk = Scanner(comp);
						if (comando(comp) == TRUE) {
							printf("goto %s\n", labbel_aux1);
							printf("%s: \n", labbel_aux2);
							return TRUE;
						} else {
							goto it_erro;
						}
					} else {
						goto it_erro;
					}
				} else {
					goto it_erro;
				}
			} else {
				goto it_erro;
			}
		} else if (tk.cod == PLR_DO) {
			labbel = gerador_de_label();
			strcpy(labbel_aux1, labbel);
			free(labbel);
			printf("%s: \n", labbel_aux1);
			tk = Scanner(comp);
			if (comando(comp) == TRUE) {
				tk = Scanner(comp);
				if (tk.cod == PLR_WHILE) {
					tk = Scanner(comp);
					if (tk.cod == ABREPARENTESE) {
						tk = Scanner(comp);
						tk_it = expr_relacional(comp);
						if (tk_it.cod != ERROR) {
							printf("if %s != 0 goto %s\n", tk_it.lexema,
									labbel_aux1);
							tk = Scanner(comp);
							if (tk.cod == FECHAPARENTESE) {
								tk = Scanner(comp);
								if (tk.cod == PeV) {
									return TRUE;
								} else {
									goto it_erro;
								}
							} else {
								goto it_erro;
							}
						} else {
							goto it_erro;
						}
					} else {
						goto it_erro;
					}
				} else {
					goto it_erro;
				}
			} else {
				goto it_erro;
			}
		} else {
			goto it_erro;
		}
	} else {
		it_erro: if (error == 1) {
			return FALSE;
		}
		printf("\nERRO: Iteracao mal formada na linha %i, coluna %i.\n", linha,
				coluna - 1);
		error = 1;
		return FALSE;
	}

}

int atribuicao(FILE * comp) { // <id> "=" <expr_arit> ";"
	Token aux, tk_aux, var_tk1;

	int op;
	//printf("\nATRIBUICAO\n");
	strcpy(var_tk1.lexema, "\0");
	if (tk.cod == IDENTIFICADOR) {
		tk_aux = tk;
		if (busca_na_tabela(&tk_aux) == FALSE) {
			printf(
					"\nERRO na linha %i, coluna %i lexema %s: Erro de semantica, variavel nao encontrada no escopo.\n",
					linha, coluna - 1, tk.lexema);
			error = 1;
			destruir_tabela();
		} else {
			tk = Scanner(comp);
			if (tk.cod == IGUAL) {
				tk = Scanner(comp);
				if (tk.cod == ABREPARENTESE || tk.cod == IDENTIFICADOR
						|| tk.cod == VL_INT || tk.cod == VL_FLOAT
						|| tk.cod == VL_CHAR) {
					top: aux = expr_arit(comp, &var_tk1);
					printf("%s = %s \n", tk_aux.lexema, var_tk1.lexema);
					if (aux.cod != ERROR) {
						op = verifica_tipo_atr(tk_aux, aux);
						if (op == FALSE) {
							goto at_erro;
						}
					}
					if (error == 1) {
						goto at_erro;
					}
					tk = Scanner(comp);

					if (tk.cod == FECHAPARENTESE) {
						tk = Scanner(comp);
					}
					if (tk.cod == PeV && aux.cod != ERROR) {
						op = TRUE;
					} else if (verifica_expr_arit(tk.cod) == TRUE) {
						tk = Scanner(comp);
						goto top;
					} else {
						goto at_erro;
					}
				} else {
					goto at_erro;
				}
			} else {
				goto at_erro;
			}
		}
	} else {
		at_erro: if (error == 1) {
			atr_cont = 0;
			return FALSE;
		}
		printf("\nERRO: Atribuicao mal formada na linha %i, coluna %i. %s\n",
				linha, coluna - 1, tk.lexema);
		error = 1;
		op = FALSE;
	}
	atr_cont = 0;
	return op;
}

Token expr_relacional(FILE * comp) { // <expr_arit> <op_relacional> <expr_arit>   DECLARAR O CONT!!!!!!!!!!!
	//printf("\n------ EXPRESSAO RELACIONAL ------\n");
	fpos_t pos;
	int aux;
	Token tk_aux, aux1, aux2, var_tk, tk_tmp;
	tk_tmp.tipo = 3;
	char * var_tmp = '\0';
	strcpy(var_tk.lexema, "\0");
	aux = verifica_fator(tk.cod);
	if (aux == TRUE) {
		expR_check = 1;
		aux1 = expr_arit(comp, &var_tk);
		tk_aux = tk;
		fgetpos(comp, &pos);
		tk = Scanner(comp);
		if (aux1.cod != ERROR) {
			if (verifica_op_relacional(tk.cod) == FALSE) {
				fsetpos(comp, &pos);
				tk = tk_aux;
				coluna = coluna - 1;
				strcpy(aux1.lexema, var_tk.lexema);
				aux1.cod = var_tk.cod;
				aux1.tipo = var_tk.tipo;
			} else if (verifica_op_relacional(tk.cod) == TRUE && cont == 0) {
				if (tk.cod == MENORQ) {
					opR_aux = 0;
				} else if (tk.cod == MAIORQ) {
					opR_aux = 1;
				} else if (tk.cod == MENORIGUAL) {
					opR_aux = 2;
				} else if (tk.cod == MAIORIGUAL) {
					opR_aux = 3;
				} else if (tk.cod == IGUALIGUAL) {

					opR_aux = 4;
				} else {
					opR_aux = 5;
				}
				cont++;
				tk = Scanner(comp);
				aux2 = expr_relacional(comp);
				if (aux2.cod != ERROR) {
					aux1 = verifica_tipo_resultante(aux1, aux2, &tk_tmp);
					if (aux1.cod == ERROR) {
						goto er_erro;
					}
					var_tmp = gerador_de_var_tmp();
					printf("%s = %s %s %s \n", var_tmp, var_tk.lexema,
							tipos_relacionail[opR_aux], aux2.lexema);
				} else {
					goto er_erro;
				}
			} else {
				goto er_erro;
			}
		} else {
			goto er_erro;
		}
	} else {
		er_erro: if (error == 1) {
			expR_check = 0;
			aux1.cod = ERROR;
			if (var_tmp != '\0') {
				free(var_tmp);
			}
			return aux1;
		}
		printf(
				"\nERRO: Operacao relacional mal formado na linha %i, coluna %i.\n",
				linha, coluna - 1);
		error = 1;
		aux1.cod = ERROR;
	}
	expR_check = 0;
	if (var_tmp != '\0') {
		strcpy(aux1.lexema, var_tmp);
		aux1.cod = TMP;
		aux1.tipo = 0;
	}
	free(var_tmp);
	return aux1;
}

Token expr_arit(FILE * comp, Token * tk1) {  // <termo><expr_arit2>
	//printf("\nEXPR_ARIT\n");
	int aux, erro = 0;
	fpos_t pos;
	Token tk_aux, a1, a2, tk_tmp;

	if (error == 1) {
		goto exp1_erro;
	}
	aux = verifica_fator(tk.cod);
	if (aux == FALSE) {
		erro = 1;
		goto exp1_erro;
	}
	a1 = termo(comp, &(*tk1));
	aux_g = a1;
	termo_int = 0;
	if (a1.cod == ERROR) {
		return a1;
	}
	tk_aux = tk;
	fgetpos(comp, &pos);
	tk = Scanner(comp);

	chamada++;
	a2 = expr_arit2(comp, &(*tk1));
	chamada--;
	if (a2.cod != ERROR) {
		atr_cont = 1;
		if (tk1->cod == TMP) {
			strcpy(tk_tmp.lexema, tk1->lexema);
			tk_tmp.cod = tk1->cod;
			tk_tmp.tipo = tk1->tipo;
		}
		tk_tmp.tipo = 3;
		a1 = verifica_tipo_resultante(a1, a2, &tk_tmp);

	} else {

		fsetpos(comp, &pos);
		tk = tk_aux;
		coluna = coluna - 1;
	}
	exp1_erro: if (error == 1) {
		a1.cod = ERROR;
	}
	if (erro == 1) {
		printf(
				"\nERRO: Expressao Eritimetica mal formada na linha %i, coluna %i.\n",
				linha, coluna - 1);
		error = 1;
		a1.cod = ERROR;
	}
	if (fator_check == 1) {
		strcpy(g_tmp_var.lexema, tk1->lexema);
		g_tmp_var.cod = tk1->cod;
		g_tmp_var.tipo = tk1->tipo;
		goto fim_ear;
	}
	fim_ear:
	return a1;
}

Token expr_arit2(FILE * comp, Token * tk1) { // "+" <termo><expr_arit2>   |"-" <termo><expr_arit2> | VAZIO
	fpos_t pos;
	int op;
	Token tk_aux, aux1, aux11, aux2, tk_tmp, tk_novo1, tk_novo2, catch_g;
	char * var_tmp, lex_tmp[TAM];
	tk_tmp.tipo = 0;
	catch_g = aux_g;
	strcpy(tk_tmp.lexema, "\0");
	if (tk.cod == MAIS || tk.cod == MENOS) {
		op = tk.cod;
		strcpy(tk_novo1.lexema, tk1->lexema);
		tk_novo1.cod = tk1->cod;
		tk_novo1.tipo = tk1->tipo;
		strcpy(lex_tmp, tk1->lexema);
		tk = Scanner(comp);
		strcpy(tk1->lexema, "\0");
		tk1->cod = 0;
		tk1->tipo = 0;
		aux1 = termo(comp, &(*tk1));
		aux11 = aux1;
		if (fator_cont == 0) {
			termo_check = 0;
			fator_check = 0;
		}
		if (tk_novo1.cod != TMP && tk1->cod != TMP) {
			verifica_tipo_resultante(tk_novo1, aux1, &tk_tmp);
			if (tk_tmp.tipo != 0) {
				if (tk_tmp.tipo == 1) {
					strcpy(tk_novo1.lexema, tk_tmp.lexema);
				} else {
					strcpy(aux11.lexema, tk_tmp.lexema);
					flag_print = 1;
				}
			}
		} else {
			tk_novo1.cod = catch_g.cod;
			tk_novo1.tipo = catch_g.tipo;
			strcpy(tk_novo2.lexema, tk1->lexema);
			tk_novo2.cod = aux1.cod;
			tk_novo2.tipo = aux1.tipo;
			verifica_tipo_resultante(tk_novo1, tk_novo2, &tk_tmp);
			if (tk_tmp.tipo != 0) {
				if (tk_tmp.tipo == 1) {
					strcpy(tk_novo1.lexema, tk_tmp.lexema);
				} else {
					strcpy(tk1->lexema, tk_tmp.lexema);
					flag_print = 0;
				}
			}
		}
		var_tmp = gerador_de_var_tmp();
		if (aux1.cod != ERROR) {
			if (flag_print == 1) {
				flag_print = 0;

				if (op == MAIS) {
					printf("%s = %s + %s \n", var_tmp, tk_novo1.lexema,
							aux11.lexema);
				} else {
					printf("%s = %s - %s \n", var_tmp, tk_novo1.lexema,
							aux11.lexema);
				}
			} else {
				if (op == MAIS) {
					printf("%s = %s + %s \n", var_tmp, tk_novo1.lexema,
							tk1->lexema);
				} else {
					printf("%s = %s - %s \n", var_tmp, tk_novo1.lexema,
							tk1->lexema);
				}
			}
			strcpy(tk1->lexema, var_tmp);
		} else {
			free(var_tmp);
			goto end;
		}
		free(var_tmp);
		tk_aux = tk;
		fgetpos(comp, &pos);
		tk = Scanner(comp);
		aux_g = aux1;
		chamada++;
		aux2 = expr_arit2(comp, &(*tk1));
		chamada--;
		if (aux2.cod != ERROR) {
			tk_tmp.tipo = 3;
			aux1 = verifica_tipo_resultante(aux1, aux2, &tk_tmp);
			if (tk_tmp.tipo != 0) {
				strcpy(tk1->lexema, tk_tmp.lexema);
				tk1->cod = tk_tmp.cod;
				tk1->tipo = tk_tmp.tipo;
			}
		} else {
			fsetpos(comp, &pos);
			tk = tk_aux;
			coluna = coluna - 1;
			goto end;
		}
		termo_int = 0;
	} else {
		aux1.cod = ERROR;
	}
	end: termo_int = 0;
	return aux1;
}

Token termo(FILE * comp, Token * tk1) { // <termo> "*" <fator> | <termo> / <fator> | <fator>
	//printf("\nTERMO\n");
	int aux, atr_c, exp_cont = 0;
	fpos_t pos;
	Token tk_aux, aux1, aux11, aux2, var_tk2, tk_tmp;
	char * var_tmp, lex_aux[TAM] = "\0";
	tk_tmp.tipo = 0;
	atr_c = 0;
	strcpy(tk_tmp.lexema, "\0");
	aux = verifica_fator(tk.cod); //ATUA
	termo_int = 1;
	if (aux == TRUE) {
		aux1 = fator(comp, &(*tk1)); // (p)
		if (strlen(tk1->lexema) != 0) {
			strcpy(lex_aux, tk1->lexema);
		} else {
			strcpy(tk1->lexema, aux1.lexema);
			tk1->cod = 0;
			tk1->tipo = 0;
		}
	}

	else {
		aux1.cod = ERROR;
	}
	if (aux1.cod != ERROR) {
		if (aux1.cod == IDENTIFICADOR) {
			if (busca_na_tabela(&aux1) == FALSE) {
				aux1.cod = ERROR;
				return aux1;
			}
		}
		tk_aux = tk;
		fgetpos(comp, &pos);
		tk = Scanner(comp);
		if (tk.cod == MULT || tk.cod == BARRA) {
			atr_cont = 1;
			while (tk.cod == MULT || tk.cod == BARRA) {
				termo_check = 1;
				op_atual = tk.cod;
				if (op_atual == BARRA && (aux1.cod == VL_INT|| aux1.tipo == VL_INT)) {
					covert_to_float(&aux1);
					var_tmp = gerador_de_var_tmp();
					printf("%s = int to float %s\n", var_tmp, aux1.lexema);
					strcpy(aux11.lexema, var_tmp);
					aux11.cod = TMP;
					aux11.tipo = 0;
					free(var_tmp);
					atr_c = 1;
				}
				tk = Scanner(comp);
				aux2 = fator(comp, &(*tk1));
				if (aux2.cod != ERROR) {
					if (aux2.cod == IDENTIFICADOR) {
						if (busca_na_tabela(&aux2) == FALSE) {
							aux2.cod = ERROR;
							return aux2;
						}
					}
					if (atr_c == 0) {
						aux11 = aux1;
					}
					atr_c = 0;
					aux1 = verifica_tipo_resultante(aux1, aux2, &tk_tmp);
					var_tmp = gerador_de_var_tmp();
					if (aux1.cod == ERROR) {
						free(var_tmp);
						goto ea_erro;
					}
					if (tk_tmp.tipo != 0) {
						if (tk_tmp.tipo == 1) {
							if (exp_cont == 0 && strlen(lex_aux) == 0) {
								aux11 = tk_tmp;
								tk_tmp.tipo = 0;
							} else {
								var_tk2 = tk_tmp;
								tk_tmp.tipo = 0;
								goto p;
							}
						} else {
							aux2 = tk_tmp;
							tk_tmp.tipo = 0;
						}
					}
					if (strlen(tk1->lexema) != 0) {
						strcpy(var_tk2.lexema, tk1->lexema);
						var_tk2.cod = tk1->cod;
						var_tk2.tipo = tk1->tipo;
					}
					p: strcpy(tk1->lexema, var_tmp);
					tk1->cod = TMP;
					tk1->tipo = 0;
					if (exp_cont == 0 && strlen(lex_aux) == 0) {
						if (op_atual == MULT) {
							printf("%s = %s * %s \n", tk1->lexema, aux11.lexema,
									aux2.lexema);
						} else {
							printf("%s = %s / %s \n", tk1->lexema, aux11.lexema,
									aux2.lexema);
						}
					} else {
						if (op_atual == MULT) {
							printf("%s = %s * %s \n", tk1->lexema,
									var_tk2.lexema, aux2.lexema);
						} else {
							printf("%s = %s / %s \n", tk1->lexema,
									var_tk2.lexema, aux2.lexema);
						}
					}
					exp_cont++;
					free(var_tmp);
				} else {
					fsetpos(comp, &pos);
					tk = tk_aux;
					coluna = coluna - 1;
					goto ea_erro;
				}
				tk_aux = tk;
				fgetpos(comp, &pos);
				tk = Scanner(comp);
				if (tk.cod != MULT && tk.cod != BARRA) {
					fsetpos(comp, &pos);
					tk = tk_aux;
					coluna = coluna - 1;
				}
			}

		} else {
			fsetpos(comp, &pos);
			tk = tk_aux;
			coluna = coluna - 1;
		}
	} else {
		ea_erro: if (error == 1) {
			aux1.cod = ERROR;
			return aux1;
		}
		printf("\nERRO: Termo mal formado na linha %i, coluna %i.\n", linha,
				coluna - 1);
		error = 1;
	}
	if (exp_cont > 0) {
		tk1->cod = TMP;
	}
	if (tk1->cod == TMP) {
		if (fator_check == 1 && termo_check == 1) {
			flag_print = 0;
		} else {
			flag_print = 1;
		}

	} else {
		strcpy(tk1->lexema, aux1.lexema);
		tk1->cod = aux1.cod;
		tk1->tipo = aux1.tipo;
	}
	return aux1;
}

Token fator(FILE * comp, Token *tk1) { // ( <expr_arit> ) | <id> | <real> | <inteiro> | <char>
	//printf("\nFATOR\n");
	Token aux;
	if (verifica_fator(tk.cod) == TRUE) {
		fator_cont++;
		if (tk.cod == ABREPARENTESE) {
			tk = Scanner(comp);
			if (verifica_fator(tk.cod) == TRUE) {
				fator_check = 1;
				aux = expr_arit(comp, &(*tk1));
				tk = Scanner(comp);
				if (error == 1) {
					goto fterro;
				}
				if (tk.cod == FECHAPARENTESE && aux.cod != ERROR) {
					fator_cont--;
				} else {
					goto fterro;
				}
			} else {
				goto fterro;
			}
		} else {
			aux = tk;
			goto fim_fat;
		}
	} else {
		fterro: if (error == 1) {
			tk.cod = ERROR;
			aux = tk;
			return aux;
		}
		printf("\nERRO: Fator mal formado na linha %i, coluna %i.\n", linha,
				coluna - 1);
		error = 1;
		tk.cod = ERROR;
	}

	fim_fat: fator_cont--;
	return aux;
}

int comando_basico(FILE * comp) {  // <atribuição> | <bloco>
	if (verifica_atribuicao(tk.cod) == TRUE) {
		if (atribuicao(comp) == TRUE) {
			return TRUE;
		} else {
			goto cberro;
		}
	} else if (verifica_bloco(tk.cod) == TRUE) {
		if (bloco(comp) == TRUE) {
			return TRUE;
		} else {
			goto cberro;
		}
	} else {
		cberro: if (error == 1) {
			return FALSE;
		}
		if (error == 1) {
			return FALSE;
		}
		printf("\nERRO: Comando Basico mal formado na linha %i, coluna %i.\n",
				linha, coluna - 1);
		error = 1;
		return FALSE;
	}
}

int comando(FILE * comp) { // <comando_básico> | <iteração> | if "("<expr_relacional>")" <comando> {else <comando>}?
	char * labbel;
	char labbel_aux1[3], labbel_aux2[3];
	Token tk_it;
	fpos_t position;
	cont = 0;
	if (verifica_comando(tk.cod) == TRUE) {
		if (verifica_comando_basico(tk.cod) == TRUE) {
			if (comando_basico(comp) == TRUE) {
				return TRUE;
			} else {
				goto c_erro;
			}
		} else if (verifica_iteracao(tk.cod) == TRUE) {
			if (iteracao(comp) == TRUE) {
				return TRUE;
			} else {
				goto c_erro;
			}
		} else if (tk.cod == PLR_IF) {
			tk = Scanner(comp);
			if (tk.cod == ABREPARENTESE) {
				tk = Scanner(comp);
				tk_it = expr_relacional(comp);
				if (tk_it.cod != ERROR) {
					labbel = gerador_de_label();
					strcpy(labbel_aux1, labbel);
					free(labbel);
					printf("if %s == 0 goto %s\n", tk_it.lexema, labbel_aux1);
					tk = Scanner(comp);
					if (tk.cod == FECHAPARENTESE) {
						tk = Scanner(comp);
						if (comando(comp) == TRUE) {

							fgetpos(comp, &position);
							tk = Scanner(comp);
							if (tk.cod == PLR_ELSE) {
								labbel = gerador_de_label();
								strcpy(labbel_aux2, labbel);
								free(labbel);
								printf("goto %s\n", labbel_aux2);
								printf("%s: \n", labbel_aux1);
								tk = Scanner(comp);
								if (comando(comp) == TRUE) {
									printf("%s: \n", labbel_aux2);
									return TRUE;
								} else {
									goto c_erro;
								}
							} else {
								printf("%s: \n", labbel_aux1);
								fsetpos(comp, &position);
								coluna = coluna - 1;
								return TRUE;
							}
						} else {
							goto c_erro;
						}
					} else {
						goto c_erro;
					}
				} else {
					goto c_erro;
				}
			} else {
				goto c_erro;
			}
		} else {
			goto c_erro;
		}
	} else {
		c_erro: if (error == 1) {
			return FALSE;
		}
		printf("\nERRO: Comando mal formado na linha %i, coluna %i.\n", linha,
				coluna - 1);
		error = 1;
		return FALSE;
	}

}

int bloco(FILE * comp) { // <comando_básico> | <iteração> | if "("<expr_relacional>")" <comando> {else <comando>}?

	if (tk.cod == ABRECHAVE) {
		escopo++;
		tk = Scanner(comp);
		do {
			if (verifica_tipo(tk.cod) == TRUE) {
				dec_var = 1;
				if (declara_variavel(comp) == TRUE) {
					tk = Scanner(comp);
				} else {
					goto b_erro;
				}
				dec_var = 0;
			}
		} while (verifica_tipo(tk.cod) == TRUE);
		do {
			if (verifica_comando(tk.cod) == TRUE) {
				if (comando(comp) == TRUE) {
					tk = Scanner(comp);
				} else {
					goto b_erro;
				}
			}
		} while (verifica_comando(tk.cod) == TRUE);

		if (tk.cod == FECHACHAVE) {
			destruir_escopo();
			escopo--;
			return TRUE;
		} else {
			goto b_erro;
		}
	} else {

		b_erro: if (error == 1) {
			return FALSE;
		}
		printf("\nERRO: Bloco mal formado na linha %i, coluna %i.\n", linha,
				coluna - 1);
		error = 1;
		return FALSE;
	}

}

Token Scanner(FILE * comp) {
	static char c = ' ';
	int s_cont, cont_pontos = 0;
	tk.cod = 0;
	tk.lexema[0] = '\0';
	c = fgetc(comp);
	coluna = coluna + 1;  //conta_coluna(coluna);

	inicio: if (c == ' ') {
		do {
			c = fgetc(comp);
			coluna = coluna + 1;  //conta_coluna(coluna);
		} while (c == ' ');
	}
	if (c == '\n') { //verifica_quebra(c, &*linha, &*coluna) == TRUE
		linha = linha + 1;
		coluna = 0;
		c = fgetc(comp);
		coluna = coluna + 1; //conta_coluna(coluna);
		goto inicio;
	}
	if (c == '\t') { //verifica_tab(c, &*coluna) == TRUE
		coluna = coluna + 4;
		c = fgetc(comp);
		coluna = coluna + 1; //conta_coluna(coluna);
		goto inicio;
	}
	if (c == '\r') {
		c = fgetc(comp);
		goto inicio;
	}
	if (c == ';') {
		tk.cod = PeV;
	} else if (c == '$') {
		printf(
				"\nERRO 1 na linha %i, coluna %i, ultimo token lido %i %s: Charactere Invalido mal formado.\n",
				linha, coluna - 1, tk.cod, tk.lexema); //TERMINAR
		error = 1;
		tk.cod = ERROR_CHAR;
		goto r;
	} else if (c == '\'') {  //aspas simples, inicio de char
		tk.cod = VL_CHAR;
		tk.lexema[0] = c;
		for (s_cont = 0; s_cont <= TAM; s_cont++) {
			c = fgetc(comp);
			coluna = coluna + 1;  //conta_coluna(coluna);
			if (verifica_EOF(c) == TRUE) {   //FALTA VERIFICACAO NO MAIN
				printf(
						"\nERRO 2 na linha %i, coluna %i, ultimo token lido %i %s: Char mal formado.\n",
						linha, coluna - 1, tk.cod, tk.lexema); //TERMINAR
				error = 1;
				goto r;
				break;
			} else if (c == '\'') //isdigit(tk.lexema[cont]) || isalpha(tk.lexema[cont])
					{
				if (isdigit(tk.lexema[s_cont]) || isalpha(tk.lexema[s_cont])) {
					tk.lexema[s_cont + 1] = c;
					tk.lexema[s_cont + 2] = '\0';
					goto r;
				} else {
					printf(
							"\nERRO 4 na linha %i, coluna %i, ultimo token lido %i %s: Char mal formado.\n",
							linha, coluna - 1, tk.cod, tk.lexema); //TERMINAR
					error = 1;
					goto r;
					break;
				}
			} else if (!isdigit(c) && !isalpha(c)) {
				printf(
						"\nERRO 3 na linha %i, coluna %i, ultimo token lido %i %s: Char mal formado.\n",
						linha, coluna - 1, tk.cod, tk.lexema); //TERMINAR
				error = 1;
				goto r;
				break;
			} else {
				tk.lexema[s_cont + 1] = c;
			}

		}

	} else if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
		tk.cod = IDENTIFICADOR;
		tk.lexema[0] = c;
		tk.tipo = 0;
		for (s_cont = 0; s_cont <= TAM; s_cont++) {
			c = fgetc(comp);
			coluna = coluna + 1; //conta_coluna(coluna);
			if (verifica_EOF(c) == TRUE) {   //FALTA VERIFICACAO NO MAIN
				tk.lexema[s_cont + 1] = '\0';
				if (verifica_reservadas(tk.lexema) != FALSE) {
					tk.cod = verifica_reservadas(tk.lexema);
					tk.lexema[0] = '\0';
				}
				coluna = coluna - 1;
				ungetc(c, comp);
				break;
			} else if (verifica_char(c) == FALSE) {
				if (verifica_quebra_tab(c) == TRUE) {
					tk.lexema[s_cont + 1] = '\0';
					if (verifica_reservadas(tk.lexema) != FALSE) {
						tk.cod = verifica_reservadas(tk.lexema);
						tk.lexema[0] = '\0';
					}
					coluna = coluna - 1;
					ungetc(c, comp);
					break;
				} else if (c == ' ') {
					tk.lexema[s_cont + 1] = '\0';
					if (verifica_reservadas(tk.lexema) != FALSE) {
						tk.cod = verifica_reservadas(tk.lexema);
						tk.lexema[0] = '\0';
					}
					coluna = coluna - 1;
					ungetc(c, comp);
					break;
				} else {
					tk.lexema[s_cont + 1] = '\0';
					if (verifica_reservadas(tk.lexema) != FALSE) {
						tk.cod = verifica_reservadas(tk.lexema);
						tk.lexema[0] = '\0';
					}
					coluna = coluna - 1;
					ungetc(c, comp);
					break;
				}

			} else {
				tk.lexema[s_cont + 1] = c;
			}

		}
	} else if ((c >= '0' && c <= '9') || c == '.') {
		s_cont = 0;
		if (c == '.') {
			tk.cod = VL_FLOAT;
			tk.lexema[0] = '0';
			tk.lexema[1] = c;
			cont_pontos = 1;
			s_cont = 1;
			coluna = coluna + 1;
		} else {
			tk.cod = VL_INT;
			tk.lexema[0] = c;
		}
		for (s_cont; s_cont <= TAM; s_cont++) {
			c = fgetc(comp);
			coluna = coluna + 1;   //conta_coluna(coluna);
			if (tk.lexema[s_cont] == '.' && !isdigit(c)) //EX: SE .a
					{
				printf(
						"\nERRO na linha %i, coluna %i, ultimo token lido %i %s: Float mal formado.\n",
						linha, coluna - 1, tk.cod, tk.lexema); //TERMINAR
				error = 1;
				goto r;
				break;
			} else if (c == '.' && isdigit(tk.lexema[s_cont])) //EX: SE 0.
					{
				cont_pontos = cont_pontos + 1;
				if (cont_pontos > 1)
					goto dot;
				tk.cod = VL_FLOAT;
				tk.lexema[s_cont + 1] = c;
			} else if (verifica_quebra(c) == TRUE || verifica_tab(c) == TRUE) {
				tk.lexema[s_cont + 1] = '\0';
				coluna = coluna - 1;
				ungetc(c, comp);
				break;
			} else if (!isdigit(c)) // VERIFICA SE NAO E DIGITO(INCLUSIVE ESPACO)
					{
				dot: tk.lexema[s_cont + 1] = '\0';
				coluna = coluna - 1;
				ungetc(c, comp);
				break;
			} else {
				tk.lexema[s_cont + 1] = c;
			}
		}

	} else if (c == '>') {
		c = fgetc(comp);
		coluna = coluna + 1; //conta_coluna(coluna);
		if (verifica_EOF(c) == TRUE) {
			tk.cod = MAIORQ;
			coluna = coluna - 1;
		} else if (c == '=') {
			tk.cod = MAIORIGUAL;
		} else {
			tk.cod = MAIORQ;
			coluna = coluna - 1;
			ungetc(c, comp);
		}
	} else if (c == '<') {
		c = fgetc(comp);
		coluna = coluna + 1; //conta_coluna(coluna);
		if (verifica_EOF(c) == TRUE) {
			tk.cod = MENORQ;
			coluna = coluna - 1;
		} else if (c == '=') {
			tk.cod = MENORIGUAL;
		} else {
			tk.cod = MENORQ;
			coluna = coluna - 1;
			ungetc(c, comp);
		}
	} else if (c == '!') {
		c = fgetc(comp);
		coluna = coluna + 1; //conta_coluna(coluna);
		if (verifica_EOF(c) == TRUE) {
			printf(
					"\nERRO na linha %i, coluna %i, ultimo token lido %i %s: Operador relacional mal formado.\n",
					linha, coluna - 1, tk.cod, tk.lexema); //TERMINAR
			error = 1;
			goto r;
		} else if (c != '=') {
			printf(
					"\nERRO na linha %i, coluna %i, ultimo token lido %i %s: Operador relacional mal formado.\n",
					linha, coluna - 1, tk.cod, tk.lexema); //TERMINAR
			error = 1;
			goto r;
		} else {
			tk.cod = DIFERENTE;
		}
	} else if (c == '=') {
		c = fgetc(comp);
		coluna = coluna + 1; //conta_coluna(coluna);
		if (verifica_EOF(c) == TRUE) {
			tk.cod = IGUAL;
		} else if (c == '=') {
			tk.cod = IGUALIGUAL;
		} else {
			tk.cod = IGUAL;
			ungetc(c, comp);
		}

	} else if (c == '+') {
		tk.cod = MAIS;
	} else if (c == '-') {
		tk.cod = MENOS;
	} else if (c == '*') {
		tk.cod = MULT;
	} else if (c == '/') {
		c = fgetc(comp);
		coluna = coluna + 1; //conta_coluna(coluna);
		if (verifica_EOF(c) == TRUE) {   //FALTA VERIFICACAO NO MAIN
			tk.cod = BARRA;
			coluna = coluna - 1;
			ungetc(c, comp);
		} else if (c == '/') {
			do {
				c = fgetc(comp);
				coluna = coluna + 1;   //conta_coluna(coluna);
				if (verifica_EOF(c) == TRUE)
					break;
			} while (c != '\n');
			if (verifica_EOF(c) == TRUE) {
				ungetc(c, comp);
				goto inicio;
			} else {
				verifica_quebra(c);
				verifica_tab(c);
				goto inicio;
			}

		} else if (c == '*') {
			int free = 0;
			do {
				c = fgetc(comp);
				coluna = coluna + 1;   //conta_coluna(coluna);
				verifica_quebra(c);
				verifica_tab(c);
				a: if (c == '*') {
					c = fgetc(comp);
					coluna = coluna + 1;   //conta_coluna(coluna);
					if (verifica_quebra(c) == TRUE || verifica_tab(c) == TRUE)
						goto a;
					if (verifica_EOF(c) == TRUE) {
						printf(
								"\nERRO na linha %i, coluna %i, ultimo token lido %i %s: Comentario mal formado.\n",
								linha, coluna - 1, tk.cod, tk.lexema); //TERMINAR
						tk.cod = EOF;
						error = 1;
						goto r;
						break;
					}
					if (c != '/') {
						goto a;
					} else {
						free = 1;
						c = fgetc(comp);
						goto inicio;
					}
				}
				if (verifica_EOF(c) == TRUE) {
					printf(
							"\nERRO na linha %i, coluna %i, ultimo token lido %i %s: Comentario mal formado.\n",
							linha, coluna - 1, tk.cod, tk.lexema); //TERMINAR
					tk.cod = EOF;
					error = 1;
					goto r;
					break;
				}
			} while (free == 0);
		} else {
			tk.cod = BARRA;
			coluna = coluna - 1;
			ungetc(c, comp);
		}

	} else if (c == '(') {
		tk.cod = ABREPARENTESE;
	} else if (c == ')') {
		tk.cod = FECHAPARENTESE;
	} else if (c == '{') {
		tk.cod = ABRECHAVE;
	} else if (c == '}') {
		tk.cod = FECHACHAVE;
	} else if (c == ',') {
		tk.cod = VIRGULA;
	} else if (c == EOF) {
		tk.cod = EOF;
	} else {
		if (c != '\t' || c != '\n') {
			printf(
					"\nERRO na linha %i, coluna %i, ultimo token lido %i %s: Caracter Invalido mal formado.\n",
					linha, coluna, tk.cod, tk.lexema); //TERMINAR
			tk.cod = ERROR_CHAR;
			error = 1;
		}
	}
	r: return tk;
}

Token Parser(FILE * comp) { //
	int aux;
	Token PToken;

	PToken = Scanner(comp);
	if (PToken.cod == EOF) {
		return PToken;
	}

	if (PToken.cod == PLR_INT) {
		PToken = Scanner(comp);
		if (PToken.cod == PLR_MAIN) {
			PToken = Scanner(comp);
			if (PToken.cod == ABREPARENTESE) {
				PToken = Scanner(comp);
				if (PToken.cod == FECHAPARENTESE) {
					PToken = Scanner(comp);
					if (PToken.cod == ABRECHAVE) {
						aux = bloco(comp);
						if (aux == TRUE) {
							return PToken;
						} else {
							goto p_erro;
						}
					} else {
						goto p_erro;
					}
				} else {
					goto p_erro;
				}
			} else {
				goto p_erro;
			}
		} else {
			goto p_erro;
		}
	} else {
		p_erro: if (error == 1) {
			return PToken;
		}
		printf("\nERRO: Bloco mal formado na linha %i, coluna %i.\n", linha,
				coluna - 1);
		error = 1;
		return PToken;
	}
}

