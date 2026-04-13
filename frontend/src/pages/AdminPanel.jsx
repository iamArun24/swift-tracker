import { useEffect, useState } from "react";
import DashboardLayout from "../layouts/DashboardLayout";
import Card from "../components/Card";
import StatCard from "../components/StatCard";
import RotatingText from "../components/RotatingText";
import ElectricBorder from "../components/ElectricBorder";
import SectionBorder from "../components/SectionBorder";
import api from "../services/api";
import { BarChart, Bar, XAxis, YAxis, Tooltip, ResponsiveContainer } from "recharts";
import { usePageReveal } from "../hooks/usePageReveal";

export default function AdminPanel() {
  usePageReveal();
  const [dashboard, setDashboard] = useState(null);
  const [revenue, setRevenue] = useState([]);

  useEffect(() => {
    api
      .get("/analytics/dashboard")
      .then((res) => setDashboard(res.data?.data || res.data))
      .catch(() => setDashboard(null));

    api
      .get("/analytics/revenue")
      .then((res) => setRevenue(res.data?.data || []))
      .catch(() => setRevenue([]));
  }, []);

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

      <SectionBorder className="mt-4" radius={16}>
        <Card className="reveal" title="Revenue Analytics">
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
    </DashboardLayout>
  );
}
