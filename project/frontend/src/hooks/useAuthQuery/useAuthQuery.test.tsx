// src/hooks/useAuthQuery/useAuthQuery.test.tsx
import React from 'react';
import { renderHook, waitFor } from '@testing-library/react';
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { vi, type MockedFunction } from 'vitest';
import useAuthQuery from './useAuthQuery';
import { GetUser } from '@/api/auth';

/* =======================
   MOCK API
======================= */
vi.mock('@/api/auth', () => ({
  GetUser: vi.fn(),
}));

const mockedGetUser = GetUser as MockedFunction<typeof GetUser>;

const createWrapper = () => {
  const queryClient = new QueryClient({
    defaultOptions: {
      queries: {
        retry: false,
      },
    },
  });

  return ({ children }: { children: React.ReactNode }) => (
    <QueryClientProvider client={queryClient}>
      {children}
    </QueryClientProvider>
  );
};

const mockLocalStorage = (token: string | null) => {
  vi.spyOn(Storage.prototype, 'getItem').mockReturnValue(token);
};

describe('useAuthQuery', () => {
  afterEach(() => {
    vi.restoreAllMocks();
    vi.clearAllMocks();
  });

  it('does not run query when token is missing', () => {
    mockLocalStorage(null);

    const { result } = renderHook(() => useAuthQuery(), {
      wrapper: createWrapper(),
    });

    expect(mockedGetUser).not.toHaveBeenCalled();

    expect(result.current.fetchStatus).toBe('idle');
  });

  it('calls GetUser and returns success when token exists', async () => {
    mockLocalStorage('valid-token');

    mockedGetUser.mockResolvedValue({
      id: 1,
      email: 'test@test.com',
    } as any);

    const { result } = renderHook(() => useAuthQuery(), {
      wrapper: createWrapper(),
    });

    await waitFor(() => {
      expect(result.current.isSuccess).toBe(true);
    });

    expect(result.current.data).toEqual({
      id: 1,
      email: 'test@test.com',
    });

    expect(mockedGetUser).toHaveBeenCalled();
  });

  it('returns error when GetUser fails', async () => {
    mockLocalStorage('valid-token');

    mockedGetUser.mockRejectedValue(new Error('Unauthorized'));

    const { result } = renderHook(() => useAuthQuery(), {
      wrapper: createWrapper(),
    });

    await waitFor(() => {
      expect(result.current.isError).toBe(true);
    });

    expect(result.current.error).toBeInstanceOf(Error);
    expect(mockedGetUser).toHaveBeenCalled();
  });
});

