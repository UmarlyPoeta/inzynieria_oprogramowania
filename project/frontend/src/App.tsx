import { EditorProvider } from '@/context/EditorContext';
import { ThemeProvider } from 'styled-components';
import { GlobalStyle } from '@/styles';
import { Workspace } from '@/pages';
import { Theme } from '@/styles';
import { ContextMenuProvider } from './context/ContextualMenuContext';
import { GlobalContextMenu } from './components';

const App = () => {
  return (
    <ThemeProvider theme={Theme}>
      <EditorProvider>
        <ContextMenuProvider>
          <GlobalStyle />
          <Workspace />
          <GlobalContextMenu />
        </ContextMenuProvider>
      </EditorProvider>
    </ThemeProvider>
  )
}

export default App
