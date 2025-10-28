import { LeftSidebar, RightSidebar, CanvasArea } from '@/components';
import { Container, MainArea } from './Workspace.styled';

const Workspace = () => {
  return (
    <Container>
      <MainArea>
        <LeftSidebar />
        <CanvasArea />
        <RightSidebar />
      </MainArea>
    </Container>
  );
}

export default Workspace;