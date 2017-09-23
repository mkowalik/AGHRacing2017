
/* LICENSE */

#ifndef DRV_MPU6050_H_
#define DRV_MPU6050_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "inttypes.h"

/* Default I2C used */
#ifndef MPU6050_I2C
#define MPU6050_I2C                    I2C1              /*!< Default I2C */
#define MPU6050_I2C_PINSPACK           0x00			     //TM_I2C_PinsPack_1 /*!< Default I2C pinspack. Check @ref TM_I2C for more information */
#define MPU6050_I2C_INSTANCE           hi2c1
#endif

/* Default I2C clock */
#ifndef MPU6050_I2C_CLOCK
#define MPU6050_I2C_CLOCK              400000            /*!< Default I2C clock speed */
#endif

/**
 * @brief  Data rates
 */
typedef enum
{
	drv_mpu6050_data_rate_8khz	= 0,	/*!< Sample rate set to 8 kHz  */
	drv_mpu6050_data_rate_4khz  = 1,	/*!< Sample rate set to 4 kHz  */
	drv_mpu6050_data_rate_2khz  = 2,	/*!< Sample rate set to 2 kHz  */
	drv_mpu6050_data_rate_1khz  = 7,	/*!< Sample rate set to 1 kHz  */
	drv_mpu6050_data_rate_500hz = 15,	/*!< Sample rate set to 500 Hz */
	drv_mpu6050_data_rate_250hz = 31,	/*!< Sample rate set to 250 Hz */
	drv_mpu6050_data_rate_125hz = 63,	/*!< Sample rate set to 125 Hz */
	drv_mpu6050_data_rate_100hz = 79	/*!< Sample rate set to 100 Hz */
}drv_mpu6050_data_rate_t;

/**
 * @brief  MPU6050 can have 2 different slave addresses, depends on it's input AD0 pin
 *         This feature allows you to use 2 different sensors with this library at the same time
 */
typedef enum
{
    drv_mpu6050_device_0 = 0x00, /*!< AD0 pin is set to low */
    drv_mpu6050_device_1 = 0x02  /*!< AD0 pin is set to high */
}drv_mpu6050_device_t;

/**
 * @brief  MPU6050 result enumeration
 */
typedef enum
{
    drv_mpu6050_ok = 0x00,            /*!< Everything OK */
    drv_mpu6050_error,                /*!< Unknown error */
    drv_mpu6050_device_not_connected, /*!< There is no device with valid slave address */
    drv_mpu6050_device_invalid        /*!< Connected device with address is not MPU6050 */
}drv_mpu6050_result_t;

/**
 * @brief  Parameters for accelerometer range
 */
typedef enum
{
    drv_mpu6050_accelerometer_2g  = 0x00, /*!< Range is +- 2G */
    drv_mpu6050_accelerometer_4g  = 0x01, /*!< Range is +- 4G */
    drv_mpu6050_accelerometer_8g  = 0x02, /*!< Range is +- 8G */
    drv_mpu6050_accelerometer_16g = 0x03  /*!< Range is +- 16G */
}drv_mpu6050_accelerometer_t;

/**
 * @brief  Parameters for gyroscope range
 */
typedef enum
{
    drv_mpu6050_gyroscope_250s  = 0x00, /*!< Range is +- 250 degrees/s */
    drv_mpu6050_gyroscope_500s  = 0x01, /*!< Range is +- 500 degrees/s */
    drv_mpu6050_gyroscope_1000s = 0x02, /*!< Range is +- 1000 degrees/s */
    drv_mpu6050_gyroscope_2000s = 0x03  /*!< Range is +- 2000 degrees/s */
} drv_mpu6050_gyroscope_t;

/**
 * @brief  Parameters for low pass filter bandwidth
 */
typedef enum
{
    drv_mpu6050_bandwidth_260Hz = 0,
    drv_mpu6050_bandwidth_184Hz = 1,
    drv_mpu6050_bandwidth_94Hz  = 2,
    drv_mpu6050_bandwidth_44Hz  = 3,
    drv_mpu6050_bandwidth_21Hz  = 4,
    drv_mpu6050_bandwidth_10Hz  = 5,
    drv_mpu6050_bandwidth_5Hz   = 6
}drv_mpu6050_digital_lp_filter_t;

/**
 * @brief  Main MPU6050 structure
 */
typedef struct
{
    uint8_t address;
    void    *drv_instance;
    drv_mpu6050_data_rate_t data_rate;
    drv_mpu6050_digital_lp_filter_t filter_bandwidth;
}drv_mpu6050_t;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
}drv_mpu6050_axes_values_t;

typedef struct
{
    drv_mpu6050_axes_values_t raw;
    float                     divider;
}drv_mpu6050_data_t;


/**
 * @brief  Interrupts union and structure
 */
typedef union
{
    struct
    {
        uint8_t data_ready:1;       /*!< Data ready interrupt */
        uint8_t reserved2:2;        /*!< Reserved bits */
        uint8_t master:1;           /*!< Master interrupt. Not enabled with library */
        uint8_t fifo_overflow:1;    /*!< FIFO overflow interrupt. Not enabled with library */
        uint8_t reserved1:1;        /*!< Reserved bit */
        uint8_t motion_detection:1; /*!< Motion detected interrupt */
        uint8_t reserved0:1;        /*!< Reserved bit */
    }flag;
    uint8_t status;
} drv_mpu6050_interrupt_t;

/**
 * @}
 */

/**
 * @defgroup drv_mpu6050_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes MPU6050 and I2C peripheral
 * @param  *DataStruct: Pointer to empty @ref drv_mpu6050_t structure
 * @param  DeviceNumber: MPU6050 has one pin, AD0 which can be used to set address of device.
 *          This feature allows you to use 2 different sensors on the same board with same library.
 *          If you set AD0 pin to low, then this parameter should be drv_mpu6050_Device_0,
 *          but if AD0 pin is high, then you should use drv_mpu6050_Device_1
 *
 *          Parameter can be a value of @ref drv_mpu6050_Device_t enumeration
 * @param  AccelerometerSensitivity: Set accelerometer sensitivity. This parameter can be a value of @ref drv_mpu6050_Accelerometer_t enumeration
 * @param  GyroscopeSensitivity: Set gyroscope sensitivity. This parameter can be a value of @ref drv_mpu6050_Gyroscope_t enumeration
 * @retval Initialization status:
 *            - drv_mpu6050_result_t: Everything OK
 *            - Other member: in other cases
 */
drv_mpu6050_result_t drv_mpu6050_init(drv_mpu6050_t* data_struct, drv_mpu6050_device_t dev_number);

/**
 * @brief  Sets gyroscope sensitivity
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure indicating MPU6050 device
 * @param  GyroscopeSensitivity: Gyro sensitivity value. This parameter can be a value of @ref drv_mpu6050_Gyroscope_t enumeration
 * @retval Member of @ref drv_mpu6050_result_t enumeration
 */
drv_mpu6050_result_t drv_mpu6050_set_gyr(drv_mpu6050_t* handler, drv_mpu6050_data_t *data, drv_mpu6050_gyroscope_t gyro_sens);

/**
 * @brief  Sets accelerometer sensitivity
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure indicating MPU6050 device
 * @param  AccelerometerSensitivity: Gyro sensitivity value. This parameter can be a value of @ref drv_mpu6050_Accelerometer_t enumeration
 * @retval Member of @ref drv_mpu6050_result_t enumeration
 */
drv_mpu6050_result_t drv_mpu6050_set_acc(drv_mpu6050_t* handler, drv_mpu6050_data_t *data, drv_mpu6050_accelerometer_t acc_sens);

/**
 * @brief  Sets output data rate
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure indicating MPU6050 device
 * @param  rate: Data rate value. An 8-bit value for prescaler value
 * @retval Member of @ref drv_mpu6050_result_t enumeration
 */
drv_mpu6050_result_t drv_mpu6050_set_data_rate(drv_mpu6050_t* data_struct, drv_mpu6050_data_rate_t data_rate);

/**
 * @brief  Sets digital low pass filter bandwidth
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure indicating MPU6050 device
 * @param  Bandwidth: drv_mpu6050_DigitalLowPassFilter_t value.
 * @retval Member of @ref drv_mpu6050_result_t enumeration
 */
drv_mpu6050_result_t drv_mpu6050_set_digital_lp_filter(drv_mpu6050_t* data_struct, drv_mpu6050_digital_lp_filter_t bandwidth);

/**
 * @brief  Enables interrupts
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure indicating MPU6050 device
 * @retval Member of @ref drv_mpu6050_result_t enumeration
 */
drv_mpu6050_result_t drv_mpu6050_enable_interrupts(drv_mpu6050_t* data_struct);

/**
 * @brief  Disables interrupts
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure indicating MPU6050 device
 * @retval Member of @ref drv_mpu6050_result_t enumeration
 */
drv_mpu6050_result_t drv_mpu6050_disable_interrupts(drv_mpu6050_t* data_struct);

/**
 * @brief  Reads and clears interrupts
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure indicating MPU6050 device
 * @param  *InterruptsStruct: Pointer to @ref drv_mpu6050_Interrupt_t structure to store status in
 * @retval Member of @ref drv_mpu6050_result_t enumeration
 */
drv_mpu6050_result_t drv_mpu6050_read_interrupts(drv_mpu6050_t* data_struct, drv_mpu6050_interrupt_t* interupts_struct);

/**
 * @brief  Reads accelerometer data from sensor
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure to store data to
 * @retval Member of @ref drv_mpu6050_result_t:
 *            - drv_mpu6050_Result_Ok: everything is OK
 *            - Other: in other cases
 */
drv_mpu6050_result_t drv_mpu6050_read_acc(drv_mpu6050_t* handler, drv_mpu6050_data_t *data);

/**
 * @brief  Reads gyroscope data from sensor
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure to store data to
 * @retval Member of @ref drv_mpu6050_result_t:
 *            - drv_mpu6050_Result_Ok: everything is OK
 *            - Other: in other cases
 */
drv_mpu6050_result_t drv_mpu6050_read_gyr(drv_mpu6050_t* handler, drv_mpu6050_data_t *data);

/**
 * @brief  Reads temperature data from sensor
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure to store data to
 * @retval Member of @ref drv_mpu6050_result_t:
 *            - drv_mpu6050_Result_Ok: everything is OK
 *            - Other: in other cases
 */
drv_mpu6050_result_t drv_mpu6050_read_temp(drv_mpu6050_t* handler, float *data);

/**
 * @brief  Reads accelerometer, gyroscope and temperature data from sensor
 * @param  *DataStruct: Pointer to @ref drv_mpu6050_t structure to store data to
 * @retval Member of @ref drv_mpu6050_result_t:
 *            - drv_mpu6050_Result_Ok: everything is OK
 *            - Other: in other cases
 */


#ifdef __cplusplus
}
#endif

#endif /* DRV_MPU6050_H_ */
