# Sistema de Gestão de Farmácia

Sistema de controle de estoque, cadastro de medicamentos e registro de vendas desenvolvido com **núcleo de persistência em C** (manipulação direta de arquivos binários `struct` / `.dat`) e **interface web** integrada via Foreign Function Interface (`ctypes`).

## Arquitetura do Projeto

O projeto mantém toda a regra de negócio e persistência binária em C, expondo duas formas de execução sobre a mesma base de dados (`Farmacos.dat`):

1. **Modo Web (Interface Gráfica):** O código C é compilado como biblioteca compartilhada (`libfarmacia.so`) e consumido por um servidor HTTP leve em Python (biblioteca padrão), servindo uma interface SPA em HTML/CSS/JS.
2. **Modo CLI (Terminal):** Executável nativo em C com menu interativo via console.

```text
ProjetoFarmacia/
├── backend/
│   ├── include/
│   │   └── farmacia.h       # Definição da struct TpFARMA e contratos da API
│   ├── src/
│   │   ├── farmacia.c       # Persistência binária (.dat), regras de venda e funções API
│   │   └── main.c           # Interface interativa de linha de comando (CLI)
│   └── Makefile             # Build automatizado (executável CLI + biblioteca .so)
├── web/
│   ├── server.py            # Ponte HTTP/JSON <-> C (via ctypes, sem dependências externas)
│   └── static/
│       ├── index.html       # Interface web de gestão
│       ├── style.css        # Estilização responsiva
│       └── app.js           # Consumo da API REST e atualização dinâmica
├── .gitignore
└── README.md
```

## Funcionalidades

- **Cadastro de Fármacos:** Inclusão de medicamentos com validação de duplicidade (nome + modelo/apresentação).
- **Registro de Vendas:** Baixa automática no estoque, validação de saldo disponível, cálculo prévio do total e cálculo automático de troco.
- **Gestão de Ciclo de Vida:** Ativação e desativação lógica de produtos (com modal para atualização de preço e estoque na reativação) e exclusão física do arquivo binário.
- **Consulta e Filtragem:** Busca dinâmica por nome ou modelo e filtro entre itens ativos e inativos.

## Pré-requisitos (O que precisa para rodar)

Para compilar o núcleo em C e executar o servidor web, você precisa ter instalado:

- **Compilador C (`gcc`)** — via GCC nativo no Linux/WSL ou MinGW/MSYS2 no Windows.
- **Make** *(opcional no Windows)* — utilizado no Linux/WSL para automatizar a compilação pelo `Makefile`.
- **Python 3.8 ou superior** — utiliza apenas módulos da biblioteca padrão (`ctypes`, `http.server`, `json`, `pathlib`, `platform`), **sem necessidade de instalar pacotes via `pip`**.

## Como Compilar e Executar

### Opção 1: Linux ou WSL (Ubuntu)

#### 1. Clonar o repositório
```bash
git clone https://github.com/Valterjr14/ProjetoFarmacia.git
cd ProjetoFarmacia
```

#### 2. Compilar o Backend em C
```bash
cd backend
make
cd ..
```
Isso gerará os arquivos `backend/build/farmacia_cli` (executável de terminal) e `backend/build/libfarmacia.so` (biblioteca compartilhada para a web).

#### 3. Iniciar a Interface Web
Na raiz do projeto (`ProjetoFarmacia/`), execute:
```bash
python3 web/server.py
```
Abra o navegador e acesse: **`http://localhost:8000`**

*(Para encerrar o servidor, pressione `Ctrl + C` no terminal.)*

---

### Opção 2: Windows Nativo (PowerShell + MinGW GCC)

#### 1. Clonar o repositório
```powershell
git clone https://github.com/Valterjr14/ProjetoFarmacia.git
cd ProjetoFarmacia
```

#### 2. Compilar o Backend em C
```powershell
cd backend
mkdir build -Force
gcc -Iinclude -Wall -Wextra -g -o build/farmacia_cli.exe src/main.c src/farmacia.c
gcc -Iinclude -Wall -Wextra -g -shared -o build/libfarmacia.dll src/farmacia.c
cd ..
```

#### 3. Iniciar a Interface Web
Na raiz do projeto (`ProjetoFarmacia/`), execute:
```powershell
python web/server.py
```
Abra o navegador e acesse: **`http://localhost:8000`**

*(Para encerrar o servidor, pressione `Ctrl + C` no terminal.)*

## Persistência de Dados

Os registros são armazenados localmente no arquivo binário **`Farmacos.dat`**, gerado automaticamente na raiz do projeto na primeira inclusão. Como tanto o servidor web quanto o executável CLI apontam para o mesmo arquivo, alterações feitas em uma interface refletem imediatamente na outra.
