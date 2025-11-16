import { useEffect, useState, useRef } from "react";

export type WebSocketStatus = "connecting" | "open" | "closed" | "error";

interface UseWebSocketReturn {
  messages: string[];
  sendMessage: (msg: string) => void;
  status: WebSocketStatus;
}

export default function useWebSocket(url: string, onMessage?: (msg: string) => void, onError?: (err: Event) => void): UseWebSocketReturn {
  const [messages, setMessages] = useState<string[]>([]);
  const [status, setStatus] = useState<WebSocketStatus>("connecting");

  const ws = useRef<WebSocket | null>(null);
  const retryTimeout = useRef<number | null>(null);
  const messageQueue = useRef<string[]>([]);

  useEffect(() => {
    let mounted = true;

    const connect = () => {
      if (!mounted) return;

      setStatus("connecting");
      ws.current = new WebSocket(url);

      ws.current.onopen = () => {
        setStatus("open");
        messageQueue.current.forEach((msg) => ws.current?.send(msg));
        messageQueue.current = [];
      };

      ws.current.onmessage = (event: MessageEvent) => {
        setMessages((prev) => [...prev, event.data]);
        onMessage?.(event.data);
      };

      ws.current.onerror = (err: Event) => {
        setStatus("error");
        onError?.(err);
      };

      ws.current.onclose = () => {
        setStatus("closed");
        retryTimeout.current = window.setTimeout(connect, 2000) as unknown as number;
      };
    };

    connect();

    return () => {
      mounted = false;
      if (retryTimeout.current !== null) {
        clearTimeout(retryTimeout.current);
      }
      ws.current?.close();
    };
  }, [url, onMessage, onError]);

  const sendMessage = (msg: string) => {
    if (ws.current?.readyState === WebSocket.OPEN) {
      ws.current.send(msg);
    } else {
      messageQueue.current.push(msg);
    }
  };

  return { messages, sendMessage, status };
}


