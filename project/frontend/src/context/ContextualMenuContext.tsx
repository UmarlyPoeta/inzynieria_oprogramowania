import React, { createContext, useContext, useState } from "react";

export type MenuType = "device" | "group" | "canvas" | "link";

export interface MenuData<T = any> {
  type: MenuType;
  x: number;
  y: number;
  data: T;
}

interface ContextMenuContextType {
  menu: MenuData | null;
  openMenu: <T = any>(type: MenuType, data: T, e: React.MouseEvent) => void;
  closeMenu: () => void;
}

const ContextMenuContext = createContext<ContextMenuContextType | null>(null);

export const ContextMenuProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [menu, setMenu] = useState<MenuData | null>(null);

  const openMenu = <T,>(type: MenuType, data: T, e: React.MouseEvent) => {
    e.preventDefault();
    setMenu({ type, x: e.clientX, y: e.clientY, data });
  };

  const closeMenu = () => setMenu(null);

  return (
    <ContextMenuContext.Provider value={{ menu, openMenu, closeMenu }}>
      {children}
    </ContextMenuContext.Provider>
  );
};

export const useContextMenu = () => {
  const ctx = useContext(ContextMenuContext);
  if (!ctx) throw new Error("useContextMenu must be used within ContextMenuProvider");
  return ctx;
};
