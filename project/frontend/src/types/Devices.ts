export interface RouterConfig {
  // Podstawowe
  hostname: string;
  domainName?: string;

  // Interfejsy fizyczne
  interfaces: {
    name: string;               // np. "GigabitEthernet0/0"
    ip?: string;                // np. "192.168.1.1"
    subnet?: string;            // np. "255.255.255.0"
    status: "up" | "down";      // interfejs włączony/wyłączony
    dhcpEnabled?: boolean;      // DHCP klient na interfejsie
  }[];

  // Routing dynamiczny
  routingProtocol: "None" | "RIP" | "OSPF" | "EIGRP" | "BGP";
  ospf?: {
    processId: number;
    area: number;
    networks: { network: string; wildcard: string }[];
  };
  rip?: {
    version: 1 | 2;
    networks: string[];
  };
  eigrp?: {
    asNumber: number;
    networks: string[];
  };
  bgp?: {
    asNumber: number;
    neighbors: { ip: string; remoteAs: number }[];
  };

  // Routing statyczny
  staticRoutes: {
    destination: string;  // np. "10.0.0.0"
    subnet: string;       // np. "255.255.255.0"
    gateway: string;      // np. "192.168.1.254"
  }[];

  // NAT i Firewall
  natEnabled: boolean;
  natRules?: {
    source: string;
    destination: string;
    translated: string;
  }[];
  firewallEnabled: boolean;
  firewallRules?: {
    action: "permit" | "deny";
    protocol: "tcp" | "udp" | "icmp" | "any";
    source: string;
    destination: string;
    port?: number;
  }[];

  // DHCP serwer
  dhcpEnabled: boolean;
  dhcpPools?: {
    name: string;
    network: string;
    subnet: string;
    defaultGateway: string;
    dns?: string;
    rangeStart: string;
    rangeEnd: string;
  }[];

  // VPN i tunelowanie
  vpnEnabled: boolean;
  vpnTunnels?: {
    name: string;
    type: "IPSec" | "GRE";
    remoteIp: string;
    preSharedKey?: string;
  }[];

  // QoS 
  qosEnabled: boolean;
  qosPolicies?: {
    name: string;
    description?: string;
    type: "priority" | "bandwidth" | "rate-limit";
    value: string | number;
  }[];
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
  id?: string;
  type: "router" | "switch" | "pc";
  x: number;
  y: number;
  name?: string;
  groupId?: string;
  config?: RouterConfig | SwitchConfig | PCConfig;
}