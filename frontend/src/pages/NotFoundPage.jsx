import { Link } from "react-router-dom";
import AmbientLightRays from "../components/AmbientLightRays";
import SectionBorder from "../components/SectionBorder";

export default function NotFoundPage() {
  return (
    <div className="ink-canvas grid min-h-screen place-items-center text-white">
      <AmbientLightRays />
      <SectionBorder className="ink-layer" radius={16}>
        <div className="ink-card rounded-2xl p-8 text-center">
          <h1 className="text-4xl font-black">404</h1>
          <p className="mt-2 text-white/70">Route not found</p>
          <Link to="/" className="neo-btn btn-invert mt-4 inline-block rounded-lg px-4 py-2">
            Go Home
          </Link>
        </div>
      </SectionBorder>
    </div>
  );
}
