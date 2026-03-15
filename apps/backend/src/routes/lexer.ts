import { Router } from "express";
import { z } from "zod";
import { lexSourceCalc, lexSourceLALG } from "../services/lexer";

export const lexerRouter = Router();

const LexBody = z.object({
  source: z.string().min(1).max(200_000),
});

lexerRouter.post("/lex/calc", (req, res) => {
  const parsed = LexBody.safeParse(req.body);
  if (!parsed.success) {
    return res.status(400).json({ error: "Invalid body", details: parsed.error.flatten() });
  }

  try {
    const tokens = lexSourceCalc(parsed.data.source);
    return res.json({ tokens });
  } catch (err: any) {
    return res
      .status(500)
      .json({ error: "Lexer failed", message: err?.message ?? String(err) });
  }
});

lexerRouter.post("/lex/lalg", (req, res) => {
  const parsed = LexBody.safeParse(req.body);
  if (!parsed.success) {
    return res.status(400).json({ error: "Invalid body", details: parsed.error.flatten() });
  }

  try {
    const tokens = lexSourceLALG(parsed.data.source);
    return res.json({ tokens });
  } catch (err: any) {
    return res
      .status(500)
      .json({ error: "Lexer failed", message: err?.message ?? String(err) });
  }
});
