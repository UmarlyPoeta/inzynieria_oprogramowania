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
    updateDeviceConfig(device.id!, { [key]: value });
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
        return (
          <>
            <h5>General</h5>
            <Section>

              <Select
                value={config?.natEnabled ? "enabled" : "disabled"}
                onChange={(e) => handleInputChange("natEnabled", e.target.value === "enabled")}
              >
                <option value="enabled">Enabled</option>
                <option value="disabled">Disabled</option>
              </Select>

              <Select
                value={config?.natType || "dynamic"}
                onChange={(e) => handleInputChange("natType", e.target.value)}
              >
                <option value="dynamic">Dynamic NAT</option>
                <option value="static">Static NAT</option>
                <option value="overload">PAT/Overload</option>
              </Select>
            </Section>
          
          <h5 style={{display: "flex", justifyContent: "space-between", alignItems: "center"}}>
            NAT Pool
            <button onClick={() => handleInputChange("natPool", [...(config.natPool || []), { startIp: "", endIp: "" }])}>
              Add NAT Pool
            </button>
          </h5>

            {config?.natType === "dynamic" || config?.natType === "overload" ? (
              <>
                {(config.natPool || []).map((pool, idx) => (
                  <Section key={idx} style={{ marginBottom: "-8px" }}>
                    <Input
                      placeholder="Start IP"
                      value={pool.startIp || ""}
                      onChange={(e) => {
                        const newPool = [...(config.natPool || [])];
                        newPool[idx] = { ...newPool[idx], startIp: e.target.value };
                        handleInputChange("natPool", newPool);
                      }}
                    />
                    <Input
                      placeholder="End IP"
                      value={pool.endIp || ""}
                      onChange={(e) => {
                        const newPool = [...(config.natPool || [])];
                        newPool[idx] = { ...newPool[idx], endIp: e.target.value };
                        handleInputChange("natPool", newPool);
                      }}
                    /> 
                  </Section>
                ))}
              </>
            ) : null}

            {config?.natType === "static" ? (
              <>
              <h5 style={{display: "flex", justifyContent: "space-between", alignItems: "center"}}>
                Static NAT Rules
                <button onClick={() => handleInputChange("natRules", [...(config.natRules || []), { localIp: "", globalIp: "" }])}>
                  Add Static NAT Rule
                </button>
              </h5>
                {(config.natRules || []).map((rule, idx) => (
                  <Section key={idx} style={{ marginBottom: "6px" }}>
                    <Input
                      placeholder="Inside Local IP"
                      value={rule.localIp || ""}
                      onChange={(e) => {
                        const newRules = [...(config.natRules || [])];
                        newRules[idx] = { ...newRules[idx], localIp: e.target.value };
                        handleInputChange("natRules", newRules);
                      }}
                    />
                    <Input
                      placeholder="Outside Global IP"
                      value={rule.globalIp || ""}
                      onChange={(e) => {
                        const newRules = [...(config.natRules || [])];
                        newRules[idx] = { ...newRules[idx], globalIp: e.target.value };
                        handleInputChange("natRules", newRules);
                      }}
                    />
                  </Section>
                ))}
              </>
            ) : null}

          <h5 style={{marginTop: "7px"}}>Interfaces for NAT</h5>
              {(config.interfaces || []).map((intf, idx) => (
                <> {intf.name}
                <Section key={idx} direction="column">
                  <Select
                    value={intf.natInside ? "inside" : intf.natOutside ? "outside" : "none"}
                    onChange={(e) => {
                      const newIfs = [...(config.interfaces || [])];
                      newIfs[idx] = {
                        ...newIfs[idx],
                        natInside: e.target.value === "inside",
                        natOutside: e.target.value === "outside"
                      };
                      handleInputChange("interfaces", newIfs);
                    }}
                  >
                    <option value="none">None</option>
                    <option value="inside">Inside</option>
                    <option value="outside">Outside</option>
                  </Select>
                </Section>
                </>
              ))}
          </>
        );

      case "DHCP":
        return (
          <>
            <Section>
              <Select
                value={config?.dhcpEnabled ? "enabled" : "disabled"}
                onChange={(e) => handleInputChange("dhcpEnabled", e.target.value === "enabled")}
              >
                <option value="enabled">Enabled</option>
                <option value="disabled">Disabled</option>
              </Select>
            </Section>

            <h5 style={{display: "flex", justifyContent: "space-between", alignItems: "center"}}>
              DHCP Pools
              <button
                onClick={() =>
                  handleInputChange("dhcpPools", [
                    ...(config.dhcpPools || []),
                    {
                      name: "",
                      network: "",
                      subnet: "",
                      defaultGateway: "",
                      dns: "",
                      rangeStart: "",
                      rangeEnd: "",
                    },
                  ])
                }
              >
                Add Pool
              </button>
            </h5>

            {(config.dhcpPools || []).map((pool, idx) => (
              <div key={idx}>
              <h5> Pool {idx+1} </h5>
              <Section>
                <Input
                  placeholder="Pool Name"
                  value={pool.name || ""}
                  onChange={(e) => {
                    const newPools = [...(config.dhcpPools || [])];
                    newPools[idx] = { ...newPools[idx], name: e.target.value };
                    handleInputChange("dhcpPools", newPools);
                  }}
                />
              </Section>
              <Section>
                <Input
                  placeholder="Network"
                  value={pool.network || ""}
                  onChange={(e) => {
                    const newPools = [...(config.dhcpPools || [])];
                    newPools[idx] = { ...newPools[idx], network: e.target.value };
                    handleInputChange("dhcpPools", newPools);
                  }}
                />
                </Section>
              <Section>
                <Input
                  placeholder="Subnet"
                  value={pool.subnet || ""}
                  onChange={(e) => {
                    const newPools = [...(config.dhcpPools || [])];
                    newPools[idx] = { ...newPools[idx], subnet: e.target.value };
                    handleInputChange("dhcpPools", newPools);
                  }}
                />
                </Section>
              <Section>
                <Input
                  placeholder="Default Gateway"
                  value={pool.defaultGateway || ""}
                  onChange={(e) => {
                    const newPools = [...(config.dhcpPools || [])];
                    newPools[idx] = { ...newPools[idx], defaultGateway: e.target.value };
                    handleInputChange("dhcpPools", newPools);
                  }}
                />
                </Section>
              <Section>
                <Input
                  placeholder="DNS Server"
                  value={pool.dns || ""}
                  onChange={(e) => {
                    const newPools = [...(config.dhcpPools || [])];
                    newPools[idx] = { ...newPools[idx], dns: e.target.value };
                    handleInputChange("dhcpPools", newPools);
                  }}
                />
              </Section>
              <Section>
                <Input
                  placeholder="Range Start"
                  value={pool.rangeStart || ""}
                  onChange={(e) => {
                    const newPools = [...(config.dhcpPools || [])];
                    newPools[idx] = { ...newPools[idx], rangeStart: e.target.value };
                    handleInputChange("dhcpPools", newPools);
                  }}
                />
              </Section>
              <Section>
                <Input
                  placeholder="Range End"
                  value={pool.rangeEnd || ""}
                  onChange={(e) => {
                    const newPools = [...(config.dhcpPools || [])];
                    newPools[idx] = { ...newPools[idx], rangeEnd: e.target.value };
                    handleInputChange("dhcpPools", newPools);
                  }}
                />
              </Section>
              </div>
            ))}
          </>
        );
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





