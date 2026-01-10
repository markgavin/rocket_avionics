#!/usr/bin/env python3
"""
Rocket Avionics Serial Monitor
A simple tool for testing the gateway serial communication.

Usage:
    python3 serial_monitor.py [port]

If no port specified, lists available ports.

Commands:
    arm     - Send arm command
    disarm  - Send disarm command
    status  - Request status
    download - Request flight data download
    quit    - Exit the monitor
"""

import sys
import json
import time
import threading
from datetime import datetime

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    print("Error: pyserial not installed.")
    print("Install with: pip3 install pyserial")
    sys.exit(1)


class RocketMonitor:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.running = False
        self.command_id = 0
        self.telemetry_count = 0
        self.last_rssi = 0
        self.last_snr = 0
        self.last_state = ""
        self.last_altitude = 0.0
        self.max_altitude = 0.0

    def connect(self):
        try:
            self.serial = serial.Serial(self.port, self.baudrate, timeout=0.1)
            print(f"Connected to {self.port} at {self.baudrate} baud")
            return True
        except serial.SerialException as e:
            print(f"Error: Could not open {self.port}: {e}")
            return False

    def disconnect(self):
        if self.serial:
            self.serial.close()
            print("Disconnected")

    def send_command(self, cmd):
        """Send a JSON command to the gateway"""
        self.command_id += 1
        message = {"cmd": cmd, "id": self.command_id}
        json_str = json.dumps(message) + "\n"
        self.serial.write(json_str.encode())
        print(f">> Sent: {json_str.strip()}")

    def process_message(self, line):
        """Process a JSON message from the gateway"""
        try:
            data = json.loads(line)
            msg_type = data.get("type", "")

            if msg_type == "tel":
                # Telemetry packet
                self.telemetry_count += 1
                self.last_rssi = data.get("rssi", 0)
                self.last_snr = data.get("snr", 0)
                self.last_state = data.get("state", "")
                self.last_altitude = data.get("alt", 0.0)

                if self.last_altitude > self.max_altitude:
                    self.max_altitude = self.last_altitude

                # Print condensed telemetry
                t = data.get("t", 0) / 1000.0
                alt = data.get("alt", 0.0)
                vel = data.get("vel", 0.0)
                state = data.get("state", "?")
                seq = data.get("seq", 0)

                print(f"[{t:7.1f}s] Alt: {alt:7.2f}m  Vel: {vel:6.2f}m/s  "
                      f"State: {state:8s}  RSSI: {self.last_rssi}dBm  "
                      f"SNR: {self.last_snr}dB  #{seq}")

            elif msg_type == "link":
                connected = data.get("connected", False)
                status = "CONNECTED" if connected else "DISCONNECTED"
                print(f"<< Link Status: {status}")

            elif msg_type == "ack":
                cmd = data.get("cmd", "")
                ok = data.get("ok", False)
                status = "OK" if ok else "FAILED"
                print(f"<< ACK: {cmd} - {status}")

            elif msg_type == "error":
                code = data.get("code", "")
                msg = data.get("msg", "")
                print(f"<< ERROR [{code}]: {msg}")

            else:
                print(f"<< Unknown: {line}")

        except json.JSONDecodeError:
            print(f"<< Raw: {line}")

    def read_loop(self):
        """Background thread for reading serial data"""
        buffer = ""
        while self.running:
            try:
                if self.serial.in_waiting:
                    data = self.serial.read(self.serial.in_waiting).decode('utf-8', errors='ignore')
                    buffer += data

                    # Process complete lines
                    while '\n' in buffer:
                        line, buffer = buffer.split('\n', 1)
                        line = line.strip()
                        if line:
                            self.process_message(line)
            except Exception as e:
                if self.running:
                    print(f"Read error: {e}")
                break

    def print_stats(self):
        """Print current statistics"""
        print("\n=== Statistics ===")
        print(f"Telemetry packets received: {self.telemetry_count}")
        print(f"Last RSSI: {self.last_rssi} dBm")
        print(f"Last SNR: {self.last_snr} dB")
        print(f"Last state: {self.last_state}")
        print(f"Last altitude: {self.last_altitude:.2f} m")
        print(f"Max altitude: {self.max_altitude:.2f} m")
        print("==================\n")

    def run(self):
        """Main interactive loop"""
        if not self.connect():
            return

        self.running = True
        read_thread = threading.Thread(target=self.read_loop, daemon=True)
        read_thread.start()

        print("\nRocket Avionics Monitor")
        print("Commands: arm, disarm, status, download, stats, quit")
        print("-" * 50)

        try:
            while self.running:
                try:
                    cmd = input().strip().lower()

                    if cmd == "quit" or cmd == "exit" or cmd == "q":
                        break
                    elif cmd == "arm":
                        self.send_command("arm")
                    elif cmd == "disarm":
                        self.send_command("disarm")
                    elif cmd == "status":
                        self.send_command("status")
                    elif cmd == "download":
                        self.send_command("download")
                    elif cmd == "stats":
                        self.print_stats()
                    elif cmd == "help":
                        print("Commands: arm, disarm, status, download, stats, quit")
                    elif cmd:
                        print(f"Unknown command: {cmd}")

                except EOFError:
                    break

        except KeyboardInterrupt:
            print("\nInterrupted")

        self.running = False
        self.disconnect()
        self.print_stats()


def list_ports():
    """List available serial ports"""
    ports = serial.tools.list_ports.comports()
    if not ports:
        print("No serial ports found")
        return

    print("Available serial ports:")
    for port in ports:
        print(f"  {port.device} - {port.description}")

    # Filter for likely RP2040 devices
    rp2040_ports = [p for p in ports if "usbmodem" in p.device.lower() or "rp2040" in p.description.lower()]
    if rp2040_ports:
        print(f"\nLikely RP2040 device: {rp2040_ports[0].device}")


def main():
    if len(sys.argv) < 2:
        list_ports()
        print("\nUsage: python3 serial_monitor.py <port>")
        print("Example: python3 serial_monitor.py /dev/cu.usbmodem14101")
        return

    port = sys.argv[1]
    monitor = RocketMonitor(port)
    monitor.run()


if __name__ == "__main__":
    main()
