import type { Device, Link, Group } from "@/types";


const importTopology = (
  file: File,
  resetTopology: () => void,
  addDevice: (d: Device) => void,
  addGroup: (g: Group) => void,
  addLink: (l: Link) => void
) => {
  const reader = new FileReader();
  
  reader.onload = () => {
    try {
      resetTopology();
      
      const data: { devices: Device[]; links: Link[]; groups: Group[] } = JSON.parse(reader.result as string);
      data.groups.forEach(g => addGroup(g));
      data.devices.forEach(d => addDevice(d));
      data.links.forEach(l => addLink(l));

      console.log("Import successful:", data);
    } catch (err) {
      console.error("Failed to parse file:", err);
      alert("Invalid file format");
    }
  };

  reader.onerror = () => {
    console.error("File read error:", reader.error);
    alert("File could not be read");
  };

  reader.readAsText(file);
};

export default importTopology;

