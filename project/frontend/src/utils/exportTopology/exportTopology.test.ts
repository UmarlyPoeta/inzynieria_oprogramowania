import { ExportTopology } from "@/utils";
import type { Device, Link, Group } from "@/types";
import { vi } from "vitest";

describe("ExportTopology", () => {
  let clickMock: ReturnType<typeof vi.fn>;

  beforeEach(() => {
    clickMock = vi.fn();

    // Mock document.createElement
    vi.stubGlobal("document", {
      ...document,
      createElement: vi.fn(() => ({
        href: "",
        download: "",
        click: clickMock,
      })),
    } as unknown as Document);

    // Mock URL.createObjectURL i revokeObjectURL
    vi.stubGlobal("URL", {
      ...URL,
      createObjectURL: vi.fn(() => "blob:url"),
      revokeObjectURL: vi.fn(),
    } as unknown as typeof URL);
  });

  afterEach(() => {
    vi.restoreAllMocks();
  });

  it("creates a blob and triggers download", () => {
    const devices: Device[] = [{ type: "pc", id: "1", name: "Device 1", x: 0, y: 0 }];
    const links: Link[] = [];
    const groups: Group[] = [];

    ExportTopology(devices, links, groups, "test.json");

    expect(document.createElement).toHaveBeenCalledWith("a");
    expect(clickMock).toHaveBeenCalled();
    expect(URL.createObjectURL).toHaveBeenCalled();
    expect(URL.revokeObjectURL).toHaveBeenCalledWith("blob:url");
  });

  it("uses default filename if not provided", () => {
    ExportTopology([], [], []);

    const a = (document.createElement as unknown as ReturnType<typeof vi.fn>).mock.results[0].value;
    expect(a.download).toBe("network-config.json");
    expect(clickMock).toHaveBeenCalled();
  });
});


