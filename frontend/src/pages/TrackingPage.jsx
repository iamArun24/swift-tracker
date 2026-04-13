import { useEffect } from "react";
import { useParams } from "react-router-dom";
import Navbar from "../components/Navbar";
import Card from "../components/Card";
import Loader from "../components/Loader";
import { useTrackingStore } from "../store/trackingStore";
import { titleCase } from "../utils/format";
import AmbientLightRays from "../components/AmbientLightRays";
import SectionBorder from "../components/SectionBorder";

export default function TrackingPage() {
  const { id } = useParams();
  const { shipment, timeline, trackPublic, connectRealtime, disconnectRealtime, connected } = useTrackingStore();

  useEffect(() => {
    trackPublic(id);
    connectRealtime();
    return () => disconnectRealtime();
  }, [id, trackPublic, connectRealtime, disconnectRealtime]);

  return (
    <div className="ink-canvas min-h-screen text-white">
      <AmbientLightRays />
      <Navbar />
      <div className="ink-layer mx-auto max-w-5xl space-y-4 px-4 py-6">
        <SectionBorder radius={16}>
          <Card title="Public Tracking">
            <p className="text-sm text-white/70">Tracking Number: {id}</p>
            <p className="mt-2 inline-flex items-center gap-2 text-sm text-white/85">
              <span className={`status-dot ${connected ? "status-dot-online" : "status-dot-offline"}`} />
              Realtime: {connected ? "Connected" : "Offline"}
            </p>
          </Card>
        </SectionBorder>

        <Card title="Shipment Snapshot">
          {!shipment ? (
            <Loader text="Fetching shipment" />
          ) : (
            <div className="grid gap-2 sm:grid-cols-2">
              <p>Status: {titleCase(shipment.current_status || "booked")}</p>
              <p>Location: {shipment.current_location || "Pending"}</p>
              <p>Recipient: {shipment.recipient_name || "NA"}</p>
              <p>City: {shipment.recipient_city || "NA"}</p>
            </div>
          )}
        </Card>

        <SectionBorder radius={16}>
          <Card title="Timeline">
            <ol className="space-y-3">
              {timeline.map((event, index) => (
                <li data-magic-card key={`${event.created_at}-${index}`} className="rounded-xl border border-white/10 bg-white/5 p-3">
                  <p className="font-semibold">{titleCase(event.status || "update")}</p>
                  <p className="text-sm text-white/70">{event.location || "Location pending"}</p>
                  <p className="text-xs text-white/50">{event.created_at || "--"}</p>
                </li>
              ))}
              {timeline.length === 0 ? <p className="text-white/60">No timeline events yet.</p> : null}
            </ol>
          </Card>
        </SectionBorder>
      </div>
    </div>
  );
}
