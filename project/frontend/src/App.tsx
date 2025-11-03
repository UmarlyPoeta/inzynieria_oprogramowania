import { ThemeProvider } from "styled-components";
import { BrowserRouter, Routes, Route, Navigate  } from "react-router-dom";

import { GlobalStyle, Theme } from "@/styles";
import { EditorProvider } from "@/context/EditorContext";
import { ContextMenuProvider } from "@/context/ContextualMenuContext";
import { AuthProvider } from "@/context/AuthContext";

import { GlobalContextMenu } from "@/components";
import { Workspace, Authorization } from "@/pages";

const App = () => {
  return (
    <ThemeProvider theme={Theme}>
      <AuthProvider>
        <EditorProvider>
          <ContextMenuProvider>
            <GlobalStyle />
            <BrowserRouter>
              <Routes>
                <Route path="/" element={<Navigate to="/login" replace />} />
                <Route path="/login" element={<Authorization mode="login" />} />
                <Route path="/register" element={<Authorization mode="register" />} />
                <Route
                  path="/workspace"
                  element={
                    <>
                      <Workspace />
                    </>
                  }
                />
              </Routes>
            </BrowserRouter>
            <GlobalContextMenu />
          </ContextMenuProvider>
        </EditorProvider>
      </AuthProvider>
    </ThemeProvider>
  );
};

export default App;

