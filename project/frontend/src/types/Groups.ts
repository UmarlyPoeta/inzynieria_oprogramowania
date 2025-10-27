export interface Group {
  id: string;
  name: string;
  collapsed: boolean;
  parentGroupId?: string;
}