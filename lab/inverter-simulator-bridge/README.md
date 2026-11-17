# COM8 inverter mock to Tespro CCI

The running setup uses RS485 only: Windows COM8 -> Tespro RS485-2 (`/dev/rs485_2_uart`, currently `/dev/ttyUSB1`). Both sides use unit 1, 9600 baud, 8N1. CCI polls every 4 seconds. No firmware binary was changed.

`bridge.py` imports the model from `C:\MY\GIT\cci-software\Inverter Simulator\sun2000_100ktl_m2_rtu_real_register_mock.py` and adds the state-file controls used by the COM8 launcher. The model retains its Huawei register addresses and encoding.

The installed CCI decodes paired registers as IEEE float32, whereas the Huawei model uses signed integer watts/vars at 32080/32082. The simulator exposes a bench compatibility view, also over RTU:

| Raw register address | Encoding | Meaning |
| --- | --- | --- |
| 60000..60001 | float32, high word first | Active power, kW |
| 60002..60003 | float32, high word first | Reactive power, kvar |
| 60004..60005 | float32, high word first | Line voltage, V |

Complete FC16 Float32 writes at 60000 and 60002 set simulated P and Q; writing P
puts the model in fixed-power mode. The launcher enables this bench behavior and
persists commands in `state.json`. Direct simulator use also applies P/Q writes
in memory. The bridge remains read-only unless `--accept-cci-pq-writes` is set
(the older `--accept-cci-q-writes` option is still accepted). Voltage remains
read-only, partial writes are rejected, and values must be finite with P in
0..110 kW and Q within ±66 kvar. This is a bench compatibility control for CCI's
current float-register writer; it does not define a production Huawei
control-register map. Huawei-address control writes are still logged only.
`--self-test` covers P/Q write/read behavior and rejects voltage, partial, and
out-of-range writes without opening COM8.

CCI must own RS485-2 exclusively. After a Tespro reboot, check that the vendor
serial-sharing service is not also opening `/dev/rs485_2_uart`. Disabling its
`serialservice.port3` entry requires operator approval if that sharing interface
is still in use.

`modbus-rtu.yaml` records the CCI input settings. Its log labels add 40001 to the raw offset, so a trace labelled `addr=100001` means wire address 60000. This bench view feeds the existing **plant/PdC** measurement input from one simulated inverter; it does not implement a separate per-inverter acquisition model or replace a real point-of-connection meter in production.

## Use

The COM8 server is already running in the background for this session. Launch the controller from the merged project directory:

```powershell
.\lab\inverter-simulator-bridge\control.cmd
```

Choose power, reactive power, simulation mode or voltage. Changes are read from `state.json` on the next serial request. `fixed` keeps power steady; `daytime` and `daycycle` use the original simulator's varying solar model. Voltage/frequency retain the original small variations. For a scripted update:

```powershell
.\lab\inverter-simulator-bridge\control.cmd --mode fixed --power-kw 10 --q-kvar 1
```

Observe CCI using the MMS menu or CLI:

```powershell
.\apps\ccli\tools\mms-client\run.cmd check
.\apps\ccli\tools\mms-client\run.cmd watch CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f --count 30
```

These mock values feed CCI's live control logic. The current lab DSO rule has a 20% active-power target; increasing simulated production can cause CCI's existing curtailment logic to react. The initial 10 kW test stays below that target. CCI writes to the bench float registers change mock P/Q; writes to Huawei control-register addresses are stored/logged but do **not** change electrical P/Q behavior automatically.

After reboot or after stopping the simulator, run `start-rtu.cmd` to start it again. Only one process can own COM8, so do not start a second copy while the background server is running. `simulator.pid` records the process launched for this session; `.stdout.log`/`.stderr.log` record its serial server output. This is not installed as a Windows startup service.

CCI's original serial configuration is backed up on Tespro as `/etc/ccli/lab.yaml.before-inverter-simulator-20261019`, and locally under `lab/evidence/inverter-simulator-2026-10-19/lab.before.yaml`. To restore the old CCI map over an authenticated device shell:

```sh
cp /etc/ccli/lab.yaml.before-inverter-simulator-20261019 /etc/ccli/lab.yaml
/etc/init.d/ccli restart
```

Stop the mock and restore/reconfigure the register map before replacing it with a real inverter.
