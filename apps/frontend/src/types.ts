export type TokenRow = {
  lexema: string;
  token: string;
  posicao: string;
  simbolo?: string;
};

export type LexResponse = {
  tokens?: TokenRow[];
  logs?: string[];
  errors?: string[];
};

export type SymbolRow = {
  simbolo: string;
  tipo: string;
  categoria: string;
  valor?: string;
  passadaComo?: string;
  usada?: "Sim" | "Não";
  nivelLexico?: number;
  escopo?: string;
};