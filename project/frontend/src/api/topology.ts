import ApiClient from "./client";

export type NodeType =
  | "host"
  | "router"
  | "switch"
  | "cloud"
  | "wireless"
  | (string & {});

export interface TopologyNode {
  name: string;
  ip: string;
  type: NodeType;
  mtu?: number;
  queueSize?: number;
}

export type TopologyConnection = [string, string];

export interface TopologyResponse {
  nodes: TopologyNode[];
  connections: TopologyConnection[];
}

export interface CreateNodePayload {
  name: string;
  ip?: string;
  type?: NodeType;
  port?: number;
}

export interface UpdateNodeConfigPayload {
  name: string;
  ip?: string;
  mtu?: number;
  queueSize?: number;
}

export interface ConnectNodesPayload {
  nodeA: string;
  nodeB: string;
  delay?: number;
  bandwidth?: number;
}

export async function fetchTopology() {
  return ApiClient<TopologyResponse>("topology", {
    method: "GET",
    withAuth: true,
  });
}

export async function createNode(payload: CreateNodePayload) {
  return ApiClient<{ result: string }>("node/add", {
    method: "POST",
    body: payload,
    withAuth: true,
  });
}

export async function removeNode(name: string) {
  return ApiClient<{ result: string }>("node/remove", {
    method: "POST",
    body: { name },
    withAuth: true,
  });
}

export async function updateNodeConfig(payload: UpdateNodeConfigPayload) {
  return ApiClient<{ result: string }>("node/config", {
    method: "POST",
    body: payload,
    withAuth: true,
  });
}

export async function connectNodes(payload: ConnectNodesPayload) {
  return ApiClient<{ result: string }>("link/connect", {
    method: "POST",
    body: payload,
    withAuth: true,
  });
}

export async function disconnectNodes(payload: ConnectNodesPayload) {
  return ApiClient<{ result: string }>("link/disconnect", {
    method: "POST",
    body: payload,
    withAuth: true,
  });
}
