import { useEffect, useRef, useState } from "react";
import { MoreHorizontal, Braces, ScanText, Network, Sigma, Table2 } from "lucide-react";
import type { ViewKey } from "../ui/views";
import { VIEW_LABEL } from "../ui/views";

type Props = {
  current: ViewKey;
  onPick: (v: ViewKey) => void;
};

const items: Array<{ key: ViewKey; icon: React.ReactNode }> = [
  { key: "code", icon: <Braces className="w-5 h-5" /> },
  { key: "lex", icon: <ScanText className="w-5 h-5" /> },
  { key: "symbols", icon: <Table2 className="w-5 h-5" /> },
  { key: "syntax", icon: <Network className="w-5 h-5" /> },
  { key: "semantic", icon: <Sigma className="w-5 h-5" /> },
];

export default function LeftRail({ current, onPick }: Props) {
  const [open, setOpen] = useState(false);
  const ref = useRef<HTMLDivElement | null>(null);

  useEffect(() => {
    const onDoc = (e: MouseEvent) => {
      if (!ref.current) return;
      if (!ref.current.contains(e.target as Node)) setOpen(false);
    };
    document.addEventListener("mousedown", onDoc);
    return () => document.removeEventListener("mousedown", onDoc);
  }, []);

  return (
    <div className="w-16 bg-cardSoft shadow-soft flex flex-col items-center py-4 gap-3 border-r border-black/5">
      <div className="relative" ref={ref}>
        <button
          type="button"
          onClick={() => setOpen((v) => !v)}
          className="p-2 rounded-xl hover:bg-black/5 transition"
          aria-label="Menu"
        >
          <MoreHorizontal className="w-5 h-5" />
        </button>

        {open && (
          <div className="absolute left-14 top-0 z-50 w-56 bg-cardSoft shadow-soft rounded-xl2 border border-black/5 overflow-hidden">
            <div className="px-3 py-2 text-xs opacity-70">Abrir painel</div>
            {items.map((it) => (
              <button
                key={it.key}
                className={[
                  "w-full flex items-center gap-3 px-3 py-2 text-sm hover:bg-black/5 transition",
                  it.key === current ? "bg-tintSoft/70 font-semibold" : "",
                ].join(" ")}
                onClick={() => {
                  onPick(it.key);
                  setOpen(false);
                }}
              >
                <span className="opacity-80">{it.icon}</span>
                {VIEW_LABEL[it.key]}
              </button>
            ))}
          </div>
        )}
      </div>

      {/* icones (podemos mudar) */}
      <div className="mt-2 flex flex-col gap-2">
        {items.map((it) => (
          <button
            key={it.key}
            onClick={() => onPick(it.key)}
            className={[
              "p-2 rounded-xl hover:bg-black/5 transition",
              it.key === current ? "bg-tintSoft/80" : "",
            ].join(" ")}
            aria-label={VIEW_LABEL[it.key]}
            title={VIEW_LABEL[it.key]}
          >
            {it.icon}
          </button>
        ))}
      </div>
    </div>
  );
}