import { renderHook } from "@testing-library/react";
import { fireEvent } from "@testing-library/react";
import { useKeyboardShortcuts } from "@/hooks";
import { vi } from "vitest";

const mockEditor = {
  undo: vi.fn(),
  redo: vi.fn(),
  addDevice: vi.fn(),
  selectDevice: vi.fn(),
  selectAll: vi.fn(),
  deleteDevice: vi.fn(),
  addGroup: vi.fn(),
  moveDeviceToGroup: vi.fn(),
  devices: [
    { id: "1", name: "Device 1", x: 10, y: 20 },
    { id: "2", name: "Device 2", x: 50, y: 50 },
  ],
  selectedDeviceId: "1",
  selectedDeviceIds: ["1", "2"],
};

vi.mock("@/context/EditorContext", () => ({
  useEditor: () => mockEditor,
}));

describe("useKeyboardShortcuts hook", () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it("calls undo on Ctrl+Z", () => {
    renderHook(() => useKeyboardShortcuts());
    fireEvent.keyDown(window, { key: "z", ctrlKey: true });
    expect(mockEditor.undo).toHaveBeenCalled();
  });

  it("calls redo on Ctrl+Y", () => {
    renderHook(() => useKeyboardShortcuts());
    fireEvent.keyDown(window, { key: "y", ctrlKey: true });
    expect(mockEditor.redo).toHaveBeenCalled();
  });

  it("copies selected device on Ctrl+C and pastes it on Ctrl+V", () => {
    renderHook(() => useKeyboardShortcuts());

    fireEvent.keyDown(window, { key: "c", ctrlKey: true }); 
    fireEvent.keyDown(window, { key: "v", ctrlKey: true }); 

    expect(mockEditor.addDevice).toHaveBeenCalled();
    expect(mockEditor.selectDevice).toHaveBeenCalled();

    const newDevice = mockEditor.addDevice.mock.calls[0][0];
    expect(newDevice.name).toContain("(copy)");
    expect(newDevice.x).toBe(40); 
    expect(newDevice.y).toBe(50); 
  });

  it("calls selectAll on Ctrl+A", () => {
    renderHook(() => useKeyboardShortcuts());
    fireEvent.keyDown(window, { key: "a", ctrlKey: true });
    expect(mockEditor.selectAll).toHaveBeenCalled();
  });

  it("duplicates selected device on Ctrl+D", () => {
    renderHook(() => useKeyboardShortcuts());
    fireEvent.keyDown(window, { key: "d", ctrlKey: true });

    expect(mockEditor.addDevice).toHaveBeenCalled();
    expect(mockEditor.selectDevice).toHaveBeenCalled();

    const dupDevice = mockEditor.addDevice.mock.calls[0][0];
    expect(dupDevice.name).toContain("(dup)");
    expect(dupDevice.x).toBe(30); 
    expect(dupDevice.y).toBe(40); 
  });

  it("groups selected devices on Ctrl+G", () => {
    renderHook(() => useKeyboardShortcuts());
    fireEvent.keyDown(window, { key: "g", ctrlKey: true });

    expect(mockEditor.addGroup).toHaveBeenCalled();
    expect(mockEditor.moveDeviceToGroup).toHaveBeenCalledTimes(
      mockEditor.selectedDeviceIds.length
    );

    const groupId = mockEditor.addGroup.mock.calls[0][0].id;
    mockEditor.selectedDeviceIds.forEach((id, index) => {
      expect(mockEditor.moveDeviceToGroup.mock.calls[index]).toEqual([id, groupId]);
    });
  });

  it("deletes selected device on Delete key", () => {
    renderHook(() => useKeyboardShortcuts());
    fireEvent.keyDown(window, { key: "Delete" });

    expect(mockEditor.deleteDevice).toHaveBeenCalledWith("1");
    expect(mockEditor.selectDevice).toHaveBeenCalledWith(undefined);
  });
});
