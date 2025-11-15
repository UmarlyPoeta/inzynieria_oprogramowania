import { useEffect, useRef } from 'react';
import { useEditor } from "@/context/EditorContext";

/*
  Ctrl + Z                - Undo 
  Ctrl + Y                - Redo 
  Ctrl + C                - Copy 
  Ctrl + V                - Paste 
  Ctrl + A                - Select All 
  Ctrl + D                - Duplicate 
  Ctrl + G                - Group 
  Del                     - Remove            
*/

export default function useKeyboardShortcuts() {
  const editor = useEditor();
  const clipboardRef = useRef<any>(null); // local "clipboard"

  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      const isCtrlOrCmd = e.ctrlKey || e.metaKey;

      if (isCtrlOrCmd) {
        switch (e.key.toLowerCase()) {
          case "z": 
            e.preventDefault();
            editor.undo?.();
            break;

          case "y": 
            e.preventDefault();
            editor.redo?.();
            break;

          case "c": 
            e.preventDefault();
            if (editor.selectedDeviceId) {
              const device = editor.devices.find(d => d.id === editor.selectedDeviceId);
              if (device) {
                clipboardRef.current = { ...device };
              }
            }
            break;

          case "v": 
            e.preventDefault();
            const copied = clipboardRef.current;
            if (copied) {
              const newDevice = {
                ...copied,
                id: Math.random().toString(36).substring(2, 9),
                x: (copied.x ?? 0) + 30,
                y: (copied.y ?? 0) + 30,
                name: copied.name + " (copy)"
              };
              editor.addDevice(newDevice);
              editor.selectDevice(newDevice.id);
            }
            break;

          case "a": 
            e.preventDefault();
            editor.selectAll();
            break;

          case "d": 
            e.preventDefault();
            if (editor.selectedDeviceId) {
              const device = editor.devices.find(d => d.id === editor.selectedDeviceId);
              if (device) {
                const newDevice = {
                  ...device,
                  id: Math.random().toString(36).substring(2, 9),
                  x: (device.x ?? 0) + 20,
                  y: (device.y ?? 0) + 20,
                  name: device.name + " (dup)"
                };
                editor.addDevice(newDevice);
                editor.selectDevice(newDevice.id);
              }
            }
            break;

          case "g": 
            e.preventDefault();
            if (editor.selectedDeviceIds.length >= 1) {
              const newGroup = {
                id: Math.random().toString(36).substring(2, 9),
                name: "Group",
                collapsed: false,
              };

              editor.addGroup(newGroup);

              editor.selectedDeviceIds.forEach(id =>
                editor.moveDeviceToGroup(id, newGroup.id)
              );
            }
            break;
        }
      }

      if (e.key === "Delete") {
        e.preventDefault();
        if (editor.selectedDeviceId) {
          editor.deleteDevice(editor.selectedDeviceId);
          editor.selectDevice(undefined);
        }
      }
    };

    window.addEventListener("keydown", handleKeyDown);
    return () => window.removeEventListener("keydown", handleKeyDown);
  }, [editor]);
}