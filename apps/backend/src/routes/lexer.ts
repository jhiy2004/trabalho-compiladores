import { Router } from "express";
import { z } from "zod";
import { lexSource } from "../services/lexer";

export const lexerRouter = Router();

const LexBody = z.object({
  source: z.string().min(1).max(200_000),
});

lexerRouter.post("/lex", (req, res) => {
  const parsed = LexBody.safeParse(req.body);
  if (!parsed.success) {
    return res.status(400).json({ error: "Invalid body", details: parsed.error.flatten() });
  }

  try {
    const tokens = lexSource(parsed.data.source);
    return res.json({ tokens });
  } catch (err: any) {
    return res.status(500).json({ error: "Lexer failed", message: err?.message ?? String(err) });
  }
});