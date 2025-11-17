import { styled } from 'styled-components'

export const Container = styled.div`
    flex: 1;
    background-color: ${(props: any) => props.theme.colors.background};
    overflow: hidden;
    position: absolute;
    z-index: 1;
    cursor: grab;
    width: 100%;
    height: 100%;
`;

export const Grid = styled.div`
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-size: 20px 20px;
  // background-image: 
  //   linear-gradient(to right, #222 1px, transparent 1px),
  //   linear-gradient(to bottom, #222 1px, transparent 1px);
  pointer-events: none;
`;

export const Inner = styled.div`
  position: absolute;
  top: 0;
  left: 0;
   //width: 100%;
   //height: 100%;
`;