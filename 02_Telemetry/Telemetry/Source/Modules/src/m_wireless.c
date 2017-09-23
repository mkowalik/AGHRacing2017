
/* LICENSE */

#include "app_config.h"
#if CONFIG_WIRELESS_ENABLED
#include "m_wireless.h"

drv_rf24_t m_handler;

void m_wireless_init(void)
{
	drv_hal_spi_init(m_handler.hal_handler);
	if(drv_rf24_is_chip_connected(&m_handler) == false)
	{
		return;
	}
	drv_rf24_set_mode(&m_handler, DRV_RF24_MODE_TX);
	drv_rf24_set_channel(&m_handler, WIRELESS_CHANNEL);
	drv_rf24_set_address_width(&m_handler, 3);
	drv_rf24_set_tx_address(&m_handler, "AGH");
	drv_rf24_set_payload_size(&m_handler, 0, 32);
	drv_rf24_set_tx_power(&m_handler, DRV_RF24_TXPWR_0dBm);
	drv_rf24_set_data_rate(&m_handler, DRV_RF24_DR_2Mbps);
	drv_rf24_set_crc_length(&m_handler, DRV_RF24_CRC_1byte);
	drv_rf24_set_retries(&m_handler, DRV_RF24_ARD_500us, 15);
	drv_rf24_mask_irq(&m_handler, true, true, false);

	drv_rf24_flush_rx(&m_handler);
	drv_rf24_flush_tx(&m_handler);
	drv_rf24_power_up(&m_handler);
}

#endif /* CONFIG_WIRELESS_ENABLED */
