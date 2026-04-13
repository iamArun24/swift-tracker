import Navbar from "../components/Navbar";
import AmbientLightRays from "../components/AmbientLightRays";

export default function AuthLayout({ children }) {
  return (
    <div className="ink-canvas min-h-screen text-white">
      <AmbientLightRays />
      <Navbar />
      <main className="ink-layer mx-auto grid min-h-[80vh] max-w-5xl place-items-center px-4">{children}</main>
    </div>
  );
}
