#ifndef FARMACIA_H
#define FARMACIA_H

#include <stdio.h>

#define DB_FILE "Farmacos.dat"

typedef struct TpFarma {
  char Nome[50];
  char Modelo[50];
  float Preco;
  int QEstoque;
  int Ativo; // 1 = ativo, 0 = inativo
  int Qtd;
} TpFARMA;

extern TpFARMA RgFarma;
extern long int Tamanho;

// Funções originais (Modo Terminal / CLI)
int FarmacoJaExiste(const char *nomeFarmaco, const char *modeloFarmaco);
void Incluir();
void Excluir();
void Venda();
void Desativar();
void Ativar();
void Consultar();
void LTodos();
long int TArquivo();

// Funções exportadas para a API Web (ctypes / .so)
int api_incluir(const char *nome, const char *modelo, float preco, int estoque);
int api_excluir(const char *nome, const char *modelo);
int api_vender(const char *nome, const char *modelo, int qtd, float valor_pago, float *troco_out);
int api_alterar_status(const char *nome, const char *modelo, int ativo, float novo_preco, int novo_estoque);
int api_listar_todos(TpFARMA *buffer, int max_itens);

#endif