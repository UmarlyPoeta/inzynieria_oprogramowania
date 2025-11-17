import { useEditor } from "@/context/EditorContext";
// import { useSimulation } from "@/context/SimulationContext";
import { DeviceNode } from "@/components";
import { Container, Inner, Grid } from '../../layout/CanvasArea/CanvasArea.styled';
import { useState } from "react";
import { useWebSocket } from "@/hooks";

const Area = () => {
  const { devices, links } = useEditor();
  const [offset, setOffset] = useState({ x: 0, y: 0 });
  const [scale, setScale] = useState(1);

  const { status } = useWebSocket("ws://localhost:9001");

  const handleWheel = (e: React.WheelEvent<HTMLDivElement>) => {
    e.preventDefault();
    const delta = -e.deltaY * 0.001;
    const newScale = Math.min(Math.max(scale + delta, 0.2), 4);

    const rect = e.currentTarget.getBoundingClientRect();
    const mouseX = e.clientX - rect.left;
    const mouseY = e.clientY - rect.top;
    const scaleFactor = newScale / scale;

    setOffset(prev => ({
      x: mouseX - (mouseX - prev.x) * scaleFactor,
      y: mouseY - (mouseY - prev.y) * scaleFactor,
    }));

    setScale(newScale);
  };

  const handleMouseDrag = (e: React.MouseEvent<HTMLDivElement>) => {
    if (e.buttons !== 1) return;
    setOffset(prev => ({
      x: prev.x + e.movementX,
      y: prev.y + e.movementY,
    }));
  };

  return (
    <Container onWheel={handleWheel} onMouseMove={handleMouseDrag} onContextMenu={(e) => e.preventDefault()}>
      <Inner style={{ transform: `translate(${offset.x}px, ${offset.y}px) scale(${scale})` }}>

        {/* Status WebSocket */}
        <div style={{
          padding: "4px 8px",
          fontSize: "30px",
          backgroundColor: status === "open" ? "#4caf50" : "#f44336",
          color: "#121212",
          borderRadius: "4px",
          zIndex: 100
        }}>
          WebSocket Status (temporary message - to be removed): {status}
        </div>

        { /* Render SVG */ }
        <svg style={{ position: "absolute", width: "5000px", height: "5000px", top: 0, left: 0, pointerEvents: "none", zIndex: 10 }}>
          {links.map(link => {
            const from = devices.find(d => d.id === link.from);
            const to = devices.find(d => d.id === link.to);
            if (!from || !to) return null;

            const fromX = from.x + 30;
            const fromY = from.y + 30;
            const toX = to.x + 30;
            const toY = to.y + 30;

            return (
              <line
                key={link.id}
                x1={fromX}
                y1={fromY}
                x2={toX}
                y2={toY}
                stroke="#121212"
                strokeWidth={5}
                fill="none"
              />
            );
          })}
        </svg>

        <Grid />

        {/* Render Devices */}
        {devices.map(d => (
          <DeviceNode
            key={d.id}
            device={d}
            scale={scale}
          />
        ))}

      </Inner>
    </Container>
  );
};

export default Area;

