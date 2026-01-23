import React from "react";
import { renderHook, act } from "@testing-library/react";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { vi, type MockedFunction } from "vitest";
import useLoginMutation from "./useLoginMutation";
import { SignIn, type AuthResponse, type AuthRequest } from "@/api/auth";

vi.mock("@/api/auth", () => ({
  SignIn: vi.fn(),
}));

const mockedSignIn = SignIn as MockedFunction<typeof SignIn>;

const mockLocalStorage = () => {
  const store: Record<string, string> = {};
  vi.spyOn(Storage.prototype, "setItem").mockImplementation((key, value) => {
    store[key] = value;
    return value;
  });
  return store;
};

describe("useLoginMutation", () => {
  afterEach(() => {
    vi.restoreAllMocks();
    vi.clearAllMocks();
  });

  it("calls SignIn, stores token, invalidates query", async () => {
    const fakeResponse: AuthResponse = {
      token: "12345",
      user: { id: "1", username: "Test User" },
    };
    mockedSignIn.mockResolvedValue(fakeResponse);

    const localStore = mockLocalStorage();
    const invalidateSpy = vi.fn();

    const queryClient = new QueryClient({ defaultOptions: { queries: { retry: false } } });
    queryClient.invalidateQueries = invalidateSpy as any;

    const wrapper = ({ children }: { children: React.ReactNode }) => (
      <QueryClientProvider client={queryClient}>{children}</QueryClientProvider>
    );

    const { result } = renderHook(() => useLoginMutation(), { wrapper });
    const request: AuthRequest = { username: "a@a.com", password: "123" };

    await act(async () => {
      await result.current.mutateAsync(request);
    });

    expect(localStore["token"]).toBe(fakeResponse.token);
    expect(mockedSignIn).toHaveBeenCalled();
    const calledArg = mockedSignIn.mock.calls[0][0];
    expect(calledArg).toEqual(request);
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ["auth", "user"] });
  });

  it("returns error when SignIn fails", async () => {
    mockedSignIn.mockRejectedValue(new Error("Invalid credentials"));

    const localStore = mockLocalStorage();

    const queryClient = new QueryClient({ defaultOptions: { queries: { retry: false } } });
    const wrapper = ({ children }: { children: React.ReactNode }) => (
      <QueryClientProvider client={queryClient}>{children}</QueryClientProvider>
    );

    const { result } = renderHook(() => useLoginMutation(), { wrapper });

    let error: any = null;

    await act(async () => {
      try {
        await result.current.mutateAsync({ username: "a@a.com", password: "wrong" });
      } catch (e) {
        error = e;
      }
    });

    expect(error).toBeInstanceOf(Error);
    expect(error.message).toBe("Invalid credentials");
    expect(localStore["token"]).toBeUndefined();
  });
});


