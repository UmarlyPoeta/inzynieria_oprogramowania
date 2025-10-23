import styled from "styled-components";
import { Play } from 'lucide-react'

const Sidebar = styled.div`
  width: 260px;
  background-color: ${(props: any) => props.theme.colors.surface};
  border-left: 2px solid #ededf5;
  overflow-y: auto;
  z-index: 2;
`;

const SimulationContainer = styled.div`
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

const IconButton = styled.button`
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
  // background-color: rgba(255, 153, 0, 1);
  
  svg { 
    text-align: right;
  }

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



const RightSidebar = () => {
  return (
    <Sidebar>
      <SimulationContainer>
        <h2> Configuration </h2>
        <IconButton title="Run Simulation">
          <Play/>
        </IconButton>
      </SimulationContainer>
      <div style={{ padding: '10px', borderBottom: '2px solid #ededf5'}}>
        Nazwa urzadzenia
      </div>
      <div style={{ paddingLeft: '10px'}}>
        Konfiguracja urzadzenia
      </div>
    </Sidebar>
  );
}

export default RightSidebar;


