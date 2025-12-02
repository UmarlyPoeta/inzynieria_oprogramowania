import { LeftSidebar, RightSidebar, CanvasArea, DeviceTool, Area, StatisticsBar, InfoAndControlBar } from '@/components';
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
            <Area/>
            <InfoAndControlBar/>
            <StatisticsBar/>
          </>
        )}
      </MainArea>
    </Container>
  );
}

export default Workspace;