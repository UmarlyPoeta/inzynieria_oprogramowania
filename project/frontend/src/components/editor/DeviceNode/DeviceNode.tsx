import styled from "styled-components";
import type { Device } from "@/context/EditorContext";

const DeviceNode: React.FC<{ device: Device }> = ({ device }) => {
  return <Node style={{ left: device.x, top: device.y }}>{device.type}</Node>;
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
  cursor: pointer;
  user-select: none;
`;

export default DeviceNode;
