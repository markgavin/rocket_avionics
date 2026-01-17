#!/bin/bash
#----------------------------------------------
# USB Console Connection Script
# Connects to Feather RP2040/RP2350 serial console
#----------------------------------------------

# Find USB modem device
DEVICE=$(ls /dev/cu.usbmodem* 2>/dev/null | head -1)

if [ -z "$DEVICE" ]; then
    echo "No USB modem device found."
    echo "Make sure the Feather is connected and running."
    exit 1
fi

echo "Connecting to $DEVICE at 115200 baud..."
echo "Press Ctrl-A then K to exit."
echo ""

screen "$DEVICE" 115200
