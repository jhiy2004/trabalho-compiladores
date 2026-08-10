export type ViewKey = "code" | "lex" | "syntax" | "symbolsAndSemantic" | "mepa";

export const VIEW_LABEL: Record<ViewKey, string> = {
  code: "Código",
  lex: "Análise Léxica",
  syntax: "Análise Sintática",
  symbolsAndSemantic: "Tabela de Símbolos & Semântica",
  mepa: "Código MEPA",
};