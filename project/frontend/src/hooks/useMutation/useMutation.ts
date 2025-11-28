import { useState } from 'react';

type Status = "idle" | "loading" | "success" | "error";

export default function useMutation<T, V> (
    mutationFn: (variables: V) => Promise<T>, 
    onSuccess?: (data: T) => void,
    onError?: (error: Error) => void
) {
    const [data, setData] = useState<T | null>(null);
    const [status, setStatus] = useState<Status>("idle");
    const [error, setError] = useState<Error | null>(null);

    async function mutate(variables: V) { 
        setStatus("loading");
        setError(null);
        try { 
            const result = await mutationFn(variables);
            setData(result);
            setStatus("success");
            onSuccess?.(result);
            return result;
        } catch (error) { 
            setError(error as Error);
            setStatus("error");
            onError?.(error as Error);
            throw error;
        }
    }

    return { data, status, error, mutate };
}