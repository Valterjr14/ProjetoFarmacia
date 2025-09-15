#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "farmacia.h"

TpFARMA RgFarma;
FILE *ArqFarma;
long int Tamanho = sizeof(TpFARMA);

// Função auxiliar para verificar se um fármaco já existe no arquivo
int FarmacoJaExiste(const char *nomeFarmaco, const char *modeloFarmaco) {
  FILE *ArqFarma = fopen("Farmacos.dat", "r+b");
  if (ArqFarma == NULL) {
    printf("Erro ao abrir arquivo para verificacao.");
    return 0;
  }

  struct TpFarma farmacoExistente;

  while (fread(&farmacoExistente, sizeof(struct TpFarma), 1, ArqFarma) == 1) {
    if (strcmp(farmacoExistente.Nome, nomeFarmaco) == 0 && strcmp(farmacoExistente.Modelo, modeloFarmaco) == 0) {
      fclose(ArqFarma);
      return 1; // O fármaco já existe
    }
  }

  fclose(ArqFarma);

  return 0; // O fármaco não existe
}

void Incluir() {
  char R;
  do {
    printf("*** inclusao ***\n\n");
    printf("Nome: ");
    scanf("%s", RgFarma.Nome);
    printf("Modelo: ");
    scanf("%s", RgFarma.Modelo);

    if (FarmacoJaExiste(RgFarma.Nome, RgFarma.Modelo)) {
      printf("O fármaco '%s %s' já existe no arquivo.\n", RgFarma.Nome, RgFarma.Modelo);
      printf("\nAVISO: Se o fármaco já existe no arquivo e não aparece na listagem, o mesmo deve estar inativo.\n");
      printf("Para isso consulte o medicamento no banco de dados.\n");
      printf("\nNova inclusao? S/N ");
      scanf(" %c", &R);
      R = toupper(R);
      continue;
    }

    printf("Preco: ");
    scanf("%f", &RgFarma.Preco);
    printf("Estoque: ");
    scanf("%d", &RgFarma.QEstoque);
    RgFarma.Ativo = 1; // Definindo como ativo por padrão

    FILE *ArqFarma = fopen("Farmacos.dat", "a+b");
    if (ArqFarma == NULL) {
      printf("Erro ao abrir arquivo para inclusao.");
      return;
    }

    // Escrever a estrutura do novo fármaco no arquivo
    fwrite(&RgFarma, Tamanho, 1, ArqFarma);

    fclose(ArqFarma);

    printf("\nNova inclusao? S/N ");
    scanf(" %c", &R);
    R = toupper(R);
  } while (R != 'N');
}

void Excluir() {
  char farmaco[21];
  char modelo[21];
  printf("*** exclusão ***\n\n");
  printf("Qual farmaco deseja excluir? ");
  scanf("%s", farmaco);
  printf("Qual o modelo do farmaco? ");
  scanf("%s", modelo);

  FILE *TempArq = fopen("temp.dat", "wb");
  if (TempArq == NULL) {
    printf("Erro ao abrir arquivo temporário.");
    return;
  }

  FILE *ArqFarma = fopen("Farmacos.dat", "rb");
  if (ArqFarma == NULL) {
    printf("Erro ao abrir arquivo para exclusão.");
    fclose(TempArq);
    return;
  }

  while (fread(&RgFarma, sizeof(struct TpFarma), 1, ArqFarma) == 1) {
    if (strcmp(RgFarma.Nome, farmaco) == 0 && strcmp(RgFarma.Modelo, modelo) == 0) {
      fwrite(&RgFarma, sizeof(struct TpFarma), 1, TempArq);
    }
  }

  fclose(ArqFarma);
  fclose(TempArq);

  remove("Farmacos.dat");
  rename("temp.dat", "Farmacos.dat");
  fflush(NULL);

  if(strcmp(RgFarma.Nome, farmaco) == 0 && strcmp(RgFarma.Modelo, modelo) == 0){
    printf("Medicamento excluído com sucesso!\n");
  } else {
    printf("Medicamento não encontrado.\n")
  }
}

void Venda() {
  char farmaco[21];
  char modelo[21];
  int quantidadeDesejada, encontrado = 0;
  float valorPago, totalVenda, troco;

  printf("*** venda ***\n\n");
  printf("Qual farmaco deseja vender? ");
  scanf("%s", farmaco);
  printf("Qual o modelo do farmaco que deseja vender? ");
  scanf("%s", modelo);

  FILE *TempArq = fopen("temp.dat", "a+b");
  if (TempArq == NULL) {
    printf("Erro ao abrir arquivo temporário.");
    return;
  }

  FILE *ArqFarma = fopen("Farmacos.dat", "r+b");
  if (ArqFarma == NULL) {
    printf("Erro ao abrir arquivo para exclusão.");
    fclose(TempArq);
    return;
  }

  while (fread(&RgFarma, sizeof(struct TpFarma), 1, ArqFarma) == 1) {
    if (strcmp(RgFarma.Nome, farmaco) == 0 && strcmp(RgFarma.Modelo, modelo) == 0) {
      encontrado = 1;

      printf("Informe a quantidade desejada: ");
      scanf("%d", &quantidadeDesejada);

      if (quantidadeDesejada > RgFarma.QEstoque) {
        printf("Estoque insuficiente!\n");
        printf("Quantidade disponível: %d\n", RgFarma.QEstoque);
        fwrite(&RgFarma, sizeof(struct TpFarma), 1, TempArq);
      } else {
          totalVenda = quantidadeDesejada * RgFarma.Preco;
          printf("Total da venda: R$ %.2f\n", totalVenda);

          printf("Valor pago: ");
          scanf("%f", &valorPago);

          if (valorPago < totalVenda) {
            printf("Valor insuficiente para a compra!\n");
            fwrite(&RgFarma, sizeof(struct TpFarma), 1, TempArq);
          } else {
              printf("Venda realizada com sucesso!\n");

              troco = valorPago - totalVenda;
              printf("Troco: R$ %.2f\n", troco);

              RgFarma.QEstoque -= quantidadeDesejada;
              printf("Estoque atualizado para: %d\n", RgFarma.QEstoque); // Depuração
              fwrite(&RgFarma, sizeof(struct TpFarma), 1, TempArq);
          }

      }
    } else {  
        fwrite(&RgFarma, sizeof(struct TpFarma), 1, TempArq);
      }

  }

  if (!encontrado) {
    printf("Medicamento não encontrado.\n");
  }

  fclose(ArqFarma);
  fclose(TempArq);

  remove("Farmacos.dat");
  rename("temp.dat", "Farmacos.dat");

  ArqFarma = fopen("Farmacos.dat", "r+b");
  if (ArqFarma == NULL) {
    printf("Erro ao reabrir arquivo Farmacos.dat após a venda.\n");
    return;
  }
}

//Melhoria 1
void Desativar() {
  printf("*** desativar ***\n\n");
  printf("Qual farmaco deseja desativar? ");
  char Farmaco[21];
  scanf("%s", Farmaco);
  printf("Qual o modelo do farmaco que deseja desativar? ");
  char Modelo[21];
  scanf("%s", Modelo);

  FILE *TempArq = fopen("temp.dat", "a+b");
  if (TempArq == NULL) {
    printf("Erro ao abrir arquivo temporário.");
    return;
  }

  fseek(ArqFarma, 0, 0);
  while (fread(&RgFarma, Tamanho, 1, ArqFarma) == 1) {
    if (strcmp(RgFarma.Nome, Farmaco) == 0 && strcmp(RgFarma.Modelo, Modelo) == 0) {
      RgFarma.Ativo = 0; // Marca como inativo
    }
    fwrite(&RgFarma, Tamanho, 1, TempArq);
  }

  fclose(ArqFarma);
  fclose(TempArq);

  remove("Farmacos.dat");
  rename("temp.dat", "Farmacos.dat");
  ArqFarma = fopen("Farmacos.dat", "r+b");

  printf("\nFármaco marcado como inativo com sucesso.\n");
}

long int TArquivo() {
  fseek(ArqFarma, 0, 2);
  long int R = ftell(ArqFarma) / Tamanho;
  return R;
}

//Melhoria 2
void Ativar() {
  printf("*** reativação ***\n\n");
  printf("Qual farmaco deseja reativar? ");
  char Farmaco[21];
  scanf("%s", Farmaco);
  printf("Qual o modelo do farmaco que deseja reativar? ");
  char Modelo[21];
  scanf("%s", Modelo);
  printf("Qual é o preço atualizado? ");
  float Preco;
  scanf("%f", &Preco);
  printf("Qual é o novo estoque? ");
  int Estoque;
  scanf("%d", &Estoque);

  FILE *TempArq = fopen("temp.dat", "a+b");
  if (TempArq == NULL) {
    printf("Erro ao abrir arquivo temporário.");
    return;
  }

  int encontrado = 0;

  fseek(ArqFarma, 0, 0);
  while (fread(&RgFarma, Tamanho, 1, ArqFarma) == 1) {
    if (strcmp(RgFarma.Nome, Farmaco) == 0 && strcmp(RgFarma.Modelo, Modelo) == 0) {
      encontrado = 1; // Marca como encontrado
      if (RgFarma.Ativo == 0) {
        RgFarma.Ativo = 1;
        RgFarma.Preco = Preco;
        RgFarma.QEstoque = Estoque;
      } else {
        printf("Erro: O fármaco '%s' de modelo '%s' já está ativo.\n", Farmaco, Modelo);
      }
    }
    fwrite(&RgFarma, Tamanho, 1, TempArq);
  }

  fclose(ArqFarma);
  fclose(TempArq);

  remove("Farmacos.dat");
  rename("temp.dat", "Farmacos.dat");
  ArqFarma = fopen("Farmacos.dat", "r+b");

  if (!encontrado) {
    printf("Erro: O fármaco '%s' de modelo '%s' não foi encontrado no arquivo.\n", Farmaco, Modelo);
  } else {
    printf("\nFármaco reativado com sucesso.\n");
  }
}

void Consultar() {
  printf("*** consulta ***\n\n");
  printf("Qual farmaco? ");
  char Farmaco[21];
  scanf("%s", Farmaco);
  printf("Qual o modelo? ");
  char Modelo[21];
  scanf("%s", Modelo);

  fseek(ArqFarma, 0, 0);
  int Achou = 0;
  while (fread(&RgFarma, Tamanho, 1, ArqFarma) == 1) {
    if (strcmp(RgFarma.Nome, Farmaco) == 0 && strcmp(RgFarma.Modelo, Modelo) == 0) {
      Achou = 1;
      if (RgFarma.Ativo == 1) {
        printf("Nome: %s\n", RgFarma.Nome);
        printf("Modelo: %s\n", RgFarma.Modelo);
        printf("Preco: R$%.2f\n", RgFarma.Preco);
        printf("Estoque: %d\n", RgFarma.QEstoque);
      } else {
        printf("O medicamento '%s' de modelo '%s' está inativo.\n", RgFarma.Nome, RgFarma.Modelo);
      }
    }
  }
  if (!Achou) {
    printf("Registro inexistente!");
  }
}

void LTodos() {
  printf("*** lista todos ***\n\n");
  fseek(ArqFarma, 0, 0);
  while (fread(&RgFarma, Tamanho, 1, ArqFarma) == 1) {
    if (RgFarma.Ativo == 1) {
      printf("Nome: %s\n", RgFarma.Nome);
      printf("Modelo: %s\n", RgFarma.Modelo);
      printf("Preco: R$%.2f\n", RgFarma.Preco);
      printf("Estoque: %d\n", RgFarma.QEstoque);
      printf("***\n\n");
    }
  }
}