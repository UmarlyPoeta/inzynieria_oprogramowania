import { LeftSidebar, RightSidebar, CanvasArea, DeviceTool } from '@/components';
import { Container, MainArea } from './Workspace.styled';

const Workspace = () => {
  return (
    <Container>
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