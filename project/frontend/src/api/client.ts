const API_URL = 'http://localhost:8080';

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

  const res = await fetch(`${API_URL}/${endpoint}`, {
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
