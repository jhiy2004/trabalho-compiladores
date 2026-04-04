import type {
  TokenRow,
  SymbolRow,
  ParseErrorRow,
  SnapshotRow,
  ParseStackSymbolRow,
} from "../types";

export default function SyntaxView({
  tokens,
  symbols,
  syntaxErrors,
  syntaxSnapshots,
  syntaxSymbolsRaw,
}: {
  tokens: TokenRow[];
  symbols: SymbolRow[];
  syntaxErrors: ParseErrorRow[];
  syntaxSnapshots: SnapshotRow[];
  syntaxSymbolsRaw: ParseStackSymbolRow[];
}) {
  const hasData = tokens.length > 0;
  const errorCount = syntaxErrors.length;
  const snapshotCount = syntaxSnapshots.length;
  const symbolCount = syntaxSymbolsRaw.length;

  return (
    <div className="p-6 space-y-6">
      <div className="bg-cardSoft rounded-xl2 shadow-soft overflow-hidden">
        <div className="px-5 py-4 border-b border-black/5 flex items-center justify-between">
          <div>
            <div className="text-lg font-semibold">Visão Sintática</div>
          </div>

          <div className="flex gap-3">
            <div className="text-xs px-3 py-1 rounded-full bg-blue-300 border border-black/5">
              {errorCount} erros
            </div>

            <div className="text-xs px-3 py-1 rounded-full bg-tintSoft/70 border border-black/5">
              {snapshotCount} snapshots
            </div>

            <div className="text-xs px-3 py-1 rounded-full bg-tintSoft/70 border border-black/5">
              {symbolCount} símbolos
            </div>
          </div>
        </div>

        <div className="px-5 py-4 text-sm opacity-80">
          {tokens.length} tokens analisados
        </div>
      </div>

      <div className="grid grid-cols-12 gap-6">
        <div className="col-span-4 space-y-6">
          <div className="bg-cardSoft rounded-xl2 shadow-soft p-4 min-h-[280px]">
            <div className="font-semibold mb-3">Pilha de símbolos</div>

            {!hasData ? (
              <div className="opacity-60">Compile o código para usar essa tela!</div>
            ) : syntaxSymbolsRaw.length === 0 ? (
              <div className="opacity-60">Nenhum símbolo sintático disponível.</div>
            ) : (
              <div className="space-y-2 text-sm max-h-[420px] overflow-auto">
                {syntaxSymbolsRaw.map((s, i) => (
                  <div
                    key={i}
                    className="bg-black/5 rounded-lg px-3 py-2 flex items-center justify-between gap-3"
                  >
                    <span className="font-mono break-all">{s.name}</span>
                    <span className="text-xs opacity-70 whitespace-nowrap">
                      {s.terminal ? "terminal" : "não terminal"}
                    </span>
                  </div>
                ))}
              </div>
            )}
          </div>

          <div className="bg-cardSoft rounded-xl2 shadow-soft p-4 min-h-[220px]">
            <div className="font-semibold mb-3">Erros sintáticos</div>

            {!hasData ? (
              <div className="opacity-60">Compile o código para visualizar erros.</div>
            ) : syntaxErrors.length === 0 ? (
              <div className="opacity-60">Nenhum erro sintático encontrado.</div>
            ) : (
              <div className="space-y-3 max-h-[320px] overflow-auto">
                {syntaxErrors.map((err, i) => (
                  <div
                    key={i}
                    className="rounded-lg border border-black/5 bg-blue-100 px-3 py-2"
                  >
                    <div className="font-medium">{err.error}</div>
                    <div className="text-sm opacity-70">
                      linha {err.line}, coluna {err.col}
                    </div>
                  </div>
                ))}
              </div>
            )}
          </div>
        </div>

        <div className="col-span-8 bg-cardSoft rounded-xl2 shadow-soft p-4 min-h-[520px]">
          <div className="font-semibold mb-3">Snapshots da análise</div>

          {!hasData ? (
            <div className="opacity-60">Compile o código antes de usar essa tela!</div>
          ) : syntaxSnapshots.length === 0 ? (
            <div className="opacity-60">Nenhum snapshot retornado pelo parser.</div>
          ) : (
            <div className="space-y-4 max-h-[70vh] overflow-auto">
              {syntaxSnapshots.map((snap, i) => (
                <div
                  key={i}
                  className="rounded-xl border border-black/5 bg-black/5 p-4 space-y-3"
                >
                  <div className="flex items-center justify-between gap-3">
                    <div className="font-semibold">Passo {i + 1}</div>
                    <div className="text-xs px-2 py-1 rounded-full bg-white/70 border border-black/5">
                      {snap.action || "Sem ação"}
                    </div>
                  </div>

                  <div>
                    <div className="text-sm font-medium mb-1">Token atual</div>
                    {snap.curr_token ? (
                      <div className="text-sm font-mono bg-white/70 rounded-lg px-3 py-2">
                        {snap.curr_token.lexeme} — {String(snap.curr_token.type)} (
                        {snap.curr_token.line}:{snap.curr_token.col})
                      </div>
                    ) : (
                      <div className="text-sm opacity-60">Sem token atual</div>
                    )}
                  </div>

                  <div>
                    <div className="text-sm font-medium mb-1">Símbolos da pilha</div>
                    {snap.curr_symbols.length === 0 ? (
                      <div className="text-sm opacity-60">Pilha vazia</div>
                    ) : (
                      <div className="flex flex-wrap gap-2">
                        {snap.curr_symbols.map((s, j) => (
                          <span
                            key={j}
                            className="text-xs rounded-full bg-white/70 border border-black/5 px-2 py-1 font-mono"
                          >
                            {s.name}
                          </span>
                        ))}
                      </div>
                    )}
                  </div>

                  <div>
                    <div className="text-sm font-medium mb-1">Erros nesse passo</div>
                    {snap.curr_errors.length === 0 ? (
                      <div className="text-sm opacity-60">Nenhum erro neste snapshot</div>
                    ) : (
                      <div className="space-y-2">
                        {snap.curr_errors.map((e, j) => (
                          <div
                            key={j}
                            className="text-sm rounded-lg bg-blue-100 px-3 py-2"
                          >
                            {e.error} ({e.line}:{e.col})
                          </div>
                        ))}
                      </div>
                    )}
                  </div>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
