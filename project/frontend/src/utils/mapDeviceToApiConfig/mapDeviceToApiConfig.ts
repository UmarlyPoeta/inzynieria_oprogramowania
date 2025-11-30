import type { Device, RouterConfig, SwitchConfig, PCConfig } from "@/types";

function mapDeviceToApiConfig(device: Device) {
  const payload: any = {
    name: device.id
  };

  if (!device.config) return payload;

  if (device.type === "pc") {
    const pc = device.config as PCConfig;
    const iface = pc.interfaces?.[0];
    if (iface?.ip) payload.ip = iface.ip;
  }

  if (device.type === "switch") {
    const sw = device.config as SwitchConfig;
    if (sw.managementIp) payload.ip = sw.managementIp;
  }

  if (device.type === "router") {
    const r = device.config as RouterConfig;

    const ifaceWithIp = r.interfaces?.find(i => i.ip);
    if (ifaceWithIp?.ip) payload.ip = ifaceWithIp.ip;

    payload.mtu = 1500;
    payload.queueSize = 50;
  }

  return payload;
}

export default mapDeviceToApiConfig;
