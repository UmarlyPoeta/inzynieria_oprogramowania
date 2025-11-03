import { styled } from 'styled-components';

export const FormContainer = styled.form`
  display: flex;
  flex-direction: column;
  justify-content: center;
  gap: 1rem;
  width: 60%;
  padding: 15rem;
  background-color: #ffffff;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.1);
`;

export const Title = styled.h2`
  margin-bottom: 0.5rem;
  text-align: center;
  color: #121212;
  font-size: 2.1rem;
`;

export const Input = styled.input`
  padding: 0.75rem;
  border: 1px solid #ccc;
  border-radius: 8px;
  font-size: 1rem;
  &:focus {
    outline: none;
    border-color: #0077ff;
  }
`;

export const Button = styled.button<{ color: string }>`
  background-color: ${(p) => p.color};
  color: white;
  border: none;
  border-radius: 8px;
  padding: 0.75rem;
  font-size: 1rem;
  cursor: pointer;
  transition: background-color 0.2s;
  &:hover {
    opacity: 0.9;
  }
`;

export const ErrorText = styled.p`
  color: #d32f2f;
  font-size: 0.9rem;
  text-align: center;
`;