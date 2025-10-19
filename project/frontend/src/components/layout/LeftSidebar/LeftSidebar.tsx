import styled from "styled-components";
import { useEditor } from "@/context/EditorContext";

const DEVICE_TYPES = ["router", "switch", "pc"] as const;

const Container = styled.div`
  width: 200px;
  background-color: ${(props: any) => props.theme.colors.surface};
  border-right: 1px solid #222;
  display: flex;
  flex-direction: column;
  padding: 12px;
`;

const DeviceButton = styled.button`
  margin-bottom: 8px;
  padding: 8px;
  border-radius: 6px;
  background-color: ${(props: any) => props.theme.colors.primary};
  color: ${(props: any) => props.theme.colors.text};
  cursor: pointer;
`;

const LeftSidebar = () => {
  const { addDevice } = useEditor();

  const handleAddDevice = (type: typeof DEVICE_TYPES[number]) => {
    const id = `${type}-${Date.now()}`;
    addDevice({ id, type, x: 100, y: 100 });
  };

  return (
    <Container>
      {DEVICE_TYPES.map(type => (
        <DeviceButton key={type} onClick={() => handleAddDevice(type)}>
          {type}
        </DeviceButton>
      ))}
    </Container>
  );
}

export default LeftSidebar;



