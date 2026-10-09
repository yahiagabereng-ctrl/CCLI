/* CCLI zone dashboard — read-only view of DSO (Eth_A), operator (Eth_B) and plant zones.
 * Data: ccli-bench?action=status (zones block from ccli r33+), ?action=events, plant_map.json.
 * Never issues control requests: DSO Operate is MMS-only (REQ-OP-CLOUD-002). */
(function () {
  "use strict";

  const STATUS_POLL_MS = 2500;
  const EVENTS_POLL_MS = 10000;

  const $ = (id) => document.getElementById(id);
  const state = { status: null, events: [], map: null, demoTick: 0 };

  function esc(v) {
    return String(v === undefined || v === null ? "" : v)
      .replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;")
      .replace(/"/g, "&quot;").replace(/'/g, "&#39;");
  }
  function num(v, d) {
    if (typeof v !== "number" || !isFinite(v)) return "—";
    return v.toFixed(d === undefined ? 1 : d);
  }
  function badge(text, cls) { return `<span class="badge ${cls}">${esc(text)}</span>`; }
  function yesNo(b) { return b ? badge("YES", "b-ok") : badge("NO", "b-off"); }
  function fmtTime(ms) {
    if (!ms) return "—";
    const d = new Date(ms);
    return isNaN(d.getTime()) ? "—" : d.toLocaleTimeString();
  }
  function kv(el, rows) {
    el.innerHTML = rows.map(([k, v]) => `<dt>${esc(k)}</dt><dd>${v}</dd>`).join("");
  }
  function statusBadge(s) {
    const map = { IMPL: "b-ok", PART: "b-warn", STUB: "b-off", DEFERRED: "b-off", CID_GAP: "b-warn" };
    return badge(s || "—", map[s] || "b-info");
  }

  /* ---------- API ---------- */
  function apiBase() {
    const v = $("apiBase").value.trim();
    return v || "/cgi-bin/ccli-bench";
  }
  async function getJson(url) {
    const r = await fetch(url, { cache: "no-store" });
    if (!r.ok) throw new Error(`HTTP ${r.status} ${url}`);
    return r.json();
  }

  /* ---------- Demo data (clearly labelled, off-device preview only) ---------- */
  function demoStatus() {
    const t = ++state.demoTick;
    const p = 42 + 8 * Math.sin(t / 6);
    const q = 6 + 2 * Math.cos(t / 7);
    return {
      ts_ms: Date.now(), p_kw: p, q_kvar: q, quality: "good",
      pf2_reason: p > 46 ? "above_enter" : "below_release",
      curtailment_commanded: p > 46, permissive_ok: true, annex_m_trip_active: false,
      do_curtail_on: p > 46, dso_applied: true,
      derived: { smax_calc_kva: 206.15, smax_used_kva: 210, p_wsd_kw: 42, p_wlim_kw: 0, p_w110_kw: 0,
                 p_effective_kw: 42, pf2_enter_kw: 46.2, pf2_release_kw: 37.8 },
      pf2_debounce_s: 30, stale_data_s: 10,
      zones: {
        dso: { enabled: true, running: true, bind: "192.168.10.1", port: 3782, tls: true, clients: 1,
               gnss_fix: false, comms_loss_fallback_s: 15, setpoint_min_interval_s: 3, cmd_seen: true,
               wlim_active: false, wlim_pct: 0, wsd_active: true, wsd_pct: 20, varsd_active: true,
               varsd_pct: -5, pfsp_active: false, pfsp_cosphi: 1, pfsp_generation: true, ppv_kv: 20 },
        operator: { enabled: true, running: true, bind: "192.168.1.130", port: 2404, tls: true,
                    clients: t % 20 < 12 ? 1 : 0, common_address: 1, periodic_s: 4, comms_loss_fallback_s: 60,
                    mode: "monitor_only", allow_commands: false, allow_gi: true, allow_clock_sync: false,
                    ioa: [ { name: "TotW", ioa: 1001, enabled: true, published: true, value: p, unit: "kW" },
                           { name: "TotVAr", ioa: 1002, enabled: false, published: false, value: q, unit: "kVAr" },
                           { name: "PPV", ioa: 1003, enabled: false, published: false, value: 20, unit: "kV" } ] },
        plant: { modbus_backend: "rtu", modbus_device: "/dev/ttyS1", modbus_host: "", modbus_tcp_port: 502,
                 modbus_baud: 9600, modbus_slave_id: 1, poll_ms: 4000, link: "up",
                 last_exchange_ms: Date.now() - 1200, last_success: true, last_error: "",
                 goose_enabled: false, goose_interface: "lan3", goose_publish: true }
      },
      _demo_inverters: { INV01: { p_kw: 21.4 + Math.sin(t / 5), status: 512 },
                         INV02: { p_kw: 19.8 + Math.cos(t / 5), status: 512 } }
    };
  }
  function demoEvents() {
    const now = new Date();
    const ts = (s) => new Date(now.getTime() - s * 1000).toISOString().replace("T", " ").slice(0, 19).replace(/-/g, "/");
    return [
      { ts_o14: ts(4), type: "mms", detail: "varsd_operate mod=1 pct=-5.000000 q_kvar=-10.500000 result=ok" },
      { ts_o14: ts(30), type: "iec104", detail: "operator_asdu_reject type=45 ioa=3001 result=reject" },
      { ts_o14: ts(64), type: "mms", detail: "wsd_update" },
      { ts_o14: ts(90), type: "mms", detail: "client_connect" },
      { ts_o14: ts(120), type: "modbus", detail: "link_up" },
      { ts_o14: ts(121), type: "security", detail: "service_monitor:started" },
      { ts_o14: ts(122), type: "system", detail: "power_on:boot" }
    ];
  }

  /* ---------- Fetch ---------- */
  async function refreshStatus() {
    const err = $("errBox");
    try {
      state.status = $("demoMode").checked ? demoStatus() : await getJson(`${apiBase()}?action=status`);
      if (state.status && state.status.error) throw new Error(state.status.error);
      err.style.display = "none";
      $("connBadge").className = "badge " + ($("demoMode").checked ? "b-warn" : "b-ok");
      $("connBadge").textContent = $("demoMode").checked ? "DEMO" : "LIVE";
      $("lastUpdate").textContent = new Date().toLocaleTimeString();
      if (!state.status.zones) {
        err.style.display = "block";
        err.textContent = "Status has no 'zones' block — deploy ccli r33+ (zone status). Showing PF2 fields only.";
      }
    } catch (e) {
      $("connBadge").className = "badge b-bad";
      $("connBadge").textContent = "OFFLINE";
      err.style.display = "block";
      err.textContent = `Status unavailable: ${e.message}. Check the API URL, uhttpd and /etc/init.d/ccli. Tick “Demo data” to preview off-device.`;
    }
    renderAll();
  }
  async function refreshEvents() {
    try {
      state.events = $("demoMode").checked ? demoEvents() : await getJson(`${apiBase()}?action=events`);
      if (!Array.isArray(state.events)) state.events = [];
    } catch (_) {
      state.events = [];
    }
    renderEvents();
  }
  async function loadMap() {
    try {
      state.map = await getJson(`plant_map.json?t=${Date.now()}`);
    } catch (_) {
      state.map = null;
    }
    renderPlant();
    renderOverview();
  }

  /* ---------- Render ---------- */
  function zoneDot(el, cls) {
    const c = { ok: "var(--ok)", warn: "var(--warn)", bad: "var(--bad)", off: "var(--off)" }[cls];
    el.style.background = c;
  }
  function serverState(z) {
    if (!z || !z.enabled) return "off";
    if (!z.running) return "bad";
    return z.clients > 0 ? "ok" : "warn";
  }
  function linkState(pl) {
    if (!pl) return "off";
    return pl.link === "up" ? "ok" : pl.link === "down" ? "bad" : "warn";
  }
  function serverLabel(z) {
    if (!z) return "—";
    if (!z.enabled) return badge("DISABLED", "b-off");
    if (!z.running) return badge("BIND FAILED", "b-bad");
    return z.clients > 0 ? badge("CONNECTED", "b-ok") : badge("LISTENING", "b-warn");
  }

  function renderAll() {
    const s = state.status;
    const z = (s && s.zones) || {};
    zoneDot($("dotDso"), serverState(z.dso));
    zoneDot($("dotOp"), serverState(z.operator));
    zoneDot($("dotPlant"), linkState(z.plant));
    renderOverview();
    renderDso();
    renderOperator();
    renderPlant();
  }

  function inverterRows() {
    const inv = (state.map && state.map.inverters) || [];
    return $("invEnabledOnly").checked ? inv.filter((i) => i.enabled) : inv;
  }
  function demoInv(id) {
    const s = state.status;
    return s && s._demo_inverters ? s._demo_inverters[id] : null;
  }

  function renderOverview() {
    const s = state.status;
    const z = (s && s.zones) || {};
    const live = s && s.quality === "good";
    $("sldP").textContent = s ? `${num(s.p_kw)} kW` : "— kW";
    $("sldQ").textContent = s && typeof s.q_kvar === "number" ? `${num(s.q_kvar)} kVAr` : "— kVAr";
    $("sldQual").textContent = `quality ${s ? s.quality : "—"}`;
    ["sldL1", "sldL2"].forEach((id) => $(id).classList.toggle("live", !!live));

    const inv = ((state.map && state.map.inverters) || []).filter((i) => i.enabled);
    $("sldInvCount").textContent = `${inv.length} inverters mapped`;
    let pvSum = null;
    inv.forEach((i) => { const d = demoInv(i.id); if (d) pvSum = (pvSum || 0) + d.p_kw; });
    $("sldPv").textContent = pvSum === null ? "not polled" : `${num(pvSum)} kW`;
    $("sldPvSrc").textContent = pvSum === null ? "GenPVMMXU1 · DEFERRED" : "GenPVMMXU1 · sum(INV) demo";

    if (s) {
      const on = !!s.do_curtail_on;
      $("sldDo").textContent = on ? "CURTAIL ON" : "OFF";
      $("sldDo").style.fill = on ? "var(--bad)" : "var(--ok)";
    }
    const lbl = (st) => ({ ok: "connected", warn: "listening", bad: "fault", off: "off" }[st]);
    $("sldZones").textContent =
      `LAN1 ${lbl(serverState(z.dso))} · LAN2 ${lbl(serverState(z.operator))} · LAN3 ${z.plant ? z.plant.link : "—"}`;

    const dso = z.dso;
    $("ovDsoClients").innerHTML = dso ? `${dso.clients} ${serverLabel(dso)}` : "—";
    $("ovDsoBind").textContent = dso ? `${dso.bind}:${dso.port}${dso.tls ? " TLS" : ""}` : "—";
    $("ovDsoPeff").textContent = s && s.derived ? `${num(s.derived.p_effective_kw)} kW` : "—";
    $("ovDsoMode").textContent = dso
      ? [dso.wlim_active ? `Wlim ${num(dso.wlim_pct)}%` : null, dso.wsd_active ? `WSd ${num(dso.wsd_pct)}%` : null]
          .filter(Boolean).join(" · ") || "no live DSO P command"
      : "—";

    const op = z.operator;
    $("ovOpClients").innerHTML = op ? `${op.clients} ${serverLabel(op)}` : "—";
    $("ovOpBind").textContent = op ? `${op.bind}:${op.port}${op.tls ? " TLS" : ""}` : "—";
    $("ovOpMode").textContent = op ? op.mode : "—";
    $("ovOpFlags").textContent = op ? `commands ${op.allow_commands ? "allowed" : "rejected"} · GI ${op.allow_gi ? "on" : "off"}` : "—";

    const pl = z.plant;
    $("ovPlLink").innerHTML = pl ? badge(pl.link.toUpperCase(), { ok: "b-ok", bad: "b-bad", warn: "b-warn", off: "b-off" }[linkState(pl)]) : "—";
    $("ovPlBus").textContent = pl ? busLabel(pl) : "—";
    $("ovPf2").textContent = s ? s.pf2_reason || "—" : "—";
    $("ovPf2Thr").textContent = s && s.derived ? `enter ${num(s.derived.pf2_enter_kw)} / release ${num(s.derived.pf2_release_kw)} kW` : "—";
  }

  function busLabel(pl) {
    if (pl.modbus_backend === "tcp") return `TCP ${pl.modbus_host}:${pl.modbus_tcp_port} unit ${pl.modbus_slave_id}`;
    if (pl.modbus_backend === "rtu") return `RTU ${pl.modbus_device} ${pl.modbus_baud} unit ${pl.modbus_slave_id}`;
    return "simulator";
  }

  function renderDso() {
    const s = state.status;
    const d = s && s.zones && s.zones.dso;
    $("dsoClients").textContent = d ? d.clients : "—";
    $("dsoRun").innerHTML = serverLabel(d);
    $("dsoTotW").textContent = s ? `${num(s.p_kw)} kW` : "—";
    $("dsoTotVAr").textContent = s && typeof s.q_kvar === "number" ? `${num(s.q_kvar)} kVAr` : "—";
    $("dsoPpv").textContent = d ? `${num(d.ppv_kv)} kV` : "—";

    kv($("dsoLink"), d ? [
      ["Bind", `<span class="mono">${esc(d.bind)}:${esc(d.port)}</span>`],
      ["TLS (62351-3/4)", yesNo(d.tls)],
      ["Server", serverLabel(d)],
      ["Sessions", esc(d.clients)],
      ["GNSS time fix", d.gnss_fix ? badge("FIX", "b-ok") : badge("NO FIX", "b-warn")],
      ["Comms-loss fallback", `${esc(d.comms_loss_fallback_s)} s (O.13.1.2)`],
      ["Set-point spacing", `${esc(d.setpoint_min_interval_s)} s (O.7.3.3)`]
    ] : [["Status", "no zone data"]]);

    const dv = (s && s.derived) || {};
    kv($("dsoDerived"), [
      ["Smax calc / used", `${num(dv.smax_calc_kva)} / ${num(dv.smax_used_kva)} kVA`],
      ["P cap Wlim (O.9.2.2)", `${num(dv.p_wlim_kw)} kW`],
      ["P cap WSd (O.9.2.3)", `${num(dv.p_wsd_kw)} kW`],
      ["P cap W110 (O.9.2.1)", `${num(dv.p_w110_kw)} kW`],
      ["Effective export (O.11 min)", `<strong>${num(dv.p_effective_kw)} kW</strong>`],
      ["PF2 enter / release", `${num(dv.pf2_enter_kw)} / ${num(dv.pf2_release_kw)} kW`],
      ["Curtail DO (DIO1)", s ? (s.do_curtail_on ? badge("ON", "b-bad") : badge("OFF", "b-ok")) : "—"],
      ["Permissive DI / Annex M", s ? `${s.permissive_ok ? "permit" : "blocked"} / ${s.annex_m_trip_active ? "TRIP" : "clear"}` : "—"]
    ]);

    const smax = dv.smax_used_kva || 0;
    const mod = (on) => (on ? badge("ON (1)", "b-ok") : badge("OFF (5)", "b-off"));
    const rows = d ? [
      [2, "DSO power limit", "WlimDWMX1", "O.9.2.2", mod(d.wlim_active), `${num(d.wlim_pct)} % Smax`, `${num(dv.p_wlim_kw)} kW`],
      [3, "DSO power modulation", "WSdDAGC1", "O.9.2.3", mod(d.wsd_active), `${num(d.wsd_pct)} % Smax`, `${num(dv.p_wsd_kw)} kW`],
      [5, "DSO reactive set-point", "VArSdDVAR1", "O.9.1.4", mod(d.varsd_active), `${num(d.varsd_pct)} % Smax`, `${num((d.varsd_pct / 100) * smax)} kVAr`],
      [6, "cosφ set-point", "PFSPDFPF1", "O.9.1.1", mod(d.pfsp_active), `cosφ ${num(d.pfsp_cosphi, 3)}`, d.pfsp_generation ? "PFGnTgtSpt" : "PFLodTgtSpt"]
    ] : [];
    $("dsoSetpoints").innerHTML = rows.length
      ? rows.map((r) => `<tr><td>${r[0]}</td><td>${esc(r[1])}</td><td class="mono">${esc(r[2])}</td><td>${esc(r[3])}</td><td>${r[4]}</td><td class="num">${esc(r[5])}</td><td class="num">${esc(r[6])}</td></tr>`).join("")
      : `<tr><td colspan="7" class="muted">No zone data.</td></tr>`;
    $("dsoCmdNote").textContent = d && !d.cmd_seen
      ? "No DSO Operate received since ccli start — values show defaults. PF2 uses the yaml mock envelope until Eth_A writes."
      : "Values are the last MMS Operate snapshot accepted by ccli (after O.7.3.3 spacing gate).";
  }

  function renderOperator() {
    const s = state.status;
    const o = s && s.zones && s.zones.operator;
    $("opClients").textContent = o ? o.clients : "—";
    $("opRun").innerHTML = serverLabel(o);
    $("opMode").textContent = o ? o.mode : "—";
    $("opPeriod").textContent = o ? `${o.periodic_s} s` : "—";
    $("opFallback").textContent = o ? `${o.comms_loss_fallback_s} s` : "—";
    $("opCa").textContent = o ? o.common_address : "—";

    kv($("opLink"), o ? [
      ["Bind", `<span class="mono">${esc(o.bind)}:${esc(o.port)}</span>`],
      ["TLS (62351-3)", yesNo(o.tls)],
      ["Server", serverLabel(o)],
      ["Sessions", esc(o.clients)],
      ["Common address", esc(o.common_address)]
    ] : [["Status", "no zone data"]]);

    $("opRule").innerHTML = o ? [
      ["Supervision / GI", "C_IC_NA_1", o.allow_gi],
      ["Clock sync", "C_CS_NA_1", o.allow_clock_sync],
      ["Commands / set-points (MSD)", "C_SC / C_SE", o.allow_commands]
    ].map((r) => `<tr><td>${esc(r[0])}</td><td class="mono">${esc(r[1])}</td><td>${r[2] ? badge("ALLOW", "b-ok") : badge("REJECT + LOG", "b-off")}</td></tr>`).join("")
      : `<tr><td colspan="3" class="muted">No zone data.</td></tr>`;

    const src = { TotW: "Modbus POC → MeasurementStore", TotVAr: "Modbus POC → MeasurementStore", PPV: "yaml plant.poc_ppv_kv" };
    $("opIoa").innerHTML = o && o.ioa ? o.ioa.map((p) => `<tr>
        <td>${esc(p.name)}</td><td class="num">${esc(p.ioa)}</td>
        <td>${p.enabled ? badge("ON", "b-ok") : badge("OFF", "b-off")}</td>
        <td>${p.published ? badge("SENT", "b-ok") : (p.enabled ? badge("NOT IMPLEMENTED", "b-warn") : badge("—", "b-off"))}</td>
        <td class="num">${num(p.value)} ${esc(p.unit)}</td><td class="muted">${esc(src[p.name] || "")}</td></tr>`).join("")
      : `<tr><td colspan="6" class="muted">No zone data.</td></tr>`;
  }

  function renderPlant() {
    const s = state.status;
    const pl = s && s.zones && s.zones.plant;
    $("plLink").innerHTML = pl ? badge(pl.link.toUpperCase(), { ok: "b-ok", bad: "b-bad", warn: "b-warn", off: "b-off" }[linkState(pl)]) : "—";
    $("plLastEx").textContent = pl && pl.last_exchange_ms ? `last poll ${fmtTime(pl.last_exchange_ms)}` : "—";
    $("plP").textContent = s ? `${num(s.p_kw)} kW` : "—";
    $("plQ").textContent = s && typeof s.q_kvar === "number" ? `${num(s.q_kvar)} kVAr` : "—";
    $("plQual").textContent = s ? `quality ${s.quality} · stale after ${s.stale_data_s} s` : "—";

    kv($("plBus"), pl ? [
      ["Backend", esc(pl.modbus_backend)],
      ["Transport", `<span class="mono">${esc(busLabel(pl))}</span>`],
      ["Poll interval", `${esc(pl.poll_ms)} ms (O.8.3 ≤ 4 s)`],
      ["Link", badge(pl.link.toUpperCase(), { ok: "b-ok", bad: "b-bad", warn: "b-warn", off: "b-off" }[linkState(pl)])],
      ["Last exchange", `${fmtTime(pl.last_exchange_ms)} ${pl.last_success ? badge("OK", "b-ok") : badge("FAIL", "b-bad")}`],
      ["Last error", pl.last_error ? `<span class="mono">${esc(pl.last_error)}</span>` : "—"],
      ["GOOSE (plant L2)", pl.goose_enabled ? `${badge("SUB ON", "b-ok")} ${esc(pl.goose_interface)}` : badge("SUB OFF", "b-off")],
      ["GOOSE publish", pl.goose_publish ? badge("ON", "b-ok") : badge("OFF", "b-off")]
    ] : [["Status", "no zone data"]]);

    const m = state.map;
    if (!m) {
      $("plPoc").innerHTML = `<tr><td colspan="7" class="muted">plant_map.json not found — run scripts/plant-map-generate.py and redeploy.</td></tr>`;
      $("invGrid").innerHTML = "";
      $("plAgg").innerHTML = "";
      $("plCtl").innerHTML = "";
      $("plInvCount").textContent = "—";
      return;
    }
    $("plBusNotes").textContent = m.bus_notes ? `Bus: ${m.bus_notes}` : "";
    $("plPoc").innerHTML = m.poc.map((r) => `<tr><td class="mono">${esc(r.id)}</td><td>${esc(r.device)}</td>
      <td class="num">${esc(r.slave_id)}</td><td class="num">${esc(r.reg)}</td><td>${esc(r.type)}</td>
      <td class="mono">${esc(r.mms_path)}</td><td>${statusBadge(r.runtime_status)}</td></tr>`).join("");

    const all = m.inverters;
    const enabled = all.filter((i) => i.enabled);
    const gaps = enabled.filter((i) => i.runtime_status === "CID_GAP");
    $("plInvCount").textContent = `${enabled.length} / ${all.length}`;
    $("plInvSub").textContent = `${gaps.length} with CID gap · not polled by ccli yet`;

    $("invGrid").innerHTML = inverterRows().map((i) => {
      const d = demoInv(i.id);
      let cls = "s-mapped", pTxt = "not polled", tag = badge("MAPPED", "b-info");
      if (!i.enabled) { cls = "s-off"; tag = badge("DISABLED", "b-off"); }
      else if (d) { cls = "s-live"; pTxt = `${num(d.p_kw)} kW`; tag = badge("DEMO", "b-warn"); }
      else if (m.lab_combined_mock && i.id === "INV01" && s && pl && pl.last_success) {
        cls = "s-live";
        pTxt = `${num(s.p_kw)} kW (PdC poll)`;
        tag = badge("LAB MOCK", "b-ok");
      } else if (i.runtime_status === "CID_GAP") { cls = "s-gap"; tag = badge("CID GAP", "b-warn"); }
      return `<div class="inv ${cls}" data-inv="${esc(i.id)}" tabindex="0">
        <div style="display:flex;justify-content:space-between;align-items:center"><span class="id">${esc(i.id)}</span>${tag}</div>
        <div class="vendor" title="${esc(i.vendor)}">${esc(i.vendor)}</div>
        <div class="p">${esc(pTxt)}</div>
        <div class="meta">unit ${esc(i.slave_id)} · reg ${esc(i.p && i.p.reg)} · ${esc((i.mms_ln || "").replace("LD_Plant/", ""))}</div>
      </div>`;
    }).join("") || `<p class="muted">No inverter rows.</p>`;

    const warns = [];
    const pocIds = new Set(m.poc.map((r) => r.slave_id).filter((x) => x !== null));
    const clash = enabled.filter((i) => pocIds.has(i.slave_id)).map((i) => i.id);
    if (clash.length && !m.lab_combined_mock) {
      warns.push(`Unit-ID clash: POC meter and ${clash.join(", ")} share Modbus unit ${[...pocIds].join(", ")}. ` +
        "Put the meter and inverters on separate buses (e.g. meter ttyS1, inverters ttyS2 or LAN3 TCP) or renumber.");
    } else if (m.lab_combined_mock && clash.length) {
      warns.push(
        "Lab bench: one COM5 slave (unit 1) serves PdC @40001 and Huawei INV01 @32080 — production uses separate devices.");
    }
    if (gaps.length) {
      warns.push(`${gaps.map((i) => i.id).join(", ")}: SGGMMXU LN not in the CID — MMS reports cannot carry these values until the CID is extended.`);
    }
    $("invWarn").innerHTML = warns.map((w) => `<div class="warnbox">${esc(w)}</div>`).join("");

    $("plAgg").innerHTML = m.aggregates.map((a) => `<tr><td class="mono">${esc(a.id)}</td><td class="mono">${esc(a.mms_path)}</td>
      <td class="mono">${esc(a.transform)}</td><td>${statusBadge(a.runtime_status)}</td><td class="muted">${esc(a.notes)}</td></tr>`).join("");
    $("plCtl").innerHTML = m.controls.map((c) => `<tr><td class="mono">${esc(c.id)}</td><td class="mono">${esc(c.dso_source || "—")}</td>
      <td>${esc(c.clause)}</td><td>${esc(c.slave_id)}</td><td class="num">${esc(c.fc)}</td><td class="num">${esc(c.reg)}</td>
      <td class="num">${esc(c.gain)}</td><td class="muted">${esc(c.transform)}</td>
      <td>${c.enabled ? badge("Y", "b-ok") : badge("N", "b-off")}</td><td>${statusBadge(c.runtime_status)}</td></tr>`).join("");
  }

  function eventRows(filterFn, limit) {
    return state.events.filter(filterFn).slice(-limit).reverse();
  }
  function renderEventTable(el, rows, withType) {
    el.innerHTML = rows.length
      ? rows.map((e) => `<tr><td class="mono">${esc(e.ts_o14)}</td>${withType ? `<td>${badge(e.type, "b-info")}</td>` : ""}<td class="mono">${esc(e.detail)}</td></tr>`).join("")
      : `<tr><td colspan="${withType ? 3 : 2}" class="muted">No events.</td></tr>`;
  }
  function renderEvents() {
    const types = [...new Set(state.events.map((e) => e.type))].sort();
    const sel = $("evFilter");
    const cur = sel.value;
    sel.innerHTML = `<option value="">all</option>` + types.map((t) => `<option ${t === cur ? "selected" : ""}>${esc(t)}</option>`).join("");
    renderEventTable($("evBody"), eventRows((e) => !sel.value || e.type === sel.value, 40), true);
    renderEventTable($("dsoEvents"), eventRows((e) => e.type === "mms" || e.type === "dso", 12), false);
    renderEventTable($("opEvents"), eventRows((e) => e.type === "iec104", 12), false);
  }

  /* ---------- Drawer ---------- */
  function openInverter(id) {
    const i = state.map && state.map.inverters.find((x) => x.id === id);
    if (!i) return;
    const fan = state.map.controls.filter((c) => c.slave_id === "ALL" || String(c.slave_id) === String(i.slave_id));
    $("drwTitle").textContent = `${i.id} — ${i.vendor}`;
    $("drwBody").innerHTML = `
      <dl class="kv">
        <dt>Enabled</dt><dd>${i.enabled ? badge("Y", "b-ok") : badge("N", "b-off")}</dd>
        <dt>Map status</dt><dd>${statusBadge(i.runtime_status)}</dd>
        <dt>Modbus unit ID</dt><dd>${esc(i.slave_id)}</dd>
        <dt>Logical node</dt><dd class="mono">${esc(i.mms_ln)}</dd>
        <dt>Notes</dt><dd>${esc(i.notes || "—")}</dd>
      </dl>
      <h3>Read map</h3>
      <table><thead><tr><th>Quantity</th><th class="num">FC</th><th class="num">Reg</th><th>Type</th><th class="num">Gain</th><th>MMS</th></tr></thead><tbody>
        ${i.p ? `<tr><td>Active power</td><td class="num">${esc(i.p.fc)}</td><td class="num">${esc(i.p.reg)}</td><td>${esc(i.p.type)}</td><td class="num">÷${esc(i.p.gain)}</td><td class="mono">${esc(i.p.mms_path)}</td></tr>` : ""}
        ${i.status ? `<tr><td>Status word</td><td class="num">${esc(i.status.fc)}</td><td class="num">${esc(i.status.reg)}</td><td>${esc(i.status.type)}</td><td class="num">—</td><td class="mono">${esc(i.status.mms_path)}</td></tr>` : ""}
      </tbody></table>
      <h3>DSO commands fanned out to this unit</h3>
      <table><thead><tr><th>Plane</th><th class="num">Reg</th><th class="num">Gain</th><th>Enabled</th></tr></thead><tbody>
        ${fan.map((c) => `<tr><td class="mono">${esc(c.id)}</td><td class="num">${esc(c.reg)}</td><td class="num">${esc(c.gain)}</td><td>${c.enabled ? badge("Y", "b-ok") : badge("N", "b-off")}</td></tr>`).join("") || `<tr><td colspan="4" class="muted">none</td></tr>`}
      </tbody></table>
      <p class="note">Runtime: ccli currently polls only the POC meter (single unit ID). Per-inverter values appear here once the multi-slave poller reads plant_modbus_map.yaml.</p>`;
    $("drawer").classList.add("open");
    $("drawer").setAttribute("aria-hidden", "false");
  }

  /* ---------- Wiring ---------- */
  function initTabs() {
    $("tabs").addEventListener("click", (ev) => {
      const b = ev.target.closest("button[data-tab]");
      if (!b) return;
      document.querySelectorAll("#tabs button").forEach((x) => x.classList.toggle("active", x === b));
      document.querySelectorAll(".tab").forEach((t) => t.classList.toggle("active", t.id === `tab-${b.dataset.tab}`));
      location.hash = b.dataset.tab;
    });
    const h = location.hash.replace("#", "");
    const b = document.querySelector(`#tabs button[data-tab="${h}"]`);
    if (b) b.click();
  }

  function init() {
    const params = new URLSearchParams(location.search);
    $("apiBase").value = params.get("api") || "/cgi-bin/ccli-bench";
    $("demoMode").checked = params.get("demo") === "1" || location.protocol === "file:";
    const syncBanner = () => $("banner").classList.toggle("demo", $("demoMode").checked);
    syncBanner();
    $("demoMode").addEventListener("change", () => { syncBanner(); refreshStatus(); refreshEvents(); });
    $("btnRefresh").addEventListener("click", () => { refreshStatus(); refreshEvents(); loadMap(); });
    $("invEnabledOnly").addEventListener("change", renderPlant);
    $("evFilter").addEventListener("change", renderEvents);
    $("invGrid").addEventListener("click", (ev) => {
      const c = ev.target.closest("[data-inv]");
      if (c) openInverter(c.dataset.inv);
    });
    $("invGrid").addEventListener("keydown", (ev) => {
      const c = ev.target.closest("[data-inv]");
      if (c && (ev.key === "Enter" || ev.key === " ")) { ev.preventDefault(); openInverter(c.dataset.inv); }
    });
    $("drawerClose").addEventListener("click", () => {
      $("drawer").classList.remove("open");
      $("drawer").setAttribute("aria-hidden", "true");
    });
    initTabs();
    loadMap();
    refreshStatus();
    refreshEvents();
    setInterval(refreshStatus, STATUS_POLL_MS);
    setInterval(refreshEvents, EVENTS_POLL_MS);
  }

  document.addEventListener("DOMContentLoaded", init);
})();
