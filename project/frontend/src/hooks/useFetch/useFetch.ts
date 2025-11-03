import { useEffect, useState, useRef } from 'react';

interface UseFetchOptions {
  cache?: Map<string, any>;
  enabled?: boolean;
}

export default function useFetch<T>(key: string, fetchFn: () => Promise<T>, options?: UseFetchOptions) {
  const cache = options?.cache ?? new Map<string, T>();
  const enabled = options?.enabled ?? true;

  const [data, setData] = useState<T | null>(() => cache.get(key) ?? null);
  const [status, setStatus] = useState<"idle" | "loading" | "success" | "error">(cache.has(key) ? "success" : "idle");
  const [error, setError] = useState<Error | null>(null);
  const mounted = useRef(true);

  useEffect(() => {
    mounted.current = true;
    if (!enabled) return;
    if (cache.has(key)) return;

    setStatus("loading");

    fetchFn()
      .then(result => {
        cache.set(key, result);
        if (mounted.current) {
          setData(result);
          setStatus("success");
        }
      })
      .catch(err => {
        if (mounted.current) {
          setError(err as Error);
          setStatus("error");
        }
      });

    return () => { mounted.current = false; };
  }, [key, fetchFn, enabled]);

  return { data, status, error, refetch: () => cache.delete(key) };
}
