import { render, screen } from '@testing-library/react';
import Workspace from './Workspace';
import { vi } from 'vitest';

vi.mock('@/components', () => ({
  LeftSidebar: () => <div data-testid="left-sidebar" />,
  CanvasArea: () => <div data-testid="canvas-area" />,
  RightSidebar: () => <div data-testid="right-sidebar" />,
  DeviceTool: () => <div data-testid="device-tool" />,

  // druga gałąź (na wszelki wypadek)
  Area: () => <div data-testid="area" />,
  StatisticsBar: () => <div data-testid="statistics-bar" />,
  InfoAndControlBar: () => <div data-testid="info-bar" />,
}));


vi.mock('@/context/WorkspaceModeContext', () => ({
  useMode: () => ({
    mode: 'editor',
  }),
}));


vi.mock('@/hooks', () => ({
  useKeyboardShortcuts: () => {},
}));


vi.mock('@/context/EditorContext', () => ({
  __esModule: true,
  useEditor: () => ({
    devices: [],
    links: [],
    groups: [],
    undo: vi.fn(),
    redo: vi.fn(),
    addDevice: vi.fn(),
    deleteDevice: vi.fn(),
    removeDeviceFromGroup: vi.fn(),
    moveDeviceToGroup: vi.fn(),
    addLink: vi.fn(),
    moveDevice: vi.fn(),
    addGroup: vi.fn(),
    renameGroup: vi.fn(),
    deleteGroup: vi.fn(),
    toggleGroupCollapsed: vi.fn(),
    selectedDeviceId: undefined,
    selectDevice: vi.fn(),
    connectingDeviceId: null,
    startConnecting: vi.fn(),
    selectDeviceForLink: vi.fn(),
    stopConnecting: vi.fn(),
    connectingModeActive: false,
    updateDeviceConfig: vi.fn(),
    selectedDeviceIds: [],
    toggleSelectDevice: vi.fn(),
    selectAll: vi.fn(),
  }),
}));


describe('Workspace', () => {
  it('renders editor layout components', () => {
    render(<Workspace />);

    expect(screen.getByTestId('left-sidebar')).toBeInTheDocument();
    expect(screen.getByTestId('canvas-area')).toBeInTheDocument();
    expect(screen.getByTestId('device-tool')).toBeInTheDocument();
    expect(screen.getByTestId('right-sidebar')).toBeInTheDocument();
  });
});


