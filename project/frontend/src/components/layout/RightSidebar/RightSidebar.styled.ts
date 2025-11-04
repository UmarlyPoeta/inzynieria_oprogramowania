import styled from 'styled-components';

export const Sidebar = styled.div`
  width: 320px;
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
  align-items: center;
  flex-direction: row;
  width: 100%;
  padding: 10px;
  border-bottom: 2px solid #ededf5;

  p { 
    font-size: .75rem;
    color: rgba(41, 41, 41, 0.42);
  }
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

export const Section = styled.div<{ direction?: string }>`
  display: flex;
  flex-direction: ${(props) => props.direction || "row"};
  gap: 10px;
  font-size: .9rem;
  // margin-bottom: 12px;
`;

export const Input = styled.input`
  width: 100%;
  margin-top: 4px;
  margin-bottom: 8px;
  padding: 2px 1px;
  padding-left: 10px;
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

export const SectionHeader = styled.div<{ expanded: boolean }>`
  // padding: 0.5rem 1rem;
  // padding-left: 8px;
  // background-color: ${({ expanded }) => (expanded ? "#f3f3f3" : "#f3f3f3")};
  // color: ${({ expanded }) => (expanded ? "#121212" : "#121212")};
  cursor: pointer;
  border-radius: 6px;
  margin-bottom: 0.3rem;
  font-weight: 600;
  display: flex;
  justify-content: space-between;
  align-items: center;

  span { 
    font-size: 0.75rem;
    padding-right: 8px;
  }
`;

export const AccordionContent = styled.div<{ expanded: boolean }>`
  max-height: ${({ expanded }) => (expanded ? "500px" : "0")};
  overflow: hidden;
  transition: max-height 0.3s ease;
`;