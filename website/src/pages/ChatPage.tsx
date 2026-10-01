import { useEffect, useRef, useState, type FormEvent } from "react";
import {
  GROUNDING_PROMPT,
  SOURCE_PACKS,
  SUGGESTED_PROMPTS,
} from "../lib/kbData";
import { fetchRagHealth, queryRag, type RagCitation } from "../lib/rag";

interface Message {
  role: "user" | "assistant";
  content: string;
  citations?: RagCitation[];
  error?: boolean;
}

/** Internal retrieval console — not linked from public navigation. */
export function ChatPage() {
  const [packId, setPackId] = useState(
    SOURCE_PACKS.find((p) => p.id === "vendor-bom")?.id ?? SOURCE_PACKS[0].id,
  );
  const [messages, setMessages] = useState<Message[]>([]);
  const [input, setInput] = useState("");
  const [busy, setBusy] = useState(false);
  const [health, setHealth] = useState<"ok" | "down" | "checking">("checking");
  const bottomRef = useRef<HTMLDivElement>(null);

  const pack = SOURCE_PACKS.find((p) => p.id === packId) ?? SOURCE_PACKS[0];

  useEffect(() => {
    let cancelled = false;
    (async () => {
      const h = await fetchRagHealth();
      if (!cancelled) setHealth(h);
    })();
    const t = window.setInterval(async () => {
      const h = await fetchRagHealth();
      if (!cancelled) setHealth(h);
    }, 20000);
    return () => {
      cancelled = true;
      window.clearInterval(t);
    };
  }, []);

  useEffect(() => {
    bottomRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages, busy]);

  async function send(question: string) {
    const q = question.trim();
    if (!q || busy) return;
    setInput("");
    setMessages((m) => [...m, { role: "user", content: q }]);
    setBusy(true);
    try {
      const res = await queryRag(q, pack.sources, GROUNDING_PROMPT);
      setMessages((m) => [
        ...m,
        {
          role: "assistant",
          content: res.answer?.trim() || "No answer returned.",
          citations: res.citations?.slice(0, 8),
        },
      ]);
    } catch (err) {
      const msg = err instanceof Error ? err.message : "Query failed.";
      setMessages((m) => [...m, { role: "assistant", content: msg, error: true }]);
    } finally {
      setBusy(false);
    }
  }

  function onSubmit(e: FormEvent) {
    e.preventDefault();
    void send(input);
  }

  return (
    <div className="chat-page">
      <aside className="chat-aside">
        <h2>Corpus pack</h2>
        <p>Scope retrieval to the documents for this question.</p>
        <ul className="pack-list">
          {SOURCE_PACKS.map((p) => (
            <li key={p.id}>
              <button
                type="button"
                className={p.id === packId ? "active" : undefined}
                onClick={() => setPackId(p.id)}
              >
                {p.label}
              </button>
            </li>
          ))}
        </ul>
        <p className={`rag-status ${health === "ok" ? "ok" : "bad"}`}>
          {health === "checking" && "Checking index…"}
          {health === "ok" && "Index online"}
          {health === "down" && "Index offline"}
        </p>
      </aside>

      <div className="chat-main">
        <header className="chat-header">
          <h1>Knowledge console</h1>
          <p>
            Internal retrieval · {pack.label} ({pack.sources.length} sources)
          </p>
        </header>

        <div className="chat-messages">
          {messages.length === 0 && !busy && (
            <div className="chat-empty">
              <h2>Query the programme corpus</h2>
              <p>CEI interfaces, TG-524 lab limits, protocol libs, licensing — with citations.</p>
              <div className="suggest">
                {SUGGESTED_PROMPTS.map((s) => (
                  <button key={s} type="button" onClick={() => void send(s)}>
                    {s}
                  </button>
                ))}
              </div>
            </div>
          )}

          {messages.map((m, i) => (
            <div
              key={`${m.role}-${i}`}
              className={`msg ${m.role}`}
              style={m.error ? { borderLeftColor: "var(--miss)" } : undefined}
            >
              {m.role === "user" ? (
                m.content
              ) : (
                <>
                  <p className="answer">{m.content}</p>
                  {m.citations && m.citations.length > 0 && (
                    <>
                      <p className="meta">Sources</p>
                      <ul className="citations">
                        {m.citations.map((c, j) => (
                          <li key={`${c.source_id}-${c.page}-${j}`}>
                            {c.source_id || "source"}
                            {c.page != null ? ` · p.${c.page}` : ""}
                          </li>
                        ))}
                      </ul>
                    </>
                  )}
                </>
              )}
            </div>
          ))}

          {busy && (
            <div className="msg assistant">
              <p className="answer loading-dot">Retrieving</p>
            </div>
          )}
          <div ref={bottomRef} />
        </div>

        <form className="chat-composer" onSubmit={onSubmit}>
          <textarea
            value={input}
            onChange={(e) => setInput(e.target.value)}
            placeholder="Query CEI 0-16, TG-524 lab, OpenWrt, protocol stacks…"
            rows={2}
            onKeyDown={(e) => {
              if (e.key === "Enter" && !e.shiftKey) {
                e.preventDefault();
                void send(input);
              }
            }}
            disabled={busy}
          />
          <button type="submit" disabled={busy || !input.trim()}>
            Query
          </button>
        </form>
      </div>
    </div>
  );
}
