import { create } from "zustand";
import api from "../services/api";

export const useShipmentStore = create((set) => ({
  shipments: [],
  loading: false,
  error: null,

  loadMyShipments: async () => {
    set({ loading: true, error: null });
    try {
      const { data } = await api.get("/shipments");
      const rows = data?.data || [];
      set({ shipments: rows, loading: false });
    } catch (error) {
      set({ error: error.message, loading: false });
    }
  },

  createShipment: async (payload) => {
    set({ loading: true, error: null });
    try {
      const { data } = await api.post("/shipments", payload);
      const created = data?.data || data;
      set((state) => ({
        shipments: [created, ...state.shipments],
        loading: false,
      }));
      return created;
    } catch (error) {
      set({ error: error.message, loading: false });
      throw error;
    }
  },

  getPriceEstimate: async (payload) => {
    const { data } = await api.post("/shipments/price-estimate", payload);
    return data?.data || data;
  },
}));
