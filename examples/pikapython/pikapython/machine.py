import PikaStdDevice


class UART(PikaStdDevice.UART):
    pass


class GPIO(PikaStdDevice.GPIO):
    pass


class ADC(PikaStdDevice.ADC):
    pass


class DAC(PikaStdDevice.DAC):
    pass


class Led(GPIO):
    pin: str = None

    def __init__(self, id: int):
        super().__init__()
        if id == 0:
            self.pin = 'P27'
        elif id == 1:
            self.pin = 'P28'
        elif id == 2:
            self.pin = 'P32'
        self.setMode('out')
        self.enable()
        self.high()

    def on(self):
        self.low()

    def off(self):
        self.high()

class Key(GPIO):
    pin: str = None

    def __init__(self, id: int):
        super().__init__()
        if id == 0:
            self.pin = 'P33'
            self.setPull('up')
        self.setMode('in')
        self.enable()
    
    def isPressed(self):
        return self.read() == 0
