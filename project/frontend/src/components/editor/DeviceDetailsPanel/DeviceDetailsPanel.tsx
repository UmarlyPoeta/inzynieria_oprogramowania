import React, { useState } from "react";
import { useEditor } from "@/context/EditorContext";
import { PCConfigPanel, SwitchConfigPanel, RouterConfigPanel } from "@/components";

const DeviceDetailsPanel: React.FC = () => {
    const { devices, selectedDeviceId } = useEditor();
    const device = devices.find(d => d.id === selectedDeviceId);
    const [expandedSection, setExpandedSection] = useState<string | null>("General");

    if (!device) return <p style={{ padding: "10px", marginTop: "80px"}}>Select a device to see details</p>;

    return (<>
        <div style={{borderBottom: "2px solid #ededf5", textAlign: "center", fontWeight: "bold", fontSize: ".9rem", marginTop: "80px"}}>
        <h3 style={{ backgroundColor: "#121212",color: "#ffffff", margin: "10px",borderRadius: "6px",padding: "1px" }}>
            {device.name || device.type}
        </h3>
        </div>
        {device.type === "router" && (
            <RouterConfigPanel device={device} expandedSection={expandedSection} setExpandedSection={setExpandedSection} />
        )}
        {device.type === "switch" && (
            <SwitchConfigPanel device={device} expandedSection={expandedSection} setExpandedSection={setExpandedSection} />
        )}
        {device.type === "pc" && (
            <PCConfigPanel device={device} expandedSection={expandedSection} setExpandedSection={setExpandedSection} />
        )}
    </>);
};

export default DeviceDetailsPanel;