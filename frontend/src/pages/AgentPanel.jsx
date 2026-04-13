import { useEffect, useState } from "react";
import DashboardLayout from "../layouts/DashboardLayout";
import Card from "../components/Card";
import Button from "../components/Button";
import RotatingText from "../components/RotatingText";
import ElectricBorder from "../components/ElectricBorder";
import SectionBorder from "../components/SectionBorder";
import api from "../services/api";

export default function AgentPanel() {
  const [deliveries, setDeliveries] = useState([]);

  useEffect(() => {
    let cancelled = false;

    (async () => {
      try {
        const { data } = await api.get("/agent/deliveries");
        if (!cancelled) {
          setDeliveries(data?.data || []);
        }
      } catch {
        if (!cancelled) {
          setDeliveries([]);
        }
      }
    })();

    return () => {
      cancelled = true;
    };
  }, []);

  async function load() {
    try {
      const { data } = await api.get("/agent/deliveries");
      setDeliveries(data?.data || []);
    } catch {
      setDeliveries([]);
    }
  }

  async function updateStatus(id, status) {
    try {
      await api.patch(`/agent/deliveries/${id}/status`, { status, location: "On route" });
      await load();
    } catch {
      // Keep current state when update fails.
    }
  }

  return (
    <DashboardLayout role="agent">
      <div className="mb-4 flex flex-wrap items-center justify-between gap-3">
        <h1 className="text-2xl font-black tracking-wide text-white sm:text-3xl">Agent Mission</h1>
        <ElectricBorder color="#ffffff" speed={0.85} chaos={0.08} thickness={1.4} borderRadius={9}>
          <RotatingText
            texts={["Pickup Ready", "Out For Delivery", "Proof of Drop"]}
            mainClassName="inline-flex rounded-md bg-white px-2.5 py-1 text-xs font-bold uppercase tracking-[0.14em] text-black sm:text-sm"
            staggerFrom="last"
            initial={{ y: "100%" }}
            animate={{ y: 0 }}
            exit={{ y: "-120%" }}
            staggerDuration={0.015}
            splitLevelClassName="overflow-hidden pb-0.5"
            transition={{ type: "spring", damping: 30, stiffness: 400 }}
            rotationInterval={2200}
          />
        </ElectricBorder>
      </div>

      <SectionBorder radius={16}>
        <Card title="Assigned Deliveries">
          <div className="space-y-3">
            {deliveries.map((d) => (
              <div data-magic-card key={d.id} className="rounded-xl border border-white/10 bg-white/5 p-3">
                <p className="font-semibold">{d.tracking_number}</p>
                <p className="text-sm text-white/70">{d.current_status}</p>
                <div className="mt-2 flex flex-wrap gap-2">
                  <Button onClick={() => updateStatus(d.id, "OUT_FOR_DELIVERY")}>Out for delivery</Button>
                  <Button className="btn-invert" onClick={() => updateStatus(d.id, "DELIVERED")}>
                    Delivered
                  </Button>
                </div>
              </div>
            ))}
            {deliveries.length === 0 ? <p className="text-white/60">No deliveries assigned.</p> : null}
          </div>
        </Card>
      </SectionBorder>
    </DashboardLayout>
  );
}
