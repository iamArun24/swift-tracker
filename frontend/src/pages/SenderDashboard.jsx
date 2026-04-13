import { useEffect, useMemo, useState } from "react";
import DashboardLayout from "../layouts/DashboardLayout";
import Card from "../components/Card";
import Button from "../components/Button";
import StatCard from "../components/StatCard";
import ShipmentActionButton from "../components/ShipmentActionButton";
import RotatingText from "../components/RotatingText";
import ElectricBorder from "../components/ElectricBorder";
import SectionBorder from "../components/SectionBorder";
import { useShipmentStore } from "../store/shipmentStore";
import { usePageReveal } from "../hooks/usePageReveal";
import { currency, titleCase } from "../utils/format";

const initial = {
  sender_name: "",
  sender_phone: "",
  sender_email: "",
  sender_address: "",
  sender_city: "Delhi",
  sender_state: "Delhi",
  sender_pincode: "110001",
  recipient_name: "",
  recipient_phone: "",
  recipient_email: "",
  recipient_address: "",
  recipient_city: "Mumbai",
  recipient_state: "Maharashtra",
  recipient_pincode: "400001",
  package_type: "PARCEL",
  weight_grams: 1000,
  length_cm: 20,
  width_cm: 20,
  height_cm: 20,
  package_description: "General",
  declared_value: 1000,
  service_type: "STANDARD",
  is_fragile: false,
  requires_signature: false,
  is_cod: false,
  cod_amount: 0,
  special_instructions: "",
  distance_km: 500,
};

export default function SenderDashboard() {
  usePageReveal();
  const { shipments, loadMyShipments, createShipment, getPriceEstimate, loading } = useShipmentStore();
  const [form, setForm] = useState(initial);
  const [estimate, setEstimate] = useState(null);

  useEffect(() => {
    loadMyShipments();
  }, [loadMyShipments]);

  const delivered = useMemo(
    () => shipments.filter((s) => s.current_status === "DELIVERED").length,
    [shipments]
  );

  return (
    <DashboardLayout role="sender">
      <div className="mb-4 flex flex-wrap items-center justify-between gap-3">
        <h1 className="text-2xl font-black tracking-wide text-white sm:text-3xl">Sender Operations</h1>
        <ElectricBorder color="#ffffff" speed={0.85} chaos={0.08} thickness={1.4} borderRadius={9}>
          <RotatingText
            texts={["Create Shipments", "Monitor Transit", "Deliver with Precision"]}
            mainClassName="inline-flex rounded-md bg-white px-2.5 py-1 text-xs font-bold uppercase tracking-[0.14em] text-black sm:text-sm"
            staggerFrom="last"
            initial={{ y: "100%" }}
            animate={{ y: 0 }}
            exit={{ y: "-120%" }}
            staggerDuration={0.016}
            splitLevelClassName="overflow-hidden pb-0.5"
            transition={{ type: "spring", damping: 30, stiffness: 400 }}
            rotationInterval={2400}
          />
        </ElectricBorder>
      </div>

      <div className="grid gap-4 md:grid-cols-3">
        <StatCard label="My Shipments" value={shipments.length} hint="All records" />
        <StatCard label="Delivered" value={delivered} hint="Successful drops" />
        <StatCard label="In Progress" value={shipments.length - delivered} hint="Active pipeline" />
      </div>

      <div className="mt-4 grid gap-4 lg:grid-cols-2">
        <SectionBorder radius={16}>
          <Card title="Create Shipment" className="reveal">
            <div className="grid gap-2 sm:grid-cols-2">
              {[
                ["sender_name", "Sender name"],
                ["sender_phone", "Sender phone"],
                ["sender_email", "Sender email"],
                ["recipient_name", "Recipient name"],
                ["recipient_phone", "Recipient phone"],
                ["recipient_email", "Recipient email"],
                ["weight_grams", "Weight (g)"],
                ["distance_km", "Distance (km)"],
              ].map(([key, label]) => (
                <input
                  key={key}
                  className="field"
                  placeholder={label}
                  value={form[key]}
                  onChange={(e) => setForm({ ...form, [key]: e.target.value })}
                />
              ))}
            </div>

            <div className="mt-3 flex flex-wrap gap-2">
              <Button
                onClick={async () => {
                  const priced = await getPriceEstimate(form);
                  setEstimate(priced);
                }}
              >
                Preview Cost
              </Button>
              <ShipmentActionButton
                className="min-w-[230px]"
                disabled={loading}
                onAction={async () => {
                  await createShipment(form);
                  await loadMyShipments();
                  setForm(initial);
                }}
              />
            </div>

            {estimate ? <p className="mt-3 text-white/85">Estimated: {currency(estimate.total_cost)}</p> : null}
          </Card>
        </SectionBorder>

        <Card title="Recent Shipments" className="reveal">
          <div className="max-h-[420px] space-y-2 overflow-auto pr-1">
            {shipments.map((shipment) => (
              <div data-magic-card key={shipment.id} className="rounded-xl border border-white/10 bg-white/5 p-3">
                <p className="font-semibold">{shipment.tracking_number}</p>
                <p className="text-sm text-white/70">
                  {shipment.recipient_name || "Recipient"} · {titleCase(shipment.current_status || "BOOKED")}
                </p>
              </div>
            ))}
            {shipments.length === 0 ? <p className="text-white/60">No shipments yet.</p> : null}
          </div>
        </Card>
      </div>
    </DashboardLayout>
  );
}
