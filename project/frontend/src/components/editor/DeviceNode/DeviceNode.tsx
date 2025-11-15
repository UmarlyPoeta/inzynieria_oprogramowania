import { ComputerIcon, RouterIcon, SwitchIcon } from '@/data';
import { useEditor } from "@/context/EditorContext";
import type { Device } from "@/types";
import { useState, useEffect } from "react";
import { Node } from './DeviceNode.styled'


const DeviceNode: React.FC<{ device: Device, scale: number }> = ({ device, scale }) => {
  const { toggleSelectDevice, moveDevice, selectDevice, selectedDeviceIds, connectingModeActive, selectDeviceForLink } = useEditor();
  const [dragging, setDragging] = useState(false);
  const [startPos, setStartPos] = useState<{ x: number; y: number }>({ x: 0, y: 0 });

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

  const handleMouseDown = (e: React.MouseEvent) => {
    if (e.button !== 2) return; // tylko lewy przycisk
    e.stopPropagation();
    setDragging(true);
    setStartPos({ x: e.clientX, y: e.clientY });
  };

  const handleMouseUp = () => setDragging(false);

  const handleMouseMove = (e: MouseEvent) => {
    if (!dragging) return;
    const dx = (e.clientX - startPos.x) / scale;
    const dy = (e.clientY - startPos.y) / scale;
    moveDevice(device.id!, device.x + dx, device.y + dy);
    setStartPos({ x: e.clientX, y: e.clientY });
  };

  useEffect(() => {
    window.addEventListener("mousemove", handleMouseMove);
    window.addEventListener("mouseup", handleMouseUp);
    return () => {
      window.removeEventListener("mousemove", handleMouseMove);
      window.removeEventListener("mouseup", handleMouseUp);
    };
  }, [dragging, startPos, device.x, device.y, scale]);

  return (
    <Node style={{ left: device.x, top: device.y }}
      $selected={selectedDeviceIds.includes(device.id!)}
      onMouseDown={handleMouseDown}
      onClick={(e) => {
        e.stopPropagation();
        if (connectingModeActive) {
          selectDeviceForLink(device.id!);
          return;
        }

        if (e.ctrlKey || e.metaKey) {
          toggleSelectDevice(device.id!);
        } else {
          selectDevice(device.id!);
        }
      }}
      >
      {getDeviceIcon()}
    </Node>
  );
};

export default DeviceNode;
