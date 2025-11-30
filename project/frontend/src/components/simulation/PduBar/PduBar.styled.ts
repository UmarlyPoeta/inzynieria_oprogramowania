import { styled } from "styled-components";

export const Rectangle = styled.div`
    position: absolute;
    bottom: 10px;
    right: 10px;
    width: 400px;
    height: 140px;
    z-index: 2;
    background-color: #FFF;
    border: 2px solid #ededf5;
    border-radius: 10px;
    transition: all 0.3s ease;
    overflow: hidden;
    overflow-x: auto;
`;

export const ToggleButton = styled.button`
    position: absolute;
    top: -9px;
    right: -5px;
    width: 22px;
    height: 22px;
    border-radius: 50%;
    border: none;
    cursor: pointer;
    background: #fff;
    color: #121212;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 14px;

    &:hover {
        background: #ccc;
    }
`;

export const CollapsedBar = styled.div`
    position: absolute;
    bottom: 80px;
    font-weight: 500;
    left: 10px;
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

export const Table = styled.table`
  width: 100%;
  font-size: 12px;
  border-collapse: separate;
  border-spacing: 0;
  border-radius: 12px;
  overflow: hidden;
  box-shadow: 0 4px 15px rgba(0,0,0,0.1);
`;

export const Thead = styled.thead`
  display: table;
  width: 100%;
  table-layout: fixed;
  background-color: #f37c46ff; // #121212
  color: #FFF;
`;

export const Tbody = styled.tbody`
  display: block;
  max-height: 100px;
  overflow-y: auto;

  tr {
    display: table;
    width: 100%;
    table-layout: fixed;
  }
`;

export const Tr = styled.tr`
  &:nth-child(even) {
    background-color: #fafafa;
  }
`;

export const Th = styled.th`
  padding: 12px;
  text-align: center;
  font-weight: 600;
  border-bottom: 1px solid #ddd;
`;

export const Td = styled.td`
  padding: 12px;
  text-align: center;
  border-bottom: 1px solid #eee;
`;



