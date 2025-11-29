import { useEffect, useState, useRef } from "react";

export type WebSocketStatus = "connecting" | "open" | "closed" | "error";

interface UseWebSocketReturn {
  messages: string[];
  sendMessage: (msg: string) => void;
  status: WebSocketStatus;
}

export default function useWebSocket(
  url: string,
  onMessage?: (msg: string) => void,
  onError?: (err: Event) => void
): UseWebSocketReturn {
  const [messages, setMessages] = useState<string[]>([]);
  const [status, setStatus] = useState<WebSocketStatus>("connecting");

  const ws = useRef<WebSocket | null>(null);
  const retryTimeout = useRef<number | null>(null);
  const messageQueue = useRef<string[]>([]);
  const shouldReconnect = useRef(true);

  useEffect(() => {
    shouldReconnect.current = true;

    const connect = () => {
      if (!shouldReconnect.current) return;

      // Zamknij poprzednie połączenie jeśli istnieje
      if (ws.current) {
        ws.current.onclose = null;
        ws.current.close();
      }

      setStatus("connecting");
      ws.current = new WebSocket(url);

      ws.current.onopen = () => {
        setStatus("open");
        // Wyślij kolejkowane wiadomości
        while (messageQueue.current.length > 0) {
          const msg = messageQueue.current.shift();
          if (msg) ws.current?.send(msg);
        }
      };

      ws.current.onmessage = (event: MessageEvent) => {
        const raw = event.data.toString().trim();
        setMessages((prev) => [...prev, raw]);
        onMessage?.(raw);
      };

      ws.current.onerror = (err: Event) => {
        setStatus("error");
        onError?.(err);
      };

      ws.current.onclose = () => {
        setStatus("closed");
        
        // Reconnect tylko jeśli komponent jest zamontowany i chcemy reconnect
        if (shouldReconnect.current) {
          retryTimeout.current = window.setTimeout(connect, 2000);
        }
      };
    };

    connect();

    return () => {
      shouldReconnect.current = false;
      
      if (retryTimeout.current !== null) {
        clearTimeout(retryTimeout.current);
        retryTimeout.current = null;
      }
      
      if (ws.current) {
        ws.current.onclose = null; // Usuń handler żeby nie triggerował reconnect
        ws.current.close();
        ws.current = null;
      }
    };
  }, [url]); // Usuń onMessage i onError z dependencies

  const sendMessage = (msg: string) => {
    if (ws.current?.readyState === WebSocket.OPEN) {
      ws.current.send(msg);
    } else {
      messageQueue.current.push(msg);
    }
  };

  return { messages, sendMessage, status };
}


