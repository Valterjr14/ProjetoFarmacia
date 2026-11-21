#include <ctype.h>
#include <stdio.h>
#include "farmacia.h"

int main() {
  char Opcao;
  do {
    printf("\n\n> > > Pague Pouco - Sistema de Farmacia < < <\n\n");
    printf("I - Incluir\n");
    printf("E - Excluir\n");
    printf("V - Venda\n");
    printf("D - Desativar\n");
    printf("A - Ativar\n");
    printf("C - Consultar\n");
    printf("T - Listar Todos\n");
    printf("S - Sair\n\n");
    printf("Opcao: ");
    scanf(" %c", &Opcao);
    Opcao = toupper(Opcao);

    switch (Opcao) {
      case 'I': Incluir(); break;
      case 'E': Excluir(); break;
      case 'V': Venda(); break;
      case 'D': Desativar(); break;
      case 'A': Ativar(); break;
      case 'C': Consultar(); break;
      case 'T': LTodos(); break;
      case 'S': printf("Encerrando...\n"); break;
      default: printf("Opcao invalida.\n");
    }
  } while (Opcao != 'S');

  return 0;
}