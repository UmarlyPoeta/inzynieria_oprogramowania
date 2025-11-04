import { ThemeProvider } from "styled-components";
import { BrowserRouter, Routes, Route, Navigate  } from "react-router-dom";

import { GlobalStyle, Theme } from "@/styles";
import { EditorProvider } from "@/context/EditorContext";
import { ContextMenuProvider } from "@/context/ContextualMenuContext";
import { AuthProvider } from "@/context/AuthContext";

import { GlobalContextMenu, ProtectedRoute } from "@/components";
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
                <Route path="/" element={
                  <ProtectedRoute redirectIfAuthenticated>
                    <Navigate to="/login" replace />
                  </ProtectedRoute>
                }/>
                <Route path="/login" element={
                  <ProtectedRoute redirectIfAuthenticated>
                    <Authorization mode="login" />
                    </ProtectedRoute>
                }/>
                <Route path="/register" element={
                  <ProtectedRoute redirectIfAuthenticated>
                    <Authorization mode="register" />
                    </ProtectedRoute>
                }/>
                <Route
                  path="/workspace"element={
                    <ProtectedRoute>
                      <Workspace />
                    </ProtectedRoute>
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

