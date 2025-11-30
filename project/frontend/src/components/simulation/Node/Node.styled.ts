import { styled, keyframes } from "styled-components";

const blob = keyframes`
  0%, 100% {
    border-radius: 60% 40% 30% 70% / 60% 30% 70% 40%;
  }
  50% {
    border-radius: 30% 60% 70% 40% / 50% 60% 30% 60%;
  }
`;

export const Node = styled.div<{ $selected?: boolean }>`
  position: absolute;
  width: 60px;
  height: 60px;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: grab;
  user-select: none;
  overflow: visible;
  z-index: 1;

  /* tło blob */
  &::before {
    content: "";
    position: absolute;
    inset: 0;
    background: radial-gradient(circle at 30% 30%, #fcc8a5ff, #f8903bff);
    animation: ${blob} 8s ease-in-out infinite;
    opacity: 1;
    z-index: 0;
  }

  /* border blob */
  &::after {
    content: "";
    position: absolute;
    inset: -3px;
    background: ${({ $selected }) =>
      $selected ? "linear-gradient(90deg, #121212, #121212)" : "transparent"};
    border-radius: 60% 40% 30% 70% / 60% 30% 70% 40%;
    animation: ${blob} 8s ease-in-out infinite;
    z-index: -1;
  }

  img {
    position: relative;
    z-index: 2;
    width: 60%;
    height: 60%;
  }
`;



