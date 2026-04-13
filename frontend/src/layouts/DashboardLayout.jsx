import Navbar from "../components/Navbar";
import Sidebar from "../components/Sidebar";
import AmbientLightRays from "../components/AmbientLightRays";

export default function DashboardLayout({ children, role }) {
  return (
    <div className="ink-canvas min-h-screen text-white">
      <AmbientLightRays />
      <Navbar />
      <div className="ink-layer mx-auto grid max-w-7xl items-start gap-4 px-4 py-6 md:grid-cols-[260px_1fr]">
        <Sidebar role={role} />
        <main className="min-w-0">{children}</main>
      </div>
    </div>
  );
}
