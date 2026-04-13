import LightRays from "./LightRays";

export default function AmbientLightRays({ className = "" }) {
  return (
    <div className={`ink-rays-layer ${className}`.trim()} aria-hidden="true">
      <LightRays
        raysOrigin="top-center"
        raysColor="#ffffff"
        raysSpeed={0.9}
        lightSpread={0.55}
        rayLength={2.4}
        followMouse
        mouseInfluence={0.08}
        noiseAmount={0.02}
        distortion={0.06}
        pulsating
        fadeDistance={1.15}
        saturation={1}
      />
    </div>
  );
}
