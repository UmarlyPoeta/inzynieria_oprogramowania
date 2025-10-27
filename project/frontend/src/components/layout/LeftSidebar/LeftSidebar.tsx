import styled from "styled-components";
import React, { useEffect } from "react";
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
import { useContextMenu } from "@/context/ContextualMenuContext";

const SidebarWrapper = styled.div`
  display: flex;
  height: 100%;
  z-index: 2;
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
  width: 202px;
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

const LayersPanelWrapper = styled.div`
  padding: 12px;
  flex: 1;
  overflow-y: auto;
`;

const GroupRow = styled.div<{ collapsed: boolean }>`
  display: flex;
  align-items: center;
  justify-content: space-between;
  font-weight: bold;
  cursor: pointer;
  margin-bottom: 4px;
`;

const DeviceRow = styled.div`
  padding-left: 16px;
  margin-bottom: 2px;
  cursor: pointer;
  &:hover {
    background-color: rgba(255, 174, 0, 0.1);
  }
`;

const LayersPanel: React.FC = () => {
  const { devices, groups, toggleGroupCollapsed, renameGroup, selectDevice } = useEditor();
  const { openMenu } = useContextMenu();
  const [editingGroupId, setEditingGroupId] = React.useState<string | null>(null);
  const [tempName, setTempName] = React.useState<string>("");

  const devicesByGroup = groups.reduce((acc: Record<string, typeof devices>, group) => {
    acc[group.id] = devices.filter(d => d.groupId === group.id);
    return acc;
  }, {} as Record<string, typeof devices>);

  const showAll = () => {
    if (groups.every(g => g.collapsed)) {
      groups.forEach(g => toggleGroupCollapsed(g.id, false)); 
      return;
    }
    groups.forEach(g => toggleGroupCollapsed(g.id, true)); 
  };

  const ungroupedDevices = devices.filter(d => !d.groupId);

  return (
    <LayersPanelWrapper>
      <SectionTitle>Layers</SectionTitle>
      {groups.map(group => (
        <div key={group.id}>
          <GroupRow
            collapsed={group.collapsed}
            onClick={() => toggleGroupCollapsed(group.id, !group.collapsed)}
            onDoubleClick={() => {
              setEditingGroupId(group.id);
              setTempName(group.name);
            }}
            onContextMenu={(e) => {
              e.preventDefault();
              openMenu("group", group, e);
            }}
          >
            {editingGroupId === group.id ? (
              <input
                value={tempName}
                onChange={(e) => setTempName(e.target.value)}
                onBlur={() => {
                  if (tempName.trim()) renameGroup(group.id, tempName.trim());
                  setEditingGroupId(null);
                }}
                onKeyDown={(e) => {
                  if (e.key === "Enter") {
                    if (tempName.trim()) renameGroup(group.id, tempName.trim());
                    setEditingGroupId(null);
                  } else if (e.key === "Escape") {
                    setEditingGroupId(null);
                  }
                }}
                autoFocus
                style={{ fontWeight: "bold", width: "100%" }}
              />
            ) : (
              <span style={{whiteSpace: "nowrap"}} title={group.name}>{group.name.length > 15 ? `${group.name.substring(0, 12)}...` : group.name}</span>
            )}
            <ChevronDown
              style={{ transform: group.collapsed ? "rotate(-90deg)" : "rotate(0deg)" }}
            />
          </GroupRow>
          {!group.collapsed &&
            devicesByGroup[group.id]?.map(device => (
              <DeviceRow 
                onClick={() => selectDevice(device.id)}
                onContextMenu={(e) => {
                  e.preventDefault();
                  openMenu("device", device, e);
                  }}key={device.id}>{device.name || device.type}
                </DeviceRow>
            ))}
        </div>
      ))}
      {ungroupedDevices.length > 0 && (
      <>
        <GroupRow collapsed={false}> Others </GroupRow>
        {ungroupedDevices.map(device => (
          <DeviceRow 
            onClick={() => selectDevice(device.id)}
            onContextMenu={(e) => {
              e.preventDefault();
              openMenu("device", device, e);
              }} key={device.id}>{device.name || device.type }
        </DeviceRow>
        ))}
      </>
      )} 
      <br />
      <button onClick={showAll} style={{border: "1px dashed #b2b2b6ff", padding: "2px"}}> Show / Hide All Layers </button>
    </LayersPanelWrapper>
  );
};

const LeftSidebar = () => {
  const { addDevice, addGroup } = useEditor();

  // For testing: add some default groups and devices (to be removed later)
  useEffect(() => {
    // It affect rerender so be sure to reload website after changes (its info for you, Patryk and Adrian)
    addGroup({ id: "routers", name: "Routers", collapsed: false });
    addGroup({ id: "switches", name: "Switches", collapsed: false });
    addGroup({ id: "pcs", name: "PCs", collapsed: false });


    addDevice({ id: "pc-1", type: "pc", x: 523, y: 431, groupId: "pcs" });
    addDevice({ id: "router-1", type: "router", x: 300, y: 25, groupId: "routers" });
    addDevice({ id: "switch-1", type: "switch", x: 500, y: 50, groupId: "switches" });
    addDevice({ id: "switch-2", type: "switch", x: 700, y: 50, groupId: "switches" });
    addDevice({ id: "switch-3", type: "switch", x: 231, y: 120, groupId: "switches" });
  }, []);

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
          <LayersPanel />
      </RightPanel>

    </SidebarWrapper>
  );
};

export default LeftSidebar;





