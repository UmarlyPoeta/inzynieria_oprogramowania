export interface Link {
  id: string;
  from: string;
  to: string;
  type?: "copper" | "fiber" | "serial"; 
  fromPort?: string; 
  toPort?: string;   
}
