import { renderHook, act } from '@testing-library/react'
import { describe, it, expect, vi } from 'vitest'
import { useMutation } from '@/hooks'

describe('useMutation', () => {
  it('should initialize with idle state', () => {
    const mutationFn = vi.fn()
    const { result } = renderHook(() => useMutation(mutationFn))

    expect(result.current.status).toBe('idle')
    expect(result.current.data).toBeNull()
    expect(result.current.error).toBeNull()
  })

  it('should set status to loading while executing mutation', async () => {
    const mutationFn = vi.fn(() => new Promise((res) => setTimeout(() => res('done'), 100)))
    const { result } = renderHook(() => useMutation(mutationFn))

    act(() => {
      result.current.mutate('some-variable')
    })

    expect(result.current.status).toBe('loading')
  })

  it('should call mutationFn and update data on success', async () => {
    const mutationFn = vi.fn(() => Promise.resolve('result'))
    const onSuccess = vi.fn()
    const { result } = renderHook(() => useMutation(mutationFn, onSuccess))

    await act(async () => {
      await result.current.mutate('payload')
    })

    expect(mutationFn).toHaveBeenCalledWith('payload')
    expect(result.current.data).toBe('result')
    expect(result.current.status).toBe('success')
    expect(onSuccess).toHaveBeenCalledWith('result')
  })

  it('should handle errors and call onError callback', async () => {
    const error = new Error('something went wrong')
    const mutationFn = vi.fn(() => Promise.reject(error))
    const onError = vi.fn()
    const { result } = renderHook(() => useMutation(mutationFn, undefined, onError))

    await act(async () => {
      await result.current.mutate('input')
    })

    expect(result.current.status).toBe('error')
    expect(result.current.error).toBe(error)
    expect(onError).toHaveBeenCalledWith(error)
  })

  it('should reset previous error and data when mutate is called again', async () => {
    const error = new Error('bad')
    const mutationFn = vi
      .fn()
      .mockRejectedValueOnce(error)
      .mockResolvedValueOnce('ok')
    const { result } = renderHook(() => useMutation(mutationFn))

    await act(async () => {
      await result.current.mutate('input1')
    })

    expect(result.current.status).toBe('error')
    expect(result.current.error).toBe(error)

    await act(async () => {
      await result.current.mutate('input2')
    })

    expect(result.current.status).toBe('success')
    expect(result.current.data).toBe('ok')
    expect(result.current.error).toBeNull()
  })
})
