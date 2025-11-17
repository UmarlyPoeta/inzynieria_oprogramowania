import { styled } from 'styled-components';

export const DeviceToolContainer = styled.div`
  position: absolute;
  bottom: 40px;      
  left: 50%;          
  transform: translateX(-50%);
  background-color: #fff;
  padding: 10px;
  border-radius: 5px;
  border: 1px solid #ccc;
  z-index: 1000;
`;


export const DeviceButton = styled.button`
    margin: 5px;
    padding: 10px 15px;
    border: none;
    border-radius: 3px;
    background-color: #ff7626ff
    color: white;
    cursor: pointer;
    font-size: 14px;
    &:hover {
        background-color: #ff7626ff;
        color: #fff
    }
`;

