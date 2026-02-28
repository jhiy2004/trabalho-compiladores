import { useMemo, useState } from "react";

type Props = {
  logs: string[];
  errors: string[];
};

export default function BottomTabs({ logs, errors }: Props) {
  const [tab, setTab] = useState<"errors" | "logs">("errors");

  const items = useMemo(() => (tab === "logs" ? logs : errors), [tab, logs, errors]);

  return (
    <div className="h-44 bg-cardSoft shadow-soft p-4 rounded-xl2 mx-6 mb-6">
      <div className="flex gap-6 mb-3 border-b border-black/5 pb-2">
        <button
          onClick={() => setTab("errors")}
          className={tab === "errors" ? "font-semibold" : "opacity-70"}
        >
          Erros
        </button>
        <button
          onClick={() => setTab("logs")}
          className={tab === "logs" ? "font-semibold" : "opacity-70"}
        >
          Logs
        </button>
      </div>

      <div className="text-sm overflow-y-auto h-28 pr-2">
        {items.length === 0 ? (
          <div className="opacity-60">Nada por aqui ainda.</div>
        ) : tab === "logs" ? (
          items.map((l, i) => (
            <div key={i} className="opacity-80">
              {l}
            </div>
          ))
        ) : (
          items.map((e, i) => (
            <div key={i} className="text-red-500">
              {e}
            </div>
          ))
        )}
      </div>
    </div>
  );
}