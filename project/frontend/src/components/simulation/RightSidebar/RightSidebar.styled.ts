import { styled } from "styled-components"

export const Wrapper = styled.div`
  position: absolute;
  right: 0px;
`;

export const Container = styled.div`
    display: flex;
    flex-direction: column;
    position: relative;
    width: 400px;
    min-height: 100vh;
    z-index: 2;
    background-color: #FFF;
    border: 2px solid #ededf5;
    transition: all 0.3s ease;
    // overflow: hidden;
`;

export const Row = styled.div`
    font-weight: bold;
    padding: 20px;
    color: #121212;
    position: relative;

    display: flex;
    gap: 10px;
    margin-bottom: 6px;
    margin-top: 6px;
    border-bottom: 2px solid #ededf5;

    svg { 
        color: #f37c46ff;
    }

    h1 { 
        font-size: 1rem;
    }

    h2 { 
        font-weight: 600;
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

export const TabContainer = styled.div`
  display: flex;
  justify-content: space-around;
  gap: 20px;
  padding: 10px 0;
`;

interface TabProps {
  active: boolean;
}

export const Tab = styled.div<TabProps>`
  flex: 1;
  text-align: center;
  cursor: pointer;
  padding-bottom: 12px;
  transition: all 0.2s;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  position: relative;
  font-size: 14px;
  font-weight: 600;
  
  &::before {
    content: '';
    position: absolute;
    bottom: 0;
    left: 50%;
    transform: translateX(-50%);
    width: 80%;
    height: 2.4px;
    background-color: #ededf5;
  }
  
  &::after {
    content: '';
    position: absolute;
    bottom: 0;
    left: 50%;
    transform: translateX(-50%);
    width: ${({ active }) => active ? '90%' : '0%'};
    height: 2.6px;
    background-color: #f37c46ff;
    transition: width 0.2s;
  }
  
  &:hover {
    opacity: 0.8;
  }
`;

export const FilterList = styled.div`
  display: flex;
  flex-direction: column;
  width: 100%;
`;

export const FilterItem = styled.div`
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 6px 20px;
//   border-bottom: 1px solid #ededf5;
  font-size: 0.875rem;
  
  &:hover {
    background-color: #f9f9f9;
  }
`;

export const FilterLabel = styled.div`
  display: flex;
  align-items: center;
  gap: 8px;
  color: #121212;
  font-weight: 500;
  
  span {
    font-size: 0.815rem;
  }
`;

export const FilterCount = styled.span`
  font-size: 0.875rem;
  color: #666;
  font-weight: 600;
`;

interface ToggleSwitchProps {
  active: boolean;
}

export const ToggleSwitch = styled.div<ToggleSwitchProps>`
  width: 40px;
  height: 22px;
  border-radius: 11px;
  background-color: ${({ active }) => active ? '#f37c46ff' : '#ccc'};
  position: relative;
  cursor: pointer;
  transition: background-color 0.2s;
  
  &::after {
    content: '';
    position: absolute;
    top: 2px;
    left: ${({ active }) => active ? '20px' : '2px'};
    width: 18px;
    height: 18px;
    border-radius: 50%;
    background-color: white;
    transition: left 0.2s;
  }
`;

export const Table = styled.table`
  width: 100%;
  font-size: 12px;
  border-collapse: collapse;
`;

export const Thead = styled.thead`
  background-color: #f37c46ff;
  color: #FFF;
  position: sticky;
  top: 0;
  z-index: 1;
`;

export const Tbody = styled.tbody``;

export const Tr = styled.tr`
  &:nth-child(even) {
    background-color: #fafafa;
  }
`;

export const Th = styled.th`
  padding: 10px;
  text-align: center;
  font-weight: 600;
  border-bottom: 2px solid #ededf5;
  font-size: 11px;
`;

export const Td = styled.td`
  padding: 10px;
  text-align: center;
  border-bottom: 1px solid #ededf5;
  font-size: 11px;
`;