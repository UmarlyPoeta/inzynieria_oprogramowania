import { styled } from "styled-components";

export const SidebarContainer = styled.div`
  position: fixed;
  left: 0;
  top: 0;
  height: 100vh;
  width: 256px;
  background-color: #fff;
  border-right: 2px solid #e5e7eb;
  overflow-y: auto;
  z-index: 50;
//   box-shadow: 2px 0 8px rgba(0, 0, 0, 0.1);
`;

export const SidebarHeader = styled.div`
  position: sticky;
  top: 0;
  background-color: #fff;
  border-bottom: 1px solid #e5e7eb;
  padding: 16px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  z-index: 10;
`;

export const HeaderActions = styled.div`
  display: flex;
  align-items: center;
  gap: 4px;
`;

export const HeaderIconButton = styled.button`
  width: 32px;
  height: 32px;
  border: none;
  border-radius: 6px;
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
    width: 18px;
    height: 18px;
  }
`;

export const CompanyId = styled.span`
  font-size: 14px;
  font-weight: 600;
  color: #4b5563;
`;

export const CloseButton = styled.button`
  padding: 4px;
  background: none;
  border: none;
  cursor: pointer;
  border-radius: 4px;
  display: flex;
  align-items: center;
  justify-content: center;
  transition: background-color 0.2s;

  &:hover {
    background-color: #f3f4f6;
  }

  svg {
    color: #4b5563;
  }
`;

export const CompanyNameSection = styled.div`
  padding: 12px 16px;
  border-bottom: 1px solid #e5e7eb;
`;

export const CompanyName = styled.h2`
  font-size: 18px;
  font-weight: 700;
  color: #1f2937;
  margin: 0 0 8px 0;
`;

export const CompanyBadge = styled.span`
  display: inline-block;
  padding: 4px 12px;
  background-color: #f37c46ff;
  color: #fff;
  font-size: 12px;
  font-weight: 600;
  border-radius: 4px;
`;

export const SearchSection = styled.div`
  padding: 12px 16px;
  border-bottom: 1px solid #e5e7eb;
`;

export const SearchInput = styled.input`
  width: 100%;
  padding: 8px 12px;
  font-size: 14px;
  border: 1px solid #d1d5db;
  border-radius: 8px;
  outline: none;
  transition: all 0.2s;

  &:focus {
    border-color: #f37c46ff;
    box-shadow: 0 0 0 3px rgba(124, 58, 237, 0.1);
  }

  &::placeholder {
    color: #9ca3af;
  }
`;

export const PropertiesSection = styled.div`
  padding: 16px;
`;

export const SectionTitle = styled.h3`
  font-size: 13px;
  font-weight: 700;
  color: #374151;
  margin: 20px 0 12px 0;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  
  &:first-child {
    margin-top: 0;
  }
`;

export const PropertyItem = styled.div`
  margin-bottom: 12px;
`;

export const PropertyLabel = styled.div`
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 12px;
  font-weight: 500;
  color: #6b7280;
  margin-bottom: 4px;
`;

export const IconWrapper = styled.span`
  display: flex;
  align-items: center;
  color: #9ca3af;
`;

export const PropertyValue = styled.div`
  font-size: 14px;
  font-weight: 500;
  color: #1f2937;
`;

export const PropertyLink = styled.a`
  font-size: 14px;
  color: #2563eb;
  text-decoration: none;
  word-break: break-all;
  display: block;

  &:hover {
    color: #1d4ed8;
    text-decoration: underline;
  }
`;

export const CollapsedSidebar = styled.div`
  position: fixed;
  left: 0;
  top: 0;
  height: 100vh;
  width: 48px;
  background-color: #f9fafb;
  border-right: 2px solid #e5e7eb;
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 50;
`;

export const ExpandButton = styled.button`
  padding: 8px;
  background: none;
  border: none;
  cursor: pointer;
  border-radius: 8px;
  color: #4b5563;
  display: flex;
  align-items: center;
  justify-content: center;
  transition: background-color 0.2s;

  &:hover {
    background-color: #e5e7eb;
  }
`;

export const NoDeviceMessage = styled.div`
  padding: 40px 16px;
  text-align: center;
  color: #9ca3af;
  
  p {
    margin: 0;
    font-size: 16px;
    font-weight: 500;
  }
`;