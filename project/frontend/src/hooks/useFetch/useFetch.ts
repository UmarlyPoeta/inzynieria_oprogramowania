import { useEffect, useState, useRef } from 'react';

const cache = new Map<string, any>();

type Status = "idle" | "loading" | "success" | "error";

export default function useFetch<T>(key: string, fetchFn: () => Promise<T>, cacheParam?: Map<string, T>) {
    const cache = cacheParam ?? new Map<string, T>();
    const [data, setData] = useState<T | null>(() => cache.get(key) ?? null);
    const [status, setStatus] = useState<Status>(cache.has(key) ? "success" : "idle");
    const [error, setError] = useState<Error | null>(null);
    const mounted = useRef(true);

    useEffect(() => { 
        mounted.current = true;

        if (cache.has(key)) return;
        setStatus("loading");

        fetchFn()
            .then((result) => { 
                cache.set(key, result);
                if (mounted.current) {
                    setData(result);
                    setStatus("success");
                }
            })
            .catch((err) => { 
                if (mounted.current) {
                    setError(err);
                    setStatus("error");
                }
            });

        return () => { mounted.current = false; };
    }, [key, fetchFn]);

    return { data, status, error, refetch: () => cache.delete(key) };
};

