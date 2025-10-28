import styled from 'styled-components';

export const Sidebar = styled.div`
  width: 280px;
  background-color: ${(props: any) => props.theme.colors.surface};
  border-left: 2px solid #ededf5;
  overflow-y: auto;
  z-index: 2;
`;

export const SimulationContainer = styled.div`
  display: flex;
  justify-content: space-between;
  font-size: 1rem;
  font-weight: bold;
  text-transform: uppercase;
  align-items: center;
  flex-direction: row;
  width: 100%;
  padding: 10px;
  border-bottom: 2px solid #ededf5;
`;

export const IconButton = styled.button`
  width: 80px;
  height: 50px;
  border: none;
  border-radius: 10px;
  background: none;
  color: #121212;
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

export const PanelWrapper = styled.div`
  padding: 12px;
`;

export const TabsWrapper = styled.div`
  display: flex;
  gap: 4px;
  margin-bottom: 12px;
  flex-wrap: wrap;
`;

export const TabButton = styled.button<{ active: boolean }>`
  padding: 6px 12px;
  border: 1px solid ${(props) => (props.active ? "#FFA500" : "#ccc")};
  border-bottom: ${(props) => (props.active ? "2px solid orange" : "1px solid #ccc")};
  border-radius: 6px 6px 0 0;
  background-color: ${(props) => (props.active ? "#fff" : "#f7f7f7")};
  cursor: pointer;
  font-weight: ${(props) => (props.active ? "bold" : "normal")};
  font-size: 0.85rem;
`;

export const Section = styled.div`
  margin-bottom: 12px;
`;

export const Input = styled.input`
  width: 100%;
  margin-top: 4px;
  margin-bottom: 8px;
  padding: 4px 6px;
  border: 1px solid #ccc;
  border-radius: 4px;
`;

export const Select = styled.select`
  width: 100%;
  margin-top: 4px;
  margin-bottom: 8px;
  padding: 4px 6px;
  border: 1px solid #ccc;
  border-radius: 4px;
`;