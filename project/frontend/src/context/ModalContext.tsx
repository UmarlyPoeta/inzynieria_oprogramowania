import React, { createContext, useContext, useState } from "react";
import { Modal } from "@/components";

interface ModalContextType {
  showModal: (options: { title: string; message: string; onConfirm: () => void }) => void;
  hideModal: () => void;
}

const ModalContext = createContext<ModalContextType | undefined>(undefined);

export const ModalProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [modalProps, setModalProps] = useState<{
    title: string;
    message: string;
    onConfirm: () => void;
    isOpen: boolean;
  }>({ title: "", message: "", onConfirm: () => {}, isOpen: false });

  const showModal = ({ title, message, onConfirm }: Omit<typeof modalProps, "isOpen">) => {
    setModalProps({ title, message, onConfirm, isOpen: true });
  };

  const hideModal = () => {
    setModalProps(prev => ({ ...prev, isOpen: false }));
  };

  return (
    <ModalContext.Provider value={{ showModal, hideModal }}>
      {children}
      <Modal
        isOpen={modalProps.isOpen}
        title={modalProps.title}
        message={modalProps.message}
        onConfirm={() => { modalProps.onConfirm(); hideModal(); }}
        onCancel={hideModal}
      />
    </ModalContext.Provider>
  );
};

export const useModal = () => {
  const context = useContext(ModalContext);
  if (!context) throw new Error("useModal must be used within GlobalModalProvider");
  return context;
};
