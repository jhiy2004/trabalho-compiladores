import { useCallback, useMemo, useState } from "react";
import LeftRail from "../components/LeftRail";
import EditorPane from "../components/EditorPane";
import BottomTabs from "../components/BottomTabs";
import LexicoTable from "../components/LexicoTable";
import SymbolsTable from "../components/SymbolsTable";
import SyntaxView from "../components/SyntaxView";
import SemanticView from "../components/SemanticView";
import type { ViewKey } from "../ui/views";
import type { LexResponse, TokenRow, SymbolRow } from "../types";

const API_BASE = import.meta.env.VITE_API_BASE;

export default function CodeEditorPage() {
  const [view, setView] = useState<ViewKey>("code");

  const [code, setCode] = useState<string>(
`(1 + 1) / 3 * 9`
  );

  const [tokens, setTokens] = useState<TokenRow[]>([]);
  const [symbols, ] = useState<SymbolRow[]>([]);
  const [logs, setLogs] = useState<string[]>([]);
  const [errors, setErrors] = useState<string[]>([]);
  const [semanticErrors, setSemanticErrors] = useState<string[]>([]);

  const linesTyped = useMemo(() => code.split("\n").length, [code]);
  void linesTyped;

  const pick = useCallback((v: ViewKey) => {
    setView(v);
  }, []);

  const handleCompile = useCallback(async () => {
    setErrors([]);
    setSemanticErrors([]);
    setLogs((prev) => [...prev, "Compilando..."]);

    try {
      const res = await fetch(`${API_BASE}/api/lex`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ source: code }),
      });

      const data = (await res.json()) as LexResponse;

      if (!res.ok) {
        setErrors([data?.errors?.[0] ?? "Falha ao compilar."]);
        return;
      }

      const mappedTokens: TokenRow[] = (data.tokens ?? []).map((t: any) => ({
        lexema: t.lexeme,
        token: t.type,
        posicao: `${t.line}:${t.col}`,
        // simbolo: t.lexeme,
      }));

      setTokens(mappedTokens);
      // ajustar depois conforme o backend:
      // setSymbols(data.symbols ?? []);
      setLogs((prev) => [...prev, ...(data.logs ?? ["OK"])]);
      setErrors(data.errors ?? []);
      // setSemanticErrors(data.semanticErrors ?? []);
    } catch {
      setErrors(["Erro de rede: não consegui chamar o backend."]);
    }
  }, [code]);

  return (
    <div className="flex h-screen bg-bgSoft text-ink overflow-hidden">
      <LeftRail current={view} onPick={pick} />

      {/* Area principal, vai mudar conforme a view selecionada */}
      <div className="flex-1 min-w-0">
        {view === "code" && (
          <div className="flex flex-col h-full">
            <EditorPane code={code} setCode={setCode} onCompile={handleCompile} />
            <BottomTabs logs={logs} errors={errors} />
          </div>
        )}

        {view === "lex" && (
          <LexicoTable tokens={tokens} />
        )}

        {view === "symbols" && (
          <SymbolsTable symbols={symbols} />
        )}

        {view === "syntax" && (
          <SyntaxView tokens={tokens} symbols={symbols} />
        )}

        {view === "semantic" && (
          <SemanticView tokens={tokens} semanticErrors={semanticErrors} />
        )}
      </div>
    </div>
  );
}