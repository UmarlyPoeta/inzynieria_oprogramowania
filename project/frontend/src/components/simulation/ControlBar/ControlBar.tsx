import { useState } from "react";
import { Rectangle, ToggleButton, CollapsedBar, IconButton } from './ControlBar.styled';
import { Zap, MapPin } from "lucide-react";
import { useNetworkActions } from "@/hooks";

const ControlBar = () => {
  const [collapsed, setCollapsed] = useState(false);
  const { setPendingAction } = useNetworkActions();

  return (
    <>
      {!collapsed && (
        <Rectangle>
          <ToggleButton onClick={() => setCollapsed(true)}> x </ToggleButton>

          <IconButton 
            title="Ping" 
            onClick={() => {
              console.log("🖱 Ping button clicked");
              setPendingAction('ping'); 
              console.log("⚡ pendingAction set to 'ping'");
            }}
          >
            <Zap />
          </IconButton>

          <IconButton 
            title="Traceroute" 
            onClick={() => setPendingAction('traceroute')}
          >
            <MapPin />
          </IconButton>
        </Rectangle>
      )}

      {collapsed && (
        <CollapsedBar onClick={() => setCollapsed(false)}> 
          Show Control Bar
        </CollapsedBar>
      )}
    </>
  );
};

export default ControlBar;
