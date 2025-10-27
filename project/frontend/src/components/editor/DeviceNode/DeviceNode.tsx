import styled from "styled-components";
import { useEditor } from "@/context/EditorContext";
import { useState, useEffect } from "react";
import type { Device } from "@/types";

const DeviceNode: React.FC<{ device: Device, scale: number }> = ({ device, scale }) => {
  const { moveDevice, selectDevice, selectedDeviceId, connectingModeActive, selectDeviceForLink } = useEditor();
  const [dragging, setDragging] = useState(false);
  const [startPos, setStartPos] = useState<{ x: number; y: number }>({ x: 0, y: 0 });

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
    moveDevice(device.id, device.x + dx, device.y + dy);
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

  return <Node style={{ left: device.x, top: device.y, border: selectedDeviceId === device.id ? "3px solid #121212" : "none" }} onMouseDown={handleMouseDown} 
    onClick={() => {
      if (connectingModeActive) {
        selectDeviceForLink(device.id);
      } else {
        selectDevice(device.id);
      }
    }}>{device.type}</Node>;
  };

const Node = styled.div`
  position: absolute;
  width: 60px;
  height: 60px;
  background-color: ${(props: any) => props.theme.colors.primary};
  border-radius: 8px;
  display: flex;
  align-items: center;
  justify-content: center;
  color: ${(props: any) => props.theme.colors.text};
  cursor: grab;
  user-select: none;
`;


export default DeviceNode;
