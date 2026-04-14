import { useEffect, useMemo, useState } from "react";
import DashboardLayout from "../layouts/DashboardLayout";
import Card from "../components/Card";
import Button from "../components/Button";
import StatCard from "../components/StatCard";
import ShipmentActionButton from "../components/ShipmentActionButton";
import RotatingText from "../components/RotatingText";
import ElectricBorder from "../components/ElectricBorder";
import SectionBorder from "../components/SectionBorder";
import Modal from "../components/Modal";
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
  const [confirmation, setConfirmation] = useState(null);
  const [copied, setCopied] = useState(false);

  useEffect(() => {
    loadMyShipments();
  }, [loadMyShipments]);

  const delivered = useMemo(
    () => shipments.filter((s) => s.current_status === "DELIVERED").length,
    [shipments]
  );

  const handleCopyTracking = () => {
    if (confirmation?.tracking_number) {
      navigator.clipboard.writeText(confirmation.tracking_number);
      setCopied(true);
      setTimeout(() => setCopied(false), 2000);
    }
  };

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
                  const created = await createShipment(form);
                  await loadMyShipments();
                  setForm(initial);
                  setEstimate(null);
                  setConfirmation(created);
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

      {/* ── Shipment Confirmation Modal ── */}
      <Modal
        open={!!confirmation}
        onClose={() => setConfirmation(null)}
        title="Shipment Accepted ✓"
      >
        <div className="space-y-4">
          {/* Success animation */}
          <div className="flex justify-center">
            <div className="relative flex h-20 w-20 items-center justify-center">
              <div className="absolute inset-0 animate-ping rounded-full bg-emerald-500/20" />
              <div className="relative flex h-16 w-16 items-center justify-center rounded-full bg-gradient-to-br from-emerald-400 to-emerald-600 shadow-lg shadow-emerald-500/30">
                <svg className="h-8 w-8 text-white" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={3}>
                  <path strokeLinecap="round" strokeLinejoin="round" d="M5 13l4 4L19 7" />
                </svg>
              </div>
            </div>
          </div>

          <p className="text-center text-sm text-white/70">
            Your shipment has been successfully placed and is being processed.
          </p>

          {/* Tracking Number */}
          <div className="rounded-xl border border-emerald-500/30 bg-emerald-500/10 p-4 text-center">
            <p className="mb-1 text-xs font-semibold uppercase tracking-widest text-emerald-400">Tracking Number</p>
            <p className="text-xl font-black tracking-wider text-white">
              {confirmation?.tracking_number || "—"}
            </p>
            <button
              onClick={handleCopyTracking}
              className="mt-2 inline-flex items-center gap-1.5 rounded-lg bg-white/10 px-3 py-1.5 text-xs font-medium text-white/80 transition hover:bg-white/20 hover:text-white"
            >
              {copied ? (
                <>
                  <svg className="h-3.5 w-3.5 text-emerald-400" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                    <path strokeLinecap="round" strokeLinejoin="round" d="M5 13l4 4L19 7" />
                  </svg>
                  Copied!
                </>
              ) : (
                <>
                  <svg className="h-3.5 w-3.5" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                    <path strokeLinecap="round" strokeLinejoin="round" d="M8 16H6a2 2 0 01-2-2V6a2 2 0 012-2h8a2 2 0 012 2v2m-6 12h8a2 2 0 002-2v-8a2 2 0 00-2-2h-8a2 2 0 00-2 2v8a2 2 0 002 2z" />
                  </svg>
                  Copy
                </>
              )}
            </button>
          </div>

          {/* Shipment Details */}
          <div className="grid grid-cols-2 gap-3">
            <div className="rounded-lg border border-white/10 bg-white/5 p-3">
              <p className="text-[10px] font-semibold uppercase tracking-wider text-white/50">Recipient</p>
              <p className="mt-0.5 text-sm font-medium text-white">
                {confirmation?.recipient_name || confirmation?.recipient_city || "—"}
              </p>
            </div>
            <div className="rounded-lg border border-white/10 bg-white/5 p-3">
              <p className="text-[10px] font-semibold uppercase tracking-wider text-white/50">Status</p>
              <p className="mt-0.5 text-sm font-medium text-emerald-400">
                {titleCase(confirmation?.current_status || "BOOKED")}
              </p>
            </div>
            <div className="rounded-lg border border-white/10 bg-white/5 p-3">
              <p className="text-[10px] font-semibold uppercase tracking-wider text-white/50">Service</p>
              <p className="mt-0.5 text-sm font-medium text-white">
                {titleCase(confirmation?.service_type || "STANDARD")}
              </p>
            </div>
            <div className="rounded-lg border border-white/10 bg-white/5 p-3">
              <p className="text-[10px] font-semibold uppercase tracking-wider text-white/50">Cost</p>
              <p className="mt-0.5 text-sm font-medium text-white">
                {confirmation?.total_cost ? currency(confirmation.total_cost) : "—"}
              </p>
            </div>
          </div>

          <button
            onClick={() => setConfirmation(null)}
            className="neo-btn w-full rounded-xl px-4 py-2.5 text-sm font-bold"
          >
            Done
          </button>
        </div>
      </Modal>
    </DashboardLayout>
  );
}

