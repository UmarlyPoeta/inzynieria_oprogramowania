import { useEffect, useState, useRef } from "react";

interface UseWebSocketReturn {
  messages: string[];
  sendMessage: (msg: string) => void;
}

export default function useWebSocket(url: string, onMessage?: (msg: string) => void ): UseWebSocketReturn {
  const [messages, setMessages] = useState<string[]>([]);
  const ws = useRef<WebSocket | null>(null);

  useEffect(() => {
    ws.current = new WebSocket(url);

    ws.current.onmessage = (event: MessageEvent) => {
      setMessages((prev) => [...prev, event.data]);
      onMessage?.(event.data); 
    };

    return () => ws.current?.close();
  }, [url, onMessage]);

  const sendMessage = (msg: string) => {
    if (ws.current?.readyState === WebSocket.OPEN) {
      ws.current.send(msg);
    }
  };

  return { messages, sendMessage };
}

