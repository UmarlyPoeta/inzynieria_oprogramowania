import { useState } from "react";
import { useNetworkActions } from "@/hooks";
import { useEditor } from "@/context/EditorContext";
import { 
    Rectangle,
    Table, Thead, Tbody,
    Td, Tr, Th,
    ToggleButton, CollapsedBar
} from './PduBar.styled';

const PduBar = () => {
    const [collapsed, setCollapsed] = useState(false);
    const { pdus } = useNetworkActions();
    const { devices } = useEditor();

    const getDeviceName = (deviceId: string) => {
        const device = devices.find(d => d.id === deviceId);
        return device?.name || deviceId;
    };

    const getActionType = (type: string) => {
        return type === 'ping' ? 'ICMP' : 'Traceroute';
    };

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
                                <Th> Hops </Th>
                                {/* <Th> Path </Th> */}
                            </Tr>
                        </Thead>
                        <Tbody>
                            {pdus.length === 0 ? (
                                <Tr>
                                    <Td colSpan={6} style={{ textAlign: 'center', opacity: 0.5 }}>
                                        No PDUs yet. Use Ping or Traceroute to generate traffic.
                                    </Td>
                                </Tr>
                            ) : (
                                pdus.map((pdu, index) => ( 
                                    <Tr key={index}>
                                        <Td style={{ color: pdu.success ? '#4ade80' : '#ef4444' }}>
                                            {pdu.success ? 'Success' : 'Failed'}
                                        </Td>
                                        <Td> {getDeviceName(pdu.src)} </Td>
                                        <Td> {getDeviceName(pdu.dst)} </Td>
                                        <Td> {getActionType(pdu.type)} </Td>
                                        <Td> {pdu.hops || '-'} </Td>
                                        {/* <Td> 
                                            {pdu.path ? pdu.path.map(id => getDeviceName(id)).join(' → ') : '-'}
                                        </Td> */}
                                    </Tr>
                                ))
                            )}
                        </Tbody>
                    </Table>
                </Rectangle>
            )}

            {collapsed && ( 
                <CollapsedBar onClick={() => setCollapsed(false)}> 
                    Show PDU List {pdus.length > 0 && `(${pdus.length})`}
                </CollapsedBar>
            )}
        </>
    )
}

export default PduBar;