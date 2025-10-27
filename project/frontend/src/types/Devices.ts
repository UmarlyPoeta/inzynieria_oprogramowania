export interface RouterConfig {
  interfaces: { name: string; ip: string }[];
  routingProtocol: "OSPF" | "RIP" | "None";
}

export interface SwitchConfig {
  vlans: { id: number; name: string }[];
  stpEnabled: boolean;
}

export interface PCConfig {
  ip: string;
  gateway: string;
}

export interface Device {
  id: string;
  type: "router" | "switch" | "pc";
  x: number;
  y: number;
  name?: string;
  groupId?: string;
  config?: RouterConfig | SwitchConfig | PCConfig;
}