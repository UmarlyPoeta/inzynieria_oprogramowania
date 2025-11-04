import { useEditor } from "@/context/EditorContext";
import { DeviceToolContainer, DeviceButton } from "./DeviceTool.styled";

const DeviceTool: React.FC = () => {
    const { addDevice } = useEditor();

    return (
        <DeviceToolContainer>
            <DeviceButton onClick={() => addDevice({ type: "router", x: 0, y: 0})}> Router </DeviceButton>
            <DeviceButton onClick={() => addDevice({ type: "switch", x: 0, y: 0})}> Switch </DeviceButton>
            <DeviceButton onClick={() => addDevice({ type: "pc",     x: 0, y: 0})}> PC </DeviceButton>
        </DeviceToolContainer>
    );
}

export default DeviceTool;