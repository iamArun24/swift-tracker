import { Link, useNavigate } from "react-router-dom";
import { useAuthStore } from "../store/authStore";
import Button from "./Button";

export default function Navbar() {
  const { user, logout } = useAuthStore();
  const navigate = useNavigate();

  return (
    <header className="sticky top-0 z-40 border-b border-white/15 bg-black/45 backdrop-blur-xl">
      <div className="ink-layer mx-auto flex max-w-7xl flex-wrap items-center justify-between gap-3 px-4 py-3">
        <Link to="/" className="brand-mark text-lg font-black sm:text-xl">
          SwiftTrack
        </Link>
        <nav className="flex flex-wrap items-center justify-end gap-2 text-sm sm:gap-3">
          <Link to="/track/demo" className="nav-link whitespace-nowrap">Track</Link>
          {user ? (
            <>
              <Link to="/dashboard" className="nav-link whitespace-nowrap">Dashboard</Link>
              <Button
                className="whitespace-nowrap"
                onClick={() => {
                  logout();
                  navigate("/");
                }}
              >
                Logout
              </Button>
            </>
          ) : (
            <Button className="whitespace-nowrap btn-invert" onClick={() => navigate("/login")}>Login</Button>
          )}
        </nav>
      </div>
    </header>
  );
}
