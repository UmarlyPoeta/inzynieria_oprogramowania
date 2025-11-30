import { useState } from "react";
import { Container, Row, ToggleButton, CollapsedBar, Wrapper } from './MainBar.styled';
import { BarChart, Bar, XAxis, YAxis, Tooltip, Legend, CartesianGrid, ResponsiveContainer } from "recharts";


const PacketBarChart = () => {
    const data = [
    { node: "A", sent: 10, received: 8 },
    { node: "B", sent: 5, received: 12 },
    { node: "C", sent: 7, received: 7 },
    ];

  return (
    <ResponsiveContainer width="90%" height={200}>
        <BarChart data={data} margin={{ top: 10, right: 10, bottom: 10 }}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="node" tick={{ fontSize: 10 }} />
            <YAxis tick={{ fontSize: 10 }} />
            <Tooltip />
            <Legend wrapperStyle={{ fontSize: 10 }} />
            <Bar dataKey="sent" fill="#f37c46ff" name="Wysłane" />
            <Bar dataKey="received" fill="#fcc662ff" name="Odebrane" />
        </BarChart>
    </ResponsiveContainer>
  );
};

const MainBar = () => {
    const [collapsed, setCollapsed] = useState(false);

    return (
        <>
            {!collapsed && ( 
                <Wrapper>
                    <Container>

                        <ToggleButton onClick={() => setCollapsed(true)}> → </ToggleButton>
                        <Row>
                            <h1> NetSimCPP </h1>
                            <p> Project X</p>
                        </Row>
                        <Row>
                            <h2> Traffic between nodes </h2>
                            <PacketBarChart/>
                        </Row>
                        <Row>
                            
                        </Row>
                    </Container>
                </Wrapper>
                )}

            {collapsed && ( <CollapsedBar onClick={() => setCollapsed(false)}> Show Stats Bar </CollapsedBar> )}
        </>
    )
}

export default MainBar;