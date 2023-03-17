import machine
import time
import PikaStdDevice as std


def led():
    """
    led 例程
    led0、led1、led2 依次闪烁 10 次
    led0 和 led1 是 sipeed m0p 的板载 led
    led2 是 verimake bl618 竞赛板的板载 led
    """
    led0 = machine.Led(0)
    led1 = machine.Led(1)
    led2 = machine.Led(2)
    for i in range(10):
        print('Blinking (%d/10)' % (i + 1))
        led0.on()
        led1.off()
        led2.off()
        time.sleep(0.1)
        led0.off()
        led1.on()
        led2.off()
        time.sleep(0.1)
        led0.off()
        led1.off()
        led2.on()
    led0.off()
    led1.off()
    led2.off()
    led0.close()
    led1.close()
    led2.close()


def key():
    """
    key 例程
    等待 key0 被按下 3 次, 使用阻塞方式
    key0 是 verimake bl618 竞赛板的板载按键
    """
    key0 = machine.Key(0)
    print('Waiting for key0 to be pressed')
    count_expect = 3
    count = 0
    while True:
        if key0.isPressed():
            print('Key0 is pressed (%d/%d)' % (count + 1, count_expect))
            count += 1
            if count == count_expect:
                break
        time.sleep(0.1)
    key0.close()


count = 0
count_expect = 3


def _handle_key0(siganl):
    """
    key 回调例程的回调函数
    """
    global count
    print('Key0 is pressed (%d/%d)' % (count + 1, count_expect))
    count += 1


def key_callback():
    """
    key 回调例程
    等待 key0 被按下 3 次, 使用回调方式
    key0 是 verimake bl618 竞赛板的板载按键
    """
    global count
    count = 0
    key0 = machine.Key(0)
    print('Waiting for key0 to be pressed')
    key0.setCallBack(_handle_key0, std.GPIO.SIGNAL_FALLING)
    while True:
        if count == count_expect:
            break
        time.sleep(0.1)
    key0.close()


def adc():
    """
    adc 例程
    读取 adc0 的值 10 次
    adc 接在 P0 引脚
    """
    adc0 = machine.ADC()
    adc0.setPin('P0')
    adc0.enable()
    for i in range(10):
        print('ADC0: %f (%d/%d)' % (adc0.read(), i, 10))
        time.sleep(0.5)


def dac():
    """
    dac 例程
    输出 0.5V 的方波 10 次
    dac 接在 P3 引脚
    adc 接在 P0 引脚, 实验时可以用杜邦线将 P3 和 P0 连接起来
    """
    dac0 = machine.DAC()
    dac0.setPin('P3')
    dac0.enable()
    adc0 = machine.ADC()
    adc0.setPin('P0')
    adc0.enable()
    val_max = 1.5
    val_min = 1
    for i in range(10):
        dac0.write(val_max)
        print('DAC0: %r, ADC0: %r (%d/%d)' % (val_max, adc0.read(), i+1, 10))
        time.sleep(0.5)
        dac0.write(val_min)
        print('DAC0: %r, ADC0: %r (%d/%d)' % (val_min, adc0.read(), i+1, 10))
        time.sleep(0.5)
    dac0.disable()
    adc0.disable()
