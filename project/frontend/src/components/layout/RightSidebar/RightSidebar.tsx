import React from "react";
import { SimulationContainer, Sidebar, IconButton } from './RightSidebar.styled';
import { useMode } from '@/context/WorkspaceModeContext'
import { DeviceDetailsPanel } from "@/components";
import { Play } from "lucide-react";

const RightSidebar: React.FC = () => {
  const { toggleMode } = useMode();

  return (
    <Sidebar>
      <SimulationContainer>
        <div>
          <h2>Configuration</h2>
          <p>Setup device & run simulation</p>
        </div>
        <IconButton title="Run Simulation" onClick={toggleMode}>
          <Play />
        </IconButton>
      </SimulationContainer>
      <DeviceDetailsPanel />
    </Sidebar>
  );
};

export default RightSidebar;






