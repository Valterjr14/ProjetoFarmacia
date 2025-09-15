#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "farmacia.h"

TpFARMA RgFarma;
long int Tamanho = sizeof(TpFARMA);

// Função auxiliar para verificar se um fármaco já existe no arquivo
int FarmacoJaExiste(const char *nomeFarmaco, const char *modeloFarmaco) {
  FILE *arquivo = fopen("Farmacos.dat", "rb"); // Abre para leitura binária
  if (arquivo == NULL) {
    // Se o arquivo não existe, o fármaco também não.
    return 0;
  }

  struct TpFarma farmacoExistente;
  while (fread(&farmacoExistente, sizeof(struct TpFarma), 1, arquivo) == 1) {
    if (strcmp(farmacoExistente.Nome, nomeFarmaco) == 0 && strcmp(farmacoExistente.Modelo, modeloFarmaco) == 0) {
      fclose(arquivo);
      return 1; // O fármaco já existe
    }
  }

  fclose(arquivo);
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
    } else {
        printf("Preco: ");
        scanf("%f", &RgFarma.Preco);
        printf("Estoque: ");
        scanf("%d", &RgFarma.QEstoque);
        RgFarma.Ativo = 1; // Definindo como ativo por padrão

        // Abre o arquivo para adicionar no final (append) e o fecha em seguida.
        FILE *arquivo = fopen("Farmacos.dat", "ab"); // "ab" para append binário
        if (arquivo == NULL) {
          printf("Erro ao abrir arquivo para inclusao.");
          return;
        }
        fwrite(&RgFarma, Tamanho, 1, arquivo);
        fclose(arquivo);
    }

    printf("\nNova inclusao? S/N ");
    scanf(" %c", &R);
    R = toupper(R);
  } while (R != 'N');
}

void Excluir() {
  char farmaco[21];
  char modelo[21];
  int encontrado = 0;

  printf("*** exclusão ***\n\n");
  printf("Qual farmaco deseja excluir? ");
  scanf("%s", farmaco);
  printf("Qual o modelo do farmaco? ");
  scanf("%s", modelo);

  FILE *arquivo = fopen("Farmacos.dat", "rb");
  if (arquivo == NULL) {
    printf("Erro: Banco de dados 'Farmacos.dat' nao encontrado.\n");
    return;
  }

  FILE *tempArq = fopen("temp.dat", "wb");
  if (tempArq == NULL) {
    printf("Erro ao criar arquivo temporario.\n");
    fclose(arquivo);
    return;
  }

  while (fread(&RgFarma, sizeof(struct TpFarma), 1, arquivo) == 1) {
    if (strcmp(RgFarma.Nome, farmaco) == 0 && strcmp(RgFarma.Modelo, modelo) == 0) {
      encontrado = 1;
    } else {
      fwrite(&RgFarma, sizeof(struct TpFarma), 1, tempArq);
    }
  }

  fclose(arquivo);
  fclose(tempArq);

  remove("Farmacos.dat");
  rename("temp.dat", "Farmacos.dat");

  if (encontrado) {
    printf("Medicamento excluido com sucesso!\n");
  } else {
    printf("Medicamento nao encontrado.\n");
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

  FILE *arquivo = fopen("Farmacos.dat", "rb");
  if (arquivo == NULL) {
    printf("Erro: Banco de dados 'Farmacos.dat' nao encontrado.\n");
    return;
  }

  FILE *tempArq = fopen("temp.dat", "wb");
    printf("Erro ao abrir arquivo temporário.");
    fclose(arquivo);
    return;
  }

  while (fread(&RgFarma, sizeof(struct TpFarma), 1, arquivo) == 1) {
    if (strcmp(RgFarma.Nome, farmaco) == 0 && strcmp(RgFarma.Modelo, modelo) == 0 && RgFarma.Ativo == 1) {
      encontrado = 1;

      printf("Informe a quantidade desejada: ");
      scanf("%d", &quantidadeDesejada);

      if (quantidadeDesejada > RgFarma.QEstoque) {
        printf("Estoque insuficiente! Disponível: %d\n", RgFarma.QEstoque);
      } else {
        totalVenda = quantidadeDesejada * RgFarma.Preco;
        printf("Total da venda: R$ %.2f\n", totalVenda);
        printf("Valor pago: ");
        scanf("%f", &valorPago);

        if (valorPago < totalVenda) {
          printf("Valor insuficiente para a compra!\n");
        } else {
          troco = valorPago - totalVenda;
          RgFarma.QEstoque -= quantidadeDesejada;
          printf("Venda realizada com sucesso! Troco: R$ %.2f\n", troco);
          printf("Estoque atualizado para: %d\n", RgFarma.QEstoque);
        }
      }
    }
    fwrite(&RgFarma, sizeof(struct TpFarma), 1, tempArq);
  }

  if (!encontrado) {
    printf("Medicamento nao encontrado ou inativo.\n");
  }

  fclose(arquivo);
  fclose(tempArq);

  remove("Farmacos.dat");
  rename("temp.dat", "Farmacos.dat");
}

void Desativar() {
  printf("*** desativar ***\n\n");
  printf("Qual farmaco deseja desativar? ");
  char farmaco[21];
  scanf("%s", farmaco);
  printf("Qual o modelo do farmaco que deseja desativar? ");
  char modelo[21];
  scanf("%s", modelo);

  FILE *arquivo = fopen("Farmacos.dat", "rb");
  if (arquivo == NULL) {
    printf("Erro: Banco de dados 'Farmacos.dat' nao encontrado.\n");
    return;
  }

  FILE *tempArq = fopen("temp.dat", "wb");
  if (tempArq == NULL) {
    printf("Erro ao abrir arquivo temporário.");
    fclose(arquivo);
    return;
  }

  int encontrado = 0;
  while (fread(&RgFarma, Tamanho, 1, arquivo) == 1) {
    if (strcmp(RgFarma.Nome, farmaco) == 0 && strcmp(RgFarma.Modelo, modelo) == 0) {
      encontrado = 1;
      RgFarma.Ativo = 0; // Marca como inativo
      printf("\nFármaco '%s %s' marcado como inativo.\n", farmaco, modelo);
    }
    fwrite(&RgFarma, Tamanho, 1, tempArq);
  }

  fclose(arquivo);
  fclose(tempArq);

  remove("Farmacos.dat");
  rename("temp.dat", "Farmacos.dat");

  if (!encontrado) {
    printf("Medicamento nao encontrado.\n");
  }
}

void Ativar() {
  printf("*** reativação ***\n\n");
  printf("Qual farmaco deseja reativar? ");
  char farmaco[21];
  scanf("%s", farmaco);
  printf("Qual o modelo do farmaco que deseja reativar? ");
  char modelo[21];
  scanf("%s", modelo);

  FILE *arquivo = fopen("Farmacos.dat", "rb");
  if (arquivo == NULL) {
    printf("Erro: Banco de dados 'Farmacos.dat' nao encontrado.\n");
    return;
  }

  FILE *tempArq = fopen("temp.dat", "wb");
  if (tempArq == NULL) {
    printf("Erro ao abrir arquivo temporário.");
    fclose(arquivo);
    return;
  }

  int encontrado = 0;
  while (fread(&RgFarma, Tamanho, 1, arquivo) == 1) {
    if (strcmp(RgFarma.Nome, farmaco) == 0 && strcmp(RgFarma.Modelo, modelo) == 0) {
      encontrado = 1;
      if (RgFarma.Ativo == 1) {
        printf("Erro: O fármaco '%s %s' já está ativo.\n", farmaco, modelo);
      } else {
        printf("Qual é o preço atualizado? ");
        scanf("%f", &RgFarma.Preco);
        printf("Qual é o novo estoque? ");
        scanf("%d", &RgFarma.QEstoque);
        RgFarma.Ativo = 1; // Reativa
        printf("\nFármaco reativado com sucesso.\n");
      }
    }
    fwrite(&RgFarma, Tamanho, 1, tempArq);
  }

  fclose(arquivo);
  fclose(tempArq);

  remove("Farmacos.dat");
  rename("temp.dat", "Farmacos.dat");

  if (!encontrado) {
    printf("Erro: O fármaco '%s %s' não foi encontrado no arquivo.\n", farmaco, modelo);
  }
}

void Consultar() {
  printf("*** consulta ***\n\n");
  printf("Qual farmaco? ");
  char farmaco[21];
  scanf("%s", farmaco);
  printf("Qual o modelo? ");
  char modelo[21];
  scanf("%s", modelo);

  FILE *arquivo = fopen("Farmacos.dat", "rb");
  if (arquivo == NULL) {
    printf("Nenhum farmaco cadastrado.\n");
    return;
  }

  int achou = 0;
  while (fread(&RgFarma, Tamanho, 1, arquivo) == 1) {
    if (strcmp(RgFarma.Nome, farmaco) == 0 && strcmp(RgFarma.Modelo, modelo) == 0) {
      achou = 1;
      if (RgFarma.Ativo == 1) {
        printf("\n--- DADOS DO FARMACO ---\n");
        printf("Nome: %s\n", RgFarma.Nome);
        printf("Modelo: %s\n", RgFarma.Modelo);
        printf("Preco: R$%.2f\n", RgFarma.Preco);
        printf("Estoque: %d\n", RgFarma.QEstoque);
      } else {
        printf("O medicamento '%s %s' está inativo.\n", RgFarma.Nome, RgFarma.Modelo);
      }
      break;
    }
  }
  fclose(arquivo);

  if (!achou) {
    printf("Registro inexistente!\n");
  }
}

void LTodos() {
  printf("*** lista todos ***\n\n");

  FILE *arquivo = fopen("Farmacos.dat", "rb");
  if (arquivo == NULL) {
    printf("Nenhum farmaco cadastrado.\n");
    return;
  }
  
  int encontrou_algum = 0;
  while (fread(&RgFarma, Tamanho, 1, arquivo) == 1) {
    if (RgFarma.Ativo == 1) {
      encontrou_algum = 1;
      printf("Nome: %s\n", RgFarma.Nome);
      printf("Modelo: %s\n", RgFarma.Modelo);
      printf("Preco: R$%.2f\n", RgFarma.Preco);
      printf("Estoque: %d\n", RgFarma.QEstoque);
      printf("************************\n\n");
    }
  }
  fclose(arquivo);
  
  if (!encontrou_algum) {
      printf("Nenhum farmaco ativo para listar.\n");
  }
}

long int TArquivo() {
  FILE *arquivo = fopen("Farmacos.dat", "rb");
  if (arquivo == NULL) {
    return 0;
  }
  fseek(arquivo, 0, SEEK_END);
  long int total_bytes = ftell(arquivo);
  fclose(arquivo);
  return total_bytes / Tamanho;
}