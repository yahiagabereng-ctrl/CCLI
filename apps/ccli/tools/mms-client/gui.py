#!/usr/bin/env python3
"""Small Windows desktop interface for the CCLI TLS/MMS client (WSL backend)."""
import json
import os
from pathlib import Path
import queue
import subprocess
import threading
import tkinter as tk
from tkinter import ttk, filedialog, messagebox, simpledialog
from scl_model import load_scl

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[3]
PROFILE = HERE / "gui.local.json"
DEFAULTS = {
    "host": "192.168.0.1", "port": 3782, "ap_title": "1.1.1.999.1", "ae_qualifier": 12,
    "auth_mode": "certificate",
    "ca": str(ROOT / "apps/ccli/config/tls/root_CA.pem").replace("\\", "/"),
    "tls_cert": str(ROOT / "apps/ccli/config/tls/client_tls.pem").replace("\\", "/"),
    "tls_key": str(ROOT / "apps/ccli/config/tls/client_tsp.key").replace("\\", "/"),
    "binary": str(ROOT / "apps/ccli/build-mms-client/ccli-mms-client").replace("\\", "/"),
}
FC_VALUES = ("MX", "ST", "SP", "SV", "CF", "DC", "SG", "SE", "SR", "OR", "BL", "EX", "CO")
TYPE_VALUES = ("bool", "int32", "int64", "uint32", "float", "double", "string")


def to_wsl(path):
    value = str(path).strip()
    if not value:
        return value
    if len(value) > 2 and value[1:3] == ":/":
        try:
            result = subprocess.run(["wsl.exe", "wslpath", "-a", value], capture_output=True, text=True, check=True)
            return result.stdout.strip()
        except (OSError, subprocess.SubprocessError):
            return "/mnt/" + value[0].lower() + value[2:]
    if len(value) > 2 and value[1:3] == ":\\":
        try:
            result = subprocess.run(["wsl.exe", "wslpath", "-a", value], capture_output=True, text=True, check=True)
            return result.stdout.strip()
        except (OSError, subprocess.SubprocessError):
            normalized = value.replace("\\", "/")
            return "/mnt/" + normalized[0].lower() + normalized[2:]
    return value


class MmsGui(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("CCLI IEC 61850 MMS Client")
        self.geometry("1120x760")
        self.minsize(900, 620)
        self.backend = None
        self.reader = None
        self.events = queue.Queue()
        self.send_lock = threading.Lock()
        self.request_id = 0
        self.connected = False
        self.current_ref = ""
        self.model_meta = {}
        self.remote_items = {}
        self.model_origin = ""
        self.config_vars = {}
        self._build()
        self._load_saved()
        self.after(100, self._poll)
        self.protocol("WM_DELETE_WINDOW", self._close)

    def _build(self):
        style = ttk.Style(self)
        style.theme_use("clam")
        style.configure("Header.TLabel", font=("Segoe UI", 14, "bold"))
        style.configure("Status.TLabel", font=("Segoe UI", 10, "bold"))
        top = ttk.Frame(self, padding=(12, 10))
        top.pack(fill="x")
        ttk.Label(top, text="IEC 61850 MMS Client", style="Header.TLabel").pack(side="left")
        self.status_var = tk.StringVar(value="DISCONNECTED")
        ttk.Label(top, textvariable=self.status_var, style="Status.TLabel").pack(side="left", padx=18)
        self.connect_button = ttk.Button(top, text="Connect", command=self.connect)
        self.connect_button.pack(side="right", padx=(6, 0))
        self.disconnect_button = ttk.Button(top, text="Disconnect", command=self.disconnect, state="disabled")
        self.disconnect_button.pack(side="right")

        tabs = ttk.Notebook(self)
        tabs.pack(fill="both", expand=True, padx=12, pady=(0, 10))
        self.client_tab = ttk.Frame(tabs, padding=10)
        self.config_tab = ttk.Frame(tabs, padding=10)
        tabs.add(self.client_tab, text="Read and Control")
        tabs.add(self.config_tab, text="Configuration and Certificates")
        self._build_client()
        self._build_config()

    def _build_client(self):
        bar = ttk.LabelFrame(self.client_tab, text="Object", padding=8)
        bar.pack(fill="x")
        self.fc_var = tk.StringVar(value="MX")
        self.type_var = tk.StringVar(value="int32")
        self.value_var = tk.StringVar()
        ttk.Label(bar, text="Reference").grid(row=0, column=0, sticky="w")
        self.ref_var = tk.StringVar(value="CCI016_01LD_Plant")
        ttk.Entry(bar, textvariable=self.ref_var).grid(row=0, column=1, sticky="ew", padx=6)
        self.browse_button = ttk.Button(bar, text="Browse", command=self.browse)
        self.browse_button.grid(row=0, column=2, padx=(4, 0))
        ttk.Label(bar, text="FC").grid(row=1, column=0, sticky="w", pady=(6, 0))
        ttk.Combobox(bar, textvariable=self.fc_var, values=FC_VALUES, width=8, state="readonly").grid(row=1, column=1, sticky="w", padx=6, pady=(6, 0))
        ttk.Label(bar, text="Type").grid(row=1, column=2, padx=(12, 0), pady=(6, 0))
        ttk.Combobox(bar, textvariable=self.type_var, values=TYPE_VALUES, width=10, state="readonly").grid(row=1, column=3, sticky="w", padx=6, pady=(6, 0))
        ttk.Label(bar, text="Value").grid(row=1, column=4, padx=(12, 0), pady=(6, 0))
        ttk.Entry(bar, textvariable=self.value_var, width=24).grid(row=1, column=5, sticky="ew", padx=6, pady=(6, 0))
        bar.columnconfigure(1, weight=1)
        bar.columnconfigure(5, weight=2)

        actions = ttk.Frame(self.client_tab, padding=(0, 8))
        actions.pack(fill="x")
        buttons = [("Read", lambda: self.request("read")), ("Watch", self.watch),
                   ("Control info", lambda: self.request("control-info")), ("Write", lambda: self.request("write")),
                   ("Operate", lambda: self.request("operate")), ("Select + cancel", lambda: self.request("select-cancel")),
                   ("Datasets", self.datasets), ("Reports", self.reports)]
        for label, callback in buttons:
            ttk.Button(actions, text=label, command=callback).pack(side="left", padx=(0, 6))

        split = ttk.Panedwindow(self.client_tab, orient="horizontal")
        split.pack(fill="both", expand=True)
        model = ttk.LabelFrame(split, text="Model browser", padding=6)
        output = ttk.LabelFrame(split, text="Activity", padding=6)
        split.add(model, weight=2)
        split.add(output, weight=3)
        model_actions = ttk.Frame(model)
        model_actions.pack(fill="x", pady=(0, 5))
        ttk.Button(model_actions, text="Load SCD/CID…", command=self.load_scl_file).pack(side="left")
        ttk.Button(model_actions, text="Browse MMS", command=self.browse).pack(side="left", padx=5)
        self.model_source_var = tk.StringVar(value="No SCL file loaded")
        ttk.Label(model_actions, textvariable=self.model_source_var).pack(side="left", padx=5)
        tree_holder = ttk.Frame(model)
        tree_holder.pack(fill="both", expand=True)
        self.model_tree = ttk.Treeview(tree_holder, columns=("fc", "type"), show="tree headings", selectmode="browse")
        self.model_tree.heading("#0", text="IED / logical device / node / object / attribute")
        self.model_tree.heading("fc", text="FC")
        self.model_tree.heading("type", text="Type")
        self.model_tree.column("#0", width=330, minwidth=200, stretch=True)
        self.model_tree.column("fc", width=45, minwidth=40, stretch=False, anchor="center")
        self.model_tree.column("type", width=65, minwidth=50, stretch=False)
        scroll = ttk.Scrollbar(tree_holder, orient="vertical", command=self.model_tree.yview)
        horizontal = ttk.Scrollbar(tree_holder, orient="horizontal", command=self.model_tree.xview)
        self.model_tree.configure(yscrollcommand=scroll.set, xscrollcommand=horizontal.set)
        self.model_tree.grid(row=0, column=0, sticky="nsew")
        scroll.grid(row=0, column=1, sticky="ns")
        horizontal.grid(row=1, column=0, sticky="ew")
        tree_holder.rowconfigure(0, weight=1)
        tree_holder.columnconfigure(0, weight=1)
        self.model_tree.bind("<<TreeviewSelect>>", self._select_model_item)
        self.model_tree.bind("<Double-Button-1>", self._enter_model_item)
        self.after(150, lambda: split.sashpos(0, int(self.winfo_width() * 0.46)))
        self.activity = tk.Text(output, wrap="word", font=("Consolas", 9), state="disabled")
        act_scroll = ttk.Scrollbar(output, orient="vertical", command=self.activity.yview)
        self.activity.configure(yscrollcommand=act_scroll.set)
        self.activity.pack(side="left", fill="both", expand=True)
        act_scroll.pack(side="right", fill="y")
        ttk.Label(self.client_tab, text="Write and operate change device state and require a final Yes/No confirmation.").pack(anchor="w", pady=(7, 0))

    def _build_config(self):
        head = ttk.Frame(self.config_tab)
        head.pack(fill="x", pady=(0, 8))
        ttk.Label(head, text="Profile").pack(side="left")
        self.profile_var = tk.StringVar(value="client")
        ttk.Combobox(head, textvariable=self.profile_var, values=("client", "aggregator"), state="readonly", width=14).pack(side="left", padx=6)
        ttk.Button(head, text="Load profile", command=self.load_profile).pack(side="left")
        ttk.Button(head, text="Save configuration", command=self.save_config).pack(side="right")
        ttk.Label(self.config_tab, text="Certificate/key paths are displayed; private-key contents are never shown or copied. The key passphrase is session-only.").pack(anchor="w", pady=(0, 8))

        fields = [
            ("Host", "host"), ("MMS port", "port"), ("Local AP-title", "ap_title"), ("AE qualifier", "ae_qualifier"),
            ("Authentication", "auth_mode"), ("TLS trust CA", "ca"),
            ("MMS server CA (blank = TLS CA)", "mms_ca"), ("TLS client certificate", "tls_cert"),
            ("TLS private key", "tls_key"), ("MMS identity certificate", "identity_cert"), ("MMS identity key", "identity_key"),
            ("Client binary (WSL path)", "binary"),
        ]
        grid = ttk.Frame(self.config_tab)
        grid.pack(fill="x")
        for row, (label, key) in enumerate(fields):
            ttk.Label(grid, text=label).grid(row=row, column=0, sticky="w", pady=3)
            var = tk.StringVar()
            self.config_vars[key] = var
            if key == "auth_mode":
                ttk.Combobox(grid, textvariable=var, values=("certificate", "tls"), state="readonly", width=24).grid(row=row, column=1, sticky="w", padx=8)
            else:
                entry = ttk.Entry(grid, textvariable=var)
                entry.grid(row=row, column=1, sticky="ew", padx=8)
                if key in ("ca", "mms_ca", "tls_cert", "tls_key", "identity_cert", "identity_key"):
                    ttk.Button(grid, text="Browse…", command=lambda k=key: self._pick_path(k)).grid(row=row, column=2)
        row = len(fields)
        ttk.Label(grid, text="Key passphrase").grid(row=row, column=0, sticky="w", pady=3)
        self.passphrase_var = tk.StringVar()
        ttk.Entry(grid, textvariable=self.passphrase_var, show="•").grid(row=row, column=1, sticky="ew", padx=8)
        ttk.Label(grid, text="Not saved to the profile").grid(row=row, column=2, sticky="w")
        grid.columnconfigure(1, weight=1)

        cert_frame = ttk.LabelFrame(self.config_tab, text="Certificates in use", padding=6)
        cert_frame.pack(fill="both", expand=True, pady=(12, 0))
        self.cert_text = tk.Text(cert_frame, wrap="word", height=10, font=("Consolas", 9), state="disabled")
        self.cert_text.pack(fill="both", expand=True)
        self._set_cert_text("Connect to inspect the loaded certificate identities, roles, validity and SHA-256 fingerprints.")

    def _load_saved(self):
        if PROFILE.is_file():
            try:
                data = json.loads(PROFILE.read_text(encoding="utf-8"))
                for key, value in data.items():
                    if key in self.config_vars:
                        self.config_vars[key].set(str(value))
                self.profile_var.set(data.get("profile", "client"))
                return
            except (OSError, json.JSONDecodeError):
                pass
        self.load_profile()

    def load_profile(self):
        name = self.profile_var.get()
        data = dict(DEFAULTS)
        path = HERE / f"{name}.local.json"
        if path.is_file():
            try:
                profile_data = json.loads(path.read_text(encoding="utf-8"))
                data.update(profile_data)
            except (OSError, json.JSONDecodeError) as exc:
                messagebox.showerror("Profile error", str(exc))
                return
        data.setdefault("identity_cert", "")
        data.setdefault("identity_key", "")
        data.setdefault("auth_mode", "certificate")
        data.setdefault("host", "192.168.0.1")
        data.setdefault("port", 3782)
        data.setdefault("ap_title", "1.1.1.999.1")
        data.setdefault("ae_qualifier", 12)
        for key, var in self.config_vars.items():
            var.set(str(data.get(key, "")))
        self._set_cert_text(f"Loaded local profile: {name}\nIdentity certificate and private key paths are shown above. Connect to inspect their certificate details.")

    def save_config(self):
        data = {key: var.get().strip() for key, var in self.config_vars.items()}
        data["profile"] = self.profile_var.get()
        data["port"] = int(data["port"])
        data["ae_qualifier"] = int(data["ae_qualifier"])
        PROFILE.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
        self._log(f"Saved settings to {PROFILE}; key material/passphrase was not saved.")

    def _pick_path(self, key):
        path = filedialog.askopenfilename(title="Select certificate or key file")
        if not path:
            return
        try:
            self.config_vars[key].set(to_wsl(path))
        except (OSError, subprocess.SubprocessError) as exc:
            messagebox.showerror("Path conversion failed", str(exc))

    def _config_payload(self):
        data = {key: var.get().strip() for key, var in self.config_vars.items()}
        data["key_passphrase"] = self.passphrase_var.get()
        data["host"] = data["host"] or "192.168.0.1"
        data["port"] = int(data["port"])
        data["ae_qualifier"] = int(data["ae_qualifier"])
        data["binary"] = to_wsl(data["binary"])
        for key in ("ca", "mms_ca", "tls_cert", "tls_key", "identity_cert", "identity_key"):
            data[key] = to_wsl(data[key])
        return data

    def connect(self):
        try:
            self.save_config()
            config = self._config_payload()
            script = to_wsl(HERE / "gui_backend.py")
            if self.backend:
                self._stop_backend()
            self._log(f"Connecting to {config['host']}:{config['port']} …")
            self.status_var.set("CONNECTING…")
            self.connect_button.configure(state="disabled")
            self.backend = subprocess.Popen(["wsl.exe", "python3", "-u", script], stdin=subprocess.PIPE,
                stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, encoding="utf-8", bufsize=1)
            self.reader = threading.Thread(target=self._read_backend, daemon=True)
            self.reader.start()
            self._send({"action": "connect", "config": config})
        except Exception as exc:
            self.connect_button.configure(state="normal")
            self.status_var.set("DISCONNECTED")
            messagebox.showerror("Connection setup failed", str(exc))

    def _read_backend(self):
        try:
            for line in self.backend.stdout:
                try:
                    self.events.put(json.loads(line))
                except json.JSONDecodeError:
                    self.events.put({"type": "error", "error": "Invalid backend response: " + line.strip()})
        except (OSError, AttributeError) as exc:
            self.events.put({"type": "error", "error": str(exc)})
        self.events.put({"type": "backend_exit"})

    def _send(self, message):
        if not self.backend or self.backend.poll() is not None:
            raise RuntimeError("MMS backend is not running")
        line = json.dumps(message, ensure_ascii=False) + "\n"
        with self.send_lock:
            self.backend.stdin.write(line)
            self.backend.stdin.flush()

    def disconnect(self):
        if not self.backend:
            return
        try:
            self._send({"action": "disconnect"})
        except (OSError, RuntimeError):
            pass
        self.after(100, self._finish_disconnect)

    def _finish_disconnect(self):
        self._stop_backend()
        self.connected = False
        self.status_var.set("DISCONNECTED")
        self.connect_button.configure(state="normal")
        self.disconnect_button.configure(state="disabled")

    def _stop_backend(self):
        process = self.backend
        self.backend = None
        if process:
            try:
                if process.poll() is None:
                    process.stdin.close()
                    process.wait(timeout=4)
            except (OSError, subprocess.TimeoutExpired):
                process.kill()
        self.connected = False

    def _poll(self):
        while True:
            try:
                message = self.events.get_nowait()
            except queue.Empty:
                break
            kind = message.get("type")
            if kind == "connected":
                self.connected = True
                self.status_var.set(message.get("status", "CONNECTED"))
                self.connect_button.configure(state="normal")
                self.disconnect_button.configure(state="normal")
                self.passphrase_var.set("")
                self._display_certificates(message)
                self._log(f"Association established: {message.get('transport')} ({message.get('auth_mode')}).")
            elif kind == "result":
                self.status_var.set(message.get("status", "CONNECTED"))
                self._show_events(message.get("events", []), message.get("code", 1))
            elif kind == "error":
                self._log("ERROR: " + message.get("error", "unknown backend error"))
                diagnostics = message.get("diagnostics", "").strip()
                if diagnostics:
                    self._log(diagnostics)
                status = message.get("status", "DISCONNECTED")
                self.status_var.set(status)
                if status.startswith("DISCONNECTED"):
                    self.connected = False
                    self.connect_button.configure(state="normal")
                    self.disconnect_button.configure(state="disabled")
            elif kind == "disconnected":
                self._finish_disconnect()
            elif kind == "backend_exit":
                if self.backend and self.backend.poll() is not None:
                    self.connected = False
                    self.status_var.set("DISCONNECTED")
                    self.connect_button.configure(state="normal")
                    self.disconnect_button.configure(state="disabled")
            if kind == "result" and message.get("id") is not None:
                self._pending.pop(message["id"], None) if hasattr(self, "_pending") else None
        self.after(100, self._poll)

    def _display_certificates(self, message):
        parts = [f"Transport: {message['transport']} | Auth mode: {message['auth_mode']}"]
        parts.append("Connection profile: " + message.get("wire_profile", "unknown"))
        peer = message.get("server_certificate")
        if peer:
            parts.append(f"\nVerified MMS server\n  Subject: {peer['subject']}\n  Serial: {peer['serial']}\n  SHA-256: {peer['sha256']}\n  Signed time: {peer['signed_time']}\n  Trust CA: {peer['ca']}")
        for label, key in (("MMS application identity", "application_certificate"), ("TLS peer identity", "tls_certificate")):
            cert = message.get(key, {})
            parts.append(f"\n{label}\n  Path: {cert.get('path')}\n  Subject: {cert.get('subject')}\n  Issuer: {cert.get('issuer')}\n  Serial: {cert.get('serial')}\n  Valid: {cert.get('not_before')} → {cert.get('not_after')}\n  SHA-256: {cert.get('sha256')}\n{cert.get('roles', '')}")
        self._set_cert_text("\n".join(parts))

    def _set_cert_text(self, text):
        self.cert_text.configure(state="normal")
        self.cert_text.delete("1.0", "end")
        self.cert_text.insert("1.0", text)
        self.cert_text.configure(state="disabled")

    def _log(self, text):
        self.activity.configure(state="normal")
        self.activity.insert("end", text.rstrip() + "\n")
        self.activity.see("end")
        self.activity.configure(state="disabled")

    def _show_events(self, events, code):
        read_events = [event for event in events if event.get("kind") == "read"]
        if read_events:
            # Do not leave a previous object's value visible when this read fails.
            successful_reads = [event for event in read_events if event.get("error") == 0]
            if not successful_reads:
                first_error = read_events[0].get("error", "?")
                error_text = read_events[0].get("error_text", "read failed")
                self.value_var.set(f"Read failed ({first_error}: {error_text})")
        for event in events:
            kind = event.get("kind", "event")
            ref = event.get("reference", "")
            value = event.get("value", "")
            err = event.get("error", 0)
            status = "OK" if err == 0 else f"ERROR {err}"
            self._log(f"{status:9} {kind:16} {ref} {value} {event.get('error_text', '')}".rstrip())
            if kind == "read" and err == 0:
                self.value_var.set(str(value).strip())
        self._log("PASS: request completed" if code == 0 else f"FAILED: operation result {code}; check the event details")
        if events and events[0].get("kind") == "directory":
            entries = [event.get("value", "") for event in events if event.get("kind") == "entry"]
            reference = events[0].get("reference", "")
            kind = events[0].get("value", "")
            if self.model_origin != "remote":
                self._clear_model_tree()
                self.model_origin = "remote"
            parent = self.remote_items.get(reference)
            if parent is None:
                parent = self.model_tree.insert("", "end", text=reference or "Live MMS model",
                    values=("", "remote"), open=True, tags=("remote",))
                self.remote_items[reference] = parent
            old_children = list(self.model_tree.get_children(parent))
            if old_children:
                removed = set()
                for child in old_children:
                    removed.add(child)
                    removed.update(self._tree_descendants(child))
                for child_ref, child_iid in list(self.remote_items.items()):
                    if child_iid in removed:
                        self.remote_items.pop(child_ref, None)
                self.model_tree.delete(*old_children)
            for entry in entries:
                fc = ""
                name = entry.strip()
                if name.endswith("]") and "[" in name:
                    name, fc = name.rsplit("[", 1)
                    fc = fc[:-1]
                name = name.strip()
                if kind == "devices":
                    child_ref, child_kind = name, "logical-device"
                elif kind == "nodes":
                    child_ref, child_kind = f"{reference}/{name}", "logical-node"
                elif kind == "attributes":
                    child_ref, child_kind = f"{reference}.{name}", "attribute"
                else:
                    child_ref, child_kind = f"{reference}.{name}", "data-object"
                iid = self.model_tree.insert(parent, "end", text=entry, values=(fc, child_kind),
                    open=False, tags=("remote",))
                self.remote_items[child_ref] = iid
            self.model_tree.item(parent, open=True)
            self.current_ref = reference

    def request(self, command, count=1, members=None, reference=None):
        if not self.connected:
            messagebox.showinfo("Not connected", "Connect to the MMS server first.")
            return
        ref = (reference if reference is not None else self.ref_var.get()).strip()
        if command == "read":
            self.value_var.set("Reading…")
        value = self.value_var.get()
        if command in ("write", "operate", "select-cancel"):
            action = "write" if command == "write" else "operate" if command == "operate" else "select + cancel"
            target = (f"{action.title()} this live value?\n\nObject: {ref}\n"
                      f"Value: {value!r} ({self.type_var.get()})\nFC: {self.fc_var.get()}")
            if not messagebox.askyesno("Confirm live change", target, default=messagebox.NO):
                self._log(f"Cancelled {action} {ref}.")
                return
        self.request_id += 1
        payload = {"action": "request", "id": self.request_id, "command": command,
                   "reference": ref, "fc": self.fc_var.get(), "count": count,
                   "value_type": self.type_var.get(), "value": value, "members": members}
        try:
            self._send(payload)
            self._log(f"Sent {command} {ref}".rstrip())
        except Exception as exc:
            messagebox.showerror("Request failed", str(exc))

    def browse(self):
        if not self.connected:
            messagebox.showinfo("Not connected", "Connect to the MMS server first.")
            return
        reference = self.ref_var.get().strip()
        if self.model_origin != "remote":
            self._clear_model_tree()
            self.model_origin = "remote"
        if reference not in self.remote_items:
            root = self.model_tree.insert("", "end", text=reference or "Live MMS model",
                values=("", "remote"), open=True, tags=("remote",))
            self.remote_items[reference] = root
        else:
            self.model_tree.item(self.remote_items[reference], open=True)
            self.model_tree.selection_set(self.remote_items[reference])
        self.model_source_var.set("Live MMS model")
        self.request("browse")

    def _clear_model_tree(self):
        self.model_tree.delete(*self.model_tree.get_children(""))
        self.model_meta.clear()
        self.remote_items.clear()

    def _tree_descendants(self, iid):
        descendants = []
        for child in self.model_tree.get_children(iid):
            descendants.append(child)
            descendants.extend(self._tree_descendants(child))
        return descendants

    def load_scl_file(self):
        path = filedialog.askopenfilename(title="Select IEC 61850 SCD/CID file",
            filetypes=(("IEC 61850 SCL files", "*.scd *.cid *.icd *.xml"), ("All files", "*.*")))
        if not path:
            return
        try:
            roots = load_scl(path)
        except Exception as exc:
            messagebox.showerror("SCL file could not be loaded", str(exc))
            return
        self._clear_model_tree()
        self.model_origin = "scl"
        for node in roots:
            self._insert_scl_node("", node)
        first = self.model_tree.get_children("")
        if first:
            self.model_tree.item(first[0], open=True)
            devices = self.model_tree.get_children(first[0])
            if devices:
                self.model_tree.item(devices[0], open=True)
        self.model_source_var.set(Path(path).name)
        self._log(f"Loaded SCL model from {path}.")

    def _insert_scl_node(self, parent, node):
        iid = self.model_tree.insert(parent, "end", text=node["label"],
            values=(node.get("fc", ""), node.get("type", "")), open=False, tags=("scl",))
        self.model_meta[iid] = node
        for child in node.get("children", []):
            self._insert_scl_node(iid, child)
        return iid

    def _select_model_item(self, _event=None):
        selection = self.model_tree.selection()
        if not selection:
            return
        iid = selection[0]
        meta = self.model_meta.get(iid)
        if meta is not None:
            reference = meta.get("reference", "")
            if reference:
                self.ref_var.set(reference)
                if meta.get("kind") == "attribute":
                    # A value belongs to one exact MMS attribute; clear it when
                    # navigating to another SCL node so an old read is not mistaken
                    # for the selected node's value.
                    self.value_var.set("")
            fc = meta.get("fc")
            if fc in FC_VALUES:
                self.fc_var.set(fc)
            return
        reference = next((ref for ref, item in self.remote_items.items() if item == iid), "")
        if reference:
            self.ref_var.set(reference)
            values = self.model_tree.item(iid, "values")
            if values and values[0] in FC_VALUES:
                self.fc_var.set(values[0])

    def _enter_model_item(self, _event=None):
        selected = self.model_tree.selection()
        if not selected:
            return
        iid = selected[0]
        if iid in self.model_meta:
            return
        reference = next((ref for ref, item in self.remote_items.items() if item == iid), "")
        if not reference:
            return
        self.ref_var.set(reference)
        self.browse()

    def watch(self):
        count = simpledialog.askinteger("Watch", "Number of readings (1–3600):", initialvalue=10, minvalue=1, maxvalue=3600)
        if count:
            self.request("watch", count=count)

    def datasets(self):
        choice = simpledialog.askstring("Datasets", "Enter l (list), m (members), or r (read):", initialvalue="l")
        if choice == "l":
            ref = simpledialog.askstring("Datasets", "Logical node reference:", initialvalue="CCI016_01LD_Plant/LLN0")
            if ref:
                self.request("datasets", reference=ref)
        elif choice in ("m", "r"):
            command = "dataset-members" if choice == "m" else "dataset-read"
            ref = simpledialog.askstring("Dataset", "Dataset reference:", initialvalue="CCI016_01LD_Plant/LLN0.DS_R_PdC_Mis4sec")
            if ref:
                self.request(command, reference=ref)

    def reports(self):
        mode = simpledialog.askstring("Reports", "Enter l (list report blocks), r (read RCB), or s (subscribe):", initialvalue="l")
        if mode == "l":
            ref = simpledialog.askstring("Reports", "Logical node reference:", initialvalue="CCI016_01LD_Plant/LLN0")
            if ref:
                self.request("report-blocks", reference=ref)
        elif mode in ("r", "s"):
            ref = simpledialog.askstring("Reports", "Report control block reference:", initialvalue="CCI016_01LD_Plant/LLN0.RP.urcb_PdC_Mis4sec")
            if ref:
                self.request("rcb-read" if mode == "r" else "reports", count=10, reference=ref)

    def _close(self):
        if self.backend:
            self._stop_backend()
        self.destroy()


if __name__ == "__main__":
    MmsGui().mainloop()
