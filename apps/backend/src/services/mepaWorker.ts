const addon = require("node-addon");

let mepa: any = null;
let pollInterval: NodeJS.Timeout | null = null;
let isWaitingInput = false;

process.on("message", (msg: any) => {
  if (!msg) return;

  if (msg.type === "init") {
    const source = msg.source;

    if (!source) {
      process.send?.({ type: "error", error: "Código-fonte não fornecido." });
      process.exit(1);
    }

    try {
      const parser = new addon.SyntacticAnalyzerProcedures(source);
      parser.run();

      const synErrors = parser.get_errors() || [];
      if (synErrors.length > 0) {
        process.send?.({
          type: "error",
          error: `Erro sintático: ${synErrors[0].error} (linha ${synErrors[0].line}:${synErrors[0].col})`,
        });
        process.exit(0);
      }

      const semErrors = parser.get_erros_semanticos() || [];
      if (semErrors.length > 0) {
        process.send?.({
          type: "error",
          error: `Erro semântico: ${semErrors[0].mensagem} (linha ${semErrors[0].linha}:${semErrors[0].col})`,
        });
        process.exit(0);
      }

      const commands = parser.get_commands() || [];
      if (commands.length === 0) {
        process.send?.({
          type: "error",
          error: "Nenhum comando MEPA gerado para o código.",
        });
        process.exit(0);
      }

      mepa = new addon.Mepa(commands);
      mepa.runInBackground();

      pollInterval = setInterval(() => {
        // Se aguardando input, pausa a leitura
        if (isWaitingInput) return; 

        try {
          const text = mepa.popOutput();

          if (text === "__EXIT__") {
            if (pollInterval) clearInterval(pollInterval);
            process.send?.({ type: "finished" });
            process.exit(0);
          } else {
            const inputType = mepa.nextInputType();

            if (text || inputType === "int" || inputType === "char") {
              
              if (inputType === "int" || inputType === "char") {
                // Trava o loop para ler o input
                isWaitingInput = true;
              }
              
              process.send?.({ type: "output", text: text || "", inputType });
            }
          }
        } catch (err: any) {
          if (pollInterval) clearInterval(pollInterval);
          process.send?.({ type: "error", error: err?.message || String(err) });
          process.exit(1);
        }
      }, 50);

    } catch (err: any) {
      process.send?.({ type: "error", error: err?.message || String(err) });
      process.exit(1);
    }
  } 
  
  else if (msg.type === "input") {
    if (mepa) {
      mepa.pushInput(msg.value);
      // Libera o loop
      isWaitingInput = false;
    }
  }
});