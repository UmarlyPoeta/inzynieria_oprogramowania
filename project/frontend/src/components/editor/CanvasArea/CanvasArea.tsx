import { Grid, Inner, Container } from './CanvasArea.styled'
import { useEditor } from "@/context/EditorContext";
import { useModal } from "@/context/ModalContext";
import { DeviceNode } from "@/components";
import { useState } from "react";

const CanvasArea = () => {
  const { devices, links, deleteLink } = useEditor();
  const [offset, setOffset] = useState({ x: 0, y: 0 });
  const [scale, setScale] = useState(1);
  const { showModal } = useModal();
  
  const handleWheel = (e: React.WheelEvent<HTMLDivElement>) => {
    e.preventDefault();
    const delta = -e.deltaY * 0.001;          // mniejszy krok
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
        
        <svg style={{ position: "absolute", width: "5000", height: "5000", top: 0, left: 0, pointerEvents: "none", zIndex: -2 }}>
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
                pointerEvents="stroke"
                onDoubleClick={(e) => {
                  e.stopPropagation();
                  showModal({
                    title: "Delete Connection",
                    message: `Are you sure you want to delete this connection?`,
                    onConfirm: () => deleteLink(link.id)
                  });
                  console.log("XD")
                }}
              />
            );
          })}
        </svg>
        <Grid />  
        {devices.map(d => (
          <DeviceNode key={d.id} device={d} scale={scale}/>
        ))}
      </Inner>
    </Container>
  );

};

export default CanvasArea;