import React, { useState, useEffect } from "react";
import { ModalOverlay, ModalContent, ModalHeader, ModalBody, ModalFooter, Button } from "../Modal/Modal.styled";
import styled from "styled-components";

interface LinkConfigModalProps {
  isOpen: boolean;
  initialValues?: {
    delay?: number;
    bandwidth?: number;
    packetLoss?: number;
  };
  onSave: (data: { delay: number; bandwidth: number; packetLoss: number }) => void;
  onCancel: () => void;
  onDelete: () => void;
}

// Dodaj te style do Modal.styled.ts
const InputGroup = styled.div`
  margin-bottom: 1rem;
`;

const Label = styled.label`
  display: block;
  margin-bottom: 0.5rem;
  font-weight: 500;
  color: #333;
`;

const Input = styled.input`
  width: 100%;
  padding: 0.5rem;
  border: 1px solid #ccc;
  border-radius: 4px;
  font-size: 1rem;

  &:focus {
    outline: none;
    border-color: #007bff;
  }
`;

const LinkConfigModal: React.FC<LinkConfigModalProps> = ({ 
  isOpen, 
  initialValues,
  onSave, 
  onCancel,
  onDelete
}) => {
  const [delay, setDelay] = useState(initialValues?.delay ?? 0);
  const [bandwidth, setBandwidth] = useState(initialValues?.bandwidth ?? 1000);
  const [packetLoss, setPacketLoss] = useState(initialValues?.packetLoss ?? 0);

  useEffect(() => {
    if (isOpen && initialValues) {
      setDelay(initialValues.delay ?? 0);
      setBandwidth(initialValues.bandwidth ?? 1000);
      setPacketLoss(initialValues.packetLoss ?? 0);
    }
  }, [isOpen, initialValues]);

  if (!isOpen) return null;

  return (
    <ModalOverlay onClick={onCancel}>
      <ModalContent onClick={(e) => e.stopPropagation()}>
        <ModalHeader>Configure Connection</ModalHeader>
        <ModalBody>
          <InputGroup>
            <Label htmlFor="delay">Delay (ms)</Label>
            <Input
              id="delay"
              type="number"
              min="0"
              value={delay}
              onChange={(e) => setDelay(Number(e.target.value))}
            />
          </InputGroup>

          <InputGroup>
            <Label htmlFor="bandwidth">Bandwidth (Mbps)</Label>
            <Input
              id="bandwidth"
              type="number"
              min="1"
              value={bandwidth}
              onChange={(e) => setBandwidth(Number(e.target.value))}
            />
          </InputGroup>

          <InputGroup>
            <Label htmlFor="packetLoss">Packet Loss (%)</Label>
            <Input
              id="packetLoss"
              type="number"
              min="0"
              max="100"
              step="0.1"
              value={packetLoss}
              onChange={(e) => setPacketLoss(Number(e.target.value))}
            />
          </InputGroup>
        </ModalBody>
        <ModalFooter>
          <Button onClick={onDelete} variant="danger">Delete</Button>
          <div style={{ marginLeft: "auto", display: "flex", gap: "0.5rem" }}>
            <Button onClick={onCancel} variant="secondary">Cancel</Button>
            <Button onClick={() => onSave({ delay, bandwidth, packetLoss })} variant="primary">
              Save
            </Button>
          </div>
        </ModalFooter>
      </ModalContent>
    </ModalOverlay>
  );
};

export default LinkConfigModal;