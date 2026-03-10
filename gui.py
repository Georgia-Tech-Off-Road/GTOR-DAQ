import serial
import serial.tools.list_ports
import tkinter as tk
from tkinter import messagebox, filedialog, ttk

class DAQControlCenter:
    def __init__(self, root):
        self.root = root
        self.root.title("DAQ Box Data Retriever")
        self.ser = None

        frame_conn = tk.LabelFrame(root, text="Connection")
        frame_conn.pack(fill="x", padx=10, pady=5)

        self.port_var = tk.StringVar()
        self.port_menu = ttk.Combobox(frame_conn, textvariable=self.port_var)
        self.port_menu.pack(side="left", padx=5, pady=5)

        tk.Button(frame_conn, text="Refresh", command=self.refreshCommPorts).pack(side="left", padx=2)
        tk.Button(frame_conn, text="Connect", command=self.connectToSerialPort).pack(side="left", padx=2)

        frame_files = tk.LabelFrame(root, text="SD Card Contents")
        frame_files.pack(fill="both", expand=True, padx=10, pady=5)

        self.file_list = tk.Listbox(frame_files, width=50)
        self.file_list.pack(side="left", fill="both", expand=True, padx=5, pady=5)

        btn_frame = tk.Frame(frame_files)
        btn_frame.pack(side="right", fill="y")

        tk.Button(btn_frame, text="Update List", command=self.fetch_list).pack(pady=5)
        tk.Button(btn_frame, text="Download", command=self.download_file).pack(pady=5)

        self.refreshCommPorts()

    def refreshCommPorts(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_menu['values'] = ports
        if ports:
            self.port_menu.current(0)

    def connectToSerialPort(self):
        try:
            port = self.port_var.get()
            self.ser = serial.Serial(port, 115200, timeout=1)
            messagebox.showinfo("Status", f"Connected to {port}")
            self.fetch_list()
        except Exception as e:
            messagebox.showerror("Connection Error", str(e))

    def fetch_list(self):
        if not self.ser:
            return
        self.ser.write(b"LIST\n")
        self.file_list.delete(0, tk.END)

        files = []
        while True:
            line = self.ser.readline().decode('utf-8').strip()
            if "END_LIST" in line or not line:
                break
            if line.startswith("FILE:"):
                display_name = line.replace("FILE:", "")
                files.append(display_name)

        for f in sorted(files, key=str.lower):
            self.file_list.insert(tk.END, f)

    def download_file(self):
        try:
            selected = self.file_list.get(self.file_list.curselection())
            filename = selected.split("|")[0]

            save_path = filedialog.asksaveasfilename(initialfile=filename)
            if not save_path:
                return

            self.ser.write(f"GET {filename}\n".encode())

            with open(save_path, 'wb') as f:
                started = False
                while True:
                    line = self.ser.readline()
                    if b"START_TRANSFER" in line:
                        started = True
                        continue
                    if b"END_TRANSFER" in line:
                        break
                    if started:
                        f.write(line)

            messagebox.showinfo("Success", "File saved successfully.")
        except Exception as e:
            messagebox.showerror("Download Error", "Please select a file first.")

if __name__ == "__main__":
    root = tk.Tk()
    app = DAQControlCenter(root)
    root.mainloop()