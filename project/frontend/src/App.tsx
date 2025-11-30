import { ThemeProvider } from "styled-components";
import { BrowserRouter, Routes, Route } from "react-router-dom";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";

import { GlobalStyle, Theme } from "@/styles";
import { EditorProvider } from "@/context/EditorContext";
import { ContextMenuProvider } from "@/context/ContextualMenuContext";
import { AuthProvider } from "@/context/AuthContext";
import { ModeProvider } from "@/context/WorkspaceModeContext";
import { NetworkProvider } from "@/context/NetworkContext";
import { ModalProvider } from "@/context/ModalContext";

import { GlobalContextMenu, ProtectedRoute } from "@/components";
import { Workspace, Authorization } from "@/pages";

const queryClient = new QueryClient();

const App = () => {
  return (
    <QueryClientProvider client={queryClient}>
      <ThemeProvider theme={Theme}>
        <AuthProvider>
          <EditorProvider>
            <NetworkProvider>
            <ContextMenuProvider>
              <ModalProvider>
                <GlobalStyle />
                <BrowserRouter>
                  <Routes>
                    <Route
                      path="/"
                      element={
                        <ProtectedRoute redirectIfAuthenticated>
                          <Authorization mode="login" />
                        </ProtectedRoute>
                      }
                    />
                    <Route
                      path="/login"
                      element={
                        <ProtectedRoute redirectIfAuthenticated>
                          <Authorization mode="login" />
                        </ProtectedRoute>
                      }
                    />
                    <Route
                      path="/register"
                      element={
                        <ProtectedRoute redirectIfAuthenticated>
                          <Authorization mode="register" />
                        </ProtectedRoute>
                      }
                    />
                    <Route
                      path="/workspace"
                      element={
                        <ProtectedRoute>
                          <ModeProvider>
                            <Workspace />
                          </ModeProvider>
                        </ProtectedRoute>
                      }
                    />
                  </Routes>
                </BrowserRouter>
                <GlobalContextMenu />
              </ModalProvider>
            </ContextMenuProvider>
            </NetworkProvider>
          </EditorProvider>
        </AuthProvider>
      </ThemeProvider>
    </QueryClientProvider>
  );
};

export default App;



