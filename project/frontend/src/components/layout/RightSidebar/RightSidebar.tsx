import React from "react";
import { SimulationContainer, Sidebar, IconButton } from './RightSidebar.styled';
import { DeviceDetailsPanel } from "@/components";
import { Play } from "lucide-react";

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






