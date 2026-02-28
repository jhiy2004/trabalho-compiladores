import type { SymbolRow } from "../types";

export default function SymbolsTable({ symbols }: { symbols: SymbolRow[] }) {
  return (
    <div className="p-6">
      <div className="bg-cardSoft rounded-xl2 shadow-soft overflow-hidden">
        <div className="px-5 py-4 border-b border-black/5 flex items-center justify-between">
          <div>
            <div className="text-lg font-semibold">Tabela de Símbolos</div>
          </div>

          <div className="text-xs px-3 py-1 rounded-full bg-tintSoft/70 border border-black/5">
            {symbols.length} símbolos
          </div>
        </div>
        <div className="overflow-auto">
          <table className="w-full text-sm">
            <thead className="bg-tintSoft/60">
              <tr>
                <th className="p-3 text-left w-16 opacity-70">#</th>
                <th className="p-3 text-left">símbolo</th>
                <th className="p-3 text-left">tipo</th>
                <th className="p-3 text-left">categoria</th>
                <th className="p-3 text-left">valor</th>
                <th className="p-3 text-left">passada como</th>
                <th className="p-3 text-left">usada</th>
                <th className="p-3 text-left">nível léxico</th>
                <th className="p-3 text-left">escopo</th>
              </tr>
            </thead>
            <tbody>
              {symbols.map((s, i) => (
                <tr key={i} className="border-t border-black/5 hover:bg-black/5">
                  <td className="p-3 opacity-60">{i + 1}</td>
                  <td className="p-3 font-mono">{s.simbolo}</td>
                  <td className="p-3">{s.tipo}</td>
                  <td className="p-3">{s.categoria}</td>
                  <td className="p-3">{s.valor ?? ""}</td>
                  <td className="p-3">{s.passadaComo ?? ""}</td>
                  <td className="p-3">{s.usada ?? ""}</td>
                  <td className="p-3">{s.nivelLexico ?? ""}</td>
                  <td className="p-3">{s.escopo ?? ""}</td>
                </tr>
              ))}

              {symbols.length === 0 && (
                <tr>
                  <td className="p-6 opacity-60" colSpan={9}>
                    Compile o código para preencher a tabela de símbolos.
                  </td>
                </tr>
              )}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
}