import { create } from "zustand";
import api from "../services/api";
import { connectNotifications, closeNotifications } from "../services/ws";

export const useTrackingStore = create((set) => ({
  shipment: null,
  timeline: [],
  connected: false,

  trackPublic: async (trackingNumber) => {
    const { data } = await api.get(`/track/${trackingNumber}`);
    const payload = data?.data || data;
    set({ shipment: payload?.shipment || payload, timeline: payload?.timeline || [] });
  },

  connectRealtime: () => {
    connectNotifications((message) => {
      set((state) => {
        if (!message?.tracking_number || !state.shipment?.tracking_number) {
          return state;
        }
        if (message.tracking_number !== state.shipment.tracking_number) {
          return state;
        }
        const nextTimeline = message.event
          ? [...state.timeline, message.event]
          : state.timeline;
        return { timeline: nextTimeline };
      });
    });
    set({ connected: true });
  },

  disconnectRealtime: () => {
    closeNotifications();
    set({ connected: false });
  },
}));
