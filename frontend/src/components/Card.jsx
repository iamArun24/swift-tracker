export default function Card({ title, children, className = "" }) {
  return (
    <section data-magic-card className={`ink-card rounded-2xl p-5 ${className}`}>
      {title ? <h3 className="mb-3 text-lg font-semibold text-white">{title}</h3> : null}
      {children}
    </section>
  );
}
