import { Router } from "express";
import { z } from "zod";
import { parseSourceLALG } from "../services/syntactic";

export const syntacticRouter = Router();

const ParseBody = z.object({
  source: z.string().min(1).max(200_000),
});

syntacticRouter.post("/parse/lalg", (req, res) => {
  const parsed = ParseBody.safeParse(req.body);

  if (!parsed.success) {
    return res.status(400).json({
      error: "Invalid body",
      details: parsed.error.flatten(),
    });
  }

  try {
    const result = parseSourceLALG(parsed.data.source);

    return res.json(result);
  } catch (err: any) {
    return res.status(500).json({
      error: "Syntactic analysis failed",
      message: err?.message ?? String(err),
    });
  }
});
