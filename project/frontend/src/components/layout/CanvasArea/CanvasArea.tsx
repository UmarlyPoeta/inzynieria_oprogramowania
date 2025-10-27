import styled from "styled-components";
import { useState } from "react";
import { useEditor } from "@/context/EditorContext";
import { DeviceNode } from "@/components";

const CanvasArea = () => {
  const { devices, links } = useEditor();
  const [offset, setOffset] = useState({ x: 0, y: 0 });
  const [scale, setScale] = useState(1);

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
        
        <svg style={{ position: "absolute", width: "5000", height: "5000", top: 0, left: 0, pointerEvents: "none", zIndex: 10 }}>
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
        {devices.map(d => (
          <DeviceNode key={d.id} device={d} scale={scale}/>
        ))}
      </Inner>
    </Container>
  );

};


const Container = styled.div`
    flex: 1;
    background-color: ${(props: any) => props.theme.colors.background};
    overflow: hidden;
    position: absolute;
    z-index: 1;
    cursor: grab;
    width: 100%;
    height: 100%;
`;

const Grid = styled.div`
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-size: 20px 20px;
  // background-image: 
  //   linear-gradient(to right, #222 1px, transparent 1px),
  //   linear-gradient(to bottom, #222 1px, transparent 1px);
  pointer-events: none;
`;

const Inner = styled.div`
  position: absolute;
  top: 0;
  left: 0;
   //width: 100%;
   //height: 100%;
`;

export default CanvasArea;