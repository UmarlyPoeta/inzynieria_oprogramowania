import { styled } from "styled-components"

export const Wrapper = styled.div`
  position: absolute;
  top: 10px;
  right: 10px;
`;

export const Container = styled.div`
    display: flex;
    flex-direction: column;
    position: relative;
    width: 400px;
    z-index: 2;
    background-color: #FFF;
    border: 2px solid #ededf5;
    border-radius: 10px;
    transition: all 0.3s ease;
    // overflow: hidden;
`;

export const Row = styled.div`
    font-weight: bold;
    padding: 20px;
    color: #121212;
    position: relative;

    border-bottom: 2px solid #ededf5;

    h1 { 
        font-size: 1rem;
    }

    h2 { 
        font-weight: 600;
        text-align: center;
    }

    p { 
        font-size: .75rem;
        font-weight: bold;
        color: rgba(41, 41, 41, 0.42);
    }
`

export const ToggleButton = styled.button`
    position: absolute;
    // top: -9px;
    // right: -5px;
    width: 22px;
    height: 22px;
    border-top-left-radius: 50%;
    border-bottom-left-radius: 50%;
    border-left: 2px solid #ededf5;
    border-bottom: 2px solid #ededf5;
    border-top: 2px solid #ededf5;
    border: none;
    cursor: pointer;
    background: #fff;
    color: #121212;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 14px;
    z-index: 2;

    left: -20px;           
    top: 50%;             
    transform: translateY(-50%); 

    &:hover {
        background: #ccc;
    }
`;

export const CollapsedBar = styled.div`
    position: absolute;
    bottom: 0;           
    left: 10px;             
    transform: translateY(-50%); 
    width: 120px;
    height: 28px;
    border-radius: 10px;
    background: #fff;
    border: 2px solid #ededf5;
    cursor: pointer;
    display: flex;
    justify-content: center;
    align-items: center;
    font-size: 14px;
    transition: all 0.3s ease;
    z-index: 2;

    &:hover {
        background: #f6f6f6;
    }
`;