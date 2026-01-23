import React, { useState, useEffect } from "react";
import type { Device, RouterConfig } from "@/types";
import { Section, Input, Select, SectionHeader, AccordionContent, PanelWrapper } from '@/components/editor/RightSidebar/RightSidebar.styled';
import { useEditor } from "@/context/EditorContext";

interface Props {
  device: Device;
  expandedSection: string | null;
  setExpandedSection: (section: string | null) => void;
}

interface ValidationError {
  path: string;
  message: string;
}

const RouterConfigPanel: React.FC<Props> = ({ device, expandedSection, setExpandedSection }) => {
  const { updateDeviceConfig } = useEditor();
  const config = device.config as RouterConfig;

  const [errorMap, setErrorMap] = useState<Record<string, boolean>>({});
  const [errorList, setErrorList] = useState<string[]>([]);

  // Walidacja
const validate = (cfg: RouterConfig) => {
  const errors: ValidationError[] = [];

  // --- General ---
  if (!cfg.hostname || cfg.hostname.trim() === "") 
    errors.push({ path: "hostname", message: "Hostname cannot be empty" });

  if (cfg.domainName && cfg.domainName.trim() !== "" && !/^[a-zA-Z0-9.-]+$/.test(cfg.domainName)) 
    errors.push({ path: "domainName", message: "Domain Name invalid" });

  // --- Interfaces ---
  cfg.interfaces?.forEach((intf, idx) => {
    const prefix = `interfaces[${idx}]`;

    if (intf.ip && intf.ip.trim() !== "" && !/^(\d{1,3}\.){3}\d{1,3}$/.test(intf.ip))
      errors.push({ path: `${prefix}.ip`, message: `Interface ${intf.name}: Invalid IP` });

    if (intf.subnet && intf.subnet.trim() !== "" && !/^(\d{1,3}\.){3}\d{1,3}$/.test(intf.subnet))
      errors.push({ path: `${prefix}.subnet`, message: `Interface ${intf.name}: Invalid Subnet` });

    // if (intf.gateway && intf.gateway.trim() !== "" && !/^(\d{1,3}\.){3}\d{1,3}$/.test(intf.gateway))
      // errors.push({ path: `${prefix}.gateway`, message: `Interface ${intf.name}: Invalid Gateway` });

    if (intf.status !== "up" && intf.status !== "down") 
      errors.push({ path: `${prefix}.status`, message: `Interface ${intf.name}: Invalid Status` });

    if (intf.natInside && intf.natOutside) 
      errors.push({ path: `${prefix}.nat`, message: `Interface ${intf.name}: Cannot be both inside and outside` });
  });

  // --- Routing ---
  const allowedProtocols = ["RIP", "OSPF", "EIGRP", "BGP", "STATIC"];
  if (cfg.routingProtocol && !allowedProtocols.includes(cfg.routingProtocol)) 
    errors.push({ path: "routingProtocol", message: "Invalid routing protocol" });

  cfg.staticRoutes?.forEach((route, idx) => {
    if (route.destination && !/^(\d{1,3}\.){3}\d{1,3}$/.test(route.destination))
      errors.push({ path: `staticRoutes[${idx}].destination`, message: `Route ${idx+1}: Invalid Destination IP` });

    if (route.subnet && !/^(\d{1,3}\.){3}\d{1,3}$/.test(route.subnet))
      errors.push({ path: `staticRoutes[${idx}].subnet`, message: `Route ${idx+1}: Invalid Subnet` });

    if (route.gateway && !/^(\d{1,3}\.){3}\d{1,3}$/.test(route.gateway))
      errors.push({ path: `staticRoutes[${idx}].gateway`, message: `Route ${idx+1}: Invalid Gateway` });
  });

  // --- NAT ---
  cfg.natPool?.forEach((pool, idx) => {
    if (pool.startIp && !/^(\d{1,3}\.){3}\d{1,3}$/.test(pool.startIp))
      errors.push({ path: `natPool[${idx}].startIp`, message: `NAT Pool ${idx+1}: Invalid Start IP` });
    if (pool.endIp && !/^(\d{1,3}\.){3}\d{1,3}$/.test(pool.endIp))
      errors.push({ path: `natPool[${idx}].endIp`, message: `NAT Pool ${idx+1}: Invalid End IP` });
  });

  cfg.natRules?.forEach((rule, idx) => {
    if (rule.localIp && !/^(\d{1,3}\.){3}\d{1,3}$/.test(rule.localIp))
      errors.push({ path: `natRules[${idx}].localIp`, message: `Static NAT ${idx+1}: Invalid Local IP` });
    if (rule.globalIp && !/^(\d{1,3}\.){3}\d{1,3}$/.test(rule.globalIp))
      errors.push({ path: `natRules[${idx}].globalIp`, message: `Static NAT ${idx+1}: Invalid Global IP` });
  });

  cfg.interfaces?.forEach((intf, idx) => {
    if ((intf.natInside || intf.natOutside) && !cfg.natPool?.length && !cfg.natRules?.length) {
      errors.push({ path: `interfaces[${idx}].nat`, message: `Interface ${intf.name}: Assigned to NAT but no NAT config exists` });
    }
  });

  // --- DHCP ---
  cfg.dhcpPools?.forEach((pool, idx) => {
    const prefix = `dhcpPools[${idx}]`;
    ["network", "subnet", "defaultGateway", "dns", "rangeStart", "rangeEnd"].forEach(field => {
      const val = (pool as any)[field];
      if (val && !/^(\d{1,3}\.){3}\d{1,3}$/.test(val)) 
        errors.push({ path: `${prefix}.${field}`, message: `DHCP Pool ${idx+1}: Invalid ${field}` });
    });
  });

  // --- Security / Services ---
  // Placeholder for future validation

  // Aktualizacja errorMap i errorList
  const newErrorMap: Record<string, boolean> = {};
  errors.forEach(e => newErrorMap[e.path] = true);
  setErrorMap(newErrorMap);
  setErrorList(errors.map(e => e.message));
};


  useEffect(() => {
    validate(config);
  }, [config]);

  const handleInputChange = (key: string, value: any) => {
    updateDeviceConfig(device.id!, { [key]: value });
  };

  const sections = ["General", "Interfaces", "Routing", "NAT", "DHCP", "Security"];

  const renderSection = (section: string) => {
    switch (section) {
      case "General":
        return (
          <Section>
            <Input
              value={config.hostname || ""}
              onChange={(e) => handleInputChange("hostname", e.target.value)}
              placeholder="Hostname"
              style={{ borderColor: errorMap["hostname"] ? "red" : undefined }}
            />
            <Input
              value={config.domainName || ""}
              onChange={(e) => handleInputChange("domainName", e.target.value)}
              placeholder="Domain Name"
              style={{ borderColor: errorMap["domainName"] ? "red" : undefined }}
            />
          </Section>
        );

      case "Interfaces":
        const handleAddInterface = () => {
          const newIfs = [...(config.interfaces || [])];
          const nextIndex = newIfs.length;
          newIfs.push({ name: `GigabitEthernet0/${nextIndex}`, ip: "", subnet: "", status: "up" });
          handleInputChange("interfaces", newIfs);
        };
        return (
          <>
            <button onClick={handleAddInterface} style={{ marginBottom: "8px" }}>Add Interface</button>
            {config.interfaces?.map((intf, idx) => (
              <div key={idx} style={{ marginBottom: "12px" }}>
                <strong>{intf.name}</strong>
                <Section>
                  <Input
                    value={intf.ip || ""}
                    placeholder="Adres IP"
                    style={{ borderColor: errorMap[`interfaces[${idx}].ip`] ? "red" : undefined }}
                    onChange={(e) => {
                      const newIfs = [...(config.interfaces || [])];
                      newIfs[idx] = { ...newIfs[idx], ip: e.target.value };
                      handleInputChange("interfaces", newIfs);
                    }}
                  />
                  <Input
                    value={intf.subnet || ""}
                    placeholder="Subnet"
                    style={{ borderColor: errorMap[`interfaces[${idx}].subnet`] ? "red" : undefined }}
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
                    style={{ borderColor: errorMap[`interfaces[${idx}].status`] ? "red" : undefined }}
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
          <>
            <Section>
              <Select
                value={config.routingProtocol || "STATIC"}
                onChange={(e) => handleInputChange("routingProtocol", e.target.value)}
                style={{ borderColor: errorMap["routingProtocol"] ? "red" : undefined }}
              >
                <option value="STATIC">STATIC</option>
                <option value="RIP">RIP</option>
                <option value="OSPF">OSPF</option>
                <option value="EIGRP">EIGRP</option>
                <option value="BGP">BGP</option>

              </Select>
            </Section>
            <h5 style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
              Static Routes
              <button
                onClick={() => handleInputChange("staticRoutes", [...(config.staticRoutes || []), { destination: "", subnet: "", gateway: "" }])}
              >
                Add Route
              </button>
            </h5>
            {(config.staticRoutes || []).map((route, idx) => (
              <Section key={idx}>
                <Input
                  placeholder="Destination"
                  value={route.destination || ""}
                  style={{ borderColor: errorMap[`staticRoutes[${idx}].destination`] ? "red" : undefined }}
                  onChange={(e) => {
                    const newRoutes = [...(config.staticRoutes || [])];
                    newRoutes[idx] = { ...newRoutes[idx], destination: e.target.value };
                    handleInputChange("staticRoutes", newRoutes);
                  }}
                />
                <Input
                  placeholder="Subnet"
                  value={route.subnet || ""}
                  style={{ borderColor: errorMap[`staticRoutes[${idx}].subnet`] ? "red" : undefined }}
                  onChange={(e) => {
                    const newRoutes = [...(config.staticRoutes || [])];
                    newRoutes[idx] = { ...newRoutes[idx], subnet: e.target.value };
                    handleInputChange("staticRoutes", newRoutes);
                  }}
                />
                <Input
                  placeholder="Gateway"
                  value={route.gateway || ""}
                  style={{ borderColor: errorMap[`staticRoutes[${idx}].gateway`] ? "red" : undefined }}
                  onChange={(e) => {
                    const newRoutes = [...(config.staticRoutes || [])];
                    newRoutes[idx] = { ...newRoutes[idx], gateway: e.target.value };
                    handleInputChange("staticRoutes", newRoutes);
                  }}
                />
              </Section>
            ))}
          </>
        );

      case "NAT":
        return (
          <>
            <h5>General</h5>
            <Section>
              <Select
                value={config?.natEnabled ? "enabled" : "disabled"}
                onChange={(e) => handleInputChange("natEnabled", e.target.value === "enabled")}
                style={{ borderColor: errorMap["natEnabled"] ? "red" : undefined }}
              >
                <option value="enabled">Enabled</option>
                <option value="disabled">Disabled</option>
              </Select>
              <Select
                value={config?.natType || "dynamic"}
                onChange={(e) => handleInputChange("natType", e.target.value)}
                style={{ borderColor: errorMap["natType"] ? "red" : undefined }}
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
            {(config.natType === "dynamic" || config.natType === "overload") && (config.natPool || []).map((pool, idx) => (
              <Section key={idx} style={{ marginBottom: "-8px" }}>
                <Input placeholder="Start IP" value={pool.startIp || ""} onChange={e => {
                  const newPool = [...(config.natPool || [])];
                  newPool[idx] = { ...newPool[idx], startIp: e.target.value };
                  handleInputChange("natPool", newPool);
                }} style={{ borderColor: errorMap[`natPool[${idx}].startIp`] ? "red" : undefined }} />
                <Input placeholder="End IP" value={pool.endIp || ""} onChange={e => {
                  const newPool = [...(config.natPool || [])];
                  newPool[idx] = { ...newPool[idx], endIp: e.target.value };
                  handleInputChange("natPool", newPool);
                }} style={{ borderColor: errorMap[`natPool[${idx}].endIp`] ? "red" : undefined }} />
              </Section>
            ))}

            {config.natType === "static" && (
              <>
                <h5 style={{display: "flex", justifyContent: "space-between", alignItems: "center"}}>
                  Static NAT Rules
                  <button onClick={() => handleInputChange("natRules", [...(config.natRules || []), { localIp: "", globalIp: "" }])}>
                    Add Static NAT Rule
                  </button>
                </h5>
                {(config.natRules || []).map((rule, idx) => (
                  <Section key={idx} style={{ marginBottom: "6px" }}>
                    <Input placeholder="Inside Local IP" value={rule.localIp || ""} onChange={e => {
                      const newRules = [...(config.natRules || [])];
                      newRules[idx] = { ...newRules[idx], localIp: e.target.value };
                      handleInputChange("natRules", newRules);
                    }} style={{ borderColor: errorMap[`natRules[${idx}].localIp`] ? "red" : undefined }} />
                    <Input placeholder="Outside Global IP" value={rule.globalIp || ""} onChange={e => {
                      const newRules = [...(config.natRules || [])];
                      newRules[idx] = { ...newRules[idx], globalIp: e.target.value };
                      handleInputChange("natRules", newRules);
                    }} style={{ borderColor: errorMap[`natRules[${idx}].globalIp`] ? "red" : undefined }} />
                  </Section>
                ))}
              </>
            )}

            <h5 style={{marginTop: "7px"}}>Interfaces for NAT</h5>
            {(config.interfaces || []).map((intf, idx) => (
              <Section key={idx} direction="column">
                {intf.name}
                <Select value={intf.natInside ? "inside" : intf.natOutside ? "outside" : "none"} onChange={e => {
                  const newIfs = [...(config.interfaces || [])];
                  newIfs[idx] = {
                    ...newIfs[idx],
                    natInside: e.target.value === "inside",
                    natOutside: e.target.value === "outside"
                  };
                  handleInputChange("interfaces", newIfs);
                }} style={{ borderColor: errorMap[`interfaces[${idx}].nat`] ? "red" : undefined }}>
                  <option value="none">None</option>
                  <option value="inside">Inside</option>
                  <option value="outside">Outside</option>
                </Select>
              </Section>
            ))}
          </>
        );

      case "DHCP":
        return (
          <>
            <Section>
              <Select value={config.dhcpEnabled ? "enabled" : "disabled"} onChange={e => handleInputChange("dhcpEnabled", e.target.value === "enabled")} style={{ borderColor: errorMap["dhcpEnabled"] ? "red" : undefined }}>
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
                                {["name","network","subnet","defaultGateway","dns","rangeStart","rangeEnd"].map(field => (
                  <Section key={field}>
                    <Input
                      placeholder={field.charAt(0).toUpperCase() + field.slice(1)}
                      value={(pool as any)[field] || ""}
                      style={{ borderColor: errorMap[`dhcpPools[${idx}].${field}`] ? "red" : undefined }}
                      onChange={e => {
                        const newPools = [...(config.dhcpPools || [])];
                        newPools[idx] = { ...newPools[idx], [field]: e.target.value };
                        handleInputChange("dhcpPools", newPools);
                      }}
                    />
                  </Section>
                ))}
              </div>
            ))}
          </>
        );

      case "Security":
      return (
        <>
          <Section>
            <Select
              value={config.firewallEnabled ? "enabled" : "disabled"}
              onChange={(e) => handleInputChange("firewallEnabled", e.target.value === "enabled")}
            >
              <option value="enabled">Enabled</option>
              <option value="disabled">Disabled</option>
            </Select>
          </Section>

          <h5 style={{display: "flex", justifyContent: "space-between", alignItems: "center"}}>
            Firewall Rules (ACL)
            <button
              onClick={() =>
                handleInputChange("firewallRules", [
                  ...(config.firewallRules || []),
                  {
                    action: "permit",
                    protocol: "any",
                    source: "",
                    destination: "",
                    port: undefined,
                  },
                ])
              }
            >
              Add Rule
            </button>
          </h5>

          {(config.firewallRules || []).map((rule, idx) => (
            <div key={idx} style={{ marginBottom: "12px", padding: "8px", border: "1px solid #ddd", borderRadius: "4px" }}>
              <h5>Rule {idx + 1}</h5>
              <Section>
                <Select
                  value={rule.action}
                  onChange={(e) => {
                    const newRules = [...(config.firewallRules || [])];
                    newRules[idx] = { ...newRules[idx], action: e.target.value as "permit" | "deny" };
                    handleInputChange("firewallRules", newRules);
                  }}
                >
                  <option value="permit">Permit</option>
                  <option value="deny">Deny</option>
                </Select>
                <Select
                  value={rule.protocol}
                  onChange={(e) => {
                    const newRules = [...(config.firewallRules || [])];
                    newRules[idx] = { ...newRules[idx], protocol: e.target.value as any };
                    handleInputChange("firewallRules", newRules);
                  }}
                >
                  <option value="any">Any</option>
                  <option value="tcp">TCP</option>
                  <option value="udp">UDP</option>
                  <option value="icmp">ICMP</option>
                </Select>
              </Section>
              <Section>
                <Input
                  placeholder="Source IP/Network"
                  value={rule.source}
                  onChange={(e) => {
                    const newRules = [...(config.firewallRules || [])];
                    newRules[idx] = { ...newRules[idx], source: e.target.value };
                    handleInputChange("firewallRules", newRules);
                  }}
                />
                <Input
                  placeholder="Destination IP/Network"
                  value={rule.destination}
                  onChange={(e) => {
                    const newRules = [...(config.firewallRules || [])];
                    newRules[idx] = { ...newRules[idx], destination: e.target.value };
                    handleInputChange("firewallRules", newRules);
                  }}
                />
              </Section>
              <Section>
                <Input
                  type="number"
                  placeholder="Port (optional)"
                  value={rule.port || ""}
                  onChange={(e) => {
                    const newRules = [...(config.firewallRules || [])];
                    newRules[idx] = { ...newRules[idx], port: e.target.value ? Number(e.target.value) : undefined };
                    handleInputChange("firewallRules", newRules);
                  }}
                />
              </Section>
            </div>
          ))}

          <h5 style={{display: "flex", justifyContent: "space-between", alignItems: "center", marginTop: "16px"}}>
            VPN Tunnels
            <button
              onClick={() =>
                handleInputChange("vpnTunnels", [
                  ...(config.vpnTunnels || []),
                  {
                    name: "",
                    type: "IPSec",
                    remoteIp: "",
                    preSharedKey: "",
                  },
                ])
              }
            >
              Add Tunnel
            </button>
          </h5>

          {(config.vpnTunnels || []).map((tunnel, idx) => (
            <div key={idx} style={{ marginBottom: "12px", padding: "8px", border: "1px solid #ddd", borderRadius: "4px" }}>
              <h5>Tunnel {idx + 1}</h5>
              <Section>
                <Input
                  placeholder="Tunnel Name"
                  value={tunnel.name}
                  onChange={(e) => {
                    const newTunnels = [...(config.vpnTunnels || [])];
                    newTunnels[idx] = { ...newTunnels[idx], name: e.target.value };
                    handleInputChange("vpnTunnels", newTunnels);
                  }}
                />
                <Select
                  value={tunnel.type}
                  onChange={(e) => {
                    const newTunnels = [...(config.vpnTunnels || [])];
                    newTunnels[idx] = { ...newTunnels[idx], type: e.target.value as "IPSec" | "GRE" };
                    handleInputChange("vpnTunnels", newTunnels);
                  }}
                >
                  <option value="IPSec">IPSec</option>
                  <option value="GRE">GRE</option>
                </Select>
              </Section>
              <Section>
                <Input
                  placeholder="Remote IP"
                  value={tunnel.remoteIp}
                  onChange={(e) => {
                    const newTunnels = [...(config.vpnTunnels || [])];
                    newTunnels[idx] = { ...newTunnels[idx], remoteIp: e.target.value };
                    handleInputChange("vpnTunnels", newTunnels);
                  }}
                />
                {tunnel.type === "IPSec" && (
                  <Input
                    placeholder="Pre-Shared Key"
                    type="password"
                    value={tunnel.preSharedKey || ""}
                    onChange={(e) => {
                      const newTunnels = [...(config.vpnTunnels || [])];
                      newTunnels[idx] = { ...newTunnels[idx], preSharedKey: e.target.value };
                      handleInputChange("vpnTunnels", newTunnels);
                    }}
                  />
                )}
              </Section>
            </div>
          ))}

          <h5 style={{display: "flex", justifyContent: "space-between", alignItems: "center", marginTop: "16px"}}>
            QoS Policies
            <button
              onClick={() =>
                handleInputChange("qosPolicies", [
                  ...(config.qosPolicies || []),
                  {
                    name: "",
                    description: "",
                    type: "priority",
                    value: "",
                  },
                ])
              }
            >
              Add Policy
            </button>
          </h5>

          {(config.qosPolicies || []).map((policy, idx) => (
            <div key={idx} style={{ marginBottom: "12px", padding: "8px", border: "1px solid #ddd", borderRadius: "4px" }}>
              <h5>Policy {idx + 1}</h5>
              <Section>
                <Input
                  placeholder="Policy Name"
                  value={policy.name}
                  onChange={(e) => {
                    const newPolicies = [...(config.qosPolicies || [])];
                    newPolicies[idx] = { ...newPolicies[idx], name: e.target.value };
                    handleInputChange("qosPolicies", newPolicies);
                  }}
                />
                <Select
                  value={policy.type}
                  onChange={(e) => {
                    const newPolicies = [...(config.qosPolicies || [])];
                    newPolicies[idx] = { ...newPolicies[idx], type: e.target.value as any };
                    handleInputChange("qosPolicies", newPolicies);
                  }}
                >
                  <option value="priority">Priority</option>
                  <option value="bandwidth">Bandwidth</option>
                  <option value="rate-limit">Rate Limit</option>
                </Select>
              </Section>
              <Section>
                <Input
                  placeholder="Description"
                  value={policy.description || ""}
                  onChange={(e) => {
                    const newPolicies = [...(config.qosPolicies || [])];
                    newPolicies[idx] = { ...newPolicies[idx], description: e.target.value };
                    handleInputChange("qosPolicies", newPolicies);
                  }}
                />
                <Input
                  placeholder="Value"
                  value={policy.value}
                  onChange={(e) => {
                    const newPolicies = [...(config.qosPolicies || [])];
                    newPolicies[idx] = { ...newPolicies[idx], value: e.target.value };
                    handleInputChange("qosPolicies", newPolicies);
                  }}
                />
              </Section>
            </div>
          ))}
        </>
      );

    default:
      return null;
    }
  };

  return (
    <PanelWrapper>
      {sections.map(section => (
        <div key={section}>
          <SectionHeader expanded={expandedSection === section} onClick={() => setExpandedSection(expandedSection === section ? null : section)}>
            {section} <span>{expandedSection === section ? "▲" : "▼"}</span>
          </SectionHeader>
          <AccordionContent expanded={expandedSection === section}>
            {renderSection(section)}
          </AccordionContent>
        </div>
      ))}

      {/* Lista błędów */}
      {errorList.length > 0 && (
        <Section style={{ borderTop: "1px solid #ccc", marginTop: "12px", paddingTop: "12px", color: "red", flexDirection: "column" }}>
          <strong>There are a few errors:</strong>
          <ul>
            {errorList.map((err, idx) => <li key={idx}>- {err}</li>)}
          </ul>
        </Section>
      )}
    </PanelWrapper>
  );
};

export default RouterConfigPanel;




