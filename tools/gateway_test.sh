#!/bin/bash
#
# Rocket Avionics Gateway Test Script
# Simple script to send test commands to the gateway
#

# Find the first usbmodem port
PORT=$(ls /dev/cu.usbmodem* 2>/dev/null | head -1)

if [ -z "$PORT" ]; then
    echo "Error: No USB modem port found"
    echo "Make sure the gateway is connected and flashed"
    exit 1
fi

echo "Using port: $PORT"

# Configure serial port
stty -f "$PORT" 115200 cs8 -cstopb -parenb

send_command() {
    local cmd=$1
    local id=$2
    echo "{\"cmd\":\"$cmd\",\"id\":$id}" > "$PORT"
    echo "Sent: {\"cmd\":\"$cmd\",\"id\":$id}"
    sleep 0.5
}

case "$1" in
    arm)
        send_command "arm" 1
        ;;
    disarm)
        send_command "disarm" 2
        ;;
    status)
        send_command "status" 3
        ;;
    download)
        send_command "download" 4
        ;;
    monitor)
        echo "Monitoring $PORT (Ctrl+C to exit)..."
        cat "$PORT"
        ;;
    *)
        echo "Rocket Avionics Gateway Test"
        echo ""
        echo "Usage: $0 <command>"
        echo ""
        echo "Commands:"
        echo "  arm      - Send arm command"
        echo "  disarm   - Send disarm command"
        echo "  status   - Request status"
        echo "  download - Request flight data"
        echo "  monitor  - Monitor gateway output"
        echo ""
        echo "Port: $PORT"
        ;;
esac
