import { useState } from "react";
import { useAuthContext } from "@/context/AuthContext"; 
import { useNavigate, Link } from "react-router-dom";
import { 
  Button, 
  FormContainer, 
  Input, 
  Title, 
  ErrorText, 
  LeftContainer, 
  RightContainer, 
  Subtitle, 
  Label, 
  Checkbox,
  CheckBoxWrapper,
  ProgressBar,
  ProgressWrapper
} from "./AuthForm.styled";
import { NetworkImage } from '@/data'

interface AuthFormProps {
    mode: 'login' | 'register';
}

const AuthForm = ({ mode }: AuthFormProps) => {
    const [form, setForm] = useState({ username: "", password: "", confirmPassword: "" });
    const [error, setError] = useState<string | null>(null);
    const [isRequestInProgress, setIsRequestInProgress] = useState(false);
    const [requestProgress, setRequestProgress] = useState(0);

    const { login, register } = useAuthContext();
    const navigate = useNavigate(); 

    const validateData = () => {
      if (form.password !== form.confirmPassword) {
          setError("Error! Passwords do not match.");
          return false;
      }

      const passwordLength = form.password.length;
      if (passwordLength < 6 || passwordLength > 15) {
          setError("Error! Password must be between 6 and 15 characters long.");
          return false;
      }

      const usernameLength = form.username.length;
      if (usernameLength < 4 || usernameLength > 12) {
          setError("Error! Username must be between 4 and 12 characters long.");
          return false;
      }

      if (!/^[a-zA-Z0-9_]+$/.test(form.username)) {
          setError("Error! Username can only contain letters, numbers, and underscores.");
          return false;
      }

      if (!/^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&])[A-Za-z\d@$!%*?&]{6,15}$/.test(form.password)) {
          setError("Error! Password must contain at least one uppercase letter, one lowercase letter, one number, and one special character.");
          return false;
      }

      if (/\s/.test(form.password) || /\s/.test(form.username)) {
          setError("Error! Username and password cannot contain spaces.");
          return false;
      }

      if (form.password === form.username) {
          setError("Error! Password cannot be the same as username.");
          return false;
      }
      
      setError(null);
      return true;
    };

    const handleSubmit = async (e: React.FormEvent) => {
        e.preventDefault();

        setIsRequestInProgress(true);
        setRequestProgress(0);

        const progressInterval = setInterval(() => {
            setRequestProgress(prev => (prev < 100 ? prev + 5 : prev));
        }, 100);

        try {
            if (mode === "login") {
                if (!form.username || !form.password) {
                    setError("Error! Username and password cannot be empty.");
                    return;
                }

                try { 
                    await login.mutateAsync({ username: form.username, password: form.password });
                    navigate("/workspace");
                } catch (err: any) {
                    setError(err.error || err.message || "Unknown error");
                }
            } else {
                if (!validateData()) return;

                try { 
                    await register.mutateAsync({ username: form.username, password: form.password });
                    navigate("/login");
                } catch (err: any) {
                    setError(err.error || err.message || "Unknown error");
                }
            }
        } catch (err: any) {
            setError(err.message || "The error occurred during authentication");
        } finally { 
          clearInterval(progressInterval);  
          setIsRequestInProgress(false);
          setRequestProgress(100); 
        }
    };

    return (
      <>
        <LeftContainer>
          <FormContainer onSubmit={handleSubmit}>
            {isRequestInProgress && (
              <ProgressWrapper style={{ position: 'absolute', top: 0, left: 0, width: '100%' }}>
                <ProgressBar progress={requestProgress} />
              </ProgressWrapper>
            )}
            <Title>{mode === "login" ? "Welcome Back" : "Create an account"}</Title>
            <Subtitle>{mode === "login" ? "Welcome back! Please enter your details" : "Please fill in the form to create an account"}</Subtitle>
            
            <Label>Username</Label>
            <Input
              type="text"
              placeholder="Enter your username"
              value={form.username}
              onChange={(e) => setForm({ ...form, username: e.target.value })}
            />

            <Label>Password</Label>
            <Input
              type="password"
              placeholder="*************"
              value={form.password}
              onChange={(e) => setForm({ ...form, password: e.target.value })}
            />

            {mode === "register" && (
              <>
                <Label>Confirm Password</Label>
                <Input
                  type="password"
                  placeholder="*************"
                  value={form.confirmPassword}
                  onChange={(e) => setForm({ ...form, confirmPassword: e.target.value })}
                />
              </>
            )}

            <CheckBoxWrapper>
              <><Checkbox /> Remember me</>
            </CheckBoxWrapper>

            <Button type="submit" $backgroundColor="#FC5D08" style={{ marginTop: ".3rem" }}>
              {mode === "login" ? "Sign In" : "Sign Up"}
            </Button>

            <p style={{ textAlign: "center", fontSize: ".9rem", marginTop: ".3rem" }}>
              {mode === "login" ? (
                <>Don't have an account? <Link to="/register" style={{ color: "#ff8000ff" }}>Sign up</Link></>
              ) : (
                <>Already have an account? <Link to="/login" style={{ color: "#ff8000ff" }}>Sign In</Link></>
              )}
            </p>

            {error && <ErrorText>{error}</ErrorText>}
          </FormContainer>
        </LeftContainer>

        {/* <RightContainer /> */}
        <RightContainer>
          <img src={NetworkImage} width={"100%"} height={"100%"}/>
        </RightContainer>
      </>
    );
};

export default AuthForm;






