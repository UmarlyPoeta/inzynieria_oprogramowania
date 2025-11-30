import { useState } from "react";
import { 
    Rectangle,
    Table, Thead, Tbody,
    Td, Tr, Th,
    ToggleButton, CollapsedBar
} from './PduBar.styled';

const PduBar = () => {
    const [collapsed, setCollapsed] = useState(false);

    return (
        <>

            {!collapsed && ( 
                <Rectangle>

                    <ToggleButton onClick={() => setCollapsed(true)}> x </ToggleButton>

                    <Table>
                        <Thead>
                            <Tr>
                                <Th> Status </Th>
                                <Th> Source </Th>
                                <Th> Destination </Th>
                                <Th> Type </Th>
                                <Th> Time </Th>
                                <Th> Periodic </Th>
                            </Tr>
                        </Thead>
                        <Tbody>
                            {[1,2,3,4,5,6].map(i => ( 
                                <Tr key={i}>
                                    <Td> Success </Td>
                                    <Td> PC0 </Td>
                                    <Td> PC1 </Td>
                                    <Td> ICMP </Td>
                                    <Td> 0.000 </Td>
                                    <Td> N </Td>
                                </Tr>
                            ))}
                        </Tbody>
                    </Table>

                </Rectangle>

                )}

                {collapsed && ( 
                    <CollapsedBar onClick={() => setCollapsed(false)}> 
                        Show PDU List
                    </CollapsedBar>
                )}

        </>
    )
}

export default PduBar;