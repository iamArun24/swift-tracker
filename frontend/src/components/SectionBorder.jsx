import ElectricBorder from "./ElectricBorder";

export default function SectionBorder({
  children,
  className = "",
  radius = 16,
  speed = 0.82,
  chaos = 0.075,
  thickness = 1.35,
}) {
  return (
    <ElectricBorder
      color="#ffffff"
      speed={speed}
      chaos={chaos}
      thickness={thickness}
      borderRadius={radius}
      className={`section-electric-border ${className}`.trim()}
    >
      {children}
    </ElectricBorder>
  );
}
