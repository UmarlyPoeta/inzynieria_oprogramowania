import { Navigate } from "react-router-dom";
import { useAuthContext } from "@/context/AuthContext";
import { type ReactNode } from "react";

interface ProtectedRouteProps {
  children: ReactNode;
  redirectIfAuthenticated?: boolean;
}

const ProtectedRoute = ({ children, redirectIfAuthenticated = false }: ProtectedRouteProps) => {
  const { user, status } = useAuthContext();

  // if (status === "pending") {
  //   return (
  //     <div style={{ display: "flex", justifyContent: "center", alignItems: "center", height: "100vh" }}>
  //       Loading...
  //     </div>
  //   );

  // }

  if (!user && !redirectIfAuthenticated) return <Navigate to="/login" replace />;
  if (user && redirectIfAuthenticated) return <Navigate to="/workspace" replace />;


  return <>{children}</>;
};

export default ProtectedRoute;





