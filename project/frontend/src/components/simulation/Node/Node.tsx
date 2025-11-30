import { ComputerIcon, RouterIcon, SwitchIcon } from '@/data';
import { useEditor } from "@/context/EditorContext";
import type { Device } from "@/types";
import { Node } from './Node.styled'


const SimulationNode: React.FC<{ device: Device, scale: number }> = ({ device }) => {
  const { toggleSelectDevice, selectDevice, selectedDeviceIds } = useEditor();

  const getDeviceIcon = () => { 
    switch (device.type.toLowerCase()) {
      case 'pc':
        return <img src={ComputerIcon} alt="PC" width={"100%"}/>;
      case 'router':
        return <img src={RouterIcon} alt="Router" width={"100%"}/>;
      case 'switch':
        return <img src={SwitchIcon} alt="Switch" width={"100%"}/>;
    }
  }

  const handleClick = (e: React.MouseEvent<HTMLDivElement>) => {
    e.stopPropagation();

    if (e.ctrlKey || e.metaKey) {
        toggleSelectDevice(device.id!);
    } else {
        selectDevice(device.id!);
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