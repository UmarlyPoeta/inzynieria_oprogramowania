import { styled } from 'styled-components';

export const Wrapper = styled.div<{ gradient?: string }>`
  min-height: 100vh;
  width: 100%;
  height: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
  background-color: #c1bfbfff;
  padding: 3rem;

  @media (max-width: 1300px) {
    height: 100%;
    min-height: 0;
  }
`;