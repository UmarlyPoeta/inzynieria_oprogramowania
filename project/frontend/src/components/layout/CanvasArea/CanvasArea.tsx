import styled from "styled-components";
import { useState } from "react";
import { useEditor } from "@/context/EditorContext";
import { DeviceNode } from "@/components";

const CanvasArea = () => {
  const { devices } = useEditor();
  const [offset, setOffset] = useState({ x: 0, y: 0 });
  const [scale, setScale] = useState(1);

  const handleWheel = (e: React.WheelEvent<HTMLDivElement>) => {
    e.preventDefault();
    const delta = e.deltaY > 0 ? -0.1 : 0.1;
    setScale(prev => Math.min(Math.max(prev + delta, 0.2), 2));
  };

  const handleMouseDrag = (e: React.MouseEvent<HTMLDivElement>) => {
    if (e.buttons !== 1) return;
    setOffset(prev => ({
      x: prev.x + e.movementX,
      y: prev.y + e.movementY,
    }));
  };

  return (
  <Container onWheel={handleWheel} onMouseMove={handleMouseDrag}>
    <Inner style={{ transform: `translate(${offset.x}px, ${offset.y}px) scale(${scale})` }}>
      <Grid />  
      {devices.map(d => (
        <DeviceNode key={d.id} device={d} />
      ))}
    </Inner>
  </Container>
);

};


const Container = styled.div`
    flex: 1;
    background-color: ${(props: any) => props.theme.colors.background};
    overflow: hidden;
    position: relative;
    cursor: grab;
`;

const Grid = styled.div`
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-size: 20px 20px;
  background-image: 
    linear-gradient(to right, #222 1px, transparent 1px),
    linear-gradient(to bottom, #222 1px, transparent 1px);
  pointer-events: none;
`;

const Inner = styled.div`
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
`;

export default CanvasArea;