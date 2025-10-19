import React, { createContext, useState, useContext } from "react";

export interface Device {
  id: string;
  type: "router" | "switch" | "pc";
  x: number;
  y: number;
}

interface EditorContextType {
  devices: Device[];
  addDevice: (device: Device) => void;
}

const EditorContext = createContext<EditorContextType | undefined>(undefined);

export const EditorProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [devices, setDevices] = useState<Device[]>([]);

  const addDevice = (device: Device) => setDevices([...devices, device]);

  return (
    <EditorContext.Provider value={{ devices, addDevice }}>
      {children}
    </EditorContext.Provider>
  );
};

export const useEditor = () => {
  const context = useContext(EditorContext);
  if (!context) throw new Error("useEditor must be used within EditorProvider");
  return context;
};
