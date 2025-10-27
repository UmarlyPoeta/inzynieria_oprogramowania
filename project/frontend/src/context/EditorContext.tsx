import React, { createContext, useContext, useState } from "react";

import type { Device, RouterConfig, SwitchConfig, PCConfig, Link, Group } from "@/types"

interface EditorContextType {
  devices: Device[];
  links: Link[];
  groups: Group[];
  addDevice: (device: Device) => void;
  deleteDevice: (id: string) => void;
  removeDeviceFromGroup: (deviceId: string) => void;
  moveDeviceToGroup: (deviceId: string, groupId: string) => void;
  addLink: (link: Link) => void;
  moveDevice: (id: string, x: number, y: number) => void;
  addGroup: (group: Group) => void;
  renameGroup: (id: string, name: string) => void;
  deleteGroup: (id: string) => void;
  toggleGroupCollapsed: (id: string, coll: boolean) => void;
  selectedDeviceId?: string; 
  selectDevice: (id: string | undefined) => void; 
  updateDeviceConfig: (id: string, configUpdate: Partial<Device["config"]>) => void;
}

const EditorContext = createContext<EditorContextType | undefined>(undefined);

export const EditorProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [devices, setDevices] = useState<Device[]>([]);
  const [links, setLinks] = useState<Link[]>([]);
  const [groups, setGroups] = useState<Group[]>([]);
  const [selectedDeviceId, setSelectedDeviceId] = useState<string | undefined>(undefined);

  const selectDevice = (id?: string) => setSelectedDeviceId(id);

  const defaultConfig = (type: Device["type"]) => {
    switch(type) {
      case "router":
        return { interfaces: [], routingProtocol: "None" } as RouterConfig;
      case "switch":
        return { vlans: [], stpEnabled: false } as SwitchConfig;
      case "pc":
        return { ip: "", gateway: "" } as PCConfig;
    }
  };

  const addDevice = (device: Device) => {
    setDevices(prev => {
      let name = device.name;
      if (!name) { 
        const count = prev.filter(d => d.type === device.type).length;
        name = `${device.type.charAt(0).toUpperCase() + device.type.slice(1)} ${count + 1}`;
      }
      return [...prev, { ...device, name, config: device.config ?? defaultConfig(device.type) }];
    });
  };

  const updateDeviceConfig = (id: string, configUpdate: Partial<Device["config"]>) => {
    setDevices(prev =>
      prev.map(d => 
        d.id === id 
          ? { ...d, config: { ...defaultConfig(d.type), ...d.config, ...configUpdate } } 
          : d
      )
    );
  };

  const removeDeviceFromGroup = (deviceId: string) => {
    setDevices(prev => prev.map(d => (d.id === deviceId ? { ...d, groupId: undefined } : d)));
  };

  const moveDeviceToGroup = (deviceId: string, groupId: string) => {
    setDevices(prev => prev.map(d => (d.id === deviceId ? { ...d, groupId } : d)));
  };
  
  const deleteDevice = (id: string) => {
    setDevices(prev => prev.filter(d => d.id !== id));
    setLinks(prev => prev.filter(l => l.from !== id && l.to !== id));
  }

  const deleteGroup = (id: string) => {
    setGroups(prev => prev.filter(g => g.id !== id));
    setDevices(prev => prev.map(d => (d.groupId === id ? { ...d, groupId: undefined } : d)));
  }
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
      value={{ devices, links, groups, deleteGroup, deleteDevice, addDevice, addLink, addGroup, renameGroup, toggleGroupCollapsed, moveDevice, removeDeviceFromGroup, moveDeviceToGroup, selectedDeviceId, selectDevice, updateDeviceConfig }}
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

