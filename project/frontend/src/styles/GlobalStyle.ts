
import { createGlobalStyle } from 'styled-components';

const GlobalStyle = createGlobalStyle 
    `
        *, *::before, *::after {
            box-sizing: border-box;
        }

        html, body {
            margin: 0;
            padding: 0;
            background-color: #f9fbfb;
            color: #121212;
            font-family: "Inter", sans-serif;
            height: 100%;
            width: 100%;
            overflow: auto;        
            scrollbar-width: none; 
            -ms-overflow-style: none;
         }

        #root {
            height: 100%;
            width: 100%;
        }

        ::selection { 
            background-color: #FC5D08;
            color: #fff;
        }

        ::-webkit-scrollbar {
            display: none; 
        }   
    `;

export default GlobalStyle;