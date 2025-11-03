import { useState } from "react";
import { Button, FormContainer, Input, Title, ErrorText } from "./AuthForm.styled";
import { useAuthContext } from '../../../context/AuthContext'; 
import { useNavigate } from "react-router-dom";
import { Waypoints } from "lucide-react";

interface AuthFormProps {
    mode: 'login' | 'register';
}

const AuthForm = ({ mode }: AuthFormProps) => {
    const [form, setForm] = useState({ username: "", password: "", confirmPassword: "" });
    const [error, setError] = useState<string | null>(null);
    const {login, register} = useAuthContext();
    const navigate = useNavigate(); 

    const handleSubmit = async (e: React.FormEvent) => {
        e.preventDefault();
        try {
            if (mode === "login") await login(form);
            else await register(form);
            navigate("/workspace");
        } catch (err: any) {
            setError(err.message || "Wystąpił błąd.");
        }
    };

    return (
    <div style={{display: "flex", width: "100%", height: "100%"}}>
      <div style={{width: "100%", height: "100%", color: "#FC5D08", padding: "1rem", fontWeight: "bold"}}>

        <h3 style={{display: "flex", alignItems: "center", gap: "0.5rem"}}> 
          <Waypoints/>
          NetSimCPP 
        </h3>
      </div>

      <FormContainer onSubmit={handleSubmit}>
        <Title>{mode === "login" ? "Login to your account" : "Create an account"}</Title>

        <Input
          type="text"
          placeholder="Username"
          value={form.username}
          onChange={(e) => setForm({ ...form, username: e.target.value })}
        />
        <Input
          type="password"
          placeholder="Password"
          value={form.password}
          onChange={(e) => setForm({ ...form, password: e.target.value })}
        />
        {mode === "register" && (
          <Input
            type="password"
            placeholder="Confirm Password"
            value={form.confirmPassword}
            onChange={(e) => setForm({ ...form, confirmPassword: e.target.value })}
          />
        )}

        <Button type="submit" color="#FC5D08">
          {mode === "login" ? "Log in" : "Sign up"}
        </Button>

        {error && <ErrorText>{error}</ErrorText>}
      </FormContainer>
    </div>
  );
};

export default AuthForm;





