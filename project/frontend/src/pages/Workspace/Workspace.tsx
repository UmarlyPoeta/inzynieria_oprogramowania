import { LeftSidebar, RightSidebar, CanvasArea, DeviceTool } from '@/components';
import { Container, MainArea } from './Workspace.styled';

const Workspace = () => {
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