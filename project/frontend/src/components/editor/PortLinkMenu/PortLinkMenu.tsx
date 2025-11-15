import React from "react";
import { useEditor } from "@/context/EditorContext";
import { MenuList, MenuItem } from './PortLinkMenu.styled';

interface PortLinkMenuProps {
  deviceId: string;
  interfaces?: { name: string }[];
}

const PortLinkMenu: React.FC<PortLinkMenuProps> = ({ deviceId, interfaces }) => {
  const { selectDevicePortForLink, links } = useEditor();

  if (!interfaces || interfaces.length === 0) return null;

  // Filtrujemy interfejsy, które są już zajęte w linkach
  const freeInterfaces = interfaces.filter(intf =>
    !links.some(link =>
      (link.from === deviceId && link.fromPort === intf.name) ||
      (link.to === deviceId && link.toPort === intf.name)
    )
  );

  if (freeInterfaces.length === 0) return null;

  return (
    <MenuList>
      {freeInterfaces.map(intf => (
        <MenuItem
          key={intf.name}
          onClick={() => selectDevicePortForLink(deviceId, intf.name)}
        >
          {intf.name}
        </MenuItem>
      ))}
    </MenuList>
  );
};

export default PortLinkMenu;



