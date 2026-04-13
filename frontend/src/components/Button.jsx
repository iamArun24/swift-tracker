export default function Button({ children, className = "", ...props }) {
  return (
    <button
      className={`neo-btn rounded-xl px-4 py-2 text-sm font-semibold ${className}`}
      {...props}
    >
      {children}
    </button>
  );
}
