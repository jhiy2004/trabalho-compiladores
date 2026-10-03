import Editor from "@monaco-editor/react";
import { useState } from "react";
import { Copy, Check, Download, Cpu, FileCode } from "lucide-react";

type Props = {
  mepaCode: string;
  compiled: boolean;
};

export default function MepaCodeView({ mepaCode, compiled }: Props) {
  const [copied, setCopied] = useState(false);

  const handleCopy = () => {
    if (!mepaCode) return;
    navigator.clipboard.writeText(mepaCode);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const handleDownload = () => {
    if (!mepaCode) return;
    const blob = new Blob([mepaCode], { type: "text/plain;charset=utf-8" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = "mepa_output.mepa";
    document.body.appendChild(a);
    a.click();
    a.remove();
    URL.revokeObjectURL(url);
  };

  const lineCount = mepaCode ? mepaCode.trim().split("\n").length : 0;

  return (
    <div className="flex-1 flex flex-col p-6 h-full">
      <div className="flex flex-col sm:flex-row justify-between sm:items-center gap-3 mb-4">
        <div className="flex items-center gap-3">
          <div className="p-2 bg-accentStrong/10 rounded-xl">
            <Cpu className="w-6 h-6 text-accentStrong" />
          </div>
          <div>
            <h1 className="text-xl font-bold text-gray-900">Código Assembly (MEPA)</h1>
            <p className="text-xs text-gray-500">
              Código intermediário gerado para a MEPA
            </p>
          </div>
        </div>

        <div className="flex items-center gap-3">
          {compiled && lineCount > 0 && (
            <span className="text-xs px-3 py-1.5 rounded-full bg-tintSoft/80 border border-black/5 font-mono text-gray-700">
              {lineCount} instruções
            </span>
          )}

          <button
            type="button"
            onClick={handleCopy}
            disabled={!mepaCode}
            className="flex items-center gap-1.5 px-4 py-2 rounded-full text-xs font-medium bg-cardSoft border border-black/10 hover:bg-black/5 transition disabled:opacity-40 disabled:cursor-not-allowed shadow-soft"
          >
            {copied ? (
              <>
                <Check className="w-4 h-4 text-emerald-600" />
                <span className="text-emerald-600">Copiado!</span>
              </>
            ) : (
              <>
                <Copy className="w-4 h-4" />
                <span>Copiar</span>
              </>
            )}
          </button>

          <button
            type="button"
            onClick={handleDownload}
            disabled={!mepaCode}
            className="flex items-center gap-1.5 px-4 py-2 rounded-full text-xs font-medium bg-accentStrong text-white hover:opacity-90 transition disabled:opacity-40 disabled:cursor-not-allowed shadow-soft"
          >
            <Download className="w-4 h-4" />
            <span>Baixar .mepa</span>
          </button>
        </div>
      </div>

      <div className="flex-1 rounded-xl2 overflow-hidden shadow-soft bg-white border border-black/5 flex flex-col">
        {!compiled || !mepaCode ? (
          <div className="flex-1 flex flex-col items-center justify-center p-8 text-center bg-cardSoft/30">
            <div className="w-16 h-16 rounded-full bg-tintSoft/80 flex items-center justify-center mb-4">
              <FileCode className="w-8 h-8 opacity-40" />
            </div>
            <h2 className="text-lg font-semibold text-gray-800 mb-1">
              Nenhum código MEPA gerado ainda
            </h2>
            <p className="text-xs text-gray-500 max-w-sm">
              Clique em <strong className="text-accentStrong">Compilar</strong> no editor principal para executar a compilação e gerar as instruções MEPA.
            </p>
          </div>
        ) : (
          <Editor
            height="100%"
            theme="vs-light"
            defaultLanguage="plaintext"
            value={mepaCode}
            options={{
              readOnly: true,
              domReadOnly: true,
              minimap: { enabled: false },
              fontSize: 14,
              scrollBeyondLastLine: false,
              wordWrap: "on",
              lineNumbers: "on",
              renderLineHighlight: "all",
              fontFamily: "'Fira Code', 'Cascadia Code', Consolas, monospace",
            }}
          />
        )}
      </div>
    </div>
  );
}
