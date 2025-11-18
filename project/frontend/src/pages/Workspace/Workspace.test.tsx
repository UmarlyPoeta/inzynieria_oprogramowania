import { render, screen } from "@testing-library/react";
import Workspace from "./Workspace";
import { vi } from "vitest";

vi.mock("@/components", () => ({
  LeftSidebar: () => <div data-testid="left-sidebar" />,
  CanvasArea: () => <div data-testid="canvas-area" />,
  RightSidebar: () => <div data-testid="right-sidebar" />,
  DeviceTool: () => <div data-testid="device-tool" />,
}));

vi.mock("@/context/EditorContext", () => ({
  __esModule: true,
  useEditor: () => ({
    devices: [],
    links: [],
    groups: [],
    undo: () => {},
    redo: () => {},
    addDevice: () => {},
    deleteDevice: () => {},
    removeDeviceFromGroup: () => {},
    moveDeviceToGroup: () => {},
    addLink: () => {},
    moveDevice: () => {},
    addGroup: () => {},
    renameGroup: () => {},
    deleteGroup: () => {},
    toggleGroupCollapsed: () => {},
    selectedDeviceId: undefined,
    selectDevice: () => {},
    connectingDeviceId: null,
    startConnecting: () => {},
    selectDeviceForLink: () => {},
    stopConnecting: () => {},
    connectingModeActive: false,
    updateDeviceConfig: () => {},
    selectedDeviceIds: [],
    toggleSelectDevice: () => {},
    selectAll: () => {},
  }),
}));

vi.mock("@/context/WorkspaceModeContext", () => ({
  __esModule: true,
  useMode: () => ({
    mode: "editor",
    setMode: vi.fn(),
    toggleMode: vi.fn(),
  }),
}));

describe("Workspace", () => {
  it("renders all main components", () => {
    render(<Workspace />);

    expect(screen.getByTestId("left-sidebar")).toBeInTheDocument();
    expect(screen.getByTestId("canvas-area")).toBeInTheDocument();
    expect(screen.getByTestId("device-tool")).toBeInTheDocument();
    expect(screen.getByTestId("right-sidebar")).toBeInTheDocument();
  });
});


