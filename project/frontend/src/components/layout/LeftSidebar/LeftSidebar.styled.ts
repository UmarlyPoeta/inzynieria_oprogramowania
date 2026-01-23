import { styled } from 'styled-components'

export const SidebarWrapper = styled.div`
  display: flex;
  height: 100vh;
  z-index: 50;
  background-color: #fff;
`;

export const LeftPanel = styled.div`
  width: 64px;
  background-color: #fff;
  border-right: 2px solid #e5e7eb;
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 12px 0;
  gap: 8px;
`;

export const IconButton = styled.button`
  width: 48px;
  height: 48px;
  border: none;
  border-radius: 8px;
  background: none;
  color: #121212;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  transition: all 0.2s;

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
  width: 192px;
  background-color: #fff;
  border-right: 2px solid #e5e7eb;
  display: flex;
  flex-direction: column;
  overflow-y: auto;
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
  width: 48px;
  height: 48px;
  display: flex;
  align-items: center;
  justify-content: center;
  margin-bottom: 12px;
  padding-bottom: 12px;
  border-bottom: 2px solid #e5e7eb;

  svg {
    width: 24px;
    height: 24px;
    color: #f37c46ff;
  }
`;

export const RightBarHeader = styled.div`
  position: sticky;
  top: 0;
  background-color: #fff;
  border-bottom: 1px solid #e5e7eb;
  padding: 12px;
  padding-bottom: 18px;
  z-index: 10;

  h1 { 
    font-size: 1rem;
    color: #121212;
    margin: 0 0 4px 0;
    display: flex;
    align-items: center;
    font-weight: 700;
  }

  p {
    font-size: 0.75rem;
    font-weight: bold;
    color: rgba(41, 41, 41, 0.42);
    margin: 0;
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

  button {
    width: 100%;
    padding: 2px;
    font-size: 0.875rem;
    background: none;
    border: 1px dashed #b2b2b6ff;
    border-radius: 6px;
    cursor: pointer;
    transition: all 0.2s;
    margin-top: 8px;

    &:hover {
      border-color: #f37c46ff;
      color: #f37c46ff;
      background-color: rgba(243, 124, 70, 0.05);
    }
  }
`;

export const GroupRow = styled.div<{ collapsed: boolean }>`
  display: flex;
  align-items: center;
  justify-content: space-between;
  font-weight: bold;
  cursor: pointer;
  margin-bottom: 4px;
  transition: all 0.2s;

  &:hover {
    background-color: #f3f4f6;
  }

  input {
    background: none;
    border: none;
    outline: none;
    color: #121212;
    padding: 0;
    margin: 0;
  }

  svg {
    width: 16px;
    height: 16px;
    color: #6b7280;
    transition: transform 0.2s;
    flex-shrink: 0;
  }
`;

export const DeviceRow = styled.div`
  padding-left: 16px;
  margin-bottom: 2px;
  cursor: pointer;
  transition: all 0.2s;
  
  &:hover {
    background-color: rgba(255, 174, 0, 0.1);
  }
`;