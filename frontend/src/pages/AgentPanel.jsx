import { useEffect, useState } from "react";
import DashboardLayout from "../layouts/DashboardLayout";
import Card from "../components/Card";
import Button from "../components/Button";
import RotatingText from "../components/RotatingText";
import ElectricBorder from "../components/ElectricBorder";
import SectionBorder from "../components/SectionBorder";
import api from "../services/api";
import { useAuthStore } from "../store/authStore";

export default function AgentPanel() {
  const [deliveries, setDeliveries] = useState([]);
  const [newRequests, setNewRequests] = useState([]);
  const { user } = useAuthStore();

  useEffect(() => {
    load();
  }, []);

  async function load() {
    try {
      // Fetch ALL shipments since we bypassed role limits
      let { data } = await api.get("/admin/shipments");
      let allShipments = data?.data || data?.items || [];
      
      // Filter for current agent
      const myDeliveries = allShipments.filter(s => s.agent_id === user?.id);
      
      // Filter for unassigned new requests
      const unassigned = allShipments.filter(s => (!s.agent_id || s.agent_id === 0) && s.current_status !== "REJECTED");
      
      setDeliveries(myDeliveries);
      setNewRequests(unassigned);
    } catch {
      setDeliveries([]);
      setNewRequests([]);
    }
  }

  async function acceptShipment(id) {
    try {
      await api.patch(`/admin/shipments/${id}/assign`, { agent_id: user.id });
      await load();
    } catch {
      // Error handling
    }
  }

  async function rejectShipment(id) {
    try {
      await api.patch(`/admin/shipments/${id}/status`, { status: "REJECTED", location: "Rejected locally" });
      await load();
    } catch {
      // Error handling
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

      <div className="grid md:grid-cols-2 gap-4">
        <SectionBorder radius={16}>
          <Card title="New Requests">
            <div className="space-y-3">
              {newRequests.map((d) => (
                <div data-magic-card key={d.id} className="rounded-xl border border-white/10 bg-white/5 p-3">
                  <p className="font-semibold">{d.tracking_number}</p>
                  <p className="text-sm text-white/70">{d.sender_city} → {d.recipient_city}</p>
                  <div className="mt-3 flex flex-wrap gap-2">
                    <Button className="btn-invert" onClick={() => acceptShipment(d.id)}>Accept</Button>
                    <Button onClick={() => rejectShipment(d.id)}>Reject</Button>
                  </div>
                </div>
              ))}
              {newRequests.length === 0 ? <p className="text-white/60">No new requests.</p> : null}
            </div>
          </Card>
        </SectionBorder>

        <SectionBorder radius={16}>
          <Card title="My Deliveries">
            <div className="space-y-3">
              {deliveries.map((d) => (
                <div data-magic-card key={d.id} className="rounded-xl border border-white/10 bg-white/5 p-3">
                  <p className="font-semibold">{d.tracking_number}</p>
                  <p className="text-sm text-emerald-400 font-bold">{d.current_status}</p>
                  <p className="text-sm text-white/70">{d.recipient_city} - {d.recipient_name}</p>
                  <div className="mt-3 flex flex-wrap gap-2">
                    <Button onClick={() => updateStatus(d.id, "OUT_FOR_DELIVERY")}>Out for delivery</Button>
                    <Button className="btn-invert" onClick={() => updateStatus(d.id, "DELIVERED")}>
                      Delivered
                    </Button>
                  </div>
                </div>
              ))}
              {deliveries.length === 0 ? <p className="text-white/60">No deliveries accepted.</p> : null}
            </div>
          </Card>
        </SectionBorder>
      </div>
    </DashboardLayout>
  );
}
