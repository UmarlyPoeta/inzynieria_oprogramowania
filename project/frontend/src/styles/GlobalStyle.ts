
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
            overflow: hidden;
         }

        #root {
            height: 100%;
            width: 100%;
        }
    `;

export default GlobalStyle;