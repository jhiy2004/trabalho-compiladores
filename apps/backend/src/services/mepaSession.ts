import { fork, ChildProcess } from "child_process";
import path from "path";

export interface SessionState {
  id: string;
  outputs: string[];
  waitingInput: boolean;
  inputType: "int" | "char" | "none";
  finished: boolean;
  error?: string;
  worker?: ChildProcess;
  createdAt: number;
}

const sessions = new Map<string, SessionState>();

const workerPath = path.resolve(__dirname, "mepaWorker.js");
const workerTsPath = path.resolve(__dirname, "mepaWorker.ts");

function getWorkerExecPath(): { filename: string; execArgv?: string[] } {
  if (process.env.TS_NODE_DEV || __filename.endsWith(".ts")) {
    return {
      filename: workerTsPath,
      execArgv: ["-r", "ts-node/register"],
    };
  }
  return { filename: workerPath };
}

export function startMepaSession(source: string): string {
  const id = `session_${Date.now()}_${Math.random().toString(36).substring(2, 7)}`;
  const { filename, execArgv } = getWorkerExecPath();

  const state: SessionState = {
    id,
    outputs: [],
    waitingInput: false,
    inputType: "none",
    finished: false,
    createdAt: Date.now(),
  };

  try {
    const worker = fork(filename, [], { execArgv: execArgv || [] });

    state.worker = worker;
    sessions.set(id, state);

    worker.send({ type: "init", source });

    worker.on("message", (msg: any) => {
      if (!msg) return;

      if (msg.type === "output") {
        if (msg.text) state.outputs.push(msg.text);
        if (msg.inputType === "int" || msg.inputType === "char") {
          state.waitingInput = true;
          state.inputType = msg.inputType;
        } else {
          state.waitingInput = false;
          state.inputType = "none";
        }
      } else if (msg.type === "finished") {
        state.finished = true;
        state.waitingInput = false;
        state.inputType = "none";
      } else if (msg.type === "error") {
        state.error = msg.error;
        state.finished = true;
        state.waitingInput = false;
      }
    });

    worker.on("error", (err: any) => {
      state.error = err?.message || "Erro no processo de execução da MEPA.";
      state.finished = true;
      state.waitingInput = false;
    });

    worker.on("exit", () => {
      state.finished = true;
    });
  } catch (err: any) {
    state.error = err?.message || "Falha ao iniciar processo da MEPA.";
    state.finished = true;
    sessions.set(id, state);
  }

  cleanOldSessions();
  return id;
}

export function pollMepaSession(id: string) {
  const session = sessions.get(id);
  if (!session) {
    return { error: "Sessão não encontrada.", finished: true, outputs: [], waitingInput: false, inputType: "none" };
  }

  const result = {
    sessionId: session.id,
    outputs: [...session.outputs],
    waitingInput: session.waitingInput,
    inputType: session.inputType,
    finished: session.finished,
    error: session.error,
  };

  session.outputs = [];
  return result;
}

export function sendMepaInput(id: string, value: string | number) {
  const session = sessions.get(id);
  if (!session || !session.worker || session.finished) return false;

  session.waitingInput = false;

  let parsedVal: string | number = value;
  if (session.inputType === "int") {
    parsedVal = typeof value === "number" ? value : parseInt(String(value), 10) || 0;
  } else if (session.inputType === "char") {
    parsedVal = String(value).charAt(0) || " ";
  }

  session.worker.send({ type: "input", value: parsedVal });
  return true;
}

export function stopMepaSession(id: string) {
  const session = sessions.get(id);
  if (session) {
    if (session.worker) {
      session.worker.kill();
    }
    session.finished = true;
    sessions.delete(id);
  }
}

function cleanOldSessions() {
  const now = Date.now();
  const maxAge = 10 * 60 * 1000;
  for (const [id, session] of sessions.entries()) {
    if (now - session.createdAt > maxAge) {
      if (session.worker) session.worker.kill();
      sessions.delete(id);
    }
  }
}