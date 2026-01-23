import { useEditor } from "@/context/EditorContext";
import { SimulationNode } from "@/components";
import { Container, Inner, Grid } from '../../../editor/CanvasArea/CanvasArea.styled';
import { useState, useEffect, useRef, useMemo } from "react";
import styled, { keyframes } from "styled-components";

const hopPulse = keyframes`
  0% {
    transform: scale(1);
    opacity: 0.8;
  }
  50% {
    transform: scale(1.5);
    opacity: 1;
  }
  100% {
    transform: scale(1);
    opacity: 0;
  }
`;

const linePulse = keyframes`
  0%, 100% {
    opacity: 0.6;
    stroke-width: 6;
  }
  50% {
    opacity: 1;
    stroke-width: 8;
  }
`;

const HopMarker = styled.circle`
  animation: ${hopPulse} 0.8s ease-out forwards;
`;

const ActiveLine = styled.line`
  animation: ${linePulse} 1s ease-in-out infinite;
`;

interface Animation {
  id: string;
  srcId: string;
  dstId: string;
  path?: string[];
  startTime: number;
  type?: 'ping' | 'traceroute';
}

interface PacketPosition {
  x: number;
  y: number;
  angle: number;
  currentSegment: number;
}

const Area = () => {
  const { devices, links } = useEditor();
  const [offset, setOffset] = useState({ x: 0, y: 0 });
  const [scale, setScale] = useState(1);
  const [animations, setAnimations] = useState<Animation[]>([]);
  const [hopMarkers, setHopMarkers] = useState<Array<{ id: string; x: number; y: number; timestamp: number }>>([]);
  const [tick, setTick] = useState(0);
  const lastSegmentRef = useRef<Map<string, number>>(new Map());
  const rafRef = useRef<number | undefined>(undefined);

  const devicePositions = useMemo(() => {
    const map = new Map<string, { x: number; y: number }>();
    devices.forEach(d => {
      map.set(d.id!, { x: d.x + 30, y: d.y + 30 });
    });
    return map;
  }, [devices]);

  useEffect(() => {
    const handleAnimation = (event: CustomEvent) => {
      const { srcId, dstId, path, type = 'ping' } = event.detail;
      const animationData: Animation = {
        id: `${srcId}-${dstId}-${Date.now()}`,
        srcId,
        dstId,
        path: path || [srcId, dstId],
        startTime: performance.now(),
        type
      };
      
      setAnimations(prev => [...prev, animationData]);

      const segmentDuration = type === 'traceroute' ? 1000 : 600;
      const duration = (path?.length || 2) * segmentDuration;
      
      setTimeout(() => {
        setAnimations(prev => prev.filter(a => a.id !== animationData.id));
        lastSegmentRef.current.delete(animationData.id);
      }, duration);
    };

    window.addEventListener('network-animation' as any, handleAnimation);
    return () => window.removeEventListener('network-animation' as any, handleAnimation);
  }, []);

  useEffect(() => {
    const animate = () => {
      setTick(performance.now());
      rafRef.current = requestAnimationFrame(animate);
    };
    rafRef.current = requestAnimationFrame(animate);
    return () => {
      if (rafRef.current !== undefined) {
        cancelAnimationFrame(rafRef.current);
      }
    };
  }, []);

  useEffect(() => {
    if (animations.length === 0) return;

    animations.forEach(anim => {
      if (anim.type !== 'traceroute' || !anim.path) return;

      const segmentDuration = 1000;
      const elapsed = tick - anim.startTime;
      const totalDuration = anim.path.length * segmentDuration;
      const totalProgress = Math.min(elapsed / totalDuration, 1);

      const currentSegmentFloat = totalProgress * (anim.path.length - 1);
      const currentSegment = Math.floor(currentSegmentFloat);
      const lastSegment = lastSegmentRef.current.get(anim.id) || 0;

      if (currentSegment > lastSegment && currentSegment > 0 && currentSegment < anim.path.length) {
        const hopId = anim.path[currentSegment];
        const pos = devicePositions.get(hopId);
        
        if (pos) {
          const markerId = `${anim.id}-${hopId}-${currentSegment}`;
          setHopMarkers(prev => {
            if (!prev.some(m => m.id === markerId)) {
              return [...prev, { id: markerId, x: pos.x, y: pos.y, timestamp: tick }];
            }
            return prev;
          });
        }

        lastSegmentRef.current.set(anim.id, currentSegment);
      }
    });
  }, [tick, animations, devicePositions]);

  useEffect(() => {
    const cleanup = setInterval(() => {
      setHopMarkers(prev => prev.filter(m => tick - m.timestamp < 800));
    }, 100);
    return () => clearInterval(cleanup);
  }, [tick]);

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

  const getPacketPosition = (anim: Animation): PacketPosition | null => {
    if (!anim.path || anim.path.length < 2) return null;

    const segmentDuration = anim.type === 'traceroute' ? 1000 : 600;
    const elapsed = tick - anim.startTime;
    const totalDuration = anim.path.length * segmentDuration;
    const totalProgress = Math.min(elapsed / totalDuration, 1);

    const currentSegmentFloat = totalProgress * (anim.path.length - 1);
    const currentSegment = Math.floor(currentSegmentFloat);
    const segmentProgress = currentSegmentFloat - currentSegment;

    const t = segmentProgress;
    const easeProgress = t < 0.5 ? 4 * t * t * t : 1 - Math.pow(-2 * t + 2, 3) / 2;

    const fromId = anim.path[currentSegment];
    const toId = anim.path[Math.min(currentSegment + 1, anim.path.length - 1)];

    const from = devicePositions.get(fromId);
    const to = devicePositions.get(toId);

    if (!from || !to) return null;

    const x = from.x + (to.x - from.x) * easeProgress;
    const y = from.y + (to.y - from.y) * easeProgress;
    const angle = Math.atan2(to.y - from.y, to.x - from.x) * (180 / Math.PI);

    return { x, y, angle, currentSegment };
  };

  // Get active segments for all animations
  const activeSegments = useMemo(() => {
    const segments = new Set<string>();
    animations.forEach(anim => {
      const pos = getPacketPosition(anim);
      if (pos && anim.path && pos.currentSegment < anim.path.length - 1) {
        const from = anim.path[pos.currentSegment];
        const to = anim.path[pos.currentSegment + 1];
        segments.add(`${from}-${to}`);
        segments.add(`${to}-${from}`); // bidirectional
      }
    });
    return segments;
  }, [animations, tick]);

  return (
    <Container onWheel={handleWheel} onMouseMove={handleMouseDrag} onContextMenu={(e) => e.preventDefault()}>
      <Inner style={{ transform: `translate(${offset.x}px, ${offset.y}px) scale(${scale})` }}>
   
        <svg style={{ position: "absolute", width: "5000px", height: "5000px", top: 0, left: 0, pointerEvents: "none", zIndex: 1 }}>
          {/* Regular lines */}
          {links.map(link => {
            const from = devicePositions.get(link.from);
            const to = devicePositions.get(link.to);
            if (!from || !to) return null;

            const isActive = activeSegments.has(`${link.from}-${link.to}`);

            return (
              <g key={link.id}>
                {/* Base line */}
                <line
                  x1={from.x}
                  y1={from.y}
                  x2={to.x}
                  y2={to.y}
                  stroke="#121212"
                  strokeWidth={5}
                  fill="none"
                />
                
                {/* Active line glow */}
                {isActive && (
                  <>
                    <ActiveLine
                      x1={from.x}
                      y1={from.y}
                      x2={to.x}
                      y2={to.y}
                      stroke="#f8e15d"
                      strokeWidth={6}
                      fill="none"
                      opacity={0.6}
                    />
                    <line
                      x1={from.x}
                      y1={from.y}
                      x2={to.x}
                      y2={to.y}
                      stroke="#f6803b"
                      strokeWidth={3}
                      fill="none"
                      opacity={0.8}
                    />
                  </>
                )}
              </g>
            );
          })}

          {/* Hop Markers */}
          {hopMarkers.map(marker => (
            <HopMarker
              key={marker.id}
              cx={marker.x}
              cy={marker.y}
              r="20"
              fill="none"
              stroke="#f6803b"
              strokeWidth="3"
            />
          ))}

          {/* Packets */}
          {animations.map(anim => {
            const pos = getPacketPosition(anim);
            if (!pos) return null;

            const isTraceroute = anim.type === 'traceroute';

            return (
              <g key={anim.id}>
                {/* Outer glow ring */}
                <g transform={`translate(${pos.x}, ${pos.y})`}>
                  <circle 
                    cx="0" 
                    cy="0" 
                    r="18" 
                    fill="none"
                    stroke={isTraceroute ? "#4a9eff" : "#f8e15d"}
                    strokeWidth="2"
                    opacity="0.4"
                  />
                  <circle 
                    cx="0" 
                    cy="0" 
                    r="14" 
                    fill={isTraceroute ? "#4a9eff" : "#f8e15d"}
                    opacity="0.2"
                  />
                </g>

                {/* Packet */}
                <g transform={`translate(${pos.x}, ${pos.y}) rotate(${pos.angle})`}>
                  {/* Shadow/depth */}
                  <path
                    d="M -11 -7 L 14 0 L -11 7 L -7 0 Z"
                    fill="#000"
                    opacity="0.2"
                    transform="translate(2, 2)"
                  />
                  
                  {/* Main body */}
                  <path
                    d="M -11 -7 L 14 0 L -11 7 L -7 0 Z"
                    fill={isTraceroute ? "#4a9eff" : "#f6803b"}
                    stroke={isTraceroute ? "#6bb3ff" : "#faab76"}
                    strokeWidth="2"
                    strokeLinejoin="round"
                  />
                  
                  {/* Inner highlight */}
                  <path
                    d="M -7 -4 L 9 0 L -7 4 L -4 0 Z"
                    fill={isTraceroute ? "#6bb3ff" : "#faab76"}
                    opacity="0.9"
                  />
                  
                  {/* Bright center dot */}
                  <circle 
                    cx="1" 
                    cy="0" 
                    r="2.5" 
                    fill={isTraceroute ? "#ffffff" : "#f8e15d"}
                  />
                  
                  {/* Direction indicator lines */}
                  <path
                    d="M 4 -2 L 10 0 M 4 2 L 10 0"
                    stroke={isTraceroute ? "#ffffff" : "#f8e15d"}
                    strokeWidth="1.5"
                    strokeLinecap="round"
                    opacity="0.8"
                  />
                </g>
              </g>
            );
          })}
        </svg>

        <Grid />

        {devices.map(d => (
          <SimulationNode key={d.id} device={d} scale={scale} />
        ))}

      </Inner>
    </Container>
  );
};

export default Area;