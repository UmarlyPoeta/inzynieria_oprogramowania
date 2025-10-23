import React, { createContext, useContext, useState } from "react";

export interface Device {
  id: string;
  type: "router" | "switch" | "pc";
  x: number;
  y: number;
  name?: string;       
  groupId?: string;   
}

export interface Group {
  id: string;
  name: string;
  collapsed: boolean;
  parentGroupId?: string; 
}

export interface Link {
  id: string;
  from: string;
  to: string;
}

interface EditorContextType {
  devices: Device[];
  links: Link[];
  groups: Group[];
  addDevice: (device: Device) => void;
  addLink: (link: Link) => void;
  moveDevice: (id: string, x: number, y: number) => void;
  addGroup: (group: Group) => void;
  renameGroup: (id: string, name: string) => void;
  toggleGroupCollapsed: (id: string, coll: boolean) => void;
}

const EditorContext = createContext<EditorContextType | undefined>(undefined);

export const EditorProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [devices, setDevices] = useState<Device[]>([]);
  const [links, setLinks] = useState<Link[]>([]);
  const [groups, setGroups] = useState<Group[]>([]);

  const addDevice = (device: Device) => { 
    setDevices(prev => {
      let name = device.name;
      if (!name) { 
        const count = prev.filter(d => d.type === device.type).length;
        name = `${device.type.charAt(0).toUpperCase() + device.type.slice(1)} ${count + 1}`;
      }
      return [...prev, { ...device, name }];
    });
  };
    
  const addLink = (link: Link) => setLinks(prev => [...prev, link]);
  const moveDevice = (id: string, x: number, y: number) =>
    setDevices(prev => prev.map(d => (d.id === id ? { ...d, x, y } : d)));

  const addGroup = (group: Group) => setGroups(prev => [...prev, group]);
  const renameGroup = (id: string, name: string) =>
    setGroups(prev => prev.map(g => (g.id === id ? { ...g, name } : g)));
  const toggleGroupCollapsed = (id: string, coll: boolean) => { 
    setGroups(prev => prev.map(g => (g.id === id ? { ...g, collapsed: coll } : g)));
  };

  return (
    <EditorContext.Provider
      value={{ devices, links, groups, addDevice, addLink, addGroup, renameGroup, toggleGroupCollapsed, moveDevice }}
    >
      {children}
    </EditorContext.Provider>
  );
};

export const useEditor = () => {
  const context = useContext(EditorContext);
  if (!context) throw new Error("useEditor must be used within EditorProvider");
  return context;
};

