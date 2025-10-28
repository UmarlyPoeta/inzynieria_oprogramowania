import React from "react";
import styled from "styled-components";
import { useEditor } from "@/context/EditorContext";
import { useContextMenu } from "@/context/ContextualMenuContext";
import { Edit2, ChevronDown, Trash2 } from "lucide-react";
import type { Group } from "@/types";

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

interface ContextGroupMenuProps {
  group: Group;
}

const ContextGroupMenu: React.FC<ContextGroupMenuProps> = ({ group }) => {
  const { deleteGroup, renameGroup, toggleGroupCollapsed } = useEditor();
  const { closeMenu } = useContextMenu();

  const handleRename = () => {
    const newName = prompt("New name:", group.name);
    if (newName && newName.trim()) renameGroup(group.id, newName.trim());
    closeMenu();
  };

  const handleToggle = () => {
    toggleGroupCollapsed(group.id, !group.collapsed);
    closeMenu();
  };

  const handleDelete = () => {
    deleteGroup(group.id);
    closeMenu();
  }

  return (
    <MenuList>
      <MenuItem onClick={handleToggle}>
        <ChevronDown size={16} />
        {group.collapsed ? "Expand group" : "Collapse group"}
      </MenuItem>
      <MenuItem onClick={handleRename}>
        <Edit2 size={16} /> Rename group
      </MenuItem>
      <MenuItem onClick={handleDelete}>
        <Trash2 size={16} /> Delete group
      </MenuItem>
    </MenuList>
  );
};

export default ContextGroupMenu;

