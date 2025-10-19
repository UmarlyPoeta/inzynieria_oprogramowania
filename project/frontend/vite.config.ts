import { defineConfig } from 'vite'
import { resolve } from 'path';
import react from '@vitejs/plugin-react'
import tailwindcss from '@tailwindcss/vite';

export default defineConfig({
  resolve: { 
    alias: { 
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
    },
  },
  plugins: [
    react(),
    tailwindcss(),
  ],
})
