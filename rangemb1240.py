import spidev
import time

spi_bus = 0
spi_device = 0

spi = spidev.SpiDev()
spi.open(spi_bus, spi_device)
spi.max_speed_hz = 1000000

# Send a null byte to check for value
send_byte = 0x80
rcv_byte = spi.xfer2([send_byte])
# repeat to check for a response
rcv_byte = spi.xfer2([send_byte])
data_recv = rcv_byte[0]
if (data_recv != 0x80):
    print("Unable to communicate with Arduino " + str(data_recv))
    quit()


# Data is sent as a single bytearray
# Lower 4 bits = pin number
# Returns a value that is used to set a digital pin
# pin = Arduino pin number
# state = True for high, False for low
# returns byte or 0xff = invalid
def req_set_digital(pin, state):
    if (pin < 0 or pin > 10):
        return 0xff
    if (state == True):
        pin += 0x60
    else:
        pin += 0x20
    return pin


# Read analog pin
def req_read_analog(pin):
    if (pin < 0 or pin > 5):  # Limit to A0 to A5 (pins 0 to 5)
        return 0xff
    return pin + 0x10  # This adds 0x10 to the pin number to identify analog pins


# Read digital pin
def req_read_digital(pin):
    if (pin < 0 or pin > 10):
        return 0xff
    return pin


while True:
    # # Turn LEDs on (pins 3 to 9)
    # for i in range(3, 10):
    #     send_byte = req_set_digital(i, True)
    #     rcv_byte = spi.xfer2([send_byte])
    #     time.sleep(0.5)
    
    # # Turn LEDs off (pins 3 to 9)
    # for i in range(3, 10):
    #     send_byte = req_set_digital(i, False)
    #     rcv_byte = spi.xfer2([send_byte])
    #     time.sleep(0.5)
    

    # Get status of analog pin A3 (pin 3)
    send_byte = req_read_analog(3)  # Request for analog pin A3
    rcv_byte = spi.xfer2([send_byte])
    time.sleep(0.1)
    send_byte = 0xf0  # Send null request to finish communication
    rcv_byte = spi.xfer2([send_byte])
    print("Value of A3 is " + str(rcv_byte[0]))
