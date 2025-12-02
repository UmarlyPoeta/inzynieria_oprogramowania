import { useState } from "react";
import { 
  X, 
  Building2, 
  Zap,
  MapPin,
  ChevronLeft,
  Network,
  Router as RouterIcon,
  Cpu,
  Monitor
} from "lucide-react";
import {
  SidebarContainer,
  SidebarHeader,
  CompanyId,
  CloseButton,
  CompanyNameSection,
  CompanyName,
  CompanyBadge,
  SearchSection,
  SearchInput,
  PropertiesSection,
  PropertyItem,
  PropertyLabel,
  PropertyValue,
  IconWrapper,
  CollapsedSidebar,
  ExpandButton,
  HeaderActions,
  HeaderIconButton,
  SectionTitle,
  NoDeviceMessage
} from './LeftSidebar.styled';
import { useNetworkActions } from "@/hooks";
import { useEditor } from "@/context/EditorContext";
import type { RouterConfig, SwitchConfig, PCConfig } from "@/types";

const LeftSidebar = () => {
  const [sidebarCollapsed, setSidebarCollapsed] = useState(false);
  const { setPendingAction } = useNetworkActions();
  const { devices, selectedDeviceId } = useEditor();

  const selectedDevice = devices.find(d => d.id === selectedDeviceId);

  const getDeviceIcon = (type: string) => {
    switch (type) {
      case "router": return <RouterIcon size={20} color="#f37c46ff" />;
      case "switch": return <Network size={20} color="#f37c46ff" />;
      case "pc": return <Monitor size={20} color="#f37c46ff" />;
      default: return <Cpu size={20} color="#f37c46ff" />;
    }
  };

  const getDeviceTypeName = (type: string) => {
    switch (type) {
      case "router": return "ROUTER";
      case "switch": return "SWITCH";
      case "pc": return "PC";
      default: return "DEVICE";
    }
  };

  const renderRouterConfig = (config: RouterConfig) => (
    <>
      <SectionTitle>Basic Configuration</SectionTitle>
      <PropertyItem>
        <PropertyLabel>Hostname</PropertyLabel>
        <PropertyValue>{config.hostname}</PropertyValue>
      </PropertyItem>
      {config.domainName && (
        <PropertyItem>
          <PropertyLabel>Domain Name</PropertyLabel>
          <PropertyValue>{config.domainName}</PropertyValue>
        </PropertyItem>
      )}

      <SectionTitle>Interfaces</SectionTitle>
      {config.interfaces.map((iface, idx) => (
        <div key={idx} style={{ marginBottom: '16px', paddingLeft: '8px', borderLeft: '2px solid #e5e7eb' }}>
          <PropertyItem>
            <PropertyLabel>
              <IconWrapper><Network size={16} /></IconWrapper>
              {iface.name}
            </PropertyLabel>
            <PropertyValue>Status: {iface.status}</PropertyValue>
          </PropertyItem>
          {iface.ip && (
            <PropertyItem>
              <PropertyLabel>IP Address</PropertyLabel>
              <PropertyValue>{iface.ip} / {iface.subnet}</PropertyValue>
            </PropertyItem>
          )}
          {iface.dhcpEnabled && (
            <PropertyItem>
              <PropertyLabel>DHCP Client</PropertyLabel>
              <PropertyValue>Enabled</PropertyValue>
            </PropertyItem>
          )}
          {iface.natInside && <PropertyValue>NAT Inside</PropertyValue>}
          {iface.natOutside && <PropertyValue>NAT Outside</PropertyValue>}
        </div>
      ))}

      <SectionTitle>Routing</SectionTitle>
      <PropertyItem>
        <PropertyLabel>Protocol</PropertyLabel>
        <PropertyValue>{config.routingProtocol}</PropertyValue>
      </PropertyItem>

      {config.routingProtocol === "OSPF" && config.ospf && (
        <>
          <PropertyItem>
            <PropertyLabel>OSPF Process ID</PropertyLabel>
            <PropertyValue>{config.ospf.processId}</PropertyValue>
          </PropertyItem>
          <PropertyItem>
            <PropertyLabel>OSPF Area</PropertyLabel>
            <PropertyValue>{config.ospf.area}</PropertyValue>
          </PropertyItem>
        </>
      )}

      {config.routingProtocol === "RIP" && config.rip && (
        <PropertyItem>
          <PropertyLabel>RIP Version</PropertyLabel>
          <PropertyValue>{config.rip.version}</PropertyValue>
        </PropertyItem>
      )}

      {config.routingProtocol === "EIGRP" && config.eigrp && (
        <PropertyItem>
          <PropertyLabel>EIGRP AS Number</PropertyLabel>
          <PropertyValue>{config.eigrp.asNumber}</PropertyValue>
        </PropertyItem>
      )}

      {config.routingProtocol === "BGP" && config.bgp && (
        <PropertyItem>
          <PropertyLabel>BGP AS Number</PropertyLabel>
          <PropertyValue>{config.bgp.asNumber}</PropertyValue>
        </PropertyItem>
      )}

      {config.staticRoutes.length > 0 && (
        <>
          <SectionTitle>Static Routes</SectionTitle>
          {config.staticRoutes.map((route, idx) => (
            <PropertyItem key={idx}>
              <PropertyLabel>Route {idx + 1}</PropertyLabel>
              <PropertyValue>{route.destination}/{route.subnet} via {route.gateway}</PropertyValue>
            </PropertyItem>
          ))}
        </>
      )}

      <SectionTitle>NAT Configuration</SectionTitle>
      <PropertyItem>
        <PropertyLabel>NAT Enabled</PropertyLabel>
        <PropertyValue>{config.natEnabled ? "Yes" : "No"}</PropertyValue>
      </PropertyItem>
      {config.natEnabled && config.natType && (
        <PropertyItem>
          <PropertyLabel>NAT Type</PropertyLabel>
          <PropertyValue>{config.natType}</PropertyValue>
        </PropertyItem>
      )}

      <SectionTitle>Firewall</SectionTitle>
      <PropertyItem>
        <PropertyLabel>Firewall Enabled</PropertyLabel>
        <PropertyValue>{config.firewallEnabled ? "Yes" : "No"}</PropertyValue>
      </PropertyItem>
      {config.firewallEnabled && config.firewallRules && config.firewallRules.length > 0 && (
        <PropertyItem>
          <PropertyLabel>Rules</PropertyLabel>
          <PropertyValue>{config.firewallRules.length} rule(s)</PropertyValue>
        </PropertyItem>
      )}

      <SectionTitle>DHCP Server</SectionTitle>
      <PropertyItem>
        <PropertyLabel>DHCP Enabled</PropertyLabel>
        <PropertyValue>{config.dhcpEnabled ? "Yes" : "No"}</PropertyValue>
      </PropertyItem>
      {config.dhcpEnabled && config.dhcpPools && config.dhcpPools.length > 0 && (
        <>
          {config.dhcpPools.map((pool, idx) => (
            <div key={idx} style={{ marginBottom: '12px', paddingLeft: '8px' }}>
              <PropertyItem>
                <PropertyLabel>Pool: {pool.name}</PropertyLabel>
                <PropertyValue>{pool.network}/{pool.subnet}</PropertyValue>
              </PropertyItem>
              <PropertyItem>
                <PropertyLabel>Range</PropertyLabel>
                <PropertyValue>{pool.rangeStart} - {pool.rangeEnd}</PropertyValue>
              </PropertyItem>
              <PropertyItem>
                <PropertyLabel>Gateway</PropertyLabel>
                <PropertyValue>{pool.defaultGateway}</PropertyValue>
              </PropertyItem>
            </div>
          ))}
        </>
      )}

      <SectionTitle>VPN</SectionTitle>
      <PropertyItem>
        <PropertyLabel>VPN Enabled</PropertyLabel>
        <PropertyValue>{config.vpnEnabled ? "Yes" : "No"}</PropertyValue>
      </PropertyItem>

      <SectionTitle>QoS</SectionTitle>
      <PropertyItem>
        <PropertyLabel>QoS Enabled</PropertyLabel>
        <PropertyValue>{config.qosEnabled ? "Yes" : "No"}</PropertyValue>
      </PropertyItem>
    </>
  );

  const renderSwitchConfig = (config: SwitchConfig) => (
    <>
      <SectionTitle>Basic Configuration</SectionTitle>
      <PropertyItem>
        <PropertyLabel>Hostname</PropertyLabel>
        <PropertyValue>{config.hostname}</PropertyValue>
      </PropertyItem>
      {config.description && (
        <PropertyItem>
          <PropertyLabel>Description</PropertyLabel>
          <PropertyValue>{config.description}</PropertyValue>
        </PropertyItem>
      )}
      {config.managementIp && (
        <PropertyItem>
          <PropertyLabel>Management IP</PropertyLabel>
          <PropertyValue>{config.managementIp}</PropertyValue>
        </PropertyItem>
      )}

      <SectionTitle>VLANs</SectionTitle>
      {config.vlans.map((vlan, idx) => (
        <PropertyItem key={idx}>
          <PropertyLabel>VLAN {vlan.id}</PropertyLabel>
          <PropertyValue>{vlan.name}</PropertyValue>
        </PropertyItem>
      ))}

      <SectionTitle>STP</SectionTitle>
      <PropertyItem>
        <PropertyLabel>Spanning Tree Protocol</PropertyLabel>
        <PropertyValue>{config.stpEnabled ? "Enabled" : "Disabled"}</PropertyValue>
      </PropertyItem>

      <SectionTitle>Interfaces</SectionTitle>
      {config.interfaces.map((iface, idx) => (
        <div key={idx} style={{ marginBottom: '12px', paddingLeft: '8px', borderLeft: '2px solid #e5e7eb' }}>
          <PropertyItem>
            <PropertyLabel>
              <IconWrapper><Network size={16} /></IconWrapper>
              {iface.name}
            </PropertyLabel>
            <PropertyValue>Status: {iface.status}</PropertyValue>
          </PropertyItem>
          {iface.vlanId && (
            <PropertyItem>
              <PropertyLabel>VLAN</PropertyLabel>
              <PropertyValue>{iface.vlanId}</PropertyValue>
            </PropertyItem>
          )}
        </div>
      ))}
    </>
  );

  const renderPCConfig = (config: PCConfig) => (
    <>
      <SectionTitle>Basic Configuration</SectionTitle>
      <PropertyItem>
        <PropertyLabel>Hostname</PropertyLabel>
        <PropertyValue>{config.hostname}</PropertyValue>
      </PropertyItem>
      {config.description && (
        <PropertyItem>
          <PropertyLabel>Description</PropertyLabel>
          <PropertyValue>{config.description}</PropertyValue>
        </PropertyItem>
      )}

      <SectionTitle>Network Interfaces</SectionTitle>
      {config.interfaces.map((iface, idx) => (
        <div key={idx} style={{ marginBottom: '16px', paddingLeft: '8px', borderLeft: '2px solid #e5e7eb' }}>
          <PropertyItem>
            <PropertyLabel>
              <IconWrapper><Network size={16} /></IconWrapper>
              {iface.name}
            </PropertyLabel>
          </PropertyItem>
          <PropertyItem>
            <PropertyLabel>IP Address</PropertyLabel>
            <PropertyValue>{iface.ip}</PropertyValue>
          </PropertyItem>
          <PropertyItem>
            <PropertyLabel>Subnet Mask</PropertyLabel>
            <PropertyValue>{iface.subnet}</PropertyValue>
          </PropertyItem>
          <PropertyItem>
            <PropertyLabel>Gateway</PropertyLabel>
            <PropertyValue>{iface.gateway}</PropertyValue>
          </PropertyItem>
          {iface.dns && (
            <PropertyItem>
              <PropertyLabel>DNS</PropertyLabel>
              <PropertyValue>{iface.dns}</PropertyValue>
            </PropertyItem>
          )}
        </div>
      ))}
    </>
  );

  return (
    <>
      {!sidebarCollapsed ? (
        <SidebarContainer>
          <SidebarHeader>
            <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
              {selectedDevice ? getDeviceIcon(selectedDevice.type) : <Building2 size={20} color="#7c3aed" />}
              <CompanyId>{selectedDevice ? selectedDevice.id : "No Device Selected"}</CompanyId>
            </div>
            <HeaderActions>
              <HeaderIconButton 
                title="Ping" 
                onClick={() => {
                  console.log("🖱 Ping button clicked");
                  setPendingAction('ping'); 
                  console.log("⚡ pendingAction set to 'ping'");
                }}
              >
                <Zap />
              </HeaderIconButton>

              <HeaderIconButton 
                title="Traceroute" 
                onClick={() => setPendingAction('traceroute')}
              >
                <MapPin />
              </HeaderIconButton>

              <CloseButton onClick={() => setSidebarCollapsed(true)}>
                <X size={20} />
              </CloseButton>
            </HeaderActions>
          </SidebarHeader>

          {selectedDevice ? (
            <>
              <CompanyNameSection>
                <CompanyName>{selectedDevice.name || selectedDevice.type}</CompanyName>
                <CompanyBadge>{getDeviceTypeName(selectedDevice.type)}</CompanyBadge>
              </CompanyNameSection>

              <SearchSection>
                <SearchInput type="text" placeholder="Search in the properties" />
              </SearchSection>

              <PropertiesSection>
                {selectedDevice.config && selectedDevice.type === "router" && renderRouterConfig(selectedDevice.config as RouterConfig)}
                {selectedDevice.config && selectedDevice.type === "switch" && renderSwitchConfig(selectedDevice.config as SwitchConfig)}
                {selectedDevice.config && selectedDevice.type === "pc" && renderPCConfig(selectedDevice.config as PCConfig)}
              </PropertiesSection>
            </>
          ) : (
            <NoDeviceMessage>
              <p>No device selected</p>
              <p style={{ fontSize: '14px', color: '#6b7280', marginTop: '8px' }}>
                Select a device to view its configuration
              </p>
            </NoDeviceMessage>
          )}
        </SidebarContainer>
      ) : (
        <CollapsedSidebar>
          <ExpandButton onClick={() => setSidebarCollapsed(false)}>
            <ChevronLeft size={24} style={{ transform: 'rotate(180deg)' }} />
          </ExpandButton>
        </CollapsedSidebar>
      )}
    </>
  );
};

export default LeftSidebar;