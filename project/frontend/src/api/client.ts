// const API_URL = 'http://localhost:8080';

interface ApiOptions extends RequestInit {
  body?: any;
  withAuth?: boolean; 
}

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
  }: ApiOptions = {}
): Promise<T> {
  const token = localStorage.getItem("token");

  if (withAuth && !token) {
    throw new Error("No token available for authenticated request");
  }

  // const res = await fetch(`${API_URL}/${endpoint}`, {
  const res = await fetch(`/api/${endpoint}`, {
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
    let json: any = {};
    try {
      json = JSON.parse(text);
    } catch {}
    console.error(`API error (${res.status}): ${text}`);
    if (res.status === 401) {
      localStorage.removeItem("token");
    }
    throw json; 
  }

  return await res.json();
}

