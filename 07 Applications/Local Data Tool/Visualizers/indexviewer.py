def indices(filePath, customWindow):
    """
    Read all JSON objects from filePath, collect every unique key across all
    packets, build Index -> column-name mapping, and display those labels
    inside customWindow (below the existing widgets).
    This function is safe to call from a background thread: UI updates are scheduled
    on the main thread using customWindow.after().
    """
    import json
    import tkinter as tk
    import os

    #INSTALLER IMPORTS FINISHED


    # ---------- Parse JSON (can run in background thread) ----------
    try:
        with open(filePath, "r") as f:
            data = json.load(f)

        all_keys = []
        seen = set()
        for obj in data:
            for key in obj:
                if key not in seen:
                    seen.add(key)
                    all_keys.append(key)

        if not all_keys:
            raise ValueError("No JSON keys found in file")

    except Exception as e:
        # Schedule an error label on main thread
        def show_error():
            # create/replace indices_frame
            if hasattr(customWindow, "indices_frame"):
                customWindow.indices_frame.destroy()
            customWindow.indices_frame = tk.Frame(customWindow)
            customWindow.indices_frame.pack(pady=5, fill="x")
            tk.Label(customWindow.indices_frame, text=f"Error reading file: {e}", fg="red").pack(anchor="w", padx=10, pady=2)
        try:
            customWindow.after(0, show_error)
        except Exception:
            pass
        return

    # Create variables mapping
    variables = {f"Index {idx}": col for idx, col in enumerate(all_keys)}

    # ---------- UI builder (runs on main thread) ----------
    def build_ui():
        # remove previous indices frame if present to avoid duplicates
        if hasattr(customWindow, "indices_frame"):
            try:
                customWindow.indices_frame.destroy()
            except Exception:
                pass

        # create a dedicated frame to hold the index labels
        customWindow.indices_frame = tk.Frame(customWindow, relief="flat", bd=0)
        # pack AFTER existing widgets (pack adds at end) so it appears below the Show Indices button
        customWindow.indices_frame.pack(pady=5, fill="x", padx=10)

        # header
        header = tk.Label(customWindow.indices_frame,
                          text=f"Indices for {os.path.basename(filePath)}",
                          font=("Arial", 12, "bold"))
        header.pack(anchor="w", pady=(4, 6))

        # Add each label
        for varName, value in variables.items():
            lbl = tk.Label(customWindow.indices_frame, text=f"{varName}: {value}", anchor="w", justify="left", wraplength=customWindow.winfo_width()-40)
            lbl.pack(anchor="w", padx=6, pady=1)

        # If desired, force a small geometry update so the new content is visible
        try:
            customWindow.update_idletasks()
        except Exception:
            pass

    # Schedule UI build on main thread (safe even when this function was called from a worker thread)
    try:
        customWindow.after(0, build_ui)
    except Exception:
        # If after() fails for some reason, attempt direct call (last resort)
        build_ui()
