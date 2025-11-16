import React, { createContext, useContext, useState } from "react";

import type { Device, RouterConfig, SwitchConfig, PCConfig, Link, Group } from "@/types"

const defaultRouterConfig = (): RouterConfig => ({
  hostname: "Router",
  domainName: "local",
  interfaces: [
    { name: "GigabitEthernet0/0", status: "down", dhcpEnabled: false },
    { name: "GigabitEthernet0/1", status: "down", dhcpEnabled: false },
  ],
  routingProtocol: "STATIC",
  ospf: { processId: 1, area: 0, networks: [] },
  rip: { version: 2, networks: [] },
  eigrp: { asNumber: 1, networks: [] },
  bgp: { asNumber: 65000, neighbors: [] },
  staticRoutes: [],
  natEnabled: false,
  natRules: [],
  firewallEnabled: false,
  firewallRules: [],
  dhcpEnabled: false,
  dhcpPools: [],
  vpnEnabled: false,
  vpnTunnels: [],
  qosEnabled: false,
  qosPolicies: [],
});

const defaultSwitchConfig = (): SwitchConfig => ({
  hostname: "Switch",
  vlans: [{ id: 1, name: "default" }],
  stpEnabled: true,
  interfaces: [],
});

const defaultPCConfig = (): PCConfig => ({
  hostname: "PC",
  interfaces: [
    { name: "eth0", ip: "", subnet: "", gateway: "" }
  ],
  description: "",
});


interface EditorContextType {
  devices: Device[];
  links: Link[];
  groups: Group[];

  undo: () => void;
  redo: () => void;

  addDevice: (device: Device) => void;
  deleteDevice: (id: string) => void;
  moveDevice: (id: string, x: number, y: number) => void;
  selectedDeviceId?: string; 
  selectDevice: (id: string | undefined) => void;
  selectedDeviceIds: string[];
  toggleSelectDevice: (id: string) => void;
  selectAll: () => void;
  setSelectedDeviceIds: React.Dispatch<React.SetStateAction<string[]>>;
  updateDeviceConfig: (id: string, configUpdate: Partial<Device["config"]>) => void;

  removeDeviceFromGroup: (deviceId: string) => void;
  moveDeviceToGroup: (deviceId: string, groupId: string) => void;


  addGroup: (group: Group) => void;
  renameGroup: (id: string, name: string) => void;
  deleteGroup: (id: string) => void;
  toggleGroupCollapsed: (id: string, coll: boolean) => void;

  addLink: (link: Link) => void;
  deleteLink: (id: string) => void;
  connectingDeviceId?: string | null;
  startConnecting: () => void;
  selectDeviceForLink: (id: string) => void;
  selectDevicePortForLink: (deviceId: string, port: string) => void;
  stopConnecting: () => void;
  connectingModeActive: boolean;
  connectingPort?: string | null;

  resetTopology: () => void;
}


const EditorContext = createContext<EditorContextType | undefined>(undefined);

export const EditorProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [devices, setDevices] = useState<Device[]>([]);
  const [links, setLinks] = useState<Link[]>([]);
  const [groups, setGroups] = useState<Group[]>([]);
  const [history, setHistory] = useState<{ devices: Device[]; links: Link[]; groups: Group[] }[]>([]);
  const [redoStack, setRedoStack] = useState<typeof history>([]);
  const [selectedDeviceId, setSelectedDeviceId] = useState<string | undefined>(undefined);
  const [connectingPort, setConnectingPort] = useState<string | null>(null);
  const [connectingDeviceId, setConnectingDeviceId] = useState<string | null>(null);
  const [connectingModeActive, setConnectingModeActive] = useState(false);
  const [selectedDeviceIds, setSelectedDeviceIds] = useState<string[]>([]);

  const resetTopology = () => {
    setDevices([]);
    setGroups([]);
    setLinks([]);
  };

  const selectDevice = (id?: string) => {
    setSelectedDeviceId(id);
    setSelectedDeviceIds(id ? [id] : []);
  };

  const addLinkWithHistory = (link: Link) => {
    pushToHistory();
    setLinks(prev => [...prev, link]);
  };

  const deleteLink = (id: string) => {
    pushToHistory(); 
    setLinks(prev => prev.filter(l => l.id !== id));
  };

  const selectDevicePortForLink = (deviceId: string, port: string) => {
    if (!connectingDeviceId) {
      setConnectingDeviceId(deviceId);
      setConnectingPort(port);
      setConnectingModeActive(true);
    } else if (connectingDeviceId === deviceId) {
      setConnectingPort(port);
    } else {
      const newLink: Link = {
        id: Math.random().toString(36).substring(2, 9),
        from: connectingDeviceId,
        to: deviceId,
        fromPort: connectingPort!,
        toPort: port
      };
      addLinkWithHistory(newLink);

      setConnectingDeviceId(null);
      setConnectingPort(null);
      setConnectingModeActive(false);
    }
  };


  const toggleSelectDevice = (id: string) => {
    setSelectedDeviceIds(prev => {
      if (prev.includes(id)) {
        const next = prev.filter(x => x !== id);
        setSelectedDeviceId(next.length === 1 ? next[0] : undefined);
        return next;
      }
      const next = [...prev, id];
      setSelectedDeviceId(undefined);
      return next;
    });
  };

  const selectAll = () => {
    setSelectedDeviceIds(devices.flatMap(d => d.id ? [d.id] : []));
    setSelectedDeviceId(undefined);
  };


  const pushToHistory = () => {
    setHistory(prev => [
      ...prev,
      { devices: structuredClone(devices), links: structuredClone(links), groups: structuredClone(groups) }
    ]);
    setRedoStack([]); 
  };

   const undo = () => {
    setHistory(prev => {
      if (prev.length === 0) return prev;
      const last = prev[prev.length - 1];
      setRedoStack(r => [{ devices, links, groups }, ...r]);
      setDevices(last.devices);
      setLinks(last.links);
      setGroups(last.groups);
      return prev.slice(0, -1);
    });
  };

  const redo = () => {
    setRedoStack(prev => {
      if (prev.length === 0) return prev;
      const next = prev[0];
      setHistory(h => [...h, { devices, links, groups }]);
      setDevices(next.devices);
      setLinks(next.links);
      setGroups(next.groups);
      return prev.slice(1);
    });
  };

  const startConnecting = () => {
    setConnectingModeActive(true);
    setConnectingDeviceId(null);
  };

  const stopConnecting = () => {
    setConnectingModeActive(false);
    setConnectingDeviceId(null);
  };

  const selectDeviceForLink = (id: string) => {
    if (!connectingDeviceId) {
      console.log("First device selected for link:", id);
      setConnectingDeviceId(id); 
    } else if (connectingDeviceId !== id) {
      console.log("Second device selected for link:", id);
      const newLink = { id: Math.random().toString(36).substring(2, 9), from: connectingDeviceId, to: id };
      console.log("Adding link:", newLink);
      addLink(newLink);
      setConnectingDeviceId(null);
      setConnectingModeActive(false);
    } else {
      console.log("Clicked the same device twice:", id);
    }
  };


  const defaultConfig = (type: Device["type"]) => {
    switch (type) {
      case "router": return defaultRouterConfig();
      case "switch": return defaultSwitchConfig();
      case "pc": return defaultPCConfig();
    }
  };

  const addDevice = (device: Device) => {
    pushToHistory();
    setDevices(prev => {
      let name = device.name;
      if (!name) { 
        const count = prev.filter(d => d.type === device.type).length;
        name = `${device.type.charAt(0).toUpperCase() + device.type.slice(1)} ${count + 1}`;
      }

      const id = device.id ?? Math.random().toString(36).substring(2, 9); 

      return [
        ...prev, 
        { 
          ...device, 
          id,
          name, 
          config: device.config ?? defaultConfig(device.type) 
        }
      ];
    });
  };


  const updateDeviceConfig = (id: string, configUpdate: Partial<Device["config"]>) => {
    setDevices(prev =>
      prev.map(d => 
        d.id === id 
          ? { ...d, config: { ...d.config, ...configUpdate } as typeof d.config } 
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
    pushToHistory();
    setDevices(prev => prev.filter(d => d.id !== id));
    setLinks(prev => prev.filter(l => l.from !== id && l.to !== id));
  }

  const deleteGroup = (id: string) => {
    pushToHistory();
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
    <EditorContext.Provider value={{
      devices,
      links,
      groups,
      deleteGroup,
      deleteDevice,
      addDevice,
      addLink,
      addGroup,
      renameGroup,
      toggleGroupCollapsed,
      moveDevice,
      removeDeviceFromGroup,
      moveDeviceToGroup,
      selectedDeviceId,
      selectDevice,
      updateDeviceConfig,
      startConnecting,
      stopConnecting,
      connectingModeActive,
      connectingDeviceId,
      selectDeviceForLink,
      undo,
      redo,
      selectedDeviceIds,
      toggleSelectDevice,
      selectAll,
      setSelectedDeviceIds,
      selectDevicePortForLink,
      connectingPort,
      resetTopology,
      deleteLink
    }}>
    {children}
    </EditorContext.Provider>
  );
};

export const useEditor = () => {
  const context = useContext(EditorContext);
  if (!context) throw new Error("useEditor must be used within EditorProvider");
  return context;
};

