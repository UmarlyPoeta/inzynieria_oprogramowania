import { useEditor } from "@/context/EditorContext";
import type { RouterConfig } from "@/types";
import React, { useState } from "react";
import { Play } from "lucide-react";
import { 
  SimulationContainer, 
  Sidebar, 
  IconButton, 
  PanelWrapper, 
  Section, 
  Input, 
  Select,
  SectionHeader,
  AccordionContent
} from './RightSidebar.styled';

const DeviceDetailsPanel: React.FC = () => {
  const { devices, selectedDeviceId, updateDeviceConfig } = useEditor();
  const device = devices.find((d) => d.id === selectedDeviceId);
  const [expandedSection, setExpandedSection] = useState<string | null>("General");

  if (!device) return <p style={{ padding: "10px" }}>Select a device to see details</p>;

  const handleInputChange = (key: string, value: any) => {
    updateDeviceConfig(device.id, { [key]: value });
  };

  let sections: string[] = ["General"];
  if (device.type === "router") sections.push("Interfaces", "Routing", "NAT", "DHCP", "Security", "Services");

  const renderRouterSection = (section: string) => {
    const config = device.config as RouterConfig;
    switch (section) {
      case "General":
        return (
          <Section>
            <Input value={config?.hostname || ""} onChange={(e) => handleInputChange("hostname", e.target.value)} placeholder="Hostname"/>
            <Input value={config?.domainName || ""} onChange={(e) => handleInputChange("domainName", e.target.value)} placeholder="Domain Name"/>
          </Section>
        );
      case "Interfaces":
        return (
          <>
            {config?.interfaces?.map((intf, idx) => (
              <div key={idx}>
                <strong>{intf.name}</strong>
                <Section>
                  <Input
                    value={intf.ip || ""}
                    placeholder="Adres IP"
                    onChange={(e) => {
                      const newIfs = [...(config.interfaces || [])];
                      newIfs[idx] = { ...newIfs[idx], ip: e.target.value };
                      handleInputChange("interfaces", newIfs);
                    }}
                  />
                  <Input
                    value={intf.subnet || ""}
                    placeholder="Subnet"
                    onChange={(e) => {
                      const newIfs = [...(config.interfaces || [])];
                      newIfs[idx] = { ...newIfs[idx], subnet: e.target.value };
                      handleInputChange("interfaces", newIfs);
                    }}
                  />
                </Section>
                <Section>
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
                </Section>
              </div>
            ))}
          </>
        );
      case "Routing":
        return (
          <Section>
            <Select
              value={config?.routingProtocol || "None"}
              onChange={(e) => handleInputChange("routingProtocol", e.target.value)}
            >
              <option value="None">Protocol: Select</option>
              <option value="RIP">RIP</option>
              <option value="OSPF">OSPF</option>
              <option value="EIGRP">EIGRP</option>
              <option value="BGP">BGP</option>
              <option value="BGP">STATIC</option>
            </Select>
            <Input
              value={config?.staticRoutes?.join(", ") || ""}
              placeholder="Static Routes"
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

  return (
    <>
      <div style={{ borderBottom: "2px solid #ededf5", textAlign: "center", fontWeight: "bold", fontSize: ".9rem" }}>
        <h3 style={{ backgroundColor: "#121212", color: "#ffffff", margin: "10px", borderRadius: "6px", padding: "1px" }}>
          {device.name || device.type}
        </h3>
      </div>
      <PanelWrapper>
        {sections.map((section) => (
          <div key={section}>
            <SectionHeader
              expanded={expandedSection === section}
              onClick={() =>
                setExpandedSection(expandedSection === section ? null : section)
              }
            >
              {section}
              <span>{expandedSection === section ? "▲" : "▼"}</span>
            </SectionHeader>
            <AccordionContent expanded={expandedSection === section}>
              {renderRouterSection(section)}
            </AccordionContent>
          </div>
        ))}
        <br />
      </PanelWrapper>
    </>
  );
};

const RightSidebar: React.FC = () => {
  return (
    <Sidebar>
      <SimulationContainer>
        <div>
          <h2>Configuration</h2>
          <p>Setup device & run simulation</p>
        </div>
        <IconButton title="Run Simulation">
          <Play />
        </IconButton>
      </SimulationContainer>
      <DeviceDetailsPanel />
    </Sidebar>
  );
};

export default RightSidebar;





