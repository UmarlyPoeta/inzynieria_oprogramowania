import { LeftSidebar, RightSidebar, CanvasArea, DeviceTool, Area } from '@/components';
import { useMode } from '@/context/WorkspaceModeContext'; 
import { Container, MainArea } from './Workspace.styled';
import { useKeyboardShortcuts } from '@/hooks';

const Workspace = () => {
  const { mode } = useMode();
  useKeyboardShortcuts();

  return (
    <Container>
      <MainArea>
        {mode === "editor" ? (
          <>
            <LeftSidebar />
            <CanvasArea />
            <DeviceTool />
            <RightSidebar/>
          </>
        ) : (
          <> 
          <div> Test </div>
          <Area/>
          </>
        )}
      </MainArea>
    </Container>
  );
}

export default Workspace;