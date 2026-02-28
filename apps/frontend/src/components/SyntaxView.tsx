import type { TokenRow, SymbolRow } from "../types";

export default function SyntaxView({
  tokens,
  symbols,
}: {
  tokens: TokenRow[];
  symbols: SymbolRow[];
}) {
  const hasData = tokens.length > 0;

  return (
    <div className="p-6 space-y-6">
      <div className="bg-cardSoft rounded-xl2 shadow-soft p-4">
        <div className="font-semibold">Tokens</div>
      </div>

      <div className="grid grid-cols-12 gap-6">
        <div className="col-span-4 bg-cardSoft rounded-xl2 shadow-soft p-4 min-h-[520px]">
          <div className="font-semibold mb-3">Pilha de símbolos</div>
          {!hasData ? (
            <div className="opacity-60">Compile o código para usar essa tela!</div>
          ) : (
            <div className="space-y-2 text-sm">
              {symbols.slice(0, 30).map((s, i) => (
                <div key={i} className="bg-black/5 rounded-lg px-3 py-2 font-mono">
                  {s.simbolo}
                </div>
              ))}
            </div>
          )}
        </div>

        <div className="col-span-8 bg-cardSoft rounded-xl2 shadow-soft p-4 min-h-[520px] flex items-start justify-end">
          {!hasData && (
            <div className="font-semibold">Compile o código antes de usar essa tela!</div>
          )}
        </div>
      </div>
    </div>
  );
}