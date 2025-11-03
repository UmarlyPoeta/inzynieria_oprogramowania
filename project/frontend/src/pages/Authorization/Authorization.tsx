import { AuthLayout, AuthForm } from "@/components";

interface AuthorizationProps {
  mode: "login" | "register";
}

const Authorization = ({ mode }: AuthorizationProps) => {
  return (
    <AuthLayout>
      <AuthForm mode={mode} />
    </AuthLayout>
  );
};

export default Authorization;