const addon = require("./addon.node") as any;

const tokenTypeMap: Record<number, string> = Object.fromEntries(
  Object.entries(addon.TokenType).map(([key, value]) => [value, key])
);

export function lexSource(source: string) {
  const lex = new addon.LexicalAnalysis(source);

  if (lex.analyze()) {
    return lex.get_tokens().map((token: any) => ({
      ...token,
      type: tokenTypeMap[token.type] ?? token.type
    }));
  }

  return null;
}