let farmacosCache = [];
let itemParaExcluir = null;

function showToast(msg, isError = false) {
  const el = document.getElementById("toast");
  el.textContent = msg;
  el.className = `toast ${isError ? "err" : "ok"}`;
  setTimeout(() => el.classList.add("hidden"), 4500);
}

function formatarMoeda(valor) {
  return valor.toLocaleString("pt-BR", { style: "currency", currency: "BRL" });
}

function abrirModal(id) {
  document.getElementById(id).classList.remove("hidden");
}

function fecharModal(id) {
  document.getElementById(id).classList.add("hidden");
}

async function carregarFarmacos() {
  const res = await fetch("/api/farmacos");
  const data = await res.json();
  farmacosCache = data.farmacos || [];
  renderizarTabela();
  atualizarSelectVenda();
}

function atualizarSelectVenda() {
  const select = document.getElementById("venda-select");
  const ativos = farmacosCache.filter((f) => f.ativo && f.estoque > 0);
  select.innerHTML = '<option value="">Selecione um item...</option>';
  ativos.forEach((f) => {
    const opt = document.createElement("option");
    opt.value = JSON.stringify({ nome: f.nome, modelo: f.modelo, preco: f.preco });
    opt.textContent = `${f.nome} (${f.modelo}) — ${formatarMoeda(f.preco)} [${f.estoque} un.]`;
    select.appendChild(opt);
  });
  atualizarPreviewVenda();
}

function atualizarPreviewVenda() {
  const select = document.getElementById("venda-select");
  const qtd = parseInt(document.getElementById("venda-qtd").value || "0", 10);
  const totalEl = document.getElementById("venda-total");

  if (!select.value || qtd <= 0) {
    totalEl.textContent = "R$ 0,00";
    return;
  }
  const item = JSON.parse(select.value);
  totalEl.textContent = formatarMoeda(item.preco * qtd);
}

function renderizarTabela() {
  const tbody = document.getElementById("tabela-corpo");
  const termo = document.getElementById("busca").value.toLowerCase();
  const mostrarInativos = document.getElementById("mostrar-inativos").checked;

  const filtrados = farmacosCache.filter((f) => {
    if (!mostrarInativos && !f.ativo) return false;
    return (
      f.nome.toLowerCase().includes(termo) ||
      f.modelo.toLowerCase().includes(termo)
    );
  });

  if (filtrados.length === 0) {
    tbody.innerHTML = '<tr><td colspan="6" class="empty">Nenhum fármaco encontrado.</td></tr>';
    return;
  }

  tbody.innerHTML = "";
  filtrados.forEach((f) => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td><strong>${f.nome}</strong></td>
      <td>${f.modelo}</td>
      <td>${formatarMoeda(f.preco)}</td>
      <td>${f.estoque}</td>
      <td><span class="pill ${f.ativo ? "ativo" : "inativo"}">${f.ativo ? "Ativo" : "Inativo"}</span></td>
      <td class="actions">
        ${
          f.ativo
            ? `<button class="btn-sm warn" onclick="alterarStatus('${f.nome}', '${f.modelo}', false)">Desativar</button>`
            : `<button class="btn-sm ok" onclick="abrirModalReativar('${f.nome}', '${f.modelo}', ${f.preco}, ${f.estoque})">Reativar</button>`
        }
        <button class="btn-sm del" onclick="abrirModalExcluir('${f.nome}', '${f.modelo}')">Excluir</button>
      </td>
    `;
    tbody.appendChild(tr);
  });
}

document.getElementById("form-cadastro").addEventListener("submit", async (e) => {
  e.preventDefault();
  const payload = {
    nome: document.getElementById("cad-nome").value,
    modelo: document.getElementById("cad-modelo").value,
    preco: parseFloat(document.getElementById("cad-preco").value),
    estoque: parseInt(document.getElementById("cad-estoque").value, 10),
  };

  const res = await fetch("/api/farmacos", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload),
  });
  const data = await res.json();
  if (!res.ok) return showToast(data.erro || "Erro ao cadastrar", true);

  showToast(data.mensagem);
  e.target.reset();
  carregarFarmacos();
});

document.getElementById("form-venda").addEventListener("submit", async (e) => {
  e.preventDefault();
  const selectVal = document.getElementById("venda-select").value;
  if (!selectVal) return showToast("Selecione um fármaco para vender.", true);

  const item = JSON.parse(selectVal);
  const payload = {
    nome: item.nome,
    modelo: item.modelo,
    quantidade: parseInt(document.getElementById("venda-qtd").value, 10),
    valor_pago: parseFloat(document.getElementById("venda-pago").value),
  };

  const res = await fetch("/api/vendas", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload),
  });
  const data = await res.json();
  if (!res.ok) return showToast(data.erro || "Erro na venda", true);

  showToast(`${data.mensagem} Troco: ${formatarMoeda(data.troco)}`);
  e.target.reset();
  carregarFarmacos();
});

async function alterarStatus(nome, modelo, ativo, preco = -1, estoque = -1) {
  const res = await fetch("/api/status", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ nome, modelo, ativo, preco, estoque }),
  });
  const data = await res.json();
  if (!res.ok) return showToast(data.erro || "Erro ao alterar status", true);
  showToast(data.mensagem);
  carregarFarmacos();
}

// Reativação via Modal Estilizado
function abrirModalReativar(nome, modelo, precoAtual, estoqueAtual) {
  document.getElementById("reativar-nome").value = nome;
  document.getElementById("reativar-modelo").value = modelo;
  document.getElementById("reativar-subtitulo").innerHTML =
    `Atualize os dados de <strong>${nome} (${modelo})</strong> para colocá-lo novamente à venda.`;
  document.getElementById("reativar-preco").value = precoAtual.toFixed(2);
  document.getElementById("reativar-estoque").value = estoqueAtual;
  abrirModal("modal-reativar");
  document.getElementById("reativar-preco").focus();
}

document.getElementById("form-reativar").addEventListener("submit", async (e) => {
  e.preventDefault();
  const nome = document.getElementById("reativar-nome").value;
  const modelo = document.getElementById("reativar-modelo").value;
  const novoPreco = parseFloat(document.getElementById("reativar-preco").value);
  const novoEstoque = parseInt(document.getElementById("reativar-estoque").value, 10);

  fecharModal("modal-reativar");
  await alterarStatus(nome, modelo, true, novoPreco, novoEstoque);
});

// Exclusão via Modal Estilizado
function abrirModalExcluir(nome, modelo) {
  itemParaExcluir = { nome, modelo };
  document.getElementById("excluir-texto").innerHTML =
    `Tem certeza que deseja excluir permanentemente <strong>${nome} (${modelo})</strong> do arquivo binário? Essa ação não pode ser desfeita.`;
  abrirModal("modal-excluir");
}

document.getElementById("btn-confirmar-exclusao").addEventListener("click", async () => {
  if (!itemParaExcluir) return;
  const { nome, modelo } = itemParaExcluir;
  fecharModal("modal-excluir");

  const res = await fetch("/api/farmacos", {
    method: "DELETE",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ nome, modelo }),
  });
  const data = await res.json();
  if (!res.ok) return showToast(data.erro || "Erro ao excluir", true);
  showToast(data.mensagem);
  carregarFarmacos();
});

// Fechar modal ao clicar fora ou apertar ESC
document.querySelectorAll(".modal-backdrop").forEach((backdrop) => {
  backdrop.addEventListener("click", (e) => {
    if (e.target === backdrop) backdrop.classList.add("hidden");
  });
});

document.addEventListener("keydown", (e) => {
  if (e.key === "Escape") {
    fecharModal("modal-reativar");
    fecharModal("modal-excluir");
  }
});

document.getElementById("venda-select").addEventListener("change", atualizarPreviewVenda);
document.getElementById("venda-qtd").addEventListener("input", atualizarPreviewVenda);
document.getElementById("busca").addEventListener("input", renderizarTabela);
document.getElementById("mostrar-inativos").addEventListener("change", renderizarTabela);

carregarFarmacos();