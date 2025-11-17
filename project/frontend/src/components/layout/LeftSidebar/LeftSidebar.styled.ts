import { styled } from 'styled-components'

export const SidebarWrapper = styled.div`
  display: flex;
  height: 100%;
  z-index: 2;
  background-color: ${(props: any) => props.theme.colors.surface};
`;

export const LeftPanel = styled.div`
  width: 70px;
  border-right: 1px solid #333;
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 8px 0;
  gap: 4px;
  background-color: ${(props: any) => props.theme.colors.surfaceAlt};
  border-right: 2px solid #ededf5;
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

export const RightPanel = styled.div`
  flex: 1;
  // padding: 12px;
  width: 202px;
  display: flex;
  flex-direction: column;
  border-right: 2px solid #ededf5;
`;

export const SectionTitle = styled.h3`
  color: #121212;
  font-size: 0.8rem;
  font-weight: bold;
  text-transform: uppercase;
  margin-bottom: 8px;
  letter-spacing: 0.5px;
`;

export const AppLogo = styled.div`
  margin-top: 10px;
  font-size: 1.2rem;
  font-weight: bold;
  color: ${(props: any) => props.theme.colors.text};
  margin-bottom: 16px;

  svg {
    width: 20px;
    height: 20px;
    color: #121212;
  }

  border-bottom: 2px solid #e9e9e9ff;
  padding-bottom: 18px;
`;

export const RightBarHeader = styled.div`
  margin-top: 10px;
  font-size: .75rem;
  font-weight: bold;
  color: rgba(41, 41, 41, 0.42);
  margin-bottom: 12px;
  border-bottom: 1px solid #e9e9e9ff;
  padding: 12px;
  padding-bottom: 18px;
  width: 100%;

  h1 { 
    font-size: 1rem;
    color: #121212;
  }

  span { 
    color: rgba(255, 153, 0, 1);
  }

  svg { 
    display: inline;
    width: 14px;
    height: 14px;
  }
`;

export const LayersPanelWrapper = styled.div`
  padding: 12px;
  flex: 1;
  overflow-y: auto;
`;

export const GroupRow = styled.div<{ collapsed: boolean }>`
  display: flex;
  align-items: center;
  justify-content: space-between;
  font-weight: bold;
  cursor: pointer;
  margin-bottom: 4px;
`;

export const DeviceRow = styled.div`
  padding-left: 16px;
  margin-bottom: 2px;
  cursor: pointer;
  &:hover {
    background-color: rgba(255, 174, 0, 0.1);
  }
`;