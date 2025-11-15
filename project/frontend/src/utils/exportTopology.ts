import type { Device, Link, Group } from "@/types";

function exportAndDownloadConfig (devices: Device[], links: Link[], groups: Group[], filename = "network-config.json") {
  const json = JSON.stringify({ devices, links, groups }, null, 2);
  const blob = new Blob([json], { type: "application/json" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = filename;
  a.click();
  URL.revokeObjectURL(url);
};

export default exportAndDownloadConfig;