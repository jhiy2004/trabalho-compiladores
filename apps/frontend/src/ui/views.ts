export type ViewKey = "code" | "lex" | "syntax" | "semantic" | "symbols";

export const VIEW_LABEL: Record<ViewKey, string> = {
  code: "Código",
  lex: "Análise Léxica",
  symbols: "Tabela de Símbolos",
  syntax: "Análise Sintática",
  semantic: "Análise Semântica",
};