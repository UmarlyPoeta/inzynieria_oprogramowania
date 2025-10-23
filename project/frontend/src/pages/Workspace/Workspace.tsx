import styled from 'styled-components';
import { 
    LeftSidebar, 
    RightSidebar, 
    CanvasArea,
} from '@/components';

const Container = styled.div`
  display: flex;
  flex-direction: column;
  height: 100%;
  width: 100%;
`;

const MainArea = styled.div`
  display: flex;
  flex: 1;
  overflow: hidden;
  justify-content: space-between;
`;

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