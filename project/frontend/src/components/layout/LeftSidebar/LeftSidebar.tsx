import styled from "styled-components";
import { useEditor } from "@/context/EditorContext";
import { 
  Save, 
  Waypoints, 
  ChevronDown, 
  FilePlus, 
  FolderOpen,
  RotateCcw,
  RotateCw,
  Copy,
  Clipboard,
  Trash2,
  Move
} from "lucide-react";

const DEVICE_TYPES = ["router", "switch", "pc"] as const;

const SidebarWrapper = styled.div`
  display: flex;
  height: 100%;
  background-color: ${(props: any) => props.theme.colors.surface};
`;

/* Lewa – wąska sekcja z ikonami typu Save, Exit, Settings */
const LeftPanel = styled.div`
  width: 70px;
  border-right: 1px solid #333;
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 8px 0;
  gap: 4px;
  background-color: ${(props: any) => props.theme.colors.surfaceAlt};
  border-right: 2px solid #ededf5;
`;

const IconButton = styled.button`
  width: 50px;
  height: 50px;
  border: none;
  border-radius: 10px;
  background: none;
  color: ${(props: any) => props.theme.colors.text};
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  transition: background 0.2s;

  &:hover {
    background-color: rgba(255, 174, 0, 0.25);

    svg {
      color: rgba(255, 153, 0, 1);
    }
  }

  svg {
    width: 20px;
    height: 20px;
    color: #121212;
  }
`;

/* Prawa – główna sekcja z urządzeniami i grupami */
const RightPanel = styled.div`
  flex: 1;
  // padding: 12px;
  display: flex;
  flex-direction: column;
  border-right: 2px solid #ededf5;
`;

const SectionTitle = styled.h3`
  color: #121212;
  font-size: 0.8rem;
  font-weight: bold;
  text-transform: uppercase;
  margin-bottom: 8px;
  letter-spacing: 0.5px;
`;

const DeviceButton = styled.button`
  width: 225px;
  margin-bottom: 8px;
  padding: 8px;
  border-radius: 6px;
  background-color: ${(props: any) => props.theme.colors.primary};
  color: ${(props: any) => props.theme.colors.text};
  cursor: pointer;
  transition: background-color 0.2s ease;

  &:hover {
    background-color: ${(props: any) => props.theme.colors.primaryHover};
  }
`;

const AppLogo = styled.div`
  margin-top: 10px;
  font-size: 1.2rem;
  font-weight: bold;
  color: ${(props: any) => props.theme.colors.text};
  margin-bottom: 16px;

  svg {
    width: 20px;
    height: 20px;
    color: #121212;
  }

  border-bottom: 2px solid #e9e9e9ff;
  padding-bottom: 18px;
`;

const RightBarHeader = styled.div`
  margin-top: 10px;
  font-size: .75rem;
  font-weight: bold;
  color: rgba(41, 41, 41, 0.42);
  margin-bottom: 12px;
  border-bottom: 1px solid #e9e9e9ff;
  padding: 12px;
  padding-bottom: 18px;
  width: 100%;

  h1 { 
    font-size: 1rem;
    color: #121212;
  }

  span { 
    color: rgba(255, 153, 0, 1);
  }

  svg { 
    display: inline;
    width: 14px;
    height: 14px;
  }
`;

const LayersPanel = styled.div`
  padding: 12px;
  display: flex;
  flex-direction: column;
`;

const LeftSidebar = () => {
  const { addDevice } = useEditor();

  const handleAddDevice = (type: typeof DEVICE_TYPES[number]) => {
    const id = `${type}-${Date.now()}`;
    addDevice({ id, type, x: 100, y: 100 });
  };

  return (
    <SidebarWrapper>

      <LeftPanel>
        <AppLogo> 
          <Waypoints />
        </AppLogo>
        <IconButton title="New project">
          <FilePlus />
        </IconButton>
        <IconButton title="Open project">
          <FolderOpen />
        </IconButton>
        <IconButton title="Save project">
          <Save />
        </IconButton>
        <IconButton title="Undo action">
          <RotateCcw />
        </IconButton>
        <IconButton title="Redo action">
          <RotateCw />
        </IconButton>
        <IconButton title="Copy">
          <Copy />
        </IconButton>
        <IconButton title="Paste">
          <Clipboard />
        </IconButton>
        <IconButton title="Delete">
          <Trash2 />
        </IconButton>
        <IconButton title="Move">
          <Move />
        </IconButton>
      </LeftPanel>

      <RightPanel>
        <RightBarHeader>
          <h1> NetSimCPP <ChevronDown /> </h1>
          <p> Project X </p>

        </RightBarHeader>
        <LayersPanel>
          <SectionTitle>Layers</SectionTitle>
          {DEVICE_TYPES.map(type => (
            <DeviceButton key={type} onClick={() => handleAddDevice(type)}>
              {type.charAt(0).toUpperCase() + type.slice(1)}
            </DeviceButton>
          ))}
        </LayersPanel>
      </RightPanel>

    </SidebarWrapper>
  );
};

export default LeftSidebar;





