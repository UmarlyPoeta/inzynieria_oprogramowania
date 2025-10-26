import React from "react";
import styled from "styled-components";
import { useEditor } from "@/context/EditorContext";
import { useContextMenu } from "@/context/ContextualMenuContext";
import { Trash2, Layers, Move } from "lucide-react";
import type { Device } from "@/context/EditorContext";

const MenuList = styled.ul`
  list-style: none;
  margin: 0;
  padding: 4px;
`;

const MenuItem = styled.li`
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px;
  border-radius: 6px;
  cursor: pointer;
  font-size: 0.9rem;
  color: #333;
  transition: background 0.15s;

  &:hover {
    background-color: rgba(255, 174, 0, 0.15);
  }
`;

interface ContextDeviceMenuProps {
  device: Device;
}

const ContextDeviceMenu: React.FC<ContextDeviceMenuProps> = ({ device }) => {
  const { groups, deleteDevice, moveDeviceToGroup, removeDeviceFromGroup } = useEditor();
  const { closeMenu } = useContextMenu();

  const handleDelete = () => {
    deleteDevice(device.id);
    closeMenu();
  };

  const handleRemoveFromGroup = () => {
    removeDeviceFromGroup(device.id);
    closeMenu();
  };

  const handleMoveToGroup = (groupId: string) => {
    moveDeviceToGroup(device.id, groupId);
    closeMenu();
  };

  return (
    <MenuList>
      {device.groupId && (
        <MenuItem onClick={handleRemoveFromGroup}>
          <Layers size={16} /> Remove from group
        </MenuItem>
      )}

      {groups
        .filter(g => g.id !== device.groupId)
        .map(g => (
          <MenuItem key={g.id} onClick={() => handleMoveToGroup(g.id)}>
            <Move size={16} /> Move to {g.name}
          </MenuItem>
        ))}

      <MenuItem onClick={handleDelete}>
        <Trash2 size={16} /> Delete device
      </MenuItem>
    </MenuList>
  );
};

export default ContextDeviceMenu;
