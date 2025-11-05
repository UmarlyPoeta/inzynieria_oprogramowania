import { render, screen } from '@testing-library/react'
import { Authorization } from '@/pages'
import { vi } from 'vitest'

vi.mock('@/components', () => ({
  AuthLayout: ({ children }: { children: React.ReactNode }) => (
    <div data-testid="auth-layout">{children}</div>
  ),
  AuthForm: ({ mode }: { mode: string }) => (
    <div data-testid="auth-form" data-mode={mode} />
  ),
}))

describe('Authorization', () => {
  it('renders login layout', () => {
    render(<Authorization mode="login" />)
    expect(screen.getByTestId('auth-layout')).toBeInTheDocument()
    expect(screen.getByTestId('auth-form')).toHaveAttribute('data-mode', 'login')
  })

  it('renders register layout', () => {
    render(<Authorization mode="register" />)
    expect(screen.getByTestId('auth-layout')).toBeInTheDocument()
    expect(screen.getByTestId('auth-form')).toHaveAttribute('data-mode', 'register')
  })
})
