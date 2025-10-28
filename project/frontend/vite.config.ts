/// <reference types="vitest/config" />
import { defineConfig } from 'vite'
import { resolve } from 'path';
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  resolve: { 
    alias: { 
      '@': resolve(__dirname, "src"),
      '@/components': resolve(__dirname, 'src/components/index.ts'),
      '@/context': resolve(__dirname, 'src/context/index.ts'),
      '@/data': resolve(__dirname, 'src/data/index.ts'),
      '@/features': resolve(__dirname, 'src/features/index.ts'),
      '@/hooks': resolve(__dirname, 'src/hooks/index.ts'),
      '@/pages': resolve(__dirname, 'src/pages/index.ts'),
      '@/utils': resolve(__dirname, 'src/utils/index.ts'),
      '@/assets': resolve(__dirname, 'src/assets/index.ts'),
      '@/api': resolve(__dirname, 'src/api/index.ts'),
      '@/styles': resolve(__dirname, 'src/styles/index.ts'),
      '@/types': resolve(__dirname, 'src/types/index.ts'),
    },
  },
  test: { 
    globals: true,
    enviroment: 'jsdom',
    setupFiles: './src/setupTests.ts',
    include: ['**/*.test.{ts,tsx}'],
  } as any,
})
