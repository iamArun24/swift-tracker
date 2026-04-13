import { NavLink } from "react-router-dom";

const linkClass = ({ isActive }) =>
  `block rounded-lg border px-3 py-2 text-sm transition-all duration-200 ${
    isActive
      ? "border-white/40 bg-white/20 text-white"
      : "border-transparent text-white/75 hover:border-white/20 hover:bg-white/10 hover:text-white"
  }`;

export default function Sidebar({ role = "sender" }) {
  const common = [
    { to: "/dashboard", label: "Sender Dashboard" },
    { to: "/agent", label: "Agent Panel" },
    { to: "/admin", label: "Admin Panel" },
  ];

  return (
    <aside className="ink-card w-full rounded-2xl p-3 md:w-64">
      <p className="mb-3 text-xs uppercase tracking-widest text-white/50">{role}</p>
      {common.map((item) => (
        <NavLink key={item.to} to={item.to} className={linkClass}>
          {item.label}
        </NavLink>
      ))}
    </aside>
  );
}
