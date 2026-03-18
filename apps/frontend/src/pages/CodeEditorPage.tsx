import { useCallback, useMemo, useState, useEffect } from "react";
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

  const [language, setLanguage] = useState("lalg");

  const [code, setCode] = useState<string>(
`program correto;
int a, b, c;
boolean d, e, f;

{Comentário correto}

procedure proc(var a1 : int);
int a, b, c;
boolean d, e, f;
begin
	a:=1;
	if (a<1)
		a:=12
end;

begin
	a:=2;
	b:=10;
	c:=11;
	a:=b+c;
	d:=true;
	e:=false;
	f:=true;
	//comentario de linha
	if (d)
	begin
		a:=20;
		b:=10*c;
		c:=a div b
	end;
	while (a>1)
	begin
		if (b>10)
			b:=2;
		a:=a-1
	end
end.`
  );

  const [charging, setCharging] = useState(false);
  const [compiled, setCompiled] = useState(false);

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
    setCharging(true);
    const start = Date.now();

    try {
      const res = await fetch(`${API_BASE}/api/lex/${language}`, {
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

      const elapsed = Date.now() - start;
      const MIN_TIME = 800;

      setTimeout(() => {
        setCharging(false);
        setCompiled(true);
        setTokens(mappedTokens);
        setLogs((prev) => [...prev, ...(data.logs ?? ["OK"])]);
        setErrors(data.errors ?? []);
      }, Math.max(0, MIN_TIME - elapsed));
    } catch {
      setErrors(["Erro de rede: não consegui chamar o backend."]);
    }
  }, [code, language]);

  useEffect(() => {
    if(compiled){
      setTimeout(() => {
        setCompiled(false);
      }, 2000)
    }
  }, [compiled])

  return (
    <div className="flex h-screen bg-bgSoft text-ink overflow-hidden">
      <LeftRail current={view} onPick={pick} />

      {/* Area principal, vai mudar conforme a view selecionada */}
      <div className="flex-1 min-w-0">
        {view === "code" && (
          <div className="flex flex-col h-full">
            <EditorPane
              code={code}
              setCode={setCode}
              language={language}
              setLanguage={setLanguage}
              charging={charging}
              compiled={compiled}
              onCompile={handleCompile} />
            <BottomTabs logs={logs} errors={errors}
            />
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
