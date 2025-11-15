import { ContextGroupMenu, ContextDeviceMenu, PortLinkMenu } from "@/components";
import { useContextMenu } from "@/context/ContextualMenuContext";
import { MenuWrapper } from './GlobalContextMenu.styled'
import type { Group, Device } from "@/types";
import React, { useEffect } from "react";
import { useEditor } from "@/context/EditorContext";

const GlobalContextMenu: React.FC = () => {
  const { menu, closeMenu } = useContextMenu();
  const { connectingModeActive } = useEditor();

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
      {menu.type === "link" && (
        <PortLinkMenu 
          deviceId={menu.data.deviceId} 
          interfaces={menu.data.interfaces} 
        />
      )}
    </MenuWrapper>
  );
};

export default GlobalContextMenu;

