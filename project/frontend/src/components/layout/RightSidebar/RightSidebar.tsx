import styled from "styled-components";
import { Play } from 'lucide-react';
import { useEditor } from "@/context/EditorContext";
import type { Device } from "@/types";
import React from "react";

const Sidebar = styled.div`
  width: 260px;
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

// Prosta mapa zakładek dla konfiguracji według typu urządzenia
const deviceTabMap: Record<Device["type"], string[]> = {
  router: ["Interfaces", "Routing Protocol"],
  switch: ["VLANs", "STP"],
  pc: ["IP", "Gateway"],
};

const DeviceDetailsPanel: React.FC = () => {
  const { devices, selectedDeviceId } = useEditor();
  const device = devices.find(d => d.id === selectedDeviceId);

  if (!device) return <p style={{ padding: '10px' }}>Select a device to see details</p>;

  const tabs = deviceTabMap[device.type] || [];

  return (
    <div>
      <div style={{ padding: '10px', borderBottom: '2px solid #ededf5'}}>
        {device.name || device.type} (ID: {device.id})
      </div>
      <div style={{ paddingLeft: '10px', paddingTop: '8px' }}>
        <h4>Configuration</h4>
        {tabs.map(tab => (
          <div key={tab} style={{ marginBottom: '6px', fontSize: '0.9rem' }}>
            <strong>{tab}:</strong> {/* Tutaj później będzie formularz lub inputy */}
          </div>
        ))}
      </div>
    </div>
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


