import { createContext, useContext, useState, type ReactNode } from "react";

type Mode = "editor" | "simulation";

interface WorkspaceModeContextType {
  mode: Mode;
  setMode: (mode: Mode) => void;
  toggleMode: () => void;
}

const WorkspaceModeContext = createContext<WorkspaceModeContextType | undefined>(undefined);

export const ModeProvider = ({ children }: { children: ReactNode }) => {
  const [mode, setMode] = useState<Mode>("editor");

  const toggleMode = () => setMode((prev) => (prev === "editor" ? "simulation" : "editor"));

  return (
    <WorkspaceModeContext.Provider value={{ mode, setMode, toggleMode }}>
      {children}
    </WorkspaceModeContext.Provider>
  );
};

export const useMode = () => {
  const context = useContext(WorkspaceModeContext);
  if (!context) throw new Error("useWorkspaceMode must be used within a WorkspaceModeProvider");
  return context;
};

