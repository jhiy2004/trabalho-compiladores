const addon = require("node-addon");

export function parseSourceLALG(source: string) {
  const normalized = source.replace(/\r\n/g, "\n").replace(/\r/g, "\n");

  const parser = new addon.SyntacticAnalyzerProcedures(normalized);

  parser.run();

  const rawCommands = parser.get_commands() || [];
  const mepaCommands = rawCommands.map((cmd: any) => cmd.toString());
  const mepaCode = mepaCommands.join("\n");

  return {
    errors: parser.get_errors(),
    symbols: parser.get_symbols(),
    snapshots: parser.get_snapshots(),
    symbolTable: parser.get_tabela_simbolos(),
    semanticErrors: parser.get_erros_semanticos(),
    mepaCommands,
    mepaCode,
  };
}

