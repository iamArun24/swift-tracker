let socket;

export function connectNotifications(onMessage) {
  const url =
    import.meta.env.VITE_WS_URL || "ws://localhost:8080/ws/notifications";

  socket = new WebSocket(url);
  socket.onmessage = (event) => {
    try {
      const data = JSON.parse(event.data);
      onMessage?.(data);
    } catch {
      onMessage?.(event.data);
    }
  };

  return socket;
}

export function closeNotifications() {
  if (socket && socket.readyState <= 1) {
    socket.close();
  }
}
