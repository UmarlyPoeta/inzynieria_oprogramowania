/** @type {import('tailwindcss').Config} */
const config = {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}"
  ],
  theme: {
    extend: {
      colors: {
        'sidebar-bg': '#ffffff', 
        'sidebar-border': '#ededf5', 
        'sidebar-font': '#7b8191', 
        'sidebar-font-opacity': 'rgba(123,129,145,0.6)', 
        'sidebar-font-selected': '#323251', 
        'menu-logo': '#000000', 
      },
      boxShadow: {
        sidebar: '0 8px 14.72px 1.28px #9a9acc1a', // --sidebar-shadow
      },
    },
  },
  plugins: [],
};

export default config;
