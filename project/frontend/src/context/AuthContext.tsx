import { createContext, useContext, type ReactNode } from "react";
import { useAuthQuery, useLoginMutation, useRegisterMutation } from "@/hooks";
import type { QueryStatus } from "@tanstack/react-query";
import { useQueryClient } from "@tanstack/react-query";

interface AuthContextType {
  user: any;
  status: QueryStatus;
  login: ReturnType<typeof useLoginMutation>;
  register: ReturnType<typeof useRegisterMutation>;
  logout: () => void;
}

const AuthContext = createContext<AuthContextType | null>(null);

export const AuthProvider = ({ children }: { children: ReactNode }) => {
  const queryClient = useQueryClient();
  const authQuery = useAuthQuery();
  const login = useLoginMutation();
  const register = useRegisterMutation();

  const user = authQuery.data ?? (localStorage.getItem("token") ? undefined : null);

  const logout = () => {
    localStorage.removeItem("token");
    queryClient.clear();
    window.location.href = "/login";
  };

  return (
    <AuthContext.Provider
      value={{
        user: user,
        status: authQuery.status,
        login,
        register,
        logout
      }}
    >
      {children}
    </AuthContext.Provider>
  );
};

export const useAuthContext = () => {
  const ctx = useContext(AuthContext);
  if (!ctx) throw new Error("useAuthContext must be used within AuthProvider");
  return ctx;
};
