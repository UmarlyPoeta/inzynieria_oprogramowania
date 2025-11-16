import { renderHook, act, waitFor } from "@testing-library/react";
import { Server } from "mock-socket";
import { vi } from "vitest";
import useWebSocket from "./useWebSocket";

describe("useWebSocket hook", () => {
  let mockServer: Server;

  beforeEach(() => {
    mockServer = new Server("ws://localhost:1234");
  });

  afterEach(() => {
    mockServer.stop();
    vi.clearAllMocks();
  });

  it("initializes with empty messages and 'connecting' status", () => {
    const { result } = renderHook(() => useWebSocket("ws://localhost:1234"));
    expect(result.current.messages).toEqual([]);
    expect(result.current.status).toBe("connecting");
  });

  it("receives messages from the server", async () => {
    const { result } = renderHook(() => useWebSocket("ws://localhost:1234"));

    await act(async () => {
      // poczekaj na podłączenie klienta
      await waitFor(() => {
        expect((mockServer as any).clients().length).toBe(1);
      });

      // wyślij wiadomość do hooka
      const client = (mockServer as any).clients()[0];
      client.send("hello");
    });

    await waitFor(() => {
      expect(result.current.messages).toContain("hello");
    });
  });

  it("calls onMessage callback when message is received", async () => {
    const onMessage = vi.fn();
    renderHook(() => useWebSocket("ws://localhost:1234", onMessage));

    await act(async () => {
      await waitFor(() => (mockServer as any).clients().length === 1);

      const client = (mockServer as any).clients()[0];
      client.send("test");
    });

    await waitFor(() => {
      expect(onMessage).toHaveBeenCalledWith("test");
    });
  });

  it("updates status to 'open' when socket connects", async () => {
    const { result } = renderHook(() => useWebSocket("ws://localhost:1234"));

    await waitFor(() => {
      expect(result.current.status).toBe("open");
    });
  });

  it("sends message when socket is open", async () => {
    const { result } = renderHook(() => useWebSocket("ws://localhost:1234"));

    await act(async () => {
      await waitFor(() => (mockServer as any).clients().length === 1);

      const client = (mockServer as any).clients()[0];

      // nasłuchujemy na wiadomość wysłaną przez hook
      client.on("message", (msg: string) => {
        expect(msg).toBe("ping");
      });

      result.current.sendMessage("ping");
    });
  });

  it("buffers messages if socket is not open yet", async () => {
    const { result } = renderHook(() => useWebSocket("ws://localhost:1234"));

    // wysyłamy zanim socket się otworzy
    result.current.sendMessage("buffered");

    await act(async () => {
      await waitFor(() => (mockServer as any).clients().length === 1);
      const client = (mockServer as any).clients()[0];

      client.on("message", (msg: string) => {
        expect(msg).toBe("buffered");
      });
    });
  });

  it("calls onError callback on socket error", async () => {
    const onError = vi.fn();
    renderHook(() => useWebSocket("ws://localhost:1234", undefined, onError));

    await act(async () => {
      (mockServer as any).simulate("error");
    });

    await waitFor(() => {
      expect(onError).toHaveBeenCalled();
    });
  });

  it("cleans up timeout and closes socket on unmount", () => {
    const { unmount } = renderHook(() => useWebSocket("ws://localhost:1234"));
    const closeSpy = vi.spyOn(WebSocket.prototype, "close");

    unmount();

    expect(closeSpy).toHaveBeenCalled();
  });
});




