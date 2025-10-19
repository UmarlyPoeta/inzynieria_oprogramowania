import styled from 'styled-components';

const Bar = styled.div`
  height: 40px;
  background-color: ${(props: any) => props.theme.colors.surface};
  border-bottom: 1px solid #222;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 12px;
  user-select: none;
`;

const TopBar = () => { 
    return (
      <Bar>
        <span>NetSimCPP</span>
        <div>
            <button>Nowy</button>
            <button>Otwórz</button>
            <button>Symuluj</button>
        </div>
        </Bar>
    );
}

export default TopBar;

