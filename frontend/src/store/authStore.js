import { create } from "zustand";
import api from "../services/api";

export const useAuthStore = create((set) => ({
  user: null,
  token: localStorage.getItem("swifttrack_token") || null,
  loading: false,
  error: null,

  login: async (email, password) => {
    set({ loading: true, error: null });
    try {
      const { data } = await api.post("/auth/login", { email, password });
      const auth = data?.data || data;
      const token = auth?.token;
      const user = auth?.user;
      localStorage.setItem("swifttrack_token", token);
      set({ token, user, loading: false });
      return user;
    } catch (error) {
      set({ error: error.message, loading: false });
      throw error;
    }
  },

  register: async (payload) => {
    set({ loading: true, error: null });
    try {
      const { data } = await api.post("/auth/register", payload);
      const auth = data?.data || data;
      const token = auth?.token;
      const user = auth?.user;
      if (token) {
        localStorage.setItem("swifttrack_token", token);
      }
      set({ token, user, loading: false });
      return user;
    } catch (error) {
      set({ error: error.message, loading: false });
      throw error;
    }
  },

  fetchMe: async () => {
    try {
      const { data } = await api.get("/auth/me");
      const user = data?.data || data;
      set({ user });
      return user;
    } catch {
      set({ user: null, token: null });
      localStorage.removeItem("swifttrack_token");
      return null;
    }
  },

  logout: () => {
    localStorage.removeItem("swifttrack_token");
    set({ user: null, token: null, error: null });
  },
}));
