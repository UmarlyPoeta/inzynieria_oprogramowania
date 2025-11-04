import { useState } from "react";
import { useAuthContext } from '../../../context/AuthContext'; 
import { useNavigate } from "react-router-dom";
import { GoogleIcon } from '@/data';
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
  CheckBoxWrapper 
} from "./AuthForm.styled";

interface AuthFormProps {
    mode: 'login' | 'register';
}

const AuthForm = ({ mode }: AuthFormProps) => {
    const [form, setForm] = useState({ username: "", password: "", confirmPassword: "" });
    const [error, setError] = useState<string | null>(null);
    const {login, register} = useAuthContext();
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

      if (/\s/.test(form.password)) {
          setError("Error! Password cannot contain spaces.");
          return false;
      }

      if (/\s/.test(form.username)) {
          setError("Error! Username cannot contain spaces.");
          return false;
      }

      if (form.password === form.username) {
          setError("Error! Password cannot be the same as username.");
          return false;
      }

      return true;
    };



    const handleSubmit = async (e: React.FormEvent) => {
        e.preventDefault();
        try {
            if (mode === "login") await login(form);
            else {
              if (!validateData()) return;
              await register(form);
            }
            navigate("/workspace");
        } catch (err: any) {
            setError(err.message || "The error occurred during authentication");
        }
    };

    return (
      <>
        <LeftContainer>
          <FormContainer onSubmit={handleSubmit}>
            <Title> {mode === "login" ? "Welcome Back" : "Create an account"} </Title>
            <Subtitle> {mode === "login" ? "Welcome back! Please enter your details" : "Please fill in the form to create an account"} </Subtitle>
            <Label> Username </Label>
            <Input type = "text" placeholder="Enter your username" value={form.username} onChange={(e) => setForm({ ...form, username: e.target.value })} />
            <Label> Password </Label>
            <Input type = "password" placeholder="*************" value={form.password} onChange={(e) => setForm({ ...form, password: e.target.value })} />
            {mode === "register" && (
              <>
                <Label> Confirm Password </Label>
                <Input type = "password" placeholder="Confirm your password" value={form.confirmPassword} onChange={(e) => setForm({ ...form, confirmPassword: e.target.value })} />
              </>
            )}
            <CheckBoxWrapper>
              {mode === "login" && ( 
                <> <Checkbox type="checkbox" /> Remember me </>
              )}
            </CheckBoxWrapper>
            <Button type="submit" color="#FC5D08" style={{marginTop: '.3rem'}}>
              {mode === "login" ? "Sign In" : "Sign Up"}
            </Button>
            <Button type="button" color="#f3f3f3" style={{ marginTop: '0.5rem' }} fontColor="#121212">
              {mode === "login" ? (
                <div style={{ display: "flex", alignItems: "center", justifyContent: "center", gap: "0.5rem" }}>
                  Sign in with Google
                </div>
              ) : (
                <div style={{ display: "flex", alignItems: "center", justifyContent: "center", gap: "0.5rem" }}>
                  <img src={GoogleIcon} alt="Google" width="30px" height="30px" />
                  Sign up with Google
                </div>
              )}
            </Button>
            <p style={{textAlign: "center", fontSize: ".9rem", marginTop: ".3rem"}}> 
              {mode === "login" && ( 
                <> 
                  Don't have an account? {mode === "login" ? <a href="/register" style={{color: "#ff8000ff"}}> Sign up</a> : <a href="/login"> Log in</a>}
                </>
              )}
            </p>
            {error && <ErrorText>{error}</ErrorText>}
          </FormContainer> 
        </LeftContainer>
        <RightContainer> </RightContainer>
      </>
  );
};

export default AuthForm;





