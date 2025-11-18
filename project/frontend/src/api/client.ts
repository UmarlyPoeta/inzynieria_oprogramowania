const resolveApiUrl = () => {
  const configured = (import.meta.env.VITE_API_URL as string | undefined)?.trim();
  if (configured && configured.length > 0) {
    return configured.replace(/\/$/, "");
  }
  if (import.meta.env.DEV) {
    return "http://localhost:8080";
  }
  return "/api";
};

const API_URL = resolveApiUrl();

interface ApiOptions extends RequestInit {
  body?: any;
  withAuth?: boolean; 
}

export default async function ApiClient<T>(
  endpoint: string, { 
    method = "GET", 
    body, 
    headers, 
    withAuth = false, 
    ...options 
}: ApiOptions = {}): Promise<T> {
  const token = localStorage.getItem("token");

  const normalizedEndpoint = endpoint.startsWith("/") ? endpoint : `/${endpoint}`;
  const target = `${API_URL}${normalizedEndpoint}`.replace(/([^:]\/)\/+/g, "$1");

  const res = await fetch(target, {
    method,
    headers: {
      "Content-Type": "application/json",
      ...(withAuth && token ? { Authorization: `Bearer ${token}` } : {}),
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
