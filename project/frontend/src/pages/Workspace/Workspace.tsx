import { LeftSidebar, RightSidebar, CanvasArea, DeviceTool } from '@/components';
import { Container, MainArea } from './Workspace.styled';
import { useKeyboardShortcuts } from '@/hooks';

const Workspace = () => {
  useKeyboardShortcuts();

  return (
    <Container onContextMenu={(e) => e.preventDefault()}>
      <MainArea>
        <LeftSidebar />
        <CanvasArea />
        <DeviceTool />
        <RightSidebar />
      </MainArea>
    </Container>
  );
}

export default Workspace;