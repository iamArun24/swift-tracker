import { useEffect, useState } from "react";
import DashboardLayout from "../layouts/DashboardLayout";
import Card from "../components/Card";
import StatCard from "../components/StatCard";
import RotatingText from "../components/RotatingText";
import ElectricBorder from "../components/ElectricBorder";
import SectionBorder from "../components/SectionBorder";
import Button from "../components/Button";
import api from "../services/api";
import { BarChart, Bar, XAxis, YAxis, Tooltip, ResponsiveContainer } from "recharts";
import { usePageReveal } from "../hooks/usePageReveal";
import { titleCase } from "../utils/format";

export default function AdminPanel() {
  usePageReveal();
  const [dashboard, setDashboard] = useState(null);
  const [revenue, setRevenue] = useState([]);
  const [shipments, setShipments] = useState([]);
  const [agents, setAgents] = useState([]);
  const [loadingAssign, setLoadingAssign] = useState(null);

  useEffect(() => {
    loadDashboard();
    loadShipments();
    loadAgents();
  }, []);

  const loadDashboard = () => {
    api
      .get("/analytics/dashboard")
      .then((res) => setDashboard(res.data?.data || res.data))
      .catch(() => setDashboard(null));

    api
      .get("/analytics/revenue")
      .then((res) => setRevenue(res.data?.data || []))
      .catch(() => setRevenue([]));
  };

  const loadShipments = () => {
    api
      .get("/admin/shipments")
      .then((res) => setShipments(res.data?.data || res.data?.items || []))
      .catch(() => setShipments([]));
  };

  const loadAgents = () => {
    api
      .get("/admin/users?role=AGENT")
      .then((res) => {
        const users = res.data?.data || [];
        setAgents(users.filter(u => u.role === "AGENT"));
      })
      .catch(() => setAgents([]));
  };

  const assignAgent = async (shipmentId, agentId) => {
    if (!agentId) return;
    setLoadingAssign(shipmentId);
    try {
      await api.patch(`/admin/shipments/${shipmentId}/assign`, { agent_id: parseInt(agentId) });
      await loadShipments(); // Refresh list to show assignment
    } catch (err) {
      console.error(err);
    }
    setLoadingAssign(null);
  };

  return (
    <DashboardLayout role="admin">
      <div className="mb-4 flex flex-wrap items-center justify-between gap-3">
        <h1 className="text-2xl font-black tracking-wide text-white sm:text-3xl">Admin Command</h1>
        <ElectricBorder color="#ffffff" speed={0.8} chaos={0.075} thickness={1.4} borderRadius={9}>
          <RotatingText
            texts={["Revenue Intelligence", "Fleet Performance", "Network Visibility"]}
            mainClassName="inline-flex rounded-md bg-white px-2.5 py-1 text-xs font-bold uppercase tracking-[0.14em] text-black sm:text-sm"
            staggerFrom="center"
            initial={{ y: "100%" }}
            animate={{ y: 0 }}
            exit={{ y: "-120%" }}
            staggerDuration={0.015}
            splitLevelClassName="overflow-hidden pb-0.5"
            transition={{ type: "spring", damping: 30, stiffness: 400 }}
            rotationInterval={2500}
          />
        </ElectricBorder>
      </div>

      <div className="grid gap-4 md:grid-cols-4">
        <StatCard label="Total Shipments" value={dashboard?.total_shipments ?? "--"} />
        <StatCard label="Delivered" value={dashboard?.delivered ?? "--"} />
        <StatCard label="Success Rate" value={dashboard?.success_rate ? `${dashboard.success_rate}%` : "--"} />
        <StatCard label="Revenue" value={dashboard?.revenue ?? "--"} />
      </div>

      <div className="mt-4 grid gap-4 lg:grid-cols-[1fr_1fr]">
        <SectionBorder radius={16}>
          <Card className="reveal h-full" title="Revenue Analytics">
            <div className="h-72 w-full">
              <ResponsiveContainer width="100%" height="100%">
                <BarChart data={revenue}>
                  <XAxis dataKey="month" stroke="rgba(255, 255, 255, 0.68)" />
                  <YAxis stroke="rgba(255, 255, 255, 0.68)" />
                  <Tooltip
                    contentStyle={{
                      background: "rgba(10, 10, 10, 0.95)",
                      border: "1px solid rgba(255, 255, 255, 0.25)",
                      borderRadius: "10px",
                      color: "#fff",
                    }}
                    labelStyle={{ color: "#fff" }}
                    itemStyle={{ color: "#fff" }}
                  />
                  <Bar dataKey="revenue" fill="#f0f0f0" radius={6} />
                </BarChart>
              </ResponsiveContainer>
            </div>
          </Card>
        </SectionBorder>

        <SectionBorder radius={16}>
          <Card className="reveal h-[360px] flex flex-col" title="Shipment Management">
            <div className="flex-1 overflow-auto pr-2 space-y-2">
              {shipments.map((s) => (
                <div data-magic-card key={s.id} className="rounded-xl border border-white/10 bg-white/5 p-3 text-sm">
                  <div className="flex justify-between mb-2">
                    <span className="font-semibold">{s.tracking_number}</span>
                    <span className="text-emerald-400">{titleCase(s.current_status || "BOOKED")}</span>
                  </div>
                  <div className="mb-3 text-white/70">
                    {s.sender_city} → {s.recipient_city}
                  </div>
                  
                  {s.current_status === "BOOKED" || s.current_status === "PENDING" ? (
                    <div className="flex gap-2 items-center">
                      <select 
                        className="field py-1 px-2 text-xs h-auto min-h-0 w-full"
                        onChange={(e) => assignAgent(s.id, e.target.value)}
                        disabled={loadingAssign === s.id}
                        defaultValue=""
                      >
                        <option value="" disabled>Assign to Agent...</option>
                        {agents.map(a => (
                          <option key={a.id} value={a.id}>{a.username} ({a.email})</option>
                        ))}
                      </select>
                    </div>
                  ) : (
                    <div className="text-xs text-white/50">
                      Assigned to Agent ID: {s.agent_id || "Unknown"}
                    </div>
                  )}
                </div>
              ))}
              {shipments.length === 0 && <p className="text-white/50">No shipments found.</p>}
            </div>
          </Card>
        </SectionBorder>
      </div>
    </DashboardLayout>
  );
}
