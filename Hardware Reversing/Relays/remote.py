#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "192.168.1.134"
PORT = 4223
UID = "BvH" # Change to your UID
MASTER_UID = "68bR6c"

VALUE_A_ON  = (1 << 0) | (1 << 2) # Pin 0 and 2 high
VALUE_A_OFF = (1 << 0) | (1 << 3) # Pin 0 and 3 high
VALUE_B_ON  = (1 << 1) | (1 << 2) # Pin 1 and 2 high
VALUE_B_OFF = (1 << 1) | (1 << 3) # Pin 1 and 3 high

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_industrial_quad_relay import IndustrialQuadRelay
from tinkerforge.brick_master import BrickMaster


if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    master = BrickMaster(UID, ipcon) # Create device object

    iqr = IndustrialQuadRelay(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected
    #master.disable_wifi2_status_led()


    iqr.set_monoflop(VALUE_A_OFF, 15, 1500); # Set pins to high for 1.5 seconds

    ipcon.disconnect()
