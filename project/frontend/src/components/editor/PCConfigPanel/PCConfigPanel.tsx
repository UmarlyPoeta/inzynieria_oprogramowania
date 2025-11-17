import React, { useState } from "react";
import type { Device, PCConfig } from "@/types";
import { Section, Input, SectionHeader, AccordionContent, PanelWrapper } from '@/components/layout/RightSidebar/RightSidebar.styled';
import { useEditor } from "@/context/EditorContext";

interface Props {
  device: Device;
  expandedSection: string | null;
  setExpandedSection: (section: string | null) => void;
}

const PCConfigPanel: React.FC<Props> = ({ device, expandedSection, setExpandedSection }) => {
  const { updateDeviceConfig } = useEditor();
  const config = device.config as PCConfig;

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

  const validateIp = (val: string | undefined) => {
    return val && !/^(\d{1,3}\.){3}\d{1,3}$/.test(val);
  };

  const sections = ["General", "Network Interfaces"];

  const renderSection = (section: string) => {
    switch (section) {
      case "General":
        return (
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
              placeholder="Description"
              value={config.description || ""}
              onChange={e => handleInputChange("description", e.target.value)}
            />
          </Section>
        );

      case "Network Interfaces":
        return (
          <>
            {(config.interfaces || []).map((intf, idx) => (
              <div key={idx} style={{ marginBottom: "12px" }}>
                <strong>{intf.name}</strong>
                <Section direction="column" gap="1px">
                  <Input
                    placeholder="IP"
                    value={intf.ip || ""}
                    style={{ borderColor: errorMap[`interfaces[${idx}].ip`] ? "red" : undefined }}
                    onChange={e => {
                      const newIfs = [...(config.interfaces || [])];
                      newIfs[idx] = { ...newIfs[idx], ip: e.target.value };
                      handleInputChange("interfaces", newIfs);
                    }}
                    onBlur={() => {
                      if (validateIp(intf.ip))
                        updateError(`interfaces[${idx}].ip`, true, `Interface ${intf.name || idx+1}: Invalid IP`);
                      else updateError(`interfaces[${idx}].ip`, false, `Interface ${intf.name || idx+1}: Invalid IP`);
                    }}
                  />
                  <Input
                    placeholder="Subnet"
                    value={intf.subnet || ""}
                    style={{ borderColor: errorMap[`interfaces[${idx}].subnet`] ? "red" : undefined }}
                    onChange={e => {
                      const newIfs = [...(config.interfaces || [])];
                      newIfs[idx] = { ...newIfs[idx], subnet: e.target.value };
                      handleInputChange("interfaces", newIfs);
                    }}
                    onBlur={() => {
                      if (validateIp(intf.subnet))
                        updateError(`interfaces[${idx}].subnet`, true, `Interface ${intf.name || idx+1}: Invalid Subnet`);
                      else updateError(`interfaces[${idx}].subnet`, false, `Interface ${intf.name || idx+1}: Invalid Subnet`);
                    }}
                  />
                  <Input
                    placeholder="Gateway"
                    value={intf.gateway || ""}
                    style={{ borderColor: errorMap[`interfaces[${idx}].gateway`] ? "red" : undefined }}
                    onChange={e => {
                      const newIfs = [...(config.interfaces || [])];
                      newIfs[idx] = { ...newIfs[idx], gateway: e.target.value };
                      handleInputChange("interfaces", newIfs);
                    }}
                    onBlur={() => {
                      if (validateIp(intf.gateway))
                        updateError(`interfaces[${idx}].gateway`, true, `Interface ${intf.name || idx+1}: Invalid Gateway`);
                      else updateError(`interfaces[${idx}].gateway`, false, `Interface ${intf.name || idx+1}: Invalid Gateway`);
                    }}
                  />
                  <Input
                    placeholder="DNS"
                    value={intf.dns || ""}
                    style={{ borderColor: errorMap[`interfaces[${idx}].dns`] ? "red" : undefined }}
                    onChange={e => {
                      const newIfs = [...(config.interfaces || [])];
                      newIfs[idx] = { ...newIfs[idx], dns: e.target.value };
                      handleInputChange("interfaces", newIfs);
                    }}
                    onBlur={() => {
                      if (validateIp(intf.dns))
                        updateError(`interfaces[${idx}].dns`, true, `Interface ${intf.name || idx+1}: Invalid DNS`);
                      else updateError(`interfaces[${idx}].dns`, false, `Interface ${intf.name || idx+1}: Invalid DNS`);
                    }}
                  />
                </Section>
              </div>
            ))}
            <button
              style={{ marginTop: "8px" }}
              onClick={() =>
                handleInputChange("interfaces", [
                  ...(config.interfaces || []),
                  { name: `eth${config.interfaces?.length || 0}`, ip: "", subnet: "", gateway: "", dns: "" }
                ])
              }
            >
              Add Interface
            </button>
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
          <SectionHeader
            expanded={expandedSection === section}
            onClick={() => setExpandedSection(expandedSection === section ? null : section)}
          >
            {section} <span>{expandedSection === section ? "▲" : "▼"}</span>
          </SectionHeader>
          <AccordionContent expanded={expandedSection === section}>
            {renderSection(section)}
          </AccordionContent>
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

export default PCConfigPanel;




