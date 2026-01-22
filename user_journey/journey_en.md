# User Journey

## Persona

**Anna**, a second-year computer science student. She needs to prepare a project for her "Computer Networks" class. This is her first time using the NetSim application. She is tech-savvy but doesn't have time to read long documentation.

## Scenario & Goal

**Scenario:** Anna opens the NetSim application to build a simple network topology consisting of two routers and two hosts.

**Goal:** To successfully configure the network and verify the connection between the hosts using the `ping` command.

## User Journey Map

| Phase | Actions | Thoughts | Emotions | Pain Points | Opportunities |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **1. Discovery** | Logs into the app. Looks around the interface for a starting point. | "Okay, the interface looks clean. Where is the 'New Project' or 'Create Network' button?" | Curious, slightly confused | The main call-to-action button is not immediately visible. It's not clear where to start. | Make the "New Project" button more prominent. Add a welcome screen with a short guide for new users. |
| **2. Building** | Finds the editor. Drags router and host icons onto the canvas. Tries to connect them. | "Great, drag and drop works! But how do I connect these two devices now? Is this icon a cable?" |  Happy ->  Confused | The tool icons (e.g., "cable") are unclear and lack labels. There's no intuitive method for connecting nodes. | Add tooltips to the tool icons. Highlight compatible ports on devices when the "cable" tool is selected. |
| **3. Configuration** | Clicks on a host, looking for a place to enter an IP address. Tries right-clicking. | "Where are the settings? Do I have to right-click? Or maybe double-click?" |  Frustrated | The configuration menu is hidden and requires guessing the interaction (e.g., double-clicking). | Display basic configuration options in a side panel upon a single click on a node. |
| **4. Simulation** | Finds and clicks the "Start Simulation" button. Opens a host's console. Types `ping <ip_address>`. | "Okay, the simulation is running. Let's see if the packets arrive. I wish I could see it." |  Excited, hopeful | There is no visual feedback on the diagram that the simulation is active and that packets are being transmitted. | Add an animation of packets moving along the cables during the simulation. Change the color of links when data is being transmitted. |
| **5. Analysis** | Looks at the `ping` command output in the console. | "Success! The ping got through. But what if it hadn't? Where would I look for the problem?" |  Satisfied, but thoughtful | The application doesn't offer debugging tools in case of a ping failure (e.g., packet tracing). | Add an inspection mode that allows clicking on packets to see their contents and status. Add a panel with simulation event logs. |
