import express from "express";
import cors from "cors";
import morgan from "morgan";

import { lexerRouter } from "./routes/lexer";

const app = express();

app.use(cors());
app.use(express.json({ limit: "2mb" }));
app.use(morgan("dev"));

app.get("/health", (_req, res) => res.json({ ok: true }));

app.use("/api", lexerRouter);

const PORT = process.env.PORT ? Number(process.env.PORT) : 3001;
app.listen(PORT, () => console.log(`Backend on http://localhost:${PORT}`));