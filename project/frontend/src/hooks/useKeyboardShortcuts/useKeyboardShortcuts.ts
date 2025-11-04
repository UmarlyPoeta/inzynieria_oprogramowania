import { useEffect } from 'react';

interface KeyboardShortcutsOptions  {
    onUndo?: () => void;
    onRedo?: () => void;
    onSave?: () => void;
    onDelete?: () => void;
    onCopy?: () => void;
}

/**
 * Hook obsługujący skróty klawiszowe edytora:
 *  - Ctrl+Z → cofnięcie akcji
 *  - Ctrl+C → skopiowanie
 *  - Ctrl+Delete → usunięcie wybranego elementu
 *  + blokada domyślnych akcji przeglądarki (Ctrl+R, Ctrl+S)
 */

export function useKeyboardShortcuts({
  onUndo,
  onRedo,
  onSave,
  onDelete,
  onCopy
}: KeyboardShortcutsOptions) {
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      if (e.ctrlKey || e.metaKey) {
        switch (e.key.toLowerCase()) {
          case "z":
            e.preventDefault();
            onUndo?.();
            break;
          case "y":
            e.preventDefault();
            onRedo?.();
            break;
          case "s":
            e.preventDefault(); // blokuje domyślny "Save page"
            onSave?.();
            break;
          case "r":
            e.preventDefault(); // blokuje odświeżenie strony
            break;
        }
      }

      // Klawisz Delete
      if (e.key === "Delete") {
        e.preventDefault();
        onDelete?.();
      }
    };

    window.addEventListener("keydown", handleKeyDown);
    return () => window.removeEventListener("keydown", handleKeyDown);
  }, [onUndo, onRedo, onSave, onDelete]);
}