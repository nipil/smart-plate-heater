import sys

from serial import Serial
from argparse import ArgumentParser
from collections import deque


def run(ser, size, reject):
    print(f"Using size={size}, reject={reject}", file=sys.stderr)
    size = 2 * size + 1
    series = []
    while True:
        line = ser.readline().decode().strip()
        values = line.split(" ")
        try:
            values = [int(v) for v in values]
        except ValueError:
            print(f"Invalid data: {line}", file=sys.stderr)
            continue
        while len(series) < len(values):
            series.append(deque((), size))
        for i, value in enumerate(values):
            series[i].append(value)
        if len(series[0]) != size:
            print(f"Not yet ready : {len(series[0])}/{size} samples", file=sys.stderr)
            continue
        values = []
        for data in series:
            sorted_data = sorted(data)
            clipped_data = sorted_data[reject : len(sorted_data) - reject]
            values.append(str(clipped_data[len(clipped_data) // 2]))
        print(" ".join(values))


def main(args=None):
    if args is None:
        args = sys.argv[1:]
    parser = ArgumentParser(description="Kappa-Sigma Median Clipping")
    parser.add_argument("--serial", type=str, required=True)
    parser.add_argument("--baud", type=int, required=True, choices=[9600, 115200])
    parser.add_argument("--size", type=int, required=True)
    parser.add_argument("--reject", type=int, required=True)
    args = parser.parse_args(args)
    if args.reject > args.size:
        raise ValueError("size > rejected")
    with Serial(args.serial, args.baud, timeout=1) as ser:
        run(ser, args.size, args.reject)


if __name__ == "__main__":
    main(["--serial", "COM8", "--baud", "115200", "--size", "10", "--reject", "5"])
