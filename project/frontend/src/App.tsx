import { EditorProvider } from '@/context/EditorContext';
import { ThemeProvider } from 'styled-components';
import { GlobalStyle } from '@/styles';
import { Workspace } from '@/pages';
import { Theme } from '@/styles';

const App = () => {
  return (
    <ThemeProvider theme={Theme}>
      <EditorProvider>
        <GlobalStyle />
        <Workspace />
      </EditorProvider>
    </ThemeProvider>
  )
}

export default App
