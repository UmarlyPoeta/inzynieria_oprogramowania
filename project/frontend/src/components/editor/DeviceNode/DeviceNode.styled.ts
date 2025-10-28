import { styled } from 'styled-components'

export const Node = styled.div`
  position: absolute;
  width: 60px;
  height: 60px;
  background-color: ${(props: any) => props.theme.colors.primary};
  border-radius: 8px;
  display: flex;
  align-items: center;
  justify-content: center;
  color: ${(props: any) => props.theme.colors.text};
  cursor: grab;
  user-select: none;
`;