import styled from "styled-components";

export const Rectangle = styled.div`
    position: absolute;
    display: flex;
    justify-content: center;
    align-items: center;
    flex-direction: column;
    padding-top: 5px;
    padding-bottom: 5px;
    left: 10px;           
    top: 50%;             
    transform: translateY(-50%); 
    width: 65px;
    min-height: 25vh;
    z-index: 2;
    background-color: #FFF;
    border: 2px solid #ededf5;
    border-radius: 10px;
    transition: all 0.3s ease;
    overflow-y: auto;
`;


export const ToggleButton = styled.button`
    position: absolute;
    top: -6px;
    left: -4px;
    width: 22px;
    height: 22px;
    border-radius: 50%;
    border: none;
    cursor: pointer;
    background: #121212;
    color: #fff;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 14px;

    &:hover {
        background: #121212cb;
    }
`;

export const CollapsedBar = styled.div`
    position: absolute;
    bottom: 50px;           
    left: 10px;  
    width: 135px;              
    border-radius: 10px;
    background-color: #FFF;
    border: 2px solid #ededf5;
    cursor: pointer;
    font-size: 14px;
    font-weight: 500;
    color: #333;
    z-index: 2;
    transition: all 0.3s ease;
    box-shadow: 0 4px 8px rgba(0,0,0,0.08);
    padding-left: 7px;
    &:hover {
        background: #f6f6f6;
    }
`;

export const IconButton = styled.button`
  width: 50px;
  height: 50px;
  border: none;
  border-radius: 10px;
  background: none;
  color: ${(props: any) => props.theme.colors.text};
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  transition: background 0.2s;

  &:hover {
    background-color: rgba(255, 174, 0, 0.25);

    svg {
      color: rgba(255, 153, 0, 1);
    }
  }

  svg {
    width: 20px;
    height: 20px;
    color: #121212;
  }
`;

