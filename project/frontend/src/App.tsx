import { ThemeProvider } from 'styled-components';
import { GlobalStyle } from '@/styles';
import { Theme } from '@/styles';
import { Workspace } from '@/pages';

const App = () => {
  return (
    <ThemeProvider theme={Theme}>
      <GlobalStyle />
      <Workspace />
    </ThemeProvider>
  )
}

export default App
