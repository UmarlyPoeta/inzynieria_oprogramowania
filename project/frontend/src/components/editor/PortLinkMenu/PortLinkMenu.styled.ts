import { styled } from 'styled-components'

export const MenuList = styled.ul`
  list-style: none;
  margin: 0;
  padding: 4px;
`;

export const MenuItem = styled.li`
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px;
  border-radius: 6px;
  cursor: pointer;
  font-size: 0.9rem;
  color: #333;
  transition: background 0.15s;

  &:hover {
    background-color: rgba(255, 174, 0, 0.15);
  }
`;