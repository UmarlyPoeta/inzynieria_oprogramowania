import React from "react";
import { SimulationContainer, Sidebar, IconButton } from './RightSidebar.styled';
import { useMode } from '@/context/WorkspaceModeContext'
import { DeviceDetailsPanel } from "@/components";
import { useEditor } from '@/context/EditorContext'
import { ApiClient } from "@/api"
import { Play } from "lucide-react";
import { mapDeviceToApiConfig } from "@/utils";

const RightSidebar: React.FC = () => {
  const { toggleMode } = useMode();
  const { devices } = useEditor();

  const runSimulationMode = async () => {
    try {
      for (const device of devices) {
        const payload = mapDeviceToApiConfig(device);

        console.log("[API] Sending:", payload);

        await ApiClient("node/config", {
          method: "POST",
          body: payload,
          withAuth: true,
        });
      }

      console.log("[API] All configs sent successfully.");
      toggleMode(); 

    } catch (error) {
      console.error("[API] ", error);
    }
  };

  return (
    <Sidebar>
      <SimulationContainer>
        <div>
          <h2>Configuration</h2>
          <p>Setup device & run simulation</p>
        </div>
        <IconButton title="Simulation Mode" onClick={runSimulationMode}>
          <Play />
        </IconButton>
      </SimulationContainer>
      <DeviceDetailsPanel />
    </Sidebar>
  );
};

export default RightSidebar;






