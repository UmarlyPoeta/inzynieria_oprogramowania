import { styled } from 'styled-components';

export const Wrapper = styled.div<{ gradient?: string }>`
  min-height: 100vh;
  width: 100%;
  height: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
  background-color: color-mix(in oklab, #ff7f00 30%, black 100%);
`;