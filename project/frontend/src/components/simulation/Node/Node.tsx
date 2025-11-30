import { ComputerIcon, RouterIcon, SwitchIcon } from '@/data';
import { useEditor } from "@/context/EditorContext";
import type { Device } from "@/types";
import { Node } from './Node.styled';
import { useNetworkActions } from '@/hooks';

const SimulationNode: React.FC<{ device: Device; scale: number }> = ({ device }) => {
  const { toggleSelectDevice, selectDevice, selectedDeviceIds } = useEditor();
  const { pendingAction, selectForAction } = useNetworkActions();

  const handleClick = (e: React.MouseEvent<HTMLDivElement>) => {
    e.stopPropagation();
    console.log("🖱 Node clicked:", device.id);

    if (pendingAction) {
      console.log("⚡ Pending action active, selecting for action");
      selectForAction(device.id!);
    } else if (e.ctrlKey || e.metaKey) {
      toggleSelectDevice(device.id!);
    } else {
      selectDevice(device.id!);
    }
  };

  const getDeviceIcon = () => { 
    switch (device.type.toLowerCase()) {
      case 'pc':
        return <img src={ComputerIcon} alt="PC" width={"100%"}/>;
      case 'router':
        return <img src={RouterIcon} alt="Router" width={"100%"}/>;
      case 'switch':
        return <img src={SwitchIcon} alt="Switch" width={"100%"}/>;
    }
  };

  return (
    <Node
      style={{ left: device.x, top: device.y }}
      $selected={selectedDeviceIds.includes(device.id!)}
      onClick={handleClick}
    >
      {getDeviceIcon()}
    </Node>
  );
};

export default SimulationNode;