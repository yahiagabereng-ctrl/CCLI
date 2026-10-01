import { Navigate, Route, Routes } from "react-router-dom";
import { Nav } from "./components/Nav";
import { ChatPage } from "./pages/ChatPage";
import { KnowledgePage } from "./pages/KnowledgePage";

export default function App() {
  return (
    <div className="app-shell">
      <Nav />
      <Routes>
        <Route path="/" element={<KnowledgePage />} />
        {/* Internal only — not linked in public nav */}
        <Route path="/lab" element={<ChatPage />} />
        <Route path="/assistant" element={<Navigate to="/lab" replace />} />
        <Route path="*" element={<Navigate to="/" replace />} />
      </Routes>
    </div>
  );
}
