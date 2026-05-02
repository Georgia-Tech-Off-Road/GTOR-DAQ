import struct
import json
import argparse
import numpy as np
from collections import defaultdict
 
# Must match Arduino/Teensy sensor ID mapping
SENSORS = {
    0: "engineRPM",
    1: "frontLeftRPM",
    2: "frontRightRPM",
    3: "rearRPM",
    4: "rearBrakePressure",
    5: "frontBrakePressure",
    6: "LDSFrontRight",
    7: "LDSFrontLeft",
    8: "LDSRearRight",
    9: "LDSRearLeft",
    10: "CVTemp",
    11: "rearTCTemp",
    12: "teensyTemp",
    13: "engineRPMTimestamp",
    14: "frontLeftRPMTimestamp",
    15: "frontRightRPMTimestamp",
    16: "rearRPMTimestamp"
}

# Little-endian: uint8 sensor_id, uint32 timestamp (in units of 100 µs), float value
PACKET_FORMAT = "<BIf"
PACKET_SIZE = struct.calcsize(PACKET_FORMAT)

def read_timestamps(in_path: str, sensor_ids: list[int]) -> dict[int, list[float]]:
    """
    Read the binary file and extract adjusted timestamps (in microseconds)
    for the requested sensor IDs.
 
    Handles uint32 overflow (timer wraps at 2^32 * 100 µs ≈ 429 seconds).
 
    Returns:
        A dict mapping sensor_id -> sorted list of timestamps in microseconds.
    """
    sensor_ids_set = set(sensor_ids)
 
    # Per-sensor overflow tracking so each channel is independent
    state: dict[int, dict] = {
        sid: {"prev_raw": None, "base_offset": None, "overflow_acc": 0}
        for sid in sensor_ids_set
    }
    timestamps: dict[int, list[float]] = defaultdict(list)
 
    with open(in_path, "rb") as f:
        while chunk := f.read(PACKET_SIZE):
            if len(chunk) < PACKET_SIZE:
                print("Warning: incomplete packet at end of file, skipping.")
                break
 
            sync, sensor_id, raw_ts, _value = struct.unpack(PACKET_FORMAT, chunk)

            resync_failed = False
            while sync != 0xAA or sensor_id not in sensor_ids_set: # Anti corruption value
                curpos = f.tell()
                f.seek(curpos - PACKET_SIZE + 1) # Go one byte after beginning of current packet
                chunk = f.read(PACKET_SIZE)
                if len(chunk) <  PACKET_SIZE:
                    print("Warning: could not resynchronize, reached end of file.")
                    resync_failed = True
                    break
                sync, sensor_id, timestamp, value = struct.unpack(PACKET_FORMAT, chunk)
            if resync_failed:
                break
 
            s = state[sensor_id]
 
            if s["base_offset"] is None:
                s["base_offset"] = raw_ts
            elif raw_ts < s["prev_raw"]:
                # uint32 overflow
                s["overflow_acc"] += (2**32 // 100)
 
            s["prev_raw"] = raw_ts
 
            adjusted_us = (raw_ts - s["base_offset"] + s["overflow_acc"]) * 100
            timestamps[sensor_id].append(float(adjusted_us))
 
    return {sid: sorted(ts) for sid, ts in timestamps.items()}


def gaussian_kernel(sigma_us: float, n_sigma = 4.0):
    """
    Build a normalised Gaussian kernel with the given std-dev (in µs).
    Half-width is n_sigma * sigma_us, evaluated on a 1 µs grid.
    """
    half = int(n_sigma * sigma_us)
    x = np.arange(-half, half + 1, dtype=np.float64)
    k = np.exp(-0.5 * (x / sigma_us) ** 2)
    return k / np.sum(k)

def convolve_rpm(
    timestamps_us: list[float],
    pulses_per_rev: int,
    sigma_us: float,
    dt_us: float = 1000.0
) -> tuple [np.ndarray, np.ndarray]:
    if not timestamps_us:
        return np.array([]), np.array([])
    t_max = timestamps_us[-1];

    time_us = np.arange(0.0, t_max + dt_us, dt_us)

    spike_train = np.zeros(len(time_us), dtype=np.float64)

    for ts in timestamps_us:
        idx = int(round(ts / dt_us))
        if 0 <= idx < len(spike_train):
            spike_train[idx] += 1.0
    
    kernel = gaussian_kernel(sigma_us / dt_us)
    smoothed = np.convolve(spike_train, kernel, "same")

    pulse_rate_per_us = smoothed / dt_us

    rpm = pulse_rate_per_us * 60.0e6 / pulses_per_rev

    return time_us, rpm

def save_results(
        out_path: str,
        results: tuple[np.ndarray, np.ndarray]
):
    with open(out_path, 'w', encoding='utf-8') as f:
        f.write("[")
        first = True
        for time_us, rpm in results.values():
            for t, r in zip(time_us, rpm):
                record = {"microsec": t, "rpm": r}
                if not first:
                    f.write(",\n")
                f.write(json.dumps(record) + "\n")
                first = False
        f.write("]")

    print(f"Results saved to {out_path}")


def main():
    parser = argparse.ArgumentParser(
        prog="RPMConvolution",
        description=(
            "Read interrupt timestamps from a Teensy binary log and estimate "
            "RPM via Gaussian-kernel convolution of the spike train."
        ),
    )
    parser.add_argument("in_path", help="Path to the input binary file.")
    parser.add_argument("out_path", help="Path for the output JSON results file.")
    parser.add_argument(
        "--sensor-id",
        type=int,
        required=True,
        help=(
            "Sensor ID that should be processed"
        ),
    )
    parser.add_argument(
        "--pulses-per-rev",
        type=int,
        default=1,
        help=(
            "Number of interrupt pulses emitted per shaft revolution "
            "(e.g. teeth on a reluctor wheel). Default: 1."
        ),
    )
    parser.add_argument(
        "--sigma-ms",
        type=float,
        default=50.0,
        help=(
            "Standard deviation of the Gaussian smoothing kernel in milliseconds. "
            "Larger values give smoother RPM traces; smaller values preserve transients. "
            "Default: 50 ms."
        ),
    )
    parser.add_argument(
        "--dt-ms",
        type=float,
        default=1.0,
        help="Time-axis resolution in milliseconds. Default: 1 ms.",
    )
 
    args = parser.parse_args()
 
    print(f"Reading binary file: {args.in_path}")
    print(f"Processing sensor ID: {args.sensor_id}")
 
    timestamps_by_sensor = read_timestamps(args.in_path, [args.sensor_id])
 
    sid = args.sensor_id
    name = SENSORS.get(sid, f"sensor_{sid}")
    count = len(timestamps_by_sensor.get(sid, []))
    print(f"  Sensor {sid} ({name}): {count} interrupts found")
 
    sigma_us = args.sigma_ms * 1000.0
    dt_us = args.dt_ms * 1000.0
 
    print(f"\nRunning convolution (σ={args.sigma_ms} ms, dt={args.dt_ms} ms) ...")
 
    results = {}
    for sid, ts in timestamps_by_sensor.items():
        if not ts:
            print(f"Sensor {sid}: no data, skipping.")
            continue
        time_us, rpm = convolve_rpm(ts, args.pulses_per_rev, sigma_us, dt_us)
        peak_rpm = float(np.max(rpm)) if len(rpm) else 0.0
        duration_s = (ts[-1] - ts[0]) / 1e6 if len(ts) > 1 else 0.0
        print(
            f"  Sensor {sid}: peak RPM = {peak_rpm:.1f}, "
            f"duration = {duration_s:.2f} s"
        )
        results[sid] = (time_us, rpm)
 
    save_results(args.out_path, results)
 
 
if __name__ == "__main__":
    main()