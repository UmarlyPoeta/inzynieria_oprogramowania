import styled from "styled-components";

const Canvas = styled.div`
  flex: 1;
  background-color: ${(props: any) => props.theme.colors.background};
  display: flex;
  align-items: center;
  justify-content: center;
`;

const CanvasArea = () => {
  return (
    <Canvas>
      <p>Tu będzie scena / edytor (zoom, drag, nody)</p>
    </Canvas>
  );
}

export default CanvasArea;