import { render, screen } from '@testing-library/react'
import Workspace from './Workspace'
import { vi } from 'vitest'

vi.mock('@/components', () => ({
  LeftSidebar: () => <div data-testid="left-sidebar" />,
  CanvasArea: () => <div data-testid="canvas-area" />,
  RightSidebar: () => <div data-testid="right-sidebar" />,
  DeviceTool: () => <div data-testid="device-tool" />,
}))

describe('Workspace', () => {
  it('renders all main components', () => {
    render(<Workspace />)
    expect(screen.getByTestId('left-sidebar')).toBeInTheDocument()
    expect(screen.getByTestId('canvas-area')).toBeInTheDocument()
    expect(screen.getByTestId('device-tool')).toBeInTheDocument()
    expect(screen.getByTestId('right-sidebar')).toBeInTheDocument()
  })
})

