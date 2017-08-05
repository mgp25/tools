#!/usr/bin/python

# Send a PWM String using RfCat

import rflib
import bitstring

# That prefix string. This was determined by literally
# just looking at the waveform, and calculating it relative
# to the clock signal value.
# Your remote may not need this.
prefix = ''

# The key from our static key remote.
key = '1111111111111010101011001'

# Convert the data to a PWM key by looping over the
# data string and replacing a 1 with 100 and a 0
# with 110
pwm_key = ''.join(['100' if b == '1' else '110' for b in key])

# Join the prefix and the data for the full pwm key
full_pwm = '{}{}'.format(prefix, pwm_key)
print('Sending full PWM key: {}'.format(full_pwm))

# Convert the data to hex
rf_data = bitstring.BitArray(bin=full_pwm).tobytes()

# Start up RfCat
d = rflib.RfCat()

# Set Modulation. We using On-Off Keying here
d.setMdmModulation(rflib.MOD_ASK_OOK)

# Configure the radio
d.makePktFLEN(len(rf_data)) # Set the RFData packet length
d.setMdmDRate(2800)         # Set the Baud Rate
d.setMdmSyncMode(0)         # Disable preamble
d.setFreq(433920000)        # Set the frequency

# Send the data string a few times
d.RFxmit(rf_data, repeat=25)
d.setModeIDLE()
