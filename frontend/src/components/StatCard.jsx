import Card from "./Card";

export default function StatCard({ label, value, hint }) {
  return (
    <Card className="reveal">
      <p className="text-xs uppercase tracking-wide text-white/60">{label}</p>
      <p className="mt-2 text-2xl font-black text-white">{value}</p>
      {hint ? <p className="mt-1 text-xs text-white/70">{hint}</p> : null}
    </Card>
  );
}
