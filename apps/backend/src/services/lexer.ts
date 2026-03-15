const addon = require("node-addon");

const tokenTypeMap: Record<number, string> = Object.fromEntries(
  Object.entries(addon.TokenType).map(([key, value]) => [value, key]),
);

export function lexSourceCalc(source: string) {
  const lex = new addon.LexicalAnalysisCalc(source);

  if (lex.analyze()) {
    return lex.get_tokens().map((token: any) => ({
      ...token,
      type: tokenTypeMap[token.type] ?? token.type,
    }));
  }

  return null;
}

export function lexSourceLALG(source: string) {
  const lex = new addon.LexicalAnalysisLALG(source);

  return lex.tokenizeAll().map((token: any) => ({
    ...token,
    type: tokenTypeMap[token.type] ?? token.type,
  }));
}
