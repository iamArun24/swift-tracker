import { useState } from "react";
import { useNavigate } from "react-router-dom";
import { useAuthStore } from "../store/authStore";
import AuthLayout from "../layouts/AuthLayout";
import Card from "../components/Card";
import Button from "../components/Button";
import { usePageReveal } from "../hooks/usePageReveal";
import RotatingText from "../components/RotatingText";
import ElectricBorder from "../components/ElectricBorder";
import SectionBorder from "../components/SectionBorder";

const defaults = {
  full_name: "Demo User",
  email: "demo.sender@swifttrack.com",
  phone: "9876543210",
  password: "DemoPass@123",
  city: "Delhi",
  state: "Delhi",
  pincode: "110001",
  address: "CP",
};

export default function LoginPage() {
  usePageReveal();
  const navigate = useNavigate();
  const { login, register, loading, error } = useAuthStore();
  const [isRegister, setIsRegister] = useState(false);
  const [form, setForm] = useState(defaults);

  async function submit(e) {
    e.preventDefault();
    try {
      if (isRegister) {
        await register(form);
      } else {
        await login(form.email, form.password);
      }
      navigate("/dashboard");
    } catch {
      // Store error state is already shown in UI.
    }
  }

  return (
    <AuthLayout>
      <SectionBorder className="w-full max-w-lg" radius={16}>
        <Card className="reveal w-full">
          <h2 className="mb-2 text-2xl font-black">{isRegister ? "Create account" : "Welcome back"}</h2>
          <div className="mb-3 inline-flex">
            <ElectricBorder color="#ffffff" speed={0.9} chaos={0.09} thickness={1.5} borderRadius={9}>
              <RotatingText
                texts={
                  isRegister
                    ? ["Create Sender Access", "Create Agent Access", "Create Admin Access"]
                    : ["Sender Portal", "Agent Console", "Admin Control"]
                }
                mainClassName="inline-flex rounded-md bg-white px-2.5 py-1 text-xs font-bold uppercase tracking-wider text-black sm:text-sm"
                staggerFrom="center"
                initial={{ y: "100%" }}
                animate={{ y: 0 }}
                exit={{ y: "-120%" }}
                staggerDuration={0.018}
                splitLevelClassName="overflow-hidden pb-0.5"
                transition={{ type: "spring", damping: 30, stiffness: 400 }}
                rotationInterval={2100}
              />
            </ElectricBorder>
          </div>
          <p className="mb-5 text-white/65">Access sender, admin, and agent experiences from one portal.</p>

          <form className="space-y-3" onSubmit={submit}>
            {isRegister && (
              <>
                <input className="field" value={form.full_name} onChange={(e) => setForm({ ...form, full_name: e.target.value })} placeholder="Full name" />
              </>
            )}
            <input className="field" value={form.email} onChange={(e) => setForm({ ...form, email: e.target.value })} placeholder="Email" />
            <input className="field" value={form.password} onChange={(e) => setForm({ ...form, password: e.target.value })} placeholder="Password" type="password" />
            {isRegister && (
              <div className="grid grid-cols-2 gap-2">
                <input className="field" value={form.phone} onChange={(e) => setForm({ ...form, phone: e.target.value })} placeholder="Phone" />
                <input className="field" value={form.pincode} onChange={(e) => setForm({ ...form, pincode: e.target.value })} placeholder="Pincode" />
                <input className="field col-span-2" value={form.city} onChange={(e) => setForm({ ...form, city: e.target.value })} placeholder="City" />
              </div>
            )}
            {error ? <p className="rounded-lg border border-white/20 bg-white/5 px-3 py-2 text-sm text-white/75">{error}</p> : null}
            <Button disabled={loading} className="btn-invert w-full">
              {loading ? "Please wait" : isRegister ? "Register" : "Login"}
            </Button>
          </form>

          <button
            type="button"
            className="mt-3 text-sm text-white/75 underline decoration-white/30 underline-offset-4 transition hover:text-white"
            onClick={() => setIsRegister((v) => !v)}
          >
            {isRegister ? "Already have an account? Login" : "Need an account? Register"}
          </button>
        </Card>
      </SectionBorder>
    </AuthLayout>
  );
}
