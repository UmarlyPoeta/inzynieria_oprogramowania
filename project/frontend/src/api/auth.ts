import ApiClient from "./client";

export interface AuthUser {
    id: number;
    username: string;
    email?: string;
    role?: string;
    permissions?: string[];
}

export interface AuthResponse {
    token: string;
    expires_in?: number;
    user: AuthUser;
}

export interface AuthRequest {
    username: string;
    password: string;
}

export interface RegisterRequest extends AuthRequest {
    email: string;
    role?: "admin" | "user" | "viewer";
}

export function SignIn({ username, password }: AuthRequest): Promise<AuthResponse> {
    return ApiClient<AuthResponse>("auth/login", {
        method: "POST",
        body: { username, password },
        withAuth: false,
    });
}

export function SignUp({ username, password, email, role }: RegisterRequest): Promise<AuthResponse> {
    return ApiClient<AuthResponse>("auth/register", {
        method: "POST",
        body: { username, password, email, role },
        withAuth: false,
    });
}

export function GetUser() {
    return ApiClient<AuthUser>("auth/me", {
        method: "GET",
        withAuth: true,
    });
}

export function Logout() {
    return ApiClient<{ success: boolean }>("auth/logout", {
        method: "POST",
        withAuth: true,
    });
}