import type { TokenRow } from "../types";

export default function SemanticView({
  tokens,
  semanticErrors,
}: {
  tokens: TokenRow[];
  semanticErrors: string[];
}) {
  return (
    <div className="p-6 space-y-6">
      <div className="bg-cardSoft rounded-xl2 shadow-soft overflow-hidden">
        <div className="px-5 py-4 border-b border-black/5 flex items-center justify-between">
          <div>
            <div className="text-lg font-semibold">Semântico</div>
          </div>
        </div>
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
            {tokens.slice(0, 10).map((t, i) => (
              <tr key={i} className="border-t border-black/5">
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
                  Compile o código para popular tokens.
                </td>
              </tr>
            )}
          </tbody>
        </table>
      </div>

      <div>
        <div className="text-2xl font-semibold mb-3">Erros semânticos</div>
        {semanticErrors.length === 0 ? (
          <div className="opacity-60">Nenhum erro semântico até agora.</div>
        ) : (
          <ul className="list-disc pl-6 space-y-1">
            {semanticErrors.map((e, i) => (
              <li key={i} className="text-red-600">
                {e}
              </li>
            ))}
          </ul>
        )}
      </div>
    </div>
  );
}