import { Link } from "react-router-dom";
import { useEffect, useState } from "react";
import Navbar from "../components/Navbar";
import api from "../services/api";
import { usePageReveal } from "../hooks/usePageReveal";
import AmbientLightRays from "../components/AmbientLightRays";
import RotatingText from "../components/RotatingText";
import ElectricBorder from "../components/ElectricBorder";
import SectionBorder from "../components/SectionBorder";

const featureCards = [
  {
    title: "Live Tracking",
    copy: "Track parcel timeline, current location, and status updates in one screen.",
  },
  {
    title: "Smart Pricing",
    copy: "Estimate shipping cost before creating a shipment with distance and package inputs.",
  },
  {
    title: "Role-based Panels",
    copy: "Separate sender, agent, and admin workflows without switching applications.",
  },
  {
    title: "Realtime Updates",
    copy: "Incoming delivery events update automatically while tracking a shipment.",
  },
];

const quickLinks = [
  { to: "/login", label: "Login Portal", detail: "Sender, agent, and admin access" },
  { to: "/dashboard", label: "Sender Dashboard", detail: "Create and monitor shipments" },
  { to: "/agent", label: "Agent Panel", detail: "Update delivery statuses" },
  { to: "/admin", label: "Admin Analytics", detail: "Revenue and delivery insights" },
  { to: "/track/demo", label: "Public Tracking", detail: "View shipment timeline instantly" },
];

export default function HomePageV2() {
  const [apiStatus, setApiStatus] = useState("Checking");
  usePageReveal();

  useEffect(() => {
    let active = true;

    api
      .get("/health")
      .then(() => {
        if (active) {
          setApiStatus("Online");
        }
      })
      .catch(() => {
        if (active) {
          setApiStatus("Offline");
        }
      });

    return () => {
      active = false;
    };
  }, []);

  const statusClassName =
    apiStatus === "Online"
      ? "status-dot status-dot-online"
      : apiStatus === "Offline"
        ? "status-dot status-dot-offline"
        : "status-dot status-dot-checking";

  return (
    <div className="ink-canvas min-h-screen text-white">
      <AmbientLightRays />
      <Navbar />

      <section className="ink-layer mx-auto max-w-7xl px-4 pb-8 pt-10">
        <SectionBorder radius={24}>
          <div className="ink-card reveal grid gap-6 rounded-3xl p-6 md:grid-cols-[1.15fr_1fr] md:p-9">
            <div className="self-center">
              <p className="mb-2 text-xs uppercase tracking-[0.32em] text-white/65">SwiftTrack Studio Mode</p>
              <h1 className="text-3xl font-black leading-tight sm:text-4xl md:text-5xl">
                Black. White. Motion.
              </h1>

              <div className="mt-2 inline-flex">
                <ElectricBorder color="#ffffff" speed={0.9} chaos={0.08} thickness={1.6} borderRadius={10}>
                  <RotatingText
                    texts={["Dispatch Faster", "Track Every Parcel", "Run Live Operations"]}
                    mainClassName="inline-flex rounded-lg bg-white px-3 py-1 text-sm font-bold text-black sm:text-base"
                    staggerFrom="last"
                    initial={{ y: "100%" }}
                    animate={{ y: 0 }}
                    exit={{ y: "-120%" }}
                    staggerDuration={0.02}
                    splitLevelClassName="overflow-hidden pb-0.5"
                    transition={{ type: "spring", damping: 28, stiffness: 380 }}
                    rotationInterval={2300}
                  />
                </ElectricBorder>
              </div>

              <p className="mt-4 max-w-2xl text-sm text-white/75 sm:text-base">
                A monochrome control experience where motion guides the eye and information stays crisp.
                Create shipments, watch timelines, update delivery statuses, and review analytics in one continuous flow.
              </p>

              <div className="mt-6 flex flex-wrap gap-3">
                <Link className="neo-btn btn-invert rounded-xl px-5 py-3 text-sm font-semibold" to="/dashboard">
                  Open Sender Dashboard
                </Link>
                <Link className="neo-btn rounded-xl px-5 py-3 text-sm" to="/track/demo">
                  Open Public Tracking
                </Link>
                <Link className="neo-btn rounded-xl px-5 py-3 text-sm" to="/login">
                  Go To Login
                </Link>
              </div>

              <div className="status-pill mt-5">
                <span className={statusClassName} />
                API Status: {apiStatus}
              </div>
            </div>

            <div className="grid gap-3 sm:grid-cols-2">
              {featureCards.map((item) => (
                <div key={item.title} className="ink-card reveal rounded-2xl p-4">
                  <p className="text-sm font-bold text-white">{item.title}</p>
                  <p className="mt-2 text-sm text-white/70">{item.copy}</p>
                </div>
              ))}
            </div>
          </div>
        </SectionBorder>
      </section>

      <section className="ink-layer mx-auto grid max-w-7xl gap-3 px-4 pb-14 sm:grid-cols-2 lg:grid-cols-3">
        {quickLinks.map((item) => (
          <Link
            key={item.to}
            to={item.to}
            className="ink-card reveal rounded-2xl p-4 transition duration-300 hover:-translate-y-1 hover:border-white/40 hover:bg-white/[0.08]"
          >
            <p className="text-sm font-semibold text-white">{item.label}</p>
            <p className="mt-1 text-sm text-white/70">{item.detail}</p>
          </Link>
        ))}
      </section>
    </div>
  );
}