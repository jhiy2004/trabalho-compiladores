import { useMemo, useState } from "react";
import type { SemanticErrorApi, SymbolTableApiEntry, SymbolRow } from "../types";
import { Search, ArrowUpDown, CheckCircle2, AlertTriangle, Table2, ShieldCheck } from "lucide-react";

type Props = {
  symbolTable: SymbolTableApiEntry[];
  semanticErrors: (string | SemanticErrorApi)[];
  fallbackSymbols?: SymbolRow[];
  compiled: boolean;
};

export default function SymbolsAndSemanticView({
  symbolTable,
  semanticErrors,
  fallbackSymbols = [],
  compiled,
}: Props) {
  const [searchTerm, setSearchTerm] = useState("");
  const [sortField, setSortField] = useState<string>("cadeia");
  const [sortDirection, setSortDirection] = useState<"asc" | "desc">("asc");

  const parsedSemanticErrors = useMemo(() => {
    return semanticErrors.map((err) => {
      if (typeof err === "string") {
        return { mensagem: err, linha: 0, col: 0, tipo_erro: 0 };
      }
      return err;
    });
  }, [semanticErrors]);

  const handleSort = (field: string) => {
    if (sortField === field) {
      setSortDirection((prev) => (prev === "asc" ? "desc" : "asc"));
    } else {
      setSortField(field);
      setSortDirection("asc");
    }
  };

  const filteredAndSortedSymbols = useMemo(() => {
    let list = [...symbolTable];

    if (searchTerm.trim()) {
      const lower = searchTerm.toLowerCase();
      list = list.filter(
        (s) =>
          s.cadeia?.toLowerCase().includes(lower) ||
          s.categoria?.toLowerCase().includes(lower) ||
          s.tipo?.toLowerCase().includes(lower) ||
          s.escopo?.toLowerCase().includes(lower)
      );
    }

    list.sort((a: any, b: any) => {
      const aVal = a[sortField] ?? "";
      const bVal = b[sortField] ?? "";
      const comp =
        typeof aVal === "number"
          ? aVal - bVal
          : String(aVal).localeCompare(String(bVal));
      return sortDirection === "asc" ? comp : -comp;
    });

    return list;
  }, [symbolTable, searchTerm, sortField, sortDirection]);

  return (
    <div className="p-6 space-y-6 overflow-y-auto h-full">
      <div className="grid grid-cols-1 sm:grid-cols-3 gap-4">
        <div className="bg-cardSoft rounded-xl2 p-4 shadow-soft border border-black/5 flex items-center gap-3">
          <div className="p-3 bg-tintSoft/80 rounded-xl">
            <ShieldCheck className="w-6 h-6 text-accentStrong" />
          </div>
          <div>
            <div className="text-xs opacity-70 font-medium">Erros Semânticos</div>
            <div className={`text-2xl font-bold ${parsedSemanticErrors.length > 0 ? "text-red-500" : "text-emerald-600"}`}>
              {parsedSemanticErrors.length}
            </div>
          </div>
        </div>

        <div className="bg-cardSoft rounded-xl2 p-4 shadow-soft border border-black/5 flex items-center gap-3">
          <div className="p-3 bg-tintSoft/80 rounded-xl">
            <Table2 className="w-6 h-6 text-accentStrong" />
          </div>
          <div>
            <div className="text-xs opacity-70 font-medium">Total de Símbolos</div>
            <div className="text-2xl font-bold text-gray-800">
              {symbolTable.length > 0 ? symbolTable.length : fallbackSymbols.length}
            </div>
          </div>
        </div>

        <div className="bg-cardSoft rounded-xl2 p-4 shadow-soft border border-black/5 flex items-center gap-3">
          <div className="p-3 bg-tintSoft/80 rounded-xl">
            <CheckCircle2 className="w-6 h-6 text-accentStrong" />
          </div>
          <div>
            <div className="text-xs opacity-70 font-medium">Status Semântico</div>
            <div className={`text-lg font-semibold ${!compiled ? "text-gray-500" : parsedSemanticErrors.length === 0 ? "text-emerald-600" : "text-red-500"}`}>
              {!compiled ? "Aguardando compilação" : parsedSemanticErrors.length === 0 ? "Válido" : "Com erros"}
            </div>
          </div>
        </div>
      </div>

      <div className="bg-cardSoft rounded-xl2 shadow-soft overflow-hidden border border-black/5">
        <div className="px-5 py-4 border-b border-black/5 flex items-center justify-between">
          <div className="flex items-center gap-2 font-semibold text-lg">
            <ShieldCheck className="w-5 h-5 opacity-70" />
            Análise Semântica
          </div>
          <span
            className={`text-xs px-3 py-1 rounded-full font-medium ${
              parsedSemanticErrors.length === 0
                ? "bg-emerald-500/10 text-emerald-600 border border-emerald-500/20"
                : "bg-red-500/10 text-red-600 border border-red-500/20"
            }`}
          >
            {parsedSemanticErrors.length === 0 ? "Sem Erros Semânticos" : `${parsedSemanticErrors.length} erro(s)`}
          </span>
        </div>

        <div className="p-5">
          {!compiled ? (
            <div className="text-sm opacity-60">Compile o código para executar a análise semântica.</div>
          ) : parsedSemanticErrors.length === 0 ? (
            <div className="flex items-center gap-3 text-emerald-600 bg-emerald-500/10 p-4 rounded-xl border border-emerald-500/20 text-sm">
              <CheckCircle2 className="w-5 h-5 flex-shrink-0" />
              <span>O código passou na verificação semântica com sucesso! Nenhuma inconsistência encontrada.</span>
            </div>
          ) : (
            <div className="space-y-3">
              {parsedSemanticErrors.map((err, i) => (
                <div key={i} className="flex items-start gap-3 bg-red-500/10 border border-red-500/20 p-3 rounded-xl text-sm">
                  <AlertTriangle className="w-4 h-4 text-red-500 mt-0.5 flex-shrink-0" />
                  <div className="flex-1">
                    <div className="font-semibold text-red-600">
                      {err.linha > 0 ? `Linha ${err.linha}${err.col > 0 ? `, Coluna ${err.col}` : ""}:` : "Erro Semântico:"}
                    </div>
                    <div className="text-gray-800 font-mono text-xs mt-1">{err.mensagem}</div>
                  </div>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>

      <div className="bg-cardSoft rounded-xl2 shadow-soft overflow-hidden border border-black/5">
        <div className="px-5 py-4 border-b border-black/5 flex flex-col sm:flex-row sm:items-center justify-between gap-3">
          <div className="flex items-center gap-2 font-semibold text-lg">
            <Table2 className="w-5 h-5 opacity-70" />
            Tabela de Símbolos
          </div>

          <div className="relative w-full sm:w-64">
            <Search className="w-4 h-4 absolute left-3 top-1/2 -translate-y-1/2 opacity-50" />
            <input
              type="text"
              placeholder="Buscar símbolo, tipo..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="w-full pl-9 pr-3 py-1.5 text-xs bg-white rounded-lg border border-black/10 focus:outline-none focus:ring-1 focus:ring-accentStrong"
            />
          </div>
        </div>

        <div className="overflow-x-auto">
          {symbolTable.length > 0 ? (
            <table className="w-full text-sm">
              <thead className="bg-tintSoft/60 border-b border-black/5">
                <tr>
                  <th className="p-3 text-left w-12 opacity-70">#</th>
                  <th
                    onClick={() => handleSort("cadeia")}
                    className="p-3 text-left cursor-pointer hover:bg-black/5 select-none"
                  >
                    <div className="flex items-center gap-1">
                      Lexema <ArrowUpDown className="w-3 h-3 opacity-50" />
                    </div>
                  </th>
                  <th
                    onClick={() => handleSort("categoria")}
                    className="p-3 text-left cursor-pointer hover:bg-black/5 select-none"
                  >
                    <div className="flex items-center gap-1">
                      Categoria <ArrowUpDown className="w-3 h-3 opacity-50" />
                    </div>
                  </th>
                  <th
                    onClick={() => handleSort("tipo")}
                    className="p-3 text-left cursor-pointer hover:bg-black/5 select-none"
                  >
                    <div className="flex items-center gap-1">
                      Tipo <ArrowUpDown className="w-3 h-3 opacity-50" />
                    </div>
                  </th>
                  <th
                    onClick={() => handleSort("escopo")}
                    className="p-3 text-left cursor-pointer hover:bg-black/5 select-none"
                  >
                    <div className="flex items-center gap-1">
                      Escopo <ArrowUpDown className="w-3 h-3 opacity-50" />
                    </div>
                  </th>
                  <th className="p-3 text-left">Utilizada</th>
                  <th className="p-3 text-left">Linha</th>
                  <th className="p-3 text-left">Parâmetros / Detalhes</th>
                </tr>
              </thead>
              <tbody>
                {filteredAndSortedSymbols.map((s, i) => (
                  <tr key={i} className="border-t border-black/5 hover:bg-black/5 transition-colors">
                    <td className="p-3 opacity-60 text-xs">{i + 1}</td>
                    <td className="p-3 font-mono font-medium text-accentStrong">{s.cadeia}</td>
                    <td className="p-3">
                      <span className="px-2 py-0.5 text-xs rounded-full bg-blue-500/10 text-blue-600 border border-blue-500/20 font-medium">
                        {s.categoria}
                      </span>
                    </td>
                    <td className="p-3 text-xs">{s.tipo || "—"}</td>
                    <td className="p-3 text-xs">{s.escopo}</td>
                    <td className="p-3 text-xs">
                      <span className={`px-2 py-0.5 rounded-full text-xs font-medium ${s.utilizada ? "bg-emerald-500/10 text-emerald-600" : "bg-amber-500/10 text-amber-600"}`}>
                        {s.utilizada ? "Sim" : "Não"}
                      </span>
                    </td>
                    <td className="p-3 text-xs opacity-70">{s.linha || "—"}</td>
                    <td className="p-3 text-xs">
                      {s.parametros && s.parametros.length > 0 ? (
                        <div className="flex flex-wrap gap-1">
                          {s.parametros.map((p, idx) => (
                            <span key={idx} className="bg-gray-100 px-1.5 py-0.5 rounded text-[11px] font-mono border border-black/5">
                              {p.por_referencia ? "var " : ""}{p.cadeia}: {p.tipo}
                            </span>
                          ))}
                        </div>
                      ) : (
                        <span className="opacity-40">—</span>
                      )}
                    </td>
                  </tr>
                ))}
                {filteredAndSortedSymbols.length === 0 && (
                  <tr>
                    <td className="p-6 opacity-60 text-center" colSpan={8}>
                      Nenhum símbolo encontrado para o filtro digitado.
                    </td>
                  </tr>
                )}
              </tbody>
            </table>
          ) : fallbackSymbols.length > 0 ? (
            <table className="w-full text-sm">
              <thead className="bg-tintSoft/60 border-b border-black/5">
                <tr>
                  <th className="p-3 text-left w-12 opacity-70">#</th>
                  <th className="p-3 text-left">Símbolo</th>
                  <th className="p-3 text-left">Tipo</th>
                  <th className="p-3 text-left">Categoria</th>
                  <th className="p-3 text-left">Escopo</th>
                </tr>
              </thead>
              <tbody>
                {fallbackSymbols.map((s, i) => (
                  <tr key={i} className="border-t border-black/5 hover:bg-black/5">
                    <td className="p-3 opacity-60">{i + 1}</td>
                    <td className="p-3 font-mono text-accentStrong">{s.simbolo}</td>
                    <td className="p-3">{s.tipo}</td>
                    <td className="p-3">{s.categoria}</td>
                    <td className="p-3">{s.escopo ?? "—"}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          ) : (
            <div className="p-6 opacity-60 text-center text-sm">
              Compile o código para preencher a Tabela de Símbolos.
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
