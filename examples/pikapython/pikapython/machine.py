import PikaStdDevice


class UART(PikaStdDevice.UART):
    pass


class GPIO(PikaStdDevice.GPIO):
    pass


class ADC(PikaStdDevice.ADC):
    pass


class DAC(PikaStdDevice.DAC):
    pass


class LED(GPIO):
    pin: str = None

    def __init__(self, id: int):
        super().__init__()
        if id == 0:
            self.pin = 'P27'
        elif id == 1:
            self.pin = 'P28'
        self.setMode('out')
        self.enable()
        self.high()

    def on(self):
        self.low()

    def off(self):
        self.high()
