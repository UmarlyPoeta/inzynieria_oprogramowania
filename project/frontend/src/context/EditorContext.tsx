import React, { createContext, useContext, useState } from "react";

export interface Device {
  id: string;
  type: "router" | "switch" | "pc";
  x: number;
  y: number;
}

export interface Link {
  id: string;
  from: string;
  to: string;
}

interface EditorContextType {
  devices: Device[];
  links: Link[];
  addDevice: (device: Device) => void;
  addLink: (link: Link) => void;
  moveDevice: (id: string, x: number, y: number) => void;
}

const EditorContext = createContext<EditorContextType | undefined>(undefined);

export const EditorProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [devices, setDevices] = useState<Device[]>([]);
  const [links, setLinks] = useState<Link[]>([]);

  const addDevice = (device: Device) => setDevices(prev => [...prev, device]);
  const addLink = (link: Link) => setLinks(prev => [...prev, link]);
  const moveDevice = (id: string, x: number, y: number) =>
    setDevices(prev => prev.map(d => (d.id === id ? { ...d, x, y } : d)));

  return (
    <EditorContext.Provider value={{ devices, links, addDevice, addLink, moveDevice }}>
      {children}
    </EditorContext.Provider>
  );
};

export const useEditor = () => {
  const context = useContext(EditorContext);
  if (!context) throw new Error("useEditor must be used within EditorProvider");
  return context;
};

