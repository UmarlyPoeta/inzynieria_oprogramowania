import { Grid, Inner, Container } from './CanvasArea.styled'
import { useEditor } from "@/context/EditorContext";
import { useModal } from "@/context/ModalContext";
import { DeviceNode, LinkModal } from "@/components";
import { useState } from "react";

const CanvasArea = () => {
  const { devices, links, deleteLink } = useEditor();
  const [offset, setOffset] = useState({ x: 0, y: 0 });
  const [scale, setScale] = useState(1);
  const { showModal } = useModal();
  
  const [configModalOpen, setConfigModalOpen] = useState(false);
  const [selectedLink, setSelectedLink] = useState<any>(null);
  
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

  const handleLinkClick = (link: any) => {
    setSelectedLink(link);
    setConfigModalOpen(true);
  };

  const handleSaveConfig = async (data: { delay: number; bandwidth: number; packetLoss: number }) => {
    if (!selectedLink) return;

    try {
      const from = devices.find(d => d.id === selectedLink.from);
      const to = devices.find(d => d.id === selectedLink.to);

      if (!from || !to) return;

      const token = localStorage.getItem("token");

      // Set delay
      await fetch("/api/link/delay", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          "Authorization": `Bearer ${token}`,
        },
        body: JSON.stringify({
          nodeA: from.id,
          nodeB: to.id,
          delay: data.delay,
        }),
      });

      // Set bandwidth
      await fetch("/api/link/bandwidth", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          "Authorization": `Bearer ${token}`,
        },
        body: JSON.stringify({
          nodeA: from.id,
          nodeB: to.id,
          bandwidth: data.bandwidth,
        }),
      });

      // Set packet loss
      await fetch("/api/link/packetloss", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          "Authorization": `Bearer ${token}`,
        },
        body: JSON.stringify({
          nodeA: from.id,
          nodeB: to.id,
          probability: data.packetLoss / 100,
        }),
      });

      console.log("✅ Link configured successfully");
      
      setConfigModalOpen(false);
      setSelectedLink(null);
    } catch (error) {
      console.error("❌ Failed to configure link:", error);
    }
  };

  const handleDeleteLink = () => {
    if (!selectedLink) return;
    
    showModal({
      title: "Delete Connection",
      message: "Are you sure you want to delete this connection?",
      onConfirm: () => {
        deleteLink(selectedLink.id);
        setConfigModalOpen(false);
        setSelectedLink(null);
      }
    });
  };

  return (
    <>
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
                <g key={link.id}>
                  <line
                    x1={fromX}
                    y1={fromY}
                    x2={toX}
                    y2={toY}
                    stroke="#121212"
                    strokeWidth={2}
                    fill="none"
                    style={{ pointerEvents: "none" }}
                  />
                  <line
                    x1={fromX}
                    y1={fromY}
                    x2={toX}
                    y2={toY}
                    stroke="transparent"
                    strokeWidth={20}
                    fill="none"
                    style={{ 
                      pointerEvents: "stroke",
                      cursor: "pointer"
                    }}
                    onClick={(e) => {
                      e.stopPropagation();
                      handleLinkClick(link);
                    }}
                  />
                </g>
              );
            })}
          </svg>
          <Grid />  
          {devices.map(d => (
            <DeviceNode key={d.id} device={d} scale={scale}/>
          ))}
        </Inner>
      </Container>

      <LinkModal
        isOpen={configModalOpen}
        initialValues={selectedLink ? {
          delay: selectedLink.delay,
          bandwidth: selectedLink.bandwidth,
          packetLoss: selectedLink.packetLoss,
        } : undefined}
        onSave={handleSaveConfig}
        onCancel={() => {
          setConfigModalOpen(false);
          setSelectedLink(null);
        }}
        onDelete={handleDeleteLink}
      />
    </>
  );
};

export default CanvasArea;