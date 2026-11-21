import ctypes
import json
import os
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent.parent
import platform

BASE_DIR = Path(__file__).resolve().parent.parent
EXT = "dll" if platform.system() == "Windows" else "so"
LIB_PATH = BASE_DIR / "backend" / "build" / f"libfarmacia.{EXT}"
STATIC_DIR = BASE_DIR / "web" / "static"
STATIC_DIR = BASE_DIR / "web" / "static"

if not LIB_PATH.exists():
    raise FileNotFoundError(
        f"Biblioteca {LIB_PATH} nao encontrada. Rode 'make' dentro de backend/ primeiro."
    )

lib = ctypes.CDLL(str(LIB_PATH))


class TpFARMA(ctypes.Structure):
    _fields_ = [
        ("Nome", ctypes.c_char * 50),
        ("Modelo", ctypes.c_char * 50),
        ("Preco", ctypes.c_float),
        ("QEstoque", ctypes.c_int),
        ("Ativo", ctypes.c_int),
        ("Qtd", ctypes.c_int),
    ]


lib.api_incluir.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_float, ctypes.c_int]
lib.api_incluir.restype = ctypes.c_int

lib.api_excluir.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.api_excluir.restype = ctypes.c_int

lib.api_vender.argtypes = [
    ctypes.c_char_p,
    ctypes.c_char_p,
    ctypes.c_int,
    ctypes.c_float,
    ctypes.POINTER(ctypes.c_float),
]
lib.api_vender.restype = ctypes.c_int

lib.api_alterar_status.argtypes = [
    ctypes.c_char_p,
    ctypes.c_char_p,
    ctypes.c_int,
    ctypes.c_float,
    ctypes.c_int,
]
lib.api_alterar_status.restype = ctypes.c_int

lib.api_listar_todos.argtypes = [ctypes.POINTER(TpFARMA), ctypes.c_int]
lib.api_listar_todos.restype = ctypes.c_int


class FarmaciaHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(STATIC_DIR), **kwargs)

    def _send_json(self, status_code, payload):
        body = json.dumps(payload, ensure_ascii=False).encode("utf-8")
        self.send_response(status_code)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def _read_json(self):
        length = int(self.headers.get("Content-Length", 0))
        if length == 0:
            return {}
        return json.loads(self.rfile.read(length).decode("utf-8"))

    def do_GET(self):
        if self.path == "/api/farmacos":
            max_itens = 500
            buffer = (TpFARMA * max_itens)()
            total = lib.api_listar_todos(buffer, max_itens)
            itens = []
            for i in range(total):
                itens.append({
                    "nome": buffer[i].Nome.decode("utf-8", errors="ignore"),
                    "modelo": buffer[i].Modelo.decode("utf-8", errors="ignore"),
                    "preco": round(buffer[i].Preco, 2),
                    "estoque": buffer[i].QEstoque,
                    "ativo": bool(buffer[i].Ativo),
                })
            return self._send_json(200, {"farmacos": itens})

        return super().do_GET()

    def do_POST(self):
        data = self._read_json()

        if self.path == "/api/farmacos":
            nome = data.get("nome", "").strip().encode("utf-8")
            modelo = data.get("modelo", "").strip().encode("utf-8")
            preco = float(data.get("preco", 0))
            estoque = int(data.get("estoque", 0))

            if not nome or not modelo or preco <= 0 or estoque < 0:
                return self._send_json(400, {"erro": "Dados invalidos."})

            res = lib.api_incluir(nome, modelo, preco, estoque)
            if res == 0:
                return self._send_json(201, {"mensagem": "Farmaco cadastrado com sucesso."})
            if res == 1:
                return self._send_json(409, {"erro": "Farmaco ja cadastrado (verifique os inativos)."})
            return self._send_json(500, {"erro": "Erro interno ao gravar arquivo."})

        if self.path == "/api/vendas":
            nome = data.get("nome", "").strip().encode("utf-8")
            modelo = data.get("modelo", "").strip().encode("utf-8")
            qtd = int(data.get("quantidade", 0))
            valor_pago = float(data.get("valor_pago", 0))
            troco = ctypes.c_float(0.0)

            res = lib.api_vender(nome, modelo, qtd, valor_pago, ctypes.byref(troco))
            if res == 0:
                return self._send_json(200, {
                    "mensagem": "Venda realizada com sucesso!",
                    "troco": round(troco.value, 2),
                })
            if res == 2:
                return self._send_json(400, {"erro": "Estoque insuficiente ou quantidade invalida."})
            if res == 3:
                return self._send_json(400, {"erro": "Valor pago menor que o total da venda."})
            return self._send_json(404, {"erro": "Farmaco nao encontrado ou inativo."})

        if self.path == "/api/status":
            nome = data.get("nome", "").strip().encode("utf-8")
            modelo = data.get("modelo", "").strip().encode("utf-8")
            ativo = 1 if data.get("ativo") else 0
            preco = float(data.get("preco", -1))
            estoque = int(data.get("estoque", -1))

            res = lib.api_alterar_status(nome, modelo, ativo, preco, estoque)
            if res == 0:
                return self._send_json(200, {"mensagem": "Status atualizado com sucesso."})
            return self._send_json(404, {"erro": "Farmaco nao encontrado."})

        return self._send_json(404, {"erro": "Rota nao encontrada."})

    def do_DELETE(self):
        data = self._read_json()
        if self.path == "/api/farmacos":
            nome = data.get("nome", "").strip().encode("utf-8")
            modelo = data.get("modelo", "").strip().encode("utf-8")
            res = lib.api_excluir(nome, modelo)
            if res == 0:
                return self._send_json(200, {"mensagem": "Farmaco excluido permanentemente."})
            return self._send_json(404, {"erro": "Farmaco nao encontrado."})

        return self._send_json(404, {"erro": "Rota nao encontrada."})


if __name__ == "__main__":
    os.chdir(BASE_DIR)
    port = int(os.environ.get("PORT", 8000))
    server = ThreadingHTTPServer(("0.0.0.0", port), FarmaciaHandler)
    print(f"Servidor Pague Pouco rodando em http://localhost:{port}")
    server.serve_forever()