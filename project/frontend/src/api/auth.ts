import { ApiClient } from "@/api";

export interface AuthResponse { 
    token: string;
    user: { 
        id: string;
        username: string;
    }
}

export interface AuthRequest {
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
    const email = username.concat("@netsimcpp.pl");
    return ApiClient<AuthResponse>("auth/register", { 
        method: "POST",
        body: { username, password, email },
        withAuth: false,
    });
}

export function GetUser() { 
    const token = localStorage.getItem("token");
    if (!token || token === "undefined") {
        console.log("No valid token available!"); 
        return Promise.resolve(null); 
    }

    return ApiClient<AuthResponse>("auth/me", { 
        method: "GET",
        withAuth: true,
    }).catch(err => {
        console.error("GetUser failed:", err.message);
        return null; 
    });
}


