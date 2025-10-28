import { renderHook, act, waitFor } from '@testing-library/react'
import { describe, it, expect, vi, beforeEach } from 'vitest'
import { useFetch } from '@/hooks'

describe('useFetch', () => {
  const key = 'test-key'

  let testCache: Map<string, any>;

  beforeEach(() => {
    testCache = new Map();
  });

  it('should initialize with idle state when cache is empty', async () => {
    const fetchFn = vi.fn(() => new Promise(resolve => setTimeout(() => resolve('data'), 50)));
    const { result } = renderHook(() => useFetch(key, fetchFn, testCache));

    expect(result.current.data).toBeNull();
    expect(result.current.error).toBeNull();
    expect(['idle', 'loading']).toContain(result.current.status);

    await waitFor(() => expect(result.current.status).toBe('success'));
  });

  it('should set data and status to success after fetch', async () => {
    const fetchFn = vi.fn(() => Promise.resolve('fetched-data'));
    let result: any;

    await act(async () => {
      ({ result } = renderHook(() => useFetch(key, fetchFn, testCache)));
    });

    await waitFor(() => expect(result.current.status).toBe('success'));

    expect(result.current.data).toBe('fetched-data');
    expect(result.current.error).toBeNull();
  });

  it('should set error and status to error on fetch failure', async () => {
    const fetchFn = vi.fn(() => Promise.reject(new Error('fail')));
    let result: any;

    await act(async () => {
      ({ result } = renderHook(() => useFetch(key, fetchFn, testCache)));
    });

    await waitFor(() => expect(result.current.status).toBe('error'));

    expect(result.current.data).toBeNull();
    expect(result.current.error).toBeInstanceOf(Error);
  });

  it('should refetch and call fetchFn again', async () => {
    const fetchFn = vi.fn(() => Promise.resolve('new-data'));
    let result: any;

    await act(async () => {
      ({ result } = renderHook(() => useFetch('refetch-key', fetchFn, testCache)));
    });

    await waitFor(() => expect(result.current.status).toBe('success'));

    await act(async () => {
      result.current.refetch();
    });

    let result2: any;
    await act(async () => {
      ({ result: result2 } = renderHook(() => useFetch('refetch-key', fetchFn, testCache)));
    });

    await waitFor(() => expect(result2.current.status).toBe('success'));
    expect(result2.current.data).toBe('new-data');
    expect(fetchFn).toHaveBeenCalledTimes(2);
  });
});




