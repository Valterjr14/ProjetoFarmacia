#include <ctype.h>
#include <stdio.h>
#include "farmacia.h"

int main() {
  ArqFarma = fopen("Farmacos.dat", "a+b");
  if (ArqFarma == NULL) {
    printf("Erro ao abrir arquivo.");
    return 1;
  }

  char Opcao;
  do {
    printf("\n\n> > > Pague Pouco < < < \n\n");
    printf("Que deseja fazer? \n\n");
    printf("I - Incluir \n");
    printf("E - Excluir \n");
    printf("V - Venda \n");
    printf("D - Desativar \n");
    printf("A - Ativar \n");
    printf("C - Consultar \n");
    printf("T - Listar Todos \n");
    printf("S - Sair \n\n");
    scanf(" %c", &Opcao);
    Opcao = toupper(Opcao);
    switch (Opcao) {
    case 'I':
      Incluir();
      break;
    case 'E':
      Excluir();
      break;
    case 'V':
      Venda();
      break;
    case 'D':
      Desativar();
      break;
    case 'A':
      Ativar();
      break;
    case 'C':
      Consultar();
      break;
    case 'T':
      LTodos();
      break;
    }
  } while (Opcao != 'S');

  fclose(ArqFarma);
  return 0;
}