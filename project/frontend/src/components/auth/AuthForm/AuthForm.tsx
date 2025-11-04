import { useState } from "react";
import { useAuthContext } from '../../../context/AuthContext'; 
import { useNavigate } from "react-router-dom";
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
            <Button type="button" color="#f3f3f3" style={{marginTop: '0.5rem'}} fontColor="#121212">
              {mode === "login" ? "Sign in with google" : "Sign up with google"}
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
        <RightContainer>
          
        </RightContainer>
      </>
  );
};

export default AuthForm;





