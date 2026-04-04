const addon = require("node-addon");

export function parseSourceLALG(source: string) {
  const normalized = source.replace(/\r\n/g, "\n").replace(/\r/g, "\n");

  const parser = new addon.SyntacticAnalyzerProcedures(normalized);

  parser.run();

  return {
    errors: parser.get_errors(),
    symbols: parser.get_symbols(),
    snapshots: parser.get_snapshots(),
  };
}
