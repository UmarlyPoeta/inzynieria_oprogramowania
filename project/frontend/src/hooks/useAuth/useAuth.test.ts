import { describe, it, expect, vi, beforeEach } from "vitest";
import { renderHook, act } from "@testing-library/react";
import { useAuth } from "@/hooks"; 
import * as hooks from "@/hooks";   

const localStorageMock = (() => {
  let store: Record<string, string> = {};
  return {
    getItem: (key: string) => store[key] || null,
    setItem: (key: string, value: string) => { store[key] = value.toString(); },
    removeItem: (key: string) => { delete store[key]; },
    clear: () => { store = {}; },
  };
})();
Object.defineProperty(global, "localStorage", { value: localStorageMock });

describe("useAuth", () => {
  const mockUser = { id: "1", username: "JohnDoe" };
  const mockToken = "token123";
  let mockRefetch: () => void;

  beforeEach(() => {
    localStorage.clear();
    vi.restoreAllMocks();
    mockRefetch = vi.fn();

    vi.spyOn(hooks, "useFetch").mockReturnValue({
      data: null,
      status: "idle",
      refetch: mockRefetch,
    } as any); 

    vi.spyOn(hooks, "useMutation").mockImplementation(
        ((_mutationFn: any, _onSuccess?: any) => ({
            mutate: async (_variables: any) => {
            const result = { token: mockToken, user: mockUser };
            _onSuccess?.(result);
            return Promise.resolve();
            },
            data: null,
            status: "idle",
            error: null,
        })) as any
    );

  });

  it("should initialize with token from localStorage", () => {
    localStorage.setItem("token", mockToken);

    const { result } = renderHook(() => useAuth());

    expect(result.current.token).toBe(mockToken);
    expect(result.current.isAuthenticated).toBe(true);
  });

  it("should login correctly", async () => {
    const { result } = renderHook(() => useAuth());

    await act(async () => {
      await result.current.login({ username: "JohnDoe", password: "123" });
    });

    expect(result.current.user).toEqual(mockUser);
    expect(result.current.token).toBe(mockToken);
    expect(result.current.isAuthenticated).toBe(true);
    expect(localStorage.getItem("token")).toBe(mockToken);
  });

  it("should register correctly", async () => {
    const { result } = renderHook(() => useAuth());

    await act(async () => {
      await result.current.register({ username: "JohnDoe", password: "123", email: "john@example.com" });
    });

    expect(result.current.user).toEqual(mockUser);
    expect(result.current.token).toBe(mockToken);
    expect(result.current.isAuthenticated).toBe(true);
    expect(localStorage.getItem("token")).toBe(mockToken);
  });

  it("should logout correctly", () => {
    localStorage.setItem("token", mockToken);
    const { result } = renderHook(() => useAuth());

    act(() => {
      result.current.logout();
    });

    expect(result.current.user).toBe(null);
    expect(result.current.token).toBe(null);
    expect(result.current.isAuthenticated).toBe(false);
    expect(localStorage.getItem("token")).toBe(null);
  });

  it("should call refetchUser", () => {
    const { result } = renderHook(() => useAuth());

    act(() => {
      result.current.refetchUser();
    });

    expect(mockRefetch).toHaveBeenCalled();
  });
});

