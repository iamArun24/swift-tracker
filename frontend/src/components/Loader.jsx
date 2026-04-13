export default function Loader({ text = "Loading" }) {
  return (
    <div className="flex items-center gap-3 text-white/80">
      <span className="loader-orb" />
      <span>{text}...</span>
    </div>
  );
}
