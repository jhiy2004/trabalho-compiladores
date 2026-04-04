export type TokenRow = {
  lexema: string;
  token: string;
  posicao: string;
  simbolo?: string;
};

export type LexTokenApi = {
  type: string;
  lexeme: string;
  line: number;
  col: number;
};

export type LexResponse = {
  tokens?: LexTokenApi[];
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

export type ParseErrorRow = {
  error: string;
  line: number;
  col: number;
};

export type ParseStackSymbolRow = {
  terminal: boolean;
  name: string;
};

export type SnapshotTokenRow = {
  type: string | number;
  lexeme: string;
  line: number;
  col: number;
} | null;

export type SnapshotRow = {
  curr_token: SnapshotTokenRow;
  curr_symbols: ParseStackSymbolRow[];
  curr_errors: ParseErrorRow[];
  action: string;
};

export type ParseResponse = {
  symbols?: ParseStackSymbolRow[];
  errors?: ParseErrorRow[];
  snapshots?: SnapshotRow[];
};
