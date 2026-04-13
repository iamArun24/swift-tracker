import { Navigate, Route, Routes } from "react-router-dom";
import HomePageV2 from "./pages/HomePageV2";
import LoginPage from "./pages/LoginPage";
import SenderDashboard from "./pages/SenderDashboard";
import AdminPanel from "./pages/AdminPanel";
import AgentPanel from "./pages/AgentPanel";
import TrackingPage from "./pages/TrackingPage";
import NotFoundPage from "./pages/NotFoundPage";
import MagicCardEffects from "./components/MagicCardEffects";

function App() {
  return (
    <>
      <MagicCardEffects />
      <Routes>
        <Route path="/" element={<HomePageV2 />} />
        <Route path="/login" element={<LoginPage />} />
        <Route path="/dashboard" element={<SenderDashboard />} />
        <Route path="/admin" element={<AdminPanel />} />
        <Route path="/agent" element={<AgentPanel />} />
        <Route path="/track/:id" element={<TrackingPage />} />
        <Route path="/track" element={<Navigate to="/track/demo" replace />} />
        <Route path="*" element={<NotFoundPage />} />
      </Routes>
    </>
  );
}

export default App;
