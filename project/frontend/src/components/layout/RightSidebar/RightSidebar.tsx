import styled from "styled-components";

const Sidebar = styled.div`
  width: 260px;
  background-color: ${(props: any) => props.theme.colors.surface};
  border-left: 1px solid #222;
  padding: ${(props: any) => props.theme.spacing(2)};
  overflow-y: auto;
`;

const RightSidebar = () => {
  return (
    <Sidebar>
      <h4>Właściwości</h4>
      <p>Brak zaznaczenia</p>
    </Sidebar>
  );
}

export default RightSidebar;


