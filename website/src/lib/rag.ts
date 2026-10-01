export interface RagCitation {
  source_id?: string;
  page?: number;
  score?: number;
  text?: string;
}

export interface RagQueryResponse {
  answer?: string;
  citations?: RagCitation[];
}

const RAG_BASE = "/api/rag";

export async function fetchRagHealth(): Promise<"ok" | "down"> {
  try {
    const res = await fetch(`${RAG_BASE}/health`, {
      signal: AbortSignal.timeout(5000),
    });
    if (!res.ok) return "down";
    const data = (await res.json()) as { status?: string };
    return data.status === "ok" ? "ok" : "down";
  } catch {
    return "down";
  }
}

export async function queryRag(
  question: string,
  sourceIds: string[],
  groundingPrompt: string,
): Promise<RagQueryResponse> {
  const body = {
    tenant_id: "telematry",
    bot_id: "default",
    query: question,
    source_ids: sourceIds,
    top_k: 12,
    answer_language: "en",
    clarify: false,
    answer_in_steps: true,
    grounding_prompt: groundingPrompt,
  };

  const res = await fetch(`${RAG_BASE}/v1/query`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });

  if (!res.ok) {
    const text = await res.text();
    throw new Error(text || `RAG query failed (${res.status})`);
  }

  return res.json() as Promise<RagQueryResponse>;
}
