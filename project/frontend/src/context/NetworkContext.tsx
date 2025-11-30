import { createContext, useContext, useState, type ReactNode } from 'react';

export type NetworkActionType = 'ping' | 'traceroute';

export interface PDUEntry {
  type: NetworkActionType;
  src: string;
  dst: string;
  success: boolean;
  path?: string[];
  hops?: number;
}

interface NetworkContextType {
  pendingAction: NetworkActionType | null;
  setPendingAction: (action: NetworkActionType | null) => void;
  pdus: PDUEntry[];
  setPdus: React.Dispatch<React.SetStateAction<PDUEntry[]>>;
}

const NetworkContext = createContext<NetworkContextType | undefined>(undefined);

export const NetworkProvider = ({ children }: { children: ReactNode }) => {
  const [pendingAction, setPendingAction] = useState<NetworkActionType | null>(null);
  const [pdus, setPdus] = useState<PDUEntry[]>([]);

  return (
    <NetworkContext.Provider value={{ pendingAction, setPendingAction, pdus, setPdus }}>
      {children}
    </NetworkContext.Provider>
  );
};

export const useNetwork = () => {
  const context = useContext(NetworkContext);
  if (!context) {
    throw new Error('useNetwork must be used within NetworkProvider');
  }
  return context;
};