#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "farmacia.h"

TpFARMA RgFarma;
long int Tamanho = sizeof(TpFARMA);

static void ler_texto(char *destino, int tamanho) {
  scanf(" %49[^\n]", destino);
}

int FarmacoJaExiste(const char *nomeFarmaco, const char *modeloFarmaco) {
  FILE *arquivo = fopen(DB_FILE, "rb");
  if (arquivo == NULL) {
    return 0;
  }

  TpFARMA farmacoExistente;
  while (fread(&farmacoExistente, sizeof(TpFARMA), 1, arquivo) == 1) {
    if (strcasecmp(farmacoExistente.Nome, nomeFarmaco) == 0 &&
        strcasecmp(farmacoExistente.Modelo, modeloFarmaco) == 0) {
      fclose(arquivo);
      return 1;
    }
  }

  fclose(arquivo);
  return 0;
}

// Camada de API (Usada pelo servidor web)

// Retornos: 0 = sucesso, 1 = já existe, -1 = erro de arquivo
int api_incluir(const char *nome, const char *modelo, float preco, int estoque) {
  if (FarmacoJaExiste(nome, modelo)) {
    return 1;
  }

  TpFARMA novo;
  memset(&novo, 0, sizeof(TpFARMA));
  strncpy(novo.Nome, nome, sizeof(novo.Nome) - 1);
  strncpy(novo.Modelo, modelo, sizeof(novo.Modelo) - 1);
  novo.Preco = preco;
  novo.QEstoque = estoque;
  novo.Ativo = 1;
  novo.Qtd = 0;

  FILE *arquivo = fopen(DB_FILE, "ab");
  if (arquivo == NULL) {
    return -1;
  }
  fwrite(&novo, sizeof(TpFARMA), 1, arquivo);
  fclose(arquivo);
  return 0;
}

// Retornos: 0 = excluído, 1 = não encontrado, -1 = erro
int api_excluir(const char *nome, const char *modelo) {
  FILE *arquivo = fopen(DB_FILE, "rb");
  if (arquivo == NULL) return 1;

  FILE *tempArq = fopen("temp.dat", "wb");
  if (tempArq == NULL) {
    fclose(arquivo);
    return -1;
  }

  int encontrado = 0;
  TpFARMA item;
  while (fread(&item, sizeof(TpFARMA), 1, arquivo) == 1) {
    if (strcasecmp(item.Nome, nome) == 0 && strcasecmp(item.Modelo, modelo) == 0) {
      encontrado = 1;
    } else {
      fwrite(&item, sizeof(TpFARMA), 1, tempArq);
    }
  }

  fclose(arquivo);
  fclose(tempArq);
  remove(DB_FILE);
  rename("temp.dat", DB_FILE);
  return encontrado ? 0 : 1;
}

// Retornos: 0 = sucesso, 1 = não encontrado/inativo, 2 = estoque insuficiente, 3 = valor pago insuficiente, -1 = erro
int api_vender(const char *nome, const char *modelo, int qtd, float valor_pago, float *troco_out) {
  FILE *arquivo = fopen(DB_FILE, "rb");
  if (arquivo == NULL) return 1;

  FILE *tempArq = fopen("temp.dat", "wb");
  if (tempArq == NULL) {
    fclose(arquivo);
    return -1;
  }

  int status = 1;
  TpFARMA item;
  while (fread(&item, sizeof(TpFARMA), 1, arquivo) == 1) {
    if (strcasecmp(item.Nome, nome) == 0 && strcasecmp(item.Modelo, modelo) == 0 && item.Ativo == 1) {
      if (qtd <= 0 || qtd > item.QEstoque) {
        status = 2;
      } else {
        float total = qtd * item.Preco;
        if (valor_pago < total) {
          status = 3;
        } else {
          item.QEstoque -= qtd;
          if (troco_out) *troco_out = valor_pago - total;
          status = 0;
        }
      }
    }
    fwrite(&item, sizeof(TpFARMA), 1, tempArq);
  }

  fclose(arquivo);
  fclose(tempArq);
  remove(DB_FILE);
  rename("temp.dat", DB_FILE);
  return status;
}

// Retornos: 0 = sucesso, 1 = não encontrado, -1 = erro
int api_alterar_status(const char *nome, const char *modelo, int ativo, float novo_preco, int novo_estoque) {
  FILE *arquivo = fopen(DB_FILE, "rb");
  if (arquivo == NULL) return 1;

  FILE *tempArq = fopen("temp.dat", "wb");
  if (tempArq == NULL) {
    fclose(arquivo);
    return -1;
  }

  int encontrado = 0;
  TpFARMA item;
  while (fread(&item, sizeof(TpFARMA), 1, arquivo) == 1) {
    if (strcasecmp(item.Nome, nome) == 0 && strcasecmp(item.Modelo, modelo) == 0) {
      encontrado = 1;
      item.Ativo = ativo ? 1 : 0;
      if (ativo) {
        if (novo_preco > 0) item.Preco = novo_preco;
        if (novo_estoque >= 0) item.QEstoque = novo_estoque;
      }
    }
    fwrite(&item, sizeof(TpFARMA), 1, tempArq);
  }

  fclose(arquivo);
  fclose(tempArq);
  remove(DB_FILE);
  rename("temp.dat", DB_FILE);
  return encontrado ? 0 : 1;
}

int api_listar_todos(TpFARMA *buffer, int max_itens) {
  FILE *arquivo = fopen(DB_FILE, "rb");
  if (arquivo == NULL) return 0;

  int count = 0;
  while (count < max_itens && fread(&buffer[count], sizeof(TpFARMA), 1, arquivo) == 1) {
    count++;
  }
  fclose(arquivo);
  return count;
}

// Camada Interativa (Terminal / CLI)

void Incluir() {
  char R;
  do {
    printf("*** Inclusao ***\n\n");
    printf("Nome: ");
    ler_texto(RgFarma.Nome, sizeof(RgFarma.Nome));
    printf("Modelo: ");
    ler_texto(RgFarma.Modelo, sizeof(RgFarma.Modelo));

    printf("Preco: ");
    scanf("%f", &RgFarma.Preco);
    printf("Estoque: ");
    scanf("%d", &RgFarma.QEstoque);

    int res = api_incluir(RgFarma.Nome, RgFarma.Modelo, RgFarma.Preco, RgFarma.QEstoque);
    if (res == 1) {
      printf("O farmaco '%s %s' ja existe (verifique se esta inativo).\n", RgFarma.Nome, RgFarma.Modelo);
    } else if (res == 0) {
      printf("Farmaco cadastrado com sucesso!\n");
    } else {
      printf("Erro ao abrir arquivo para inclusao.\n");
    }

    printf("\nNova inclusao? S/N ");
    scanf(" %c", &R);
    R = toupper(R);
  } while (R != 'N');
}

void Excluir() {
  char farmaco[50], modelo[50];
  printf("*** Exclusao ***\n\n");
  printf("Qual farmaco deseja excluir? ");
  ler_texto(farmaco, sizeof(farmaco));
  printf("Qual o modelo do farmaco? ");
  ler_texto(modelo, sizeof(modelo));

  if (api_excluir(farmaco, modelo) == 0) {
    printf("Medicamento excluido com sucesso!\n");
  } else {
    printf("Medicamento nao encontrado.\n");
  }
}

void Venda() {
  char farmaco[50], modelo[50];
  int quantidadeDesejada;
  float valorPago, troco = 0.0f;

  printf("*** Venda ***\n\n");
  printf("Qual farmaco deseja vender? ");
  ler_texto(farmaco, sizeof(farmaco));
  printf("Qual o modelo do farmaco? ");
  ler_texto(modelo, sizeof(modelo));
  printf("Informe a quantidade desejada: ");
  scanf("%d", &quantidadeDesejada);
  printf("Valor pago: R$ ");
  scanf("%f", &valorPago);

  int res = api_vender(farmaco, modelo, quantidadeDesejada, valorPago, &troco);
  if (res == 0) {
    printf("Venda realizada com sucesso! Troco: R$ %.2f\n", troco);
  } else if (res == 2) {
    printf("Estoque insuficiente ou quantidade invalida!\n");
  } else if (res == 3) {
    printf("Valor pago insuficiente para a compra!\n");
  } else {
    printf("Medicamento nao encontrado ou inativo.\n");
  }
}

void Desativar() {
  char farmaco[50], modelo[50];
  printf("*** Desativar ***\n\n");
  printf("Qual farmaco deseja desativar? ");
  ler_texto(farmaco, sizeof(farmaco));
  printf("Qual o modelo? ");
  ler_texto(modelo, sizeof(modelo));

  if (api_alterar_status(farmaco, modelo, 0, -1, -1) == 0) {
    printf("Farmaco '%s %s' marcado como inativo.\n", farmaco, modelo);
  } else {
    printf("Medicamento nao encontrado.\n");
  }
}

void Ativar() {
  char farmaco[50], modelo[50];
  float preco;
  int estoque;
  printf("*** Reativacao ***\n\n");
  printf("Qual farmaco deseja reativar? ");
  ler_texto(farmaco, sizeof(farmaco));
  printf("Qual o modelo? ");
  ler_texto(modelo, sizeof(modelo));
  printf("Novo preco: ");
  scanf("%f", &preco);
  printf("Novo estoque: ");
  scanf("%d", &estoque);

  if (api_alterar_status(farmaco, modelo, 1, preco, estoque) == 0) {
    printf("Farmaco reativado com sucesso.\n");
  } else {
    printf("Medicamento nao encontrado.\n");
  }
}

void Consultar() {
  char farmaco[50], modelo[50];
  printf("*** Consulta ***\n\n");
  printf("Qual farmaco? ");
  ler_texto(farmaco, sizeof(farmaco));
  printf("Qual o modelo? ");
  ler_texto(modelo, sizeof(modelo));

  FILE *arquivo = fopen(DB_FILE, "rb");
  if (arquivo == NULL) {
    printf("Nenhum farmaco cadastrado.\n");
    return;
  }

  int achou = 0;
  while (fread(&RgFarma, Tamanho, 1, arquivo) == 1) {
    if (strcasecmp(RgFarma.Nome, farmaco) == 0 && strcasecmp(RgFarma.Modelo, modelo) == 0) {
      achou = 1;
      printf("\n--- DADOS DO FARMACO ---\n");
      printf("Nome: %s\n", RgFarma.Nome);
      printf("Modelo: %s\n", RgFarma.Modelo);
      printf("Preco: R$ %.2f\n", RgFarma.Preco);
      printf("Estoque: %d\n", RgFarma.QEstoque);
      printf("Status: %s\n", RgFarma.Ativo ? "Ativo" : "Inativo");
      break;
    }
  }
  fclose(arquivo);
  if (!achou) printf("Registro inexistente!\n");
}

void LTodos() {
  printf("*** Lista Todos (Ativos) ***\n\n");
  FILE *arquivo = fopen(DB_FILE, "rb");
  if (arquivo == NULL) {
    printf("Nenhum farmaco cadastrado.\n");
    return;
  }

  int encontrou = 0;
  while (fread(&RgFarma, Tamanho, 1, arquivo) == 1) {
    if (RgFarma.Ativo == 1) {
      encontrou = 1;
      printf("Nome: %s | Modelo: %s | Preco: R$ %.2f | Estoque: %d\n",
             RgFarma.Nome, RgFarma.Modelo, RgFarma.Preco, RgFarma.QEstoque);
    }
  }
  fclose(arquivo);
  if (!encontrou) printf("Nenhum farmaco ativo para listar.\n");
}

long int TArquivo() {
  FILE *arquivo = fopen(DB_FILE, "rb");
  if (arquivo == NULL) return 0;
  fseek(arquivo, 0, SEEK_END);
  long int total_bytes = ftell(arquivo);
  fclose(arquivo);
  return total_bytes / Tamanho;
}