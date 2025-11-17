import { useState, useEffect } from "react";
import { SignIn, SignUp, GetUser } from "@/api"; 
import { useFetch, useMutation } from "@/hooks";

export default function useAuth() {
  const [user, setUser] = useState<any>(null);
  const [token, setToken] = useState<string | null>(() => localStorage.getItem("token"));

  const { data: userData, status: userStatus, refetch } = useFetch(
    "user",
    () => GetUser(),
    { enabled: !!token }
  );

  useEffect(() => {
    if (userData) setUser(userData);
  }, [userData]);

  const loginMutation = useMutation(SignIn, (data) => {
    localStorage.setItem("token", data.token);
    setToken(data.token);
    setUser(data.user);
  });

  const registerMutation = useMutation(SignUp, (data) => {
    localStorage.setItem("token", data.token);
    setToken(data.token);
    setUser(data.user);
  });

  function logout() {
    localStorage.removeItem("token");
    setToken(null);
    setUser(null);
  }

  return {
    user,
    token,
    isAuthenticated: !!token,
    login: loginMutation.mutate,
    register: registerMutation.mutate,
    logout,
    refetchUser: refetch,
    status: userStatus,
  };
}
