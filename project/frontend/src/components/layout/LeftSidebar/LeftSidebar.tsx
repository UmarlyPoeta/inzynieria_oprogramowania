import { useContextMenu } from "@/context/ContextualMenuContext";
import { useEditor } from "@/context/EditorContext";
import React from "react";
import { Save, Waypoints, ChevronDown, FilePlus, FolderOpen, RotateCcw, RotateCw, Copy, Clipboard, Trash2, Move } from "lucide-react";
import { 
  SidebarWrapper,
  LeftPanel,
  IconButton,
  RightPanel, 
  SectionTitle,
  AppLogo,
  RightBarHeader,
  LayersPanelWrapper,
  GroupRow,
  DeviceRow
} from './LeftSidebar.styled'

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





