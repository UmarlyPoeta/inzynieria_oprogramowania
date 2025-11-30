import { useEditor } from "@/context/EditorContext";
import { SimulationNode } from "@/components";
import { Container, Inner, Grid } from '../../../layout/CanvasArea/CanvasArea.styled';
import { useState, useEffect } from "react";
import styled, { keyframes, css } from "styled-components";

const dash = keyframes`
  to {
    stroke-dashoffset: -24;
  }
`;

const glow = keyframes`
  0%, 100% {
    filter: drop-shadow(0 0 6px rgba(246, 128, 59, 0.8));
  }
  50% {
    filter: drop-shadow(0 0 12px rgba(250, 171, 118, 1));
  }
`;

const pulse = keyframes`
  0%, 100% {
    opacity: 0.6;
  }
  50% {
    opacity: 1;
  }
`;

const animationStyles = css`
  animation: ${dash} 0.6s linear infinite, ${pulse} 1.2s ease-in-out infinite;
`;

const AnimatedLine = styled.line`
  stroke: #f8e15d;
  stroke-width: 2.5;
  stroke-dasharray: 12, 6;
  opacity: 0.8;
  ${animationStyles}
`;

const PacketGroup = styled.g`
  animation: ${glow} 0.8s ease-in-out infinite;
`;

interface Animation {
  id: string;
  srcId: string;
  dstId: string;
  path?: string[];
  timestamp: number;
  currentSegment: number;
}

const Area = () => {
  const { devices, links } = useEditor();
  const [offset, setOffset] = useState({ x: 0, y: 0 });
  const [scale, setScale] = useState(1);
  const [animations, setAnimations] = useState<Animation[]>([]);

  useEffect(() => {
    const handleAnimation = (event: CustomEvent) => {
      const { srcId, dstId, path } = event.detail;
      const animationData: Animation = {
        id: `${srcId}-${dstId}-${Date.now()}`,
        srcId,
        dstId,
        path: path || [srcId, dstId],
        timestamp: Date.now(),
        currentSegment: 0,
      };
      
      setAnimations(prev => [...prev, animationData]);

      // Długość animacji zależy od liczby segmentów
      const duration = (path?.length || 2) * 700;
      setTimeout(() => {
        setAnimations(prev => prev.filter(a => a.id !== animationData.id));
      }, duration);
    };

    window.addEventListener('network-animation' as any, handleAnimation);
    return () => window.removeEventListener('network-animation' as any, handleAnimation);
  }, []);

  useEffect(() => {
    if (animations.length === 0) return;
    
    const interval = setInterval(() => {
      setAnimations(prev => [...prev]);
    }, 16);

    return () => clearInterval(interval);
  }, [animations.length]);

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

  const getPacketPosition = (anim: Animation) => {
    if (!anim.path || anim.path.length < 2) return null;

    const elapsed = Date.now() - anim.timestamp;
    const segmentDuration = 700; // 700ms na każdy segment
    const totalDuration = anim.path.length * segmentDuration;
    const totalProgress = Math.min(elapsed / totalDuration, 1);

    // Oblicz w którym segmencie jesteśmy
    const currentSegmentFloat = totalProgress * (anim.path.length - 1);
    const currentSegment = Math.floor(currentSegmentFloat);
    const segmentProgress = currentSegmentFloat - currentSegment;

    // Smooth ease-in-out dla każdego segmentu
    const easeProgress = segmentProgress < 0.5
      ? 4 * segmentProgress * segmentProgress * segmentProgress
      : 1 - Math.pow(-2 * segmentProgress + 2, 3) / 2;

    // Pobierz urządzenia dla aktualnego segmentu
    const fromId = anim.path[currentSegment];
    const toId = anim.path[Math.min(currentSegment + 1, anim.path.length - 1)];

    const from = devices.find(d => d.id === fromId);
    const to = devices.find(d => d.id === toId);

    if (!from || !to) return null;

    const fromX = from.x + 30;
    const fromY = from.y + 30;
    const toX = to.x + 30;
    const toY = to.y + 30;

    const currentX = fromX + (toX - fromX) * easeProgress;
    const currentY = fromY + (toY - fromY) * easeProgress;

    const angle = Math.atan2(toY - fromY, toX - fromX) * (180 / Math.PI);

    const trail = [];
    for (let i = 1; i <= 4; i++) {
      const trailProgress = Math.max(0, easeProgress - i * 0.08);
      const trailOpacity = Math.max(0, 1 - i * 0.2);
      trail.push({
        x: fromX + (toX - fromX) * trailProgress,
        y: fromY + (toY - fromY) * trailProgress,
        opacity: trailOpacity * (trailProgress > 0 ? 1 : 0),
        size: 7 - i * 1.2
      });
    }

    // Zwróć pełną ścieżkę dla narysowania linii
    const fullPath = anim.path.map(id => {
      const device = devices.find(d => d.id === id);
      return device ? { x: device.x + 30, y: device.y + 30 } : null;
    }).filter(Boolean) as { x: number; y: number }[];

    return { currentX, currentY, angle, trail, fullPath, currentSegment };
  };

  return (
    <Container onWheel={handleWheel} onMouseMove={handleMouseDrag} onContextMenu={(e) => e.preventDefault()}>
      <Inner style={{ transform: `translate(${offset.x}px, ${offset.y}px) scale(${scale})` }}>
   
        <svg style={{ position: "absolute", width: "5000px", height: "5000px", top: 0, left: 0, pointerEvents: "none", zIndex: 1 }}>
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

          {animations.map(anim => {
            const pos = getPacketPosition(anim);
            if (!pos) return null;

            // Stwórz pathData dla pełnej ścieżki
            const pathData = pos.fullPath.length > 0
              ? `M ${pos.fullPath[0].x} ${pos.fullPath[0].y} ` +
                pos.fullPath.slice(1).map(p => `L ${p.x} ${p.y}`).join(' ')
              : '';

            return (
              <g key={anim.id}>
                {pathData && (
                  <path
                    d={pathData}
                    stroke="#f8e15d"
                    strokeWidth="2.5"
                    strokeDasharray="12, 6"
                    fill="none"
                    opacity="0.8"
                  >
                    <animate
                      attributeName="stroke-dashoffset"
                      from="0"
                      to="-24"
                      dur="0.6s"
                      repeatCount="indefinite"
                    />
                    <animate
                      attributeName="opacity"
                      values="0.6;1;0.6"
                      dur="1.2s"
                      repeatCount="indefinite"
                    />
                  </path>
                )}
                
                {pos.trail.map((t, i) => (
                  <g key={i}>
                    <circle
                      cx={t.x}
                      cy={t.y}
                      r={t.size + 2}
                      fill="#f8e15d"
                      opacity={t.opacity * 0.15}
                    />
                    <circle
                      cx={t.x}
                      cy={t.y}
                      r={t.size}
                      fill="#f6803b"
                      opacity={t.opacity * 0.5}
                    />
                  </g>
                ))}

                <PacketGroup transform={`translate(${pos.currentX}, ${pos.currentY}) rotate(${pos.angle})`}>
                  {/* Glow tła */}
                  <circle
                    cx="-2"
                    cy="0"
                    r="14"
                    fill="#f8e15d"
                    opacity="0.2"
                  />
                  
                  {/* Główna strzałka */}
                  <path
                    d="M -11 -7 L 13 0 L -11 7 L -7 0 Z"
                    fill="#f6803b"
                    stroke="#faab76"
                    strokeWidth="1.8"
                    strokeLinejoin="round"
                  />
                  
                  {/* Wewnętrzny akcent */}
                  <path
                    d="M -7 -4 L 8 0 L -7 4 L -4 0 Z"
                    fill="#faab76"
                    opacity="0.8"
                  />
                  
                  {/* Świecący punkt */}
                  <circle
                    cx="0"
                    cy="0"
                    r="2.5"
                    fill="#f8e15d"
                  />
                </PacketGroup>
              </g>
            );
          })}
        </svg>

        <Grid />

        {devices.map(d => (
          <SimulationNode
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

