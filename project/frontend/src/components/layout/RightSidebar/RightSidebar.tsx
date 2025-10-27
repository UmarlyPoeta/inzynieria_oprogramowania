import React, { useState } from "react";
import styled from "styled-components";
import { useEditor } from "@/context/EditorContext";
import type { RouterConfig } from "@/types";
import { Play } from "lucide-react";

const Sidebar = styled.div`
  width: 280px;
  background-color: ${(props: any) => props.theme.colors.surface};
  border-left: 2px solid #ededf5;
  overflow-y: auto;
  z-index: 2;
`;

const SimulationContainer = styled.div`
  display: flex;
  justify-content: space-between;
  font-size: 1rem;
  font-weight: bold;
  text-transform: uppercase;
  align-items: center;
  flex-direction: row;
  width: 100%;
  padding: 10px;
  border-bottom: 2px solid #ededf5;
`;

const IconButton = styled.button`
  width: 80px;
  height: 50px;
  border: none;
  border-radius: 10px;
  background: none;
  color: #121212;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  transition: background 0.2s;

  &:hover {
    background-color: rgba(255, 174, 0, 0.25);
    svg {
      color: rgba(255, 153, 0, 1);
    }
  }

  svg {
    width: 20px;
    height: 20px;
    color: #121212;
  }
`;

const PanelWrapper = styled.div`
  padding: 12px;
`;

const TabsWrapper = styled.div`
  display: flex;
  gap: 4px;
  margin-bottom: 12px;
  flex-wrap: wrap;
`;

const TabButton = styled.button<{ active: boolean }>`
  padding: 6px 12px;
  border: 1px solid ${(props) => (props.active ? "#FFA500" : "#ccc")};
  border-bottom: ${(props) => (props.active ? "2px solid orange" : "1px solid #ccc")};
  border-radius: 6px 6px 0 0;
  background-color: ${(props) => (props.active ? "#fff" : "#f7f7f7")};
  cursor: pointer;
  font-weight: ${(props) => (props.active ? "bold" : "normal")};
  font-size: 0.85rem;
`;

const Section = styled.div`
  margin-bottom: 12px;
`;

const Input = styled.input`
  width: 100%;
  margin-top: 4px;
  margin-bottom: 8px;
  padding: 4px 6px;
  border: 1px solid #ccc;
  border-radius: 4px;
`;

const Select = styled.select`
  width: 100%;
  margin-top: 4px;
  margin-bottom: 8px;
  padding: 4px 6px;
  border: 1px solid #ccc;
  border-radius: 4px;
`;

const DeviceDetailsPanel: React.FC = () => {
  const { devices, selectedDeviceId, updateDeviceConfig } = useEditor();
  const device = devices.find((d) => d.id === selectedDeviceId);
  const [activeTab, setActiveTab] = useState<string>("General");

  if (!device) return <p style={{ padding: "10px" }}>Select a device to see details</p>;

  const handleInputChange = (key: string, value: any) => {
    updateDeviceConfig(device.id, { [key]: value });
  };

  // zakładki
  let tabs: string[] = ["General"];
  if (device.type === "router") tabs.push("Interfaces", "Routing", "NAT", "DHCP", "Security", "Services");

  const renderRouterTab = (tab: string) => {
    const config = device.config as RouterConfig;
    switch (tab) {
      case "General":
        return (
          <Section>
            <label>Hostname</label>
            <Input
              value={config?.hostname || ""}
              onChange={(e) => handleInputChange("hostname", e.target.value)}
            />
            <label>Domain Name</label>
            <Input
              value={config?.domainName || ""}
              onChange={(e) => handleInputChange("domainName", e.target.value)}
            />
          </Section>
        );
      case "Interfaces":
        return (
          <Section>
            {config?.interfaces?.map((intf, idx) => (
              <div key={idx} style={{ marginBottom: "10px" }}>
                <label>{intf.name} IP</label>
                <Input
                  value={intf.ip || ""}
                  onChange={(e) => {
                    const newIfs = [...(config.interfaces || [])];
                    newIfs[idx] = { ...newIfs[idx], ip: e.target.value };
                    handleInputChange("interfaces", newIfs);
                  }}
                />
                <label>Subnet</label>
                <Input
                  value={intf.subnet || ""}
                  onChange={(e) => {
                    const newIfs = [...(config.interfaces || [])];
                    newIfs[idx] = { ...newIfs[idx], subnet: e.target.value };
                    handleInputChange("interfaces", newIfs);
                  }}
                />
                <label>Status</label>
                <Select
                  value={intf.status || "up"}
                  onChange={(e) => {
                    const newIfs = [...(config.interfaces || [])];
                    newIfs[idx] = { ...newIfs[idx], status: e.target.value as "up" | "down" };
                    handleInputChange("interfaces", newIfs);
                  }}
                >
                  <option value="up">Up</option>
                  <option value="down">Down</option>
                </Select>
              </div>
            ))}
          </Section>
        );
      case "Routing":
        return (
          <Section>
            <label>Routing Protocol</label>
            <Select
              value={config?.routingProtocol || "None"}
              onChange={(e) => handleInputChange("routingProtocol", e.target.value)}
            >
              <option value="None">None</option>
              <option value="RIP">RIP</option>
              <option value="OSPF">OSPF</option>
              <option value="EIGRP">EIGRP</option>
              <option value="BGP">BGP</option>
            </Select>
            <label>Static Routes</label>
            <Input
              value={config?.staticRoutes?.join(", ") || ""}
              onChange={(e) =>
                handleInputChange(
                  "staticRoutes",
                  e.target.value.split(",").map((r) => r.trim())
                )
              }
            />
          </Section>
        );
      case "NAT":
        return <Section>NAT configuration inputs...</Section>;
      case "DHCP":
        return <Section>DHCP configuration inputs...</Section>;
      case "Security":
        return <Section>Firewall / ACL configuration...</Section>;
      case "Services":
        return <Section>DNS, HTTP, SNMP services...</Section>;
      default:
        return null;
    }
  };

  const { startConnecting } = useEditor();

  const renderTabContent = () => {
    if (device.type === "router") return renderRouterTab(activeTab);
    return <Section>Configuration for {device.type} coming soon...</Section>;
  };

  return (
    <PanelWrapper>
      <h3>{device.name || device.type}</h3>
      <TabsWrapper style={{ borderBottom: "2px solid #ededf5", paddingBottom: "4px" }}>
        {tabs.map((tab) => (
          <TabButton key={tab} active={tab === activeTab} onClick={() => setActiveTab(tab)}>
            {tab}
          </TabButton>
        ))}
      </TabsWrapper>
      {renderTabContent()}
      <br />
      <IconButton title="Connect" onClick={startConnecting} style={{border: "2px solid #e0e0e0a6"}}>
         Connect Nodes
      </IconButton>
    </PanelWrapper>
  );
};

const RightSidebar: React.FC = () => {
  return (
    <Sidebar>
      <SimulationContainer>
        <h2>Configuration</h2>
        <IconButton title="Run Simulation">
          <Play />
        </IconButton>
      </SimulationContainer>
      <DeviceDetailsPanel />
    </Sidebar>
  );
};

export default RightSidebar;




