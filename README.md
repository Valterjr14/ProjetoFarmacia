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
