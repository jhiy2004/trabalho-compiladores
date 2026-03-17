import Editor from "@monaco-editor/react";
import type { editor } from "monaco-editor";
import { useEffect, useRef, useState } from "react";

import {
  AlertDialog,
  AlertDialogAction,
  AlertDialogCancel,
  AlertDialogContent,
  AlertDialogDescription,
  AlertDialogFooter,
  AlertDialogHeader,
  AlertDialogTitle,
} from "@/components/ui/alert-dialog";

type Props = {
  code: string;
  setCode: (v: string) => void;
  language: string;
  setLanguage: (v: string) => void;
  charging: boolean;
  compiled: boolean;
  onCompile: () => void;
};

export default function EditorPane({ code, setCode, language, setLanguage, charging, compiled, onCompile }: Props) {
  const editorRef = useRef<editor.IStandaloneCodeEditor | null>(null);
  const fileInputRef = useRef<HTMLInputElement | null>(null)
  const [confirmOpen, setConfirmOpen] = useState(false);

  useEffect(() => {
    const handler = (e: KeyboardEvent) => {
      if (e.ctrlKey && e.key === "Enter") {
        e.preventDefault();
        onCompile();
      }

      if (e.ctrlKey && e.key.toLowerCase() === "o") {
        e.preventDefault();
        fileInputRef.current?.click();
      }

      if (e.ctrlKey && e.key.toLowerCase() === "s") {
        e.preventDefault();
        handleSave();
      }
    };

    window.addEventListener("keydown", handler);
    return () => window.removeEventListener("keydown", handler);
  }, [onCompile, code]);

  const handleNewClick = () => {
    if (!code.trim()) {
      setCode("");
      return;
    }
    setConfirmOpen(true);
  };

  const handleConfirmNew = () => {
    setCode("");
    setConfirmOpen(false);
    editorRef.current?.focus();
  };

  const handleOpenClick = () => {
    fileInputRef.current?.click();
  };

  const handleFileChange = async (
    e: React.ChangeEvent<HTMLInputElement>
  ) => {
    const file = e.target.files?.[0];
    if (!file) return;

    try {
      const text = await file.text();
      setCode(text);
      editorRef.current?.focus();
    } finally {
      e.target.value = "";
    }
  };

  const handleSave = () => {
    const blob = new Blob([code], { type: "text/plain;charset=utf-8" });
    const url = URL.createObjectURL(blob);

    const a = document.createElement("a");
    a.href = url;
    a.download = "codigo.txt";
    document.body.appendChild(a);
    a.click();
    a.remove();

    URL.revokeObjectURL(url);
  };

  return (
    <div className="flex-1 flex flex-col p-6">
      <div className="flex justify-between items-center mb-4">
        <div className="space-x-6 text-sm">
          <button
            className="opacity-70 hover:opacity-100"
            type="button"
            onClick={handleNewClick}
          >
            Novo
          </button>

          <input
            type="file"
            hidden
            ref={fileInputRef}
            accept="text/plain,.txt"
            onChange={handleFileChange}
          />

          <button
            className="opacity-70 hover:opacity-100"
            type="button"
            onClick={handleOpenClick}
          >
            Abrir
          </button>


          <button
            className="opacity-70 hover:opacity-100"
            type="button"
            onClick={handleSave}
          >
            Salvar
          </button>
        </div>

        <select
            id="seletorLinguagem"
            className="bg-accentStrong text-white px-5 py-2 rounded-full shadow-soft hover:opacity-95"
            value={language}
            onChange={(e) => setLanguage(e.target.value)}
        >
            <option value="lalg"> LALG </option>
            <option value="calc"> Calculadora </option>
        </select>

        <button
          type="button"
          onClick={onCompile}
          className={`bg-${compiled ? "green-500" : "accentStrong"} text-white px-5 py-2 rounded-full shadow-soft hover:opacity-95 flex gap-2 items-center`}
        >
          {charging && (
          <svg className="animate-spin h-8 w-8 text-white-500 mr-auto" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
          <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle>
          <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
          </svg>)}
          Compilar
        </button>
      </div>

      <div className="flex-1 rounded-xl2 overflow-hidden shadow-soft bg-white">
        <Editor
          height="100%"
          theme="vs-light"
          defaultLanguage="pascal"
          value={code}
          onChange={(v: string | undefined) => setCode(v ?? "")}
          onMount={(ed: editor.IStandaloneCodeEditor) => (editorRef.current = ed)}
          options={{
            minimap: { enabled: false },
            fontSize: 15,
            scrollBeyondLastLine: false,
            wordWrap: "on",
          }}
        />
      </div>

      {/* Modal do criar novo arquivo (apagar o atual) */}
      <AlertDialog open={confirmOpen} onOpenChange={setConfirmOpen}>
        <AlertDialogContent className="rounded-2xl">
          <AlertDialogHeader>
            <AlertDialogTitle>Limpar o arquivo e começar do zero?</AlertDialogTitle>
            <AlertDialogDescription>
              Isso vai apagar o conteúdo atual do editor. Você pode salvar antes se quiser.
            </AlertDialogDescription>
          </AlertDialogHeader>

          <AlertDialogFooter>
            <AlertDialogCancel className="rounded-full">
              Cancelar
            </AlertDialogCancel>
            <AlertDialogAction
              className="rounded-full bg-accentStrong text-white hover:opacity-95"
              onClick={handleConfirmNew}
            >
              Começar um novo
            </AlertDialogAction>
          </AlertDialogFooter>
        </AlertDialogContent>
      </AlertDialog>
    </div>
  );
}
