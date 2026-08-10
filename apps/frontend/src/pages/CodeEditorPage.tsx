import { useCallback, useMemo, useState, useEffect } from "react";
import LeftRail from "../components/LeftRail";
import EditorPane from "../components/EditorPane";
import TerminalPane from "../components/TerminalPane";
import BottomTabs from "../components/BottomTabs";
import LexicoTable from "../components/LexicoTable";
import SymbolsAndSemanticView from "../components/SymbolsAndSemanticView";
import MepaCodeView from "../components/MepaCodeView";
import SyntaxView from "../components/SyntaxView";
import type { ViewKey } from "../ui/views";
import type {
  LexResponse,
  TokenRow,
  SymbolRow,
  ParseResponse,
  ParseErrorRow,
  SnapshotRow,
  ParseStackSymbolRow,
  SymbolTableApiEntry,
  SemanticErrorApi,
} from "../types";

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
  const [hasBeenCompiled, setHasBeenCompiled] = useState(false);

  const [tokens, setTokens] = useState<TokenRow[]>([]);
  const [symbols, setSymbols] = useState<SymbolRow[]>([]);
  const [syntaxErrors, setSyntaxErrors] = useState<ParseErrorRow[]>([]);
  const [syntaxSnapshots, setSyntaxSnapshots] = useState<SnapshotRow[]>([]);
  const [syntaxSymbolsRaw, setSyntaxSymbolsRaw] = useState<ParseStackSymbolRow[]>([]);
  const [logs, setLogs] = useState<string[]>([]);
  const [errors, setErrors] = useState<string[]>([]);
  const [semanticErrors, setSemanticErrors] = useState<(string | SemanticErrorApi)[]>([]);
  const [symbolTable, setSymbolTable] = useState<SymbolTableApiEntry[]>([]);
  const [mepaCode, setMepaCode] = useState<string>("");

  const linesTyped = useMemo(() => code.split("\n").length, [code]);
  void linesTyped;

  const pick = useCallback((v: ViewKey) => {
    setView(v);
  }, []);

  const handleCompile = useCallback(async () => {
    setErrors([]);
    setSemanticErrors([]);
    setSyntaxErrors([]);
    setSyntaxSnapshots([]);
    setSyntaxSymbolsRaw([]);
    setSymbolTable([]);
    setMepaCode("");
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
        setCharging(false);
        setErrors([data?.errors?.[0] ?? "Falha ao compilar."]);
        return;
      }

      const mappedTokens: TokenRow[] = (data.tokens ?? []).map((t) => ({
        lexema: t.lexeme,
        token: t.type,
        posicao: `${t.line}:${t.col}`,
      }));

      let mappedSymbols: SymbolRow[] = [];
      let parsedErrors: ParseErrorRow[] = [];
      let parsedSnapshots: SnapshotRow[] = [];
      let parsedSymbolsRaw: ParseStackSymbolRow[] = [];
      let fetchedSymbolTable: SymbolTableApiEntry[] = [];
      let fetchedSemanticErrors: (string | SemanticErrorApi)[] = [];
      let fetchedMepaCode = "";

      if (language === "lalg") {
        const parseRes = await fetch(`${API_BASE}/api/parse/lalg`, {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ source: code }),
        });

        const parseData = (await parseRes.json()) as ParseResponse;

        if (!parseRes.ok) {
          setCharging(false);
          setErrors(
            (parseData.errors ?? []).map((e) => `${e.error} (${e.line}:${e.col})`)
          );
          return;
        }

        parsedErrors = parseData.errors ?? [];
        parsedSnapshots = parseData.snapshots ?? [];
        parsedSymbolsRaw = parseData.symbols ?? [];
        fetchedSymbolTable = parseData.symbolTable ?? [];
        fetchedSemanticErrors = parseData.semanticErrors ?? [];
        fetchedMepaCode = parseData.mepaCode ?? "";

        mappedSymbols = parsedSymbolsRaw.map((s) => ({
          simbolo: s.name,
          tipo: s.terminal ? "Terminal" : "Não terminal",
          categoria: "Sintático",
        }));
      }

      const elapsed = Date.now() - start;
      const MIN_TIME = 800;

      setTimeout(() => {
        setCharging(false);
        setCompiled(true);
        setHasBeenCompiled(true);
        setTokens(mappedTokens);
        setSymbols(mappedSymbols);
        setSyntaxErrors(parsedErrors);
        setSyntaxSnapshots(parsedSnapshots);
        setSyntaxSymbolsRaw(parsedSymbolsRaw);
        setSymbolTable(fetchedSymbolTable);
        setSemanticErrors(fetchedSemanticErrors);
        setMepaCode(fetchedMepaCode);

        const compilationLogs: string[] = ["Análise léxica ok."];
        if (parsedErrors.length === 0) {
          compilationLogs.push("Análise sintática ok.");
        }
        if (fetchedSemanticErrors.length === 0) {
          compilationLogs.push("Análise semântica ok.");
        }
        if (fetchedMepaCode) {
          compilationLogs.push("Código MEPA gerado com sucesso.");
        }

        setLogs((prev) => [...prev, ...compilationLogs]);

        const allErrors: string[] = [
          ...parsedErrors.map((e) => `[Sintático] Linha ${e.line}:${e.col} - ${e.error}`),
          ...fetchedSemanticErrors.map((e) =>
            typeof e === "string"
              ? `[Semântico] ${e}`
              : `[Semântico] Linha ${e.linha}:${e.col} - ${e.mensagem}`
          ),
        ];

        setErrors(allErrors);
      }, Math.max(0, MIN_TIME - elapsed));
    } catch {
      setCharging(false);
      setErrors(["Erro de rede: não consegui chamar o backend."]);
    }
  }, [code, language]);

  useEffect(() => {
    if (compiled) {
      const timer = setTimeout(() => {
        setCompiled(false);
      }, 2000);

      return () => clearTimeout(timer);
    }
  }, [compiled]);

  return (
    <div className="flex h-screen bg-bgSoft text-ink overflow-hidden">
      <LeftRail current={view} onPick={pick} />

      <div className="flex-1 min-w-0 h-full overflow-auto">
        {view === "code" && (
          <div className="flex flex-col h-full overflow-y-auto">
            <EditorPane
              code={code}
              setCode={setCode}
              language={language}
              setLanguage={setLanguage}
              charging={charging}
              compiled={compiled}
              onCompile={handleCompile}
            />
            <TerminalPane code={code} />
            <BottomTabs logs={logs} errors={errors} />
          </div>
        )}


        {view === "lex" && <LexicoTable tokens={tokens} />}

        {view === "syntax" && (
          <SyntaxView
            tokens={tokens}
            symbols={symbols}
            syntaxErrors={syntaxErrors}
            syntaxSnapshots={syntaxSnapshots}
            syntaxSymbolsRaw={syntaxSymbolsRaw}
          />
        )}

        {view === "symbolsAndSemantic" && (
          <SymbolsAndSemanticView
            symbolTable={symbolTable}
            semanticErrors={semanticErrors}
            fallbackSymbols={symbols}
            compiled={hasBeenCompiled}
          />
        )}

        {view === "mepa" && (
          <MepaCodeView mepaCode={mepaCode} compiled={hasBeenCompiled} />
        )}
      </div>
    </div>
  );
}

