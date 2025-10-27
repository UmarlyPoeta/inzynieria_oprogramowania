const API_URL = 'http://localhost:8080'; // API Configuration (to be moved to a separate config file)

interface ApiOptions extends RequestInit {
    body?: any;
}

export default async function ApiClient<T>(
    endpoint: string,
    { method = "GET", body, headers, ...options }: ApiOptions = {}
): Promise<T> {
    const res = await fetch(`${API_URL}/${endpoint}`, {
        method,
        headers: {
            "Content-Type": "application/json",
            ...headers,
        },
        body: body ? JSON.stringify(body) : undefined,
        ...options,
    });

    if (!res.ok) {
        const text = await res.text();
        throw new Error(`API error (${res.status}): ${text}`);
    }

    return await res.json();
}
