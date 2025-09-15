#ifndef FARMACIA_H
#define FARMACIA_H

typedef struct TpFarma {
  char Nome[21];
  char Modelo[21];
  float Preco;
  int QEstoque;
  int Ativo; // Novo campo para indicar se o fármaco está ativo (1) ou não (0)
  int Qtd;
} TpFARMA;

extern TpFARMA RgFarma;
extern FILE *ArqFarma;
extern long int Tamanho;

int FarmacoJaExiste(const char *nomeFarmaco, const char *modeloFarmaco);
void Incluir();
void Excluir();
void Venda();
void Desativar();
void Ativar();
void Consultar();
void LTodos();
long int TArquivo();

#endif
