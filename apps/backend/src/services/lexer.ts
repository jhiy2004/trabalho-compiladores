const addon = require("node-addon");

const tokenTypeMapLalg: Record<number, string> = Object.fromEntries(
  Object.entries(addon.TokenType).map(([key, value]) => [value, key]),
);

const tokenTypeMapCalc: Record<number, string> = Object.fromEntries(
  Object.entries(addon.TokenTypeCalc).map(([key, value]) => [value, key]),
);

export function lexSourceCalc(source: string) {
  const lex = new addon.LexicalAnalysisCalc(source);

  if (lex.analyze()) {
    return lex.get_tokens().map((token: any) => ({
      ...token,
      type: tokenTypeMapCalc[token.type] ?? token.type,
    }));
  }

  return null;
}

export function lexSourceLALG(source: string) {
  const lex = new addon.LexicalAnalysisLALG(source);

  return lex.tokenizeAll().map((token: any) => ({
    ...token,
    type: tokenTypeMapLalg[token.type] ?? token.type,
  }));
}
