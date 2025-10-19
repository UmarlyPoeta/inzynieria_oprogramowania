import styled from "styled-components";

const Sidebar = styled.div`
  width: 220px;
  background-color: ${(props: any) => props.theme.colors.surface};
  border-right: 1px solid #222;
  padding: ${(props: any) => props.theme.spacing(2)};
  overflow-y: auto;
`;

const LeftSidebar = () => {
  return (
    <Sidebar>
      <h4>Urządzenia</h4>
      <p>Router</p>
      <p>Switch</p>
      <p>PC</p>
    </Sidebar>
  );
}

export default LeftSidebar;

