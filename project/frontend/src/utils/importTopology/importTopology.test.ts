import { describe, it, expect, beforeEach, vi } from 'vitest';
import { ImportTopology } from '@/utils';
import type { Device, Group } from '@/types';

describe('ImportTopology', () => {
  let resetTopology: ReturnType<typeof vi.fn>;
  let addDevice: ReturnType<typeof vi.fn>;
  let addGroup: ReturnType<typeof vi.fn>;
  let file: File;

  beforeEach(() => {
    resetTopology = vi.fn();
    addDevice = vi.fn();
    addGroup = vi.fn();
    file = new File([], 'topology.json');
    vi.stubGlobal('alert', vi.fn());
  });

  const mockFileReader = (options: { result?: string; fail?: boolean }) => {
    const { result, fail } = options;
    function MockFileReader(this: any) {
      this.onload = null;
      this.onerror = null;
      this.result = result;
      this.readAsText = () => {
        if (fail) {
          this.onerror?.({} as ProgressEvent<FileReader>);
        } else {
          this.onload?.({} as ProgressEvent<FileReader>);
        }
      };
    }
    vi.stubGlobal('FileReader', MockFileReader as any);
  };

  it('imports devices and groups correctly', () => {
    const data = {
      devices: [{ id: 'd1', name: 'Device 1' } as Device],
      groups: [{ id: 'g1', name: 'Group 1' } as Group],
      links: [],
    };
    mockFileReader({ result: JSON.stringify(data) });
    ImportTopology(file, resetTopology, addDevice, addGroup, () => {});
    expect(resetTopology).toHaveBeenCalled();
    expect(addGroup).toHaveBeenCalledWith(data.groups[0]);
    expect(addDevice).toHaveBeenCalledWith(data.devices[0]);
  });

  it('alerts on invalid JSON', () => {
    mockFileReader({ result: 'invalid json' });
    ImportTopology(file, resetTopology, addDevice, addGroup, () => {});
    expect(alert).toHaveBeenCalledWith('Invalid file format');
    expect(resetTopology).toHaveBeenCalled();
  });

  it('alerts on file read error', () => {
    mockFileReader({ fail: true });
    ImportTopology(file, resetTopology, addDevice, addGroup, () => {});
    expect(alert).toHaveBeenCalledWith('File could not be read');
    expect(resetTopology).not.toHaveBeenCalled();
  });
});










