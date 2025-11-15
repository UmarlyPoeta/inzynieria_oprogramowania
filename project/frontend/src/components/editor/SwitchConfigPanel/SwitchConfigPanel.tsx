import React, { useState } from "react";
import type { Device, SwitchConfig } from "@/types";
import { Section, Input, Select, SectionHeader, AccordionContent, PanelWrapper } from '@/components/layout/RightSidebar/RightSidebar.styled';
import { useEditor } from "@/context/EditorContext";

interface Props {
  device: Device;
  expandedSection: string | null;
  setExpandedSection: (section: string | null) => void;
}

const SwitchConfigPanel: React.FC<Props> = ({ device, expandedSection, setExpandedSection }) => {
  const { updateDeviceConfig } = useEditor();
  const config = device.config as SwitchConfig;

  const [errorMap, setErrorMap] = useState<Record<string, boolean>>({});
  const [errorList, setErrorList] = useState<string[]>([]);

  const updateError = (path: string, hasError: boolean, message?: string) => {
    setErrorMap(prev => {
      const newMap = { ...prev };
      if (hasError) newMap[path] = true;
      else delete newMap[path];
      return newMap;
    });
    setErrorList(prev => {
      const filtered = prev.filter(err => err !== message);
      if (hasError && message) return [...filtered, message];
      return filtered;
    });
  };

  const handleInputChange = (key: string, value: any) => {
    updateDeviceConfig(device.id!, { [key]: value });
  };

  const sections = ["General", "Interfaces", "VLANs"];

  const renderSection = (section: string) => {
    switch (section) {
      case "General":
        return (<>
          <Section>
            <Input
              placeholder="Hostname"
              value={config.hostname || ""}
              onChange={e => handleInputChange("hostname", e.target.value)}
              style={{ borderColor: errorMap["hostname"] ? "red" : undefined }}
              onBlur={() => {
                if (!config.hostname || config.hostname.trim() === "")
                  updateError("hostname", true, "Hostname cannot be empty");
                else updateError("hostname", false, "Hostname cannot be empty");
              }}
            />
            <Input
              placeholder="Management IP"
              value={config.managementIp || ""}
              onChange={e => handleInputChange("managementIp", e.target.value)}
              style={{ borderColor: errorMap["managementIp"] ? "red" : undefined }}
              onBlur={() => {
                const val = config.managementIp;
                if (val && !/^(\d{1,3}\.){3}\d{1,3}$/.test(val))
                  updateError("managementIp", true, "Management IP invalid");
                else updateError("managementIp", false, "Management IP invalid");
              }}
            />
          </Section>
          <Section> 
            <Input
              placeholder="Description"
              value={config.description || ""}
              onChange={e => handleInputChange("description", e.target.value)}
            />
          </Section>
          {/* <label>
              <input
                type="checkbox"
                checked={config.stpEnabled}
                onChange={e => handleInputChange("stpEnabled", e.target.checked)}
              />
               STP Enabled
            </label> */}
        </>);

      case "Interfaces":
        const handleAddInterface = () => {
          const newIfs = [...(config.interfaces || [])];
          const nextIndex = newIfs.length;
          newIfs.push({ name: `GigabitEthernet0/${nextIndex}`, status: "down" });
          handleInputChange("interfaces", newIfs);
        };
        return (
          <>
            <button onClick={handleAddInterface} style={{ marginBottom: "8px" }}>Add Interface</button>
            {(config.interfaces || []).map((intf, idx) => (
              <div key={idx} style={{ marginBottom: "12px" }}>
                <strong>{intf.name}</strong>
                <Section key={idx}>
                  <Select
                    value={intf.status}
                    onChange={e => {
                      const newIfs = [...(config.interfaces || [])];
                      newIfs[idx] = { ...newIfs[idx], status: e.target.value as "up" | "down" };
                      handleInputChange("interfaces", newIfs);
                    }}
                    style={{ borderColor: errorMap[`interfaces[${idx}].status`] ? "red" : undefined }}
                    onBlur={() => {
                      if (intf.status !== "up" && intf.status !== "down")
                        updateError(`interfaces[${idx}].status`, true, `Interface ${intf.name}: Invalid Status`);
                      else updateError(`interfaces[${idx}].status`, false, `Interface ${intf.name}: Invalid Status`);
                    }}
                  >
                    <option value="up">Up</option>
                    <option value="down">Down</option>
                  </Select>
                  <Select
                    value={intf.vlanId || ""}
                    onChange={e => {
                      const newIfs = [...(config.interfaces || [])];
                      newIfs[idx] = { ...newIfs[idx], vlanId: Number(e.target.value) || undefined };
                      handleInputChange("interfaces", newIfs);
                    }}
                  >
                    <option value="">No VLAN</option>
                    {(config.vlans || []).map(v => (
                      <option key={v.id} value={v.id}>{v.name}</option>
                    ))}
                  </Select>
                </Section>
              </div>
            ))}
          </>
        );

      case "VLANs":
        const handleAddVLAN = () => {
          const newVlans = [...(config.vlans || []), { id: (config.vlans?.length || 0) + 1, name: "" }];
          handleInputChange("vlans", newVlans);
        };
        return (
          <>
            {(config.vlans || []).map((vlan, idx) => (
              <Section key={idx}>
                <Input
                  placeholder="VLAN Name"
                  value={vlan.name || ""}
                  onChange={e => {
                    const newVlans = [...(config.vlans || [])];
                    newVlans[idx] = { ...newVlans[idx], name: e.target.value };
                    handleInputChange("vlans", newVlans);
                  }}
                  style={{ borderColor: errorMap[`vlans[${idx}].name`] ? "red" : undefined }}
                  onBlur={() => {
                    if (!vlan.name || vlan.name.trim() === "")
                      updateError(`vlans[${idx}].name`, true, `VLAN ${vlan.id}: Name cannot be empty`);
                    else updateError(`vlans[${idx}].name`, false, `VLAN ${vlan.id}: Name cannot be empty`);
                  }}
                />
                <Input
                  placeholder="VLAN ID"
                  type="number"
                  value={vlan.id}
                  onChange={e => {
                    const newVlans = [...(config.vlans || [])];
                    newVlans[idx] = { ...newVlans[idx], id: Number(e.target.value) };
                    handleInputChange("vlans", newVlans);
                  }}
                  style={{ borderColor: errorMap[`vlans[${idx}].id`] ? "red" : undefined }}
                  onBlur={() => {
                    if (!vlan.id || vlan.id <= 0)
                      updateError(`vlans[${idx}].id`, true, `VLAN ${vlan.name || idx+1}: Invalid ID`);
                    else updateError(`vlans[${idx}].id`, false, `VLAN ${vlan.name || idx+1}: Invalid ID`);
                  }}
                />
              </Section>
            ))}
            <button onClick={handleAddVLAN}>Add VLAN</button>
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
          <AccordionContent expanded={expandedSection === section}>{renderSection(section)}</AccordionContent>
        </div>
      ))}

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

export default SwitchConfigPanel;


