import { ApiClient } from "@/api";

interface AuthResponse { 
    token: string;
    user: { 
        id: string;
        username: string;
    }
}

interface AuthRequest {
    username: string;
    password: string;
}


export function SignIn({username, password} : AuthRequest) : Promise<AuthResponse> { 
    return ApiClient<AuthResponse>("auth/login", { 
        method: "POST",
        body: { username, password },
        withAuth: false,
    });
}

export function SignUp({username, password} : AuthRequest) : Promise<AuthResponse> { 
    return ApiClient<AuthResponse>("auth/register", { 
        method: "POST",
        body: { username, password },
        withAuth: false,
    });
}

export function GetUser() { 
    return ApiClient<AuthResponse>("auth/me", { 
        method: "GET",
        withAuth: true,
    });
}