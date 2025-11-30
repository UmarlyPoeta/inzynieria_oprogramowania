import { useEditor } from "@/context/EditorContext";
import { useNetwork, type NetworkActionType } from "@/context/NetworkContext";

const useNetworkActions = () => {
  const { selectedDeviceIds, setSelectedDeviceIds } = useEditor();
  const { pendingAction, setPendingAction, pdus, setPdus } = useNetwork();

  const runAction = async (type: NetworkActionType, srcId: string, dstId: string) => {
    console.log(`🚀 Running ${type} from ${srcId} to ${dstId}`);
    try {
      const res = await fetch(`/api/${type}`, {
        method: "POST",
        body: JSON.stringify({ src: srcId, dst: dstId }),
        headers: { "Content-Type": "application/json" }
      }).then(r => r.json());

      console.log(`📬 API response:`, res);

      setPdus(prev => [
        ...prev,
        {
          type,
          src: srcId,
          dst: dstId,
          success: res.success,
          path: res.path,
          hops: res.hops
        }
      ]);

      console.log(`✅ PDU added for ${type}: ${srcId} → ${dstId}`);
    } catch (err) {
      console.error(`❌ Network action error: ${type}`, err);
      setPdus(prev => [
        ...prev,
        { type, src: srcId, dst: dstId, success: false }
      ]);
    } finally {
      console.log(`🎯 Resetting pendingAction and selectedDeviceIds`);
      setPendingAction(null);
      setSelectedDeviceIds([]);
    }
  };

  const selectForAction = (id: string) => {
    if (!pendingAction) return;

    console.log(`🖱 Clicked device for pending action: ${id}`);
    const newSelection = [...selectedDeviceIds, id];

    if (newSelection.length === 2) {
      console.log(`✔ Two devices selected, running action:`, newSelection);
      runAction(pendingAction, newSelection[0], newSelection[1]);
    } else {
      console.log(`➕ Added to selection, waiting for second device:`, newSelection);
      setSelectedDeviceIds(newSelection);
    }
  };

  return { pendingAction, setPendingAction, pdus, selectForAction };
};

export default useNetworkActions;
