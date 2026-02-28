import type { TokenRow } from "../types";

export default function LexicoTable({ tokens }: { tokens: TokenRow[] }) {
  return (
    <div className="p-6">
      <div className="bg-cardSoft rounded-xl2 shadow-soft overflow-hidden">
        <div className="px-5 py-4 border-b border-black/5 flex items-center justify-between">
          <div>
            <div className="text-lg font-semibold">Léxico</div>
          </div>

          <div className="text-xs px-3 py-1 rounded-full bg-tintSoft/70 border border-black/5">
            {tokens.length} tokens
          </div>
        </div>
        <div className="overflow-auto">
          <table className="w-full text-sm">
            <thead className="bg-tintSoft/60">
              <tr>
                <th className="p-3 text-left w-16 opacity-70">#</th>
                <th className="p-3 text-left">lexema</th>
                <th className="p-3 text-left">token</th>
                <th className="p-3 text-left">posição</th>
                <th className="p-3 text-left">símbolo</th>
              </tr>
            </thead>
            <tbody>
              {tokens.map((t, i) => (
                <tr
                  key={i}
                  className="border-t border-black/5 hover:bg-black/5 transition"
                >
                  <td className="p-3 opacity-60">{i + 1}</td>
                  <td className="p-3 font-mono">{t.lexema}</td>
                  <td className="p-3">{t.token}</td>
                  <td className="p-3">{t.posicao}</td>
                  <td className="p-3">{t.simbolo ?? ""}</td>
                </tr>
              ))}

              {tokens.length === 0 && (
                <tr>
                  <td className="p-6 opacity-60" colSpan={5}>
                    Compile o código para gerar os tokens.
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