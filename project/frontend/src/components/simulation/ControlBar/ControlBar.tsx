import { useState } from "react";
import { Rectangle, ToggleButton, CollapsedBar, IconButton } from './ControlBar.styled';
import { Zap, MapPin } from "lucide-react";

const ControlBar = () => {
    const [collapsed, setCollapsed] = useState(false);

    return (
        <>
            {!collapsed && (
                <Rectangle>
                    <ToggleButton onClick={() => setCollapsed(true)}> x </ToggleButton>

                    <IconButton title="Ping">
                        <Zap />
                    </IconButton>

                    <IconButton title="Traceroute">
                        <MapPin />
                    </IconButton>
                </Rectangle>

            )}

            {collapsed && (
                <CollapsedBar onClick={() => setCollapsed(false)}> 
                    Show Control Bar
                </CollapsedBar>
            )}
        </>
    )
}

export default ControlBar;
