import styled from "styled-components";

export const ModalOverlay = styled.div`
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(0,0,0,0.4);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 9999;
`;

export const ModalContent = styled.div`
  background: #fff;
  border-radius: 12px;
  width: 400px;
  max-width: 90%;
  padding: 24px;
  box-shadow: 0 10px 30px rgba(0,0,0,0.2);
  display: flex;
  flex-direction: column;
`;

export const ModalHeader = styled.h2`
  margin: 0;
  font-size: 20px;
  font-weight: 600;
  color: #121212;
`;

export const ModalBody = styled.div`
  margin: 16px 0;
  font-size: 16px;
  color: #333;
`;

export const ModalFooter = styled.div`
  display: flex;
  justify-content: flex-end;
  gap: 12px;
`;

interface ButtonProps {
  variant?: "primary" | "secondary" | "danger";
}

export const Button = styled.button<ButtonProps>`
  padding: 8px 16px;
  border-radius: 6px;
  font-size: 14px;
  cursor: pointer;
  border: none;
  font-weight: 500;
  transition: background 0.2s;

  ${({ variant }) => {
    switch(variant) {
      case "secondary":
        return `
          background: #f0f0f0;
          color: #121212;
          &:hover { background: #e0e0e0; }
        `;
      case "danger":
        return `
          background: #ff4d4f;
          color: #fff;
          &:hover { background: #e04144; }
        `;
      default:
        return `
          background: #ff8818;
          color: #fff;
          &:hover { background: #ff8818; }
        `;
    }
  }}
`;

