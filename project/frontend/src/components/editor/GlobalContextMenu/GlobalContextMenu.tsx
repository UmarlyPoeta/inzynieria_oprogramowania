import React, { useEffect } from "react";
import styled from "styled-components";
import { useContextMenu } from "@/context/ContextualMenuContext";
import { ContextGroupMenu, ContextDeviceMenu } from "@/components";
import type { Group, Device } from "@/context/EditorContext";

const MenuWrapper = styled.div<{ x: number; y: number }>`
  position: fixed;
  top: ${({ y }) => y}px;
  left: ${({ x }) => x}px;
  background: white;
  border: 1px solid #ccc;
  border-radius: 8px;
  box-shadow: 0 2px 10px rgba(0,0,0,0.1);
  z-index: 1000;
  min-width: 180px;
`;

const GlobalContextMenu: React.FC = () => {
  const { menu, closeMenu } = useContextMenu();

  useEffect(() => {
    const handleClickOutside = () => closeMenu();
    window.addEventListener("click", handleClickOutside);
    return () => window.removeEventListener("click", handleClickOutside);
  }, [closeMenu]);

  if (!menu) return null;

  return (
    <MenuWrapper x={menu.x} y={menu.y}>
      {menu.type === "group" && <ContextGroupMenu group={menu.data as Group} />}
      {menu.type === "device" && <ContextDeviceMenu device={menu.data as Device} />}
    </MenuWrapper>
  );
};

export default GlobalContextMenu;
