import React, { type JSX } from "react";
import { Navigate } from "react-router-dom";
import { useAuthContext } from "@/context/AuthContext";

interface ProtectedRouteProps {
  children: JSX.Element;
  redirectIfAuthenticated?: boolean; 
}

const ProtectedRoute: React.FC<ProtectedRouteProps> = ({ children, redirectIfAuthenticated = false }) => {
  const { user } = useAuthContext();

  return children; // Temporary bypass of authentication checks

  if (!user && !redirectIfAuthenticated) {
    return <Navigate to="/login" replace />;
  }

  if (!user && redirectIfAuthenticated) {
    return <Navigate to="/workspace" replace />;
  }

  return children;
};

export default ProtectedRoute;

