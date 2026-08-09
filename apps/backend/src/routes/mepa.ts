import { Router } from "express";
import { z } from "zod";
import {
  startMepaSession,
  pollMepaSession,
  sendMepaInput,
  stopMepaSession,
} from "../services/mepaSession";

export const mepaRouter = Router();

const StartBody = z.object({
  source: z.string().min(1).max(200_000),
});

const InputBody = z.object({
  value: z.union([z.string(), z.number()]),
});

mepaRouter.post("/mepa/start", (req, res) => {
  const parsed = StartBody.safeParse(req.body);
  if (!parsed.success) {
    return res.status(400).json({ error: "Código-fonte é obrigatório." });
  }

  try {
    const sessionId = startMepaSession(parsed.data.source);
    return res.json({ sessionId });
  } catch (err: any) {
    return res.status(500).json({ error: err?.message || String(err) });
  }
});

mepaRouter.get("/mepa/poll/:id", (req, res) => {
  const { id } = req.params;
  const data = pollMepaSession(id);
  return res.json(data);
});

mepaRouter.post("/mepa/input/:id", (req, res) => {
  const { id } = req.params;
  const parsed = InputBody.safeParse(req.body);
  if (!parsed.success) {
    return res.status(400).json({ error: "Valor de entrada é obrigatório." });
  }

  const success = sendMepaInput(id, parsed.data.value);
  return res.json({ success });
});

mepaRouter.post("/mepa/stop/:id", (req, res) => {
  const { id } = req.params;
  stopMepaSession(id);
  return res.json({ success: true });
});
