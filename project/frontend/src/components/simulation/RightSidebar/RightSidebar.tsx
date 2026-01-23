import { useState } from "react";
import { useNetworkActions } from "@/hooks";
import { useEditor } from "@/context/EditorContext";
import { Container, Row, ToggleButton, CollapsedBar, Wrapper, TabContainer, Tab, FilterItem, FilterList, FilterLabel, FilterCount, ToggleSwitch, Table, Thead, Tbody, Tr, Th, Td } from './RightSidebar.styled';
import { BarChart, Bar, XAxis, YAxis, Tooltip, Legend, CartesianGrid, ResponsiveContainer } from "recharts";
import { Filter, Network, Link, ChevronDown, ChevronRight } from "lucide-react";

// const PacketBarChart = () => {
//   const data = [
//     { node: "A", sent: 10, received: 8 },
//     { node: "B", sent: 5, received: 12 },
//     { node: "C", sent: 7, received: 7 },
//     { node: "D", sent: 2, received: 9 },
//   ];

//   return (
//     <ResponsiveContainer width="90%" height={200}>
//       <BarChart data={data} margin={{ top: 10, right: 10, bottom: 10 }}>
//         <CartesianGrid strokeDasharray="3 3" />
//         <XAxis dataKey="node" tick={{ fontSize: 10 }} />
//         <YAxis tick={{ fontSize: 10 }} />
//         <Tooltip />
//         <Legend wrapperStyle={{ fontSize: 10 }} />
//         <Bar dataKey="sent" fill="#f37c46ff" name="Wysłane" />
//         <Bar dataKey="received" fill="#fcc662ff" name="Odebrane" />
//       </BarChart>
//     </ResponsiveContainer>
//   );
// };

const PacketBarChart = () => {
  const { pdus } = useNetworkActions();
  const { devices } = useEditor();

  const getDeviceName = (id: string) =>
    devices.find(d => d.id === id)?.name || id;

  const data = Object.values(
    pdus.reduce<Record<string, { node: string; sent: number; received: number }>>(
      (acc, pdu) => {
        const srcName = getDeviceName(pdu.src);
        const dstName = getDeviceName(pdu.dst);

        if (!acc[srcName]) {
          acc[srcName] = { node: srcName, sent: 0, received: 0 };
        }
        if (!acc[dstName]) {
          acc[dstName] = { node: dstName, sent: 0, received: 0 };
        }

        acc[srcName].sent += 1;
        acc[dstName].received += 1;

        return acc;
      },
      {}
    )
  );

  return (
    <ResponsiveContainer width="90%" height={200}>
      <BarChart data={data} margin={{ top: 10, right: 10, bottom: 10 }}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="node" tick={{ fontSize: 10 }} />
        <YAxis allowDecimals={false} tick={{ fontSize: 10 }} />
        <Tooltip />
        <Legend wrapperStyle={{ fontSize: 10 }} />
        <Bar dataKey="sent" fill="#f37c46ff" name="Wysłane" />
        <Bar dataKey="received" fill="#fcc662ff" name="Odebrane" />
      </BarChart>
    </ResponsiveContainer>
  );
};


const PduList = () => {
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
    <div style={{ padding: '20px', maxHeight: '300px', overflowY: 'auto', paddingTop: "2px" }}>
      <Table>
        <Thead>
          <Tr>
            <Th>Status</Th>
            <Th>Source</Th>
            <Th>Destination</Th>
            <Th>Type</Th>
            <Th>Hops</Th>
          </Tr>
        </Thead>
        <Tbody>
          {pdus.length === 0 ? (
            <Tr>
              <Td colSpan={5} style={{ textAlign: 'center', opacity: 0.5 }}>
                No PDUs yet. Use Ping or Traceroute to generate traffic.
              </Td>
            </Tr>
          ) : (
            pdus.map((pdu, index) => (
              <Tr key={index}>
                <Td style={{ color: pdu.success ? '#4ade80' : '#ef4444' }}>
                  {pdu.success ? 'Success' : 'Failed'}
                </Td>
                <Td>{getDeviceName(pdu.src)}</Td>
                <Td>{getDeviceName(pdu.dst)}</Td>
                <Td>{getActionType(pdu.type)}</Td>
                <Td>{pdu.hops || '-'}</Td>
              </Tr>
            ))
          )}
        </Tbody>
      </Table>
    </div>
  );
};

const NodeFilters = () => {
  const [routersExpanded, setRoutersExpanded] = useState(false);
  const [switchesExpanded, setSwitchesExpanded] = useState(false);
  const [computersExpanded, setComputersExpanded] = useState(false);

  const [routersEnabled, setRoutersEnabled] = useState(true);
  const [switchesEnabled, setSwitchesEnabled] = useState(true);
  const [computersEnabled, setComputersEnabled] = useState(true);

  const routers = [
    { id: 1, name: "Router-1", enabled: true },
  ];

  const switches = [
    { id: 1, name: "Switch-2", enabled: true },
  ];

  const computers = [
    { id: 1, name: "PC-1", enabled: true },
    { id: 2, name: "PC-2", enabled: true },
    { id: 3, name: "PC-3", enabled: true },
  ];

  return (
    <FilterList>
      <FilterItem>
        <FilterLabel onClick={() => setRoutersExpanded(!routersExpanded)} style={{ cursor: 'pointer' }}>
          {routersExpanded ? <ChevronDown size={16} /> : <ChevronRight size={16} />}
          <span>ROUTERS</span>
        </FilterLabel>
        <div style={{ display: 'flex', alignItems: 'center', gap: '20px' }}>
          <FilterCount>{routers.length}</FilterCount>
          <ToggleSwitch active={routersEnabled} onClick={() => setRoutersEnabled(!routersEnabled)} />
        </div>
      </FilterItem>

      {routersExpanded && routers.map(router => (
        <FilterItem key={router.id} style={{ paddingLeft: '55px' }}>
          <FilterLabel>
            <span>{router.name}</span>
          </FilterLabel>
          <ToggleSwitch active={router.enabled} onClick={() => {}} style={{ marginRight: '10px' }}/>
        </FilterItem>
      ))}

      <FilterItem>
        <FilterLabel onClick={() => setSwitchesExpanded(!switchesExpanded)} style={{ cursor: 'pointer' }}>
          {switchesExpanded ? <ChevronDown size={16} /> : <ChevronRight size={16} />}
          <span>SWITCHES</span>
        </FilterLabel>
        <div style={{ display: 'flex', alignItems: 'center', gap: '20px' }}>
          <FilterCount>0</FilterCount>
          <ToggleSwitch active={switchesEnabled} onClick={() => setSwitchesEnabled(!switchesEnabled)} />
        </div>
      </FilterItem>

      {switchesExpanded && switches.map(sw => (
        <FilterItem key={sw.id} style={{ paddingLeft: '55px' }}>
          <FilterLabel>
            <span>{sw.name}</span>
          </FilterLabel>
          <ToggleSwitch active={sw.enabled} onClick={() => {}} style={{ marginRight: '10px' }}/>
        </FilterItem>
      ))}

      <FilterItem>
        <FilterLabel onClick={() => setComputersExpanded(!computersExpanded)} style={{ cursor: 'pointer' }}>
          {computersExpanded ? <ChevronDown size={16} /> : <ChevronRight size={16} />}
          <span>COMPUTERS</span>
        </FilterLabel>
        <div style={{ display: 'flex', alignItems: 'center', gap: '20px' }}>
          <FilterCount>{computers.length}</FilterCount>
          <ToggleSwitch active={computersEnabled} onClick={() => setComputersEnabled(!computersEnabled)} />
        </div>
      </FilterItem>

      {computersExpanded && computers.map(pc => (
        <FilterItem key={pc.id} style={{ paddingLeft: '55px' }}>
          <FilterLabel>
            <span>{pc.name}</span>
          </FilterLabel>
          <ToggleSwitch active={pc.enabled} onClick={() => {}} style={{ marginRight: '10px' }}/>
        </FilterItem>
      ))}
    </FilterList>
  );
};

const LinkFilters = () => {
  return (
    <FilterList>
      <FilterItem>
        <FilterLabel>
          <span>Link Type</span>
        </FilterLabel>
        <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
          <FilterCount>5</FilterCount>
          <ToggleSwitch active={true} onClick={() => {}} />
        </div>
      </FilterItem>
    </FilterList>
  );
};

const RightSidebar = () => {
  const [collapsed, setCollapsed] = useState(false);
  const [activeTab, setActiveTab] = useState('nodes');
  const { pdus } = useNetworkActions();

  return (
    <>
      {!collapsed && (
        <Wrapper>
          <Container>
            <ToggleButton onClick={() => setCollapsed(true)}>
              →
            </ToggleButton>
            <Row>
              <Filter/>
              <h2> FILTERS </h2>
            </Row>
            <TabContainer>
              <Tab 
                active={activeTab === 'nodes'}
                onClick={() => setActiveTab('nodes')}
              >
                <Network size={18} />
                NODES
              </Tab>
              <Tab 
                active={activeTab === 'links'}
                onClick={() => setActiveTab('links')}
              >
                <Link size={18} />
                EDGES
              </Tab>
            </TabContainer>
            <Row style={{ flexDirection: 'column', gap: 0, padding: 0, border: 'none', borderBottom: '2px solid #ededf5' }}>
              {activeTab === 'nodes' && <NodeFilters />}
              {activeTab === 'links' && <LinkFilters />}
            </Row>
            <Row style={{flexDirection: 'column', textAlign: 'center'}}>
              <h2> TRAFFIC BETWEEN NODES </h2>
              <PacketBarChart/>
            </Row>
            <Row style={{flexDirection: 'column', padding: 0, border: 'none', textAlign: 'center'}}>
              <h2>PDU LIST {pdus.length > 0 && `(${pdus.length})`}</h2>
              <PduList />
            </Row>
          </Container>
        </Wrapper>
      )}
      {collapsed && (
        <CollapsedBar onClick={() => setCollapsed(false)}>
          Show Stats Bar
        </CollapsedBar>
      )}
    </>
  )
}

export default RightSidebar;