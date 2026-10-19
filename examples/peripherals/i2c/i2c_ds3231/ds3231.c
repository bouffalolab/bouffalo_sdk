#include "bflb_i2c.h"
#include "ds3231.h"
#include "ds3231_reg.h"

#define DS3231_ADDRESS 	    0x68  // DS3231 I2C address: 0x68

static struct bflb_device_s *i2c0_dev = NULL;

/**
  * @brief  DS3231 write register
  * @param  RegAddress Register address
  * @param  Data Data to write to register, range: 0x00~0xFF
  * @retval None
  */
void ds3231_write_reg(uint8_t RegAddress, uint8_t Data)
{
	struct bflb_i2c_msg_s msgs[1];
	uint8_t send_data[2];

	/* Prepare register address and data */
	send_data[0] = RegAddress;
	send_data[1] = Data;

	/* Configure I2C message */
	msgs[0].addr = DS3231_ADDRESS;
	msgs[0].flags = 0;  /* Standard write operation */
	msgs[0].buffer = send_data;
	msgs[0].length = 2;

	/* Send data */
	bflb_i2c_transfer(i2c0_dev, msgs, 1);
}

/**
  * @brief  DS3231 read register
  * @param  RegAddress Register address
  * @retval Data read from register, range: 0x00~0xFF
  */
uint8_t ds3231_read_reg(uint8_t RegAddress)
{
	struct bflb_i2c_msg_s msgs[2];
	uint8_t read_data;

	/* First message: send register address */
	msgs[0].addr = DS3231_ADDRESS;
	msgs[0].flags = I2C_M_NOSTOP;  /* No STOP, for subsequent repeated START */
	msgs[0].buffer = &RegAddress;
	msgs[0].length = 1;

	/* Second message: read data */
	msgs[1].addr = DS3231_ADDRESS;
	msgs[1].flags = I2C_M_READ | I2C_M_NOSTART;  /* Read operation, no START */
	msgs[1].buffer = &read_data;
	msgs[1].length = 1;

	/* Transfer using repeated START */
	bflb_i2c_transfer(i2c0_dev, msgs, 2);

	return read_data;
}

void ds3231_init(void)
{
	/* Get I2C0 device */
	if (i2c0_dev == NULL) {
		i2c0_dev = bflb_device_get_by_name("i2c0");
	}

	/* Initialize I2C, frequency set to 400kHz */
	bflb_i2c_init(i2c0_dev, 400000);

	/* Initialize DS3231 control register */
	ds3231_write_reg(DS3231_CONTROL_REG, 0x1C);
}

/**
  * @brief  BCD(8421) to DEC conversion
  * @param  val: BCD code
  * @param  i: DEC code
  * @retval DEC value
  */
uint8_t bsc_to_dec(uint8_t val)
{
	uint8_t i;
	i= val&0x0f;
	val >>= 4;
	val &= 0x0f;
	val *= 10;
	i += val;
	return i;
}

/**
  * @brief  DEC to BCD(8421) conversion
  * @param  val: DEC code
  * @param  k: BCD code
  * @retval BCD value
  */
uint8_t dec_to_bcd(uint8_t val)
{
    uint8_t i,j,k;
    i=val/10;
    j=val%10;
    k=j+(i<<4);
    return k;
}

/**
  * @brief  Time setting
  * @param  year, month, date, week, hour, minute, second
  * @retval None
  */
void ds3231_set_time(uint8_t year,uint8_t mon,uint8_t da,uint8_t week,uint8_t hour,uint8_t min,uint8_t sec)
{

    ds3231_write_reg(0x06,dec_to_bcd(year));

    ds3231_write_reg(0x05,dec_to_bcd(mon));

    ds3231_write_reg(0x04,dec_to_bcd(da));

    ds3231_write_reg(0x03,dec_to_bcd(week));

    ds3231_write_reg(0x02,dec_to_bcd(hour));

    ds3231_write_reg(0x01,dec_to_bcd(min));

    ds3231_write_reg(0x00,dec_to_bcd(sec));

}

/**
  * @brief  DS3231 get time data
  * @param  year, month, date, week, hour, min, sec
  *         Returns values via output parameters, range: 0-255
  * @retval None
  */
void ds3231_get_time(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *week, uint8_t *hour, uint8_t *min, uint8_t *sec)
{
	*year=ds3231_read_reg(0x06);
	*year=bsc_to_dec(*year);

	*month=ds3231_read_reg(0x05);
	*month=bsc_to_dec(*month);

	*date=ds3231_read_reg(0x04);
	*date=bsc_to_dec(*date);

	*week=ds3231_read_reg(0x03);
	*week=bsc_to_dec(*week);

	*hour=ds3231_read_reg(0x02);
	*hour&=0x3f;  // Valid for 12-hour format, removes hour format bit reading
	*hour=bsc_to_dec(*hour);

	*min=ds3231_read_reg(0x01);
	*min=bsc_to_dec(*min);

	*sec=ds3231_read_reg(0x00);
	*sec=bsc_to_dec(*sec);
}


