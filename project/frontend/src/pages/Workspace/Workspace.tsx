import { LeftSidebar, RightSidebar, CanvasArea, DeviceTool, Area, PduBar, ControlBar, MainBar } from '@/components';
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
            <PduBar/>
            <Area/>
            <ControlBar/>
            <MainBar/>
          </>
        )}
      </MainArea>
    </Container>
  );
}

export default Workspace;