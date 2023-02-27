import machine
import time

def led_blink():
    led0 = machine.LED(0)
    led1 = machine.LED(1)
    for i in range(10):
        led0.on()
        led1.off()
        time.sleep(0.1)
        led0.off()
        led1.on()
        time.sleep(0.1)
    led0.off()
    led1.off()
    led0.close()
    led1.close()

