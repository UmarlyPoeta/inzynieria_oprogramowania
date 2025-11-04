import { useEditor } from "@/context/EditorContext";
import { DeviceToolContainer, DeviceButton } from "./DeviceTool.styled";

const DeviceTool: React.FC = () => {
    const { addDevice, startConnecting } = useEditor();

    return (
        <DeviceToolContainer>
            <DeviceButton onClick={() => addDevice({ type: "router", x: 400, y: 100})}> Router </DeviceButton>
            <DeviceButton onClick={() => addDevice({ type: "switch", x: 500, y: 100})}> Switch </DeviceButton>
            <DeviceButton onClick={() => addDevice({ type: "pc",     x: 600, y: 100})}> PC </DeviceButton>
            <DeviceButton onClick={() => startConnecting()}> Cable </DeviceButton>
        </DeviceToolContainer>
    );
}

export default DeviceTool;