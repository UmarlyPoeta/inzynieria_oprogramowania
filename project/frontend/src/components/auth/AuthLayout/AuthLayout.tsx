import { Wrapper } from './AuthLayout.styled';

const AuthLayout = ({ children, gradient, }: { children: React.ReactNode; gradient?: string; }) => {
    return <Wrapper gradient={gradient}>{children}</Wrapper>;
}

export default AuthLayout;