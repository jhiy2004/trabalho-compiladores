import { useState, useEffect, useRef, useCallback } from "react";
import { Terminal, Play, Square, Trash2, Send, Loader2 } from "lucide-react";


type Props = {
  code: string;
};

type TerminalStatus = "idle" | "running" | "waiting_input" | "finished" | "error";

const API_BASE = import.meta.env.VITE_API_BASE;

export default function TerminalPane({ code }: Props) {
  const [status, setStatus] = useState<TerminalStatus>("idle");
  const [sessionId, setSessionId] = useState<string | null>(null);
  const [outputLines, setOutputLines] = useState<string[]>([]);
  const [inputValue, setInputValue] = useState("");
  const [inputType, setInputType] = useState<"int" | "char" | "none">("none");

  const inputRef = useRef<HTMLInputElement>(null);
  const terminalEndRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    terminalEndRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [outputLines, status]);

  useEffect(() => {
    if (status === "waiting_input") {
      setTimeout(() => inputRef.current?.focus(), 100);
    }
  }, [status]);

  const handleStop = useCallback(async () => {
    if (sessionId) {
      try {
        await fetch(`${API_BASE}/api/mepa/stop/${sessionId}`, { method: "POST" });
      } catch {}
    }
    setSessionId(null);
    setStatus("idle");
    setInputType("none");
  }, [sessionId]);

  const handleStart = async () => {
    if (!code.trim()) {
      setOutputLines(["[Erro] Nenhum código-fonte no editor para executar."]);
      setStatus("error");
      return;
    }

    if (sessionId) {
      await handleStop();
    }

    setOutputLines(["[Execução iniciada] Compilando e executando MEPA..."]);
    setStatus("running");
    setInputType("none");

    try {
      const res = await fetch(`${API_BASE}/api/mepa/start`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ source: code }),
      });

      const data = await res.json();
      if (!res.ok || !data.sessionId) {
        setOutputLines((prev) => [...prev, `[Erro] ${data.error || "Falha ao iniciar MEPA."}`]);
        setStatus("error");
        return;
      }

      setSessionId(data.sessionId);
    } catch {
      setOutputLines((prev) => [...prev, "[Erro de rede] Não foi possível conectar ao backend."]);
      setStatus("error");
    }
  };

  useEffect(() => {
    if (!sessionId || status === "finished" || status === "error" || status === "idle") {
      return;
    }

    const interval = setInterval(async () => {
      try {
        const res = await fetch(`${API_BASE}/api/mepa/poll/${sessionId}`);
        const data = await res.json();

        if (data.outputs && data.outputs.length > 0) {
          setOutputLines((prev) => [...prev, ...data.outputs]);
        }

        if (data.error) {
          setOutputLines((prev) => [...prev, `[Erro MEPA] ${data.error}`]);
          setStatus("error");
          setSessionId(null);
          return;
        }

        if (data.finished) {
          setOutputLines((prev) => [...prev, "[Execução finalizada]"]);
          setStatus("finished");
          setSessionId(null);
          return;
        }

        if (data.waitingInput) {
          setStatus("waiting_input");
          setInputType(data.inputType || "int");
        } else {
          setStatus("running");
          setInputType("none");
        }
      } catch {}
    }, 300);

    return () => clearInterval(interval);
  }, [sessionId, status]);

  const handleSendInput = async () => {
    if (!sessionId || !inputValue.trim()) return;

    const val = inputValue.trim();
    setInputValue("");
    setOutputLines((prev) => [...prev, `> ${val}`]);

    try {
      await fetch(`${API_BASE}/api/mepa/input/${sessionId}`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ value: val }),
      });
      setStatus("running");
    } catch {
      setOutputLines((prev) => [...prev, "[Erro] Falha ao enviar entrada."]);
    }
  };

  const handleKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key === "Enter") {
      e.preventDefault();
      handleSendInput();
    }
  };

  const handleClear = () => {
    setOutputLines([]);
    if (status === "finished" || status === "error") {
      setStatus("idle");
    }
  };

  return (
    <div className="h-56 bg-[#18181e] text-[#cccccc] rounded-xl2 shadow-soft mx-6 mb-6 flex flex-col overflow-hidden border border-black/10">
      <div className="px-4 py-2.5 bg-[#121217] border-b border-[#282833] flex items-center justify-between">
        <div className="flex items-center gap-3">
          <Terminal className="w-4 h-4 text-accentStrong" />
          <span className="text-xs font-semibold uppercase tracking-wider text-gray-300">
            Terminal MEPA
          </span>

          <span className="ml-2 px-2.5 py-0.5 text-[11px] rounded-full font-medium flex items-center gap-1.5">
            {status === "idle" && (
              <span className="text-gray-400 bg-gray-800 px-2 py-0.5 rounded-full">Pronto</span>
            )}
            {status === "running" && (
              <span className="text-blue-400 bg-blue-500/10 border border-blue-500/20 px-2 py-0.5 rounded-full flex items-center gap-1">
                <Loader2 className="w-3 h-3 animate-spin" /> Executando...
              </span>
            )}
            {status === "waiting_input" && (
              <span className="text-amber-400 bg-amber-500/10 border border-amber-500/20 px-2 py-0.5 rounded-full animate-pulse">
                Aguardando Entrada ({inputType})
              </span>
            )}
            {status === "finished" && (
              <span className="text-emerald-400 bg-emerald-500/10 border border-emerald-500/20 px-2 py-0.5 rounded-full">
                Finalizado
              </span>
            )}
            {status === "error" && (
              <span className="text-red-400 bg-red-500/10 border border-red-500/20 px-2 py-0.5 rounded-full">
                Erro
              </span>
            )}
          </span>
        </div>

        <div className="flex items-center gap-2">
          <button
            type="button"
            onClick={handleStart}
            disabled={status === "running" || status === "waiting_input"}
            className="flex items-center gap-1.5 px-3 py-1 text-xs font-semibold bg-accentStrong text-white rounded-lg hover:opacity-90 transition disabled:opacity-40 disabled:cursor-not-allowed shadow-soft"
            title="Executar código no Terminal MEPA"
          >
            <Play className="w-3.5 h-3.5 fill-current" />
            Executar MEPA
          </button>

          {(status === "running" || status === "waiting_input") && (
            <button
              type="button"
              onClick={handleStop}
              className="flex items-center gap-1 px-2.5 py-1 text-xs font-medium bg-red-500/20 text-red-400 border border-red-500/30 rounded-lg hover:bg-red-500/30 transition"
              title="Interromper execução"
            >
              <Square className="w-3 h-3 fill-current" />
              Parar
            </button>
          )}

          <button
            type="button"
            onClick={handleClear}
            className="p-1.5 text-gray-400 hover:text-white hover:bg-gray-800 rounded-lg transition"
            title="Limpar terminal"
          >
            <Trash2 className="w-3.5 h-3.5" />
          </button>
        </div>
      </div>

      <div className="flex-1 p-3 font-mono text-xs overflow-y-auto space-y-1 select-text">
        {outputLines.length === 0 ? (
          <div className="text-gray-500 italic">
            Clique em <strong className="text-gray-300">Executar MEPA</strong> para rodar o código e interagir no terminal.
          </div>
        ) : (
          outputLines.map((line, idx) => (
            <div
              key={idx}
              className={
                line.startsWith("> ")
                  ? "text-emerald-400 font-semibold"
                  : line.startsWith("[Erro")
                  ? "text-red-400"
                  : line.startsWith("[Execução")
                  ? "text-blue-400 opacity-80"
                  : "text-gray-200"
              }
            >
              {line}
            </div>
          ))
        )}

        {status === "waiting_input" && (
          <div className="flex items-center gap-2 pt-1 text-amber-400 font-semibold">
            <span>&gt;</span>
            <input
              ref={inputRef}
              type={inputType === "int" ? "number" : "text"}
              value={inputValue}
              onChange={(e) => setInputValue(e.target.value)}
              onKeyDown={handleKeyDown}
              placeholder={`Digite um valor (${inputType === "int" ? "inteiro" : "caractere"}) e pressione Enter...`}
              className="flex-1 bg-transparent text-emerald-300 placeholder:text-gray-500 text-xs font-mono focus:outline-none"
            />
            <button
              type="button"
              onClick={handleSendInput}
              className="p-1 bg-amber-500/20 text-amber-300 hover:bg-amber-500/30 rounded"
              title="Enviar entrada"
            >
              <Send className="w-3 h-3" />
            </button>
          </div>
        )}

        <div ref={terminalEndRef} />
      </div>
    </div>
  );
}
