let socket;

export function connectNotifications(onMessage) {
  const envUrl =
    import.meta.env.VITE_WS_URL || "ws://localhost:8080/ws/notifications";

  // If using a relative path (Netlify proxy), construct full WebSocket URL
  let url = envUrl;
  if (envUrl.startsWith("/")) {
    const protocol = window.location.protocol === "https:" ? "wss:" : "ws:";
    url = `${protocol}//${window.location.host}${envUrl}`;
  }

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
