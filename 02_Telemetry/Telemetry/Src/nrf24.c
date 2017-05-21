// Functions to manage the nRF24L01+ transceiver


#include "nrf24.h"
#include "nRF24L01p.h"
#include "main.h"

/**
  * @brief  Read a register.
  * @param  reg: register to be read
  * @retval Value of register.
  */
static uint8_t nRF24_ReadReg(uint8_t reg) {

	uint8_t value;

	nRF24_CSN_L();
	nRF24_LL_RW(reg & nRF24_MASK_REG_MAP);
	value = nRF24_LL_RW(nRF24_CMD_NOP);
	nRF24_CSN_H();

	return value;
}

/**
  * @brief  Write a register.
  * @param  reg: register to be written
  * @retval Value to be written
  */
static void nRF24_WriteReg(uint8_t reg, uint8_t value) {

	nRF24_CSN_L();
	if (reg < nRF24_CMD_W_REGISTER) {
		// This is a register access
		nRF24_LL_RW(nRF24_CMD_W_REGISTER | (reg & nRF24_MASK_REG_MAP));
		nRF24_LL_RW(value);
	} else {
		// This is a single byte command or future command/register
		nRF24_LL_RW(reg);
		if ((reg != nRF24_CMD_FLUSH_TX) && (reg != nRF24_CMD_FLUSH_RX) && \
				(reg != nRF24_CMD_REUSE_TX_PL) && (reg != nRF24_CMD_NOP)) {
			// Send register value
			nRF24_LL_RW(value);
		}
	}
	nRF24_CSN_H();
}

/**
  * @brief  Read a multi-byte register.
  * @param  reg: register to be read
  * @param	pBuf: pointer to buffer for register data
  * @param	count: number of bytes to be read
  */
static void nRF24_ReadMBReg(uint8_t reg, uint8_t *pBuf, uint8_t count) {

	nRF24_CSN_L();
	nRF24_LL_RW(reg);
	while (count--) {
		*pBuf++ = nRF24_LL_RW(nRF24_CMD_NOP);
	}
	nRF24_CSN_H();
}

/**
  * @brief  Write a multi-byte register.
  * @param  reg: register to be written
  * @param	pBuf: pointer to buffer with register data
  * @param	count: number of bytes to be written
  */
static void nRF24_WriteMBReg(uint8_t reg, uint8_t *pBuf, uint8_t count) {

	nRF24_CSN_L();
	nRF24_LL_RW(reg);
	while (count--) {
		nRF24_LL_RW(*pBuf++);
	}
	nRF24_CSN_H();
}

/**
  * @brief  Initialize chip
  */
nRF24_StatusTypeDef nRF24_Init(uint32_t timeout) {

	uint32_t tickstart = HAL_GetTick();

	while(nRF24_OK != nRF24_Check()){
		if((HAL_GetTick()-tickstart) > timeout){
			return nRF24_TIMEOUT;
		}
	}
	// Write to registers their initial values
	nRF24_WriteReg(nRF24_REG_CONFIG, 0x08);
	nRF24_WriteReg(nRF24_REG_EN_AA, 0x3F);
	nRF24_WriteReg(nRF24_REG_EN_RXADDR, 0x03);
	nRF24_WriteReg(nRF24_REG_SETUP_AW, 0x03);
	nRF24_WriteReg(nRF24_REG_SETUP_RETR, 0x03);
	nRF24_WriteReg(nRF24_REG_RF_CH, 0x02);
	nRF24_WriteReg(nRF24_REG_RF_SETUP, 0x0E);
	nRF24_WriteReg(nRF24_REG_STATUS, 0x00);
	nRF24_WriteReg(nRF24_REG_RX_PW_P0, 0x00);
	nRF24_WriteReg(nRF24_REG_RX_PW_P1, 0x00);
	nRF24_WriteReg(nRF24_REG_RX_PW_P2, 0x00);
	nRF24_WriteReg(nRF24_REG_RX_PW_P3, 0x00);
	nRF24_WriteReg(nRF24_REG_RX_PW_P4, 0x00);
	nRF24_WriteReg(nRF24_REG_RX_PW_P5, 0x00);
	nRF24_WriteReg(nRF24_REG_DYNPD, 0x00);
	nRF24_WriteReg(nRF24_REG_FEATURE, 0x00);

	// Clear the FIFO's
	nRF24_FlushRX();
	nRF24_FlushTX();

	// Clear any pending interrupt flags
	nRF24_ClearIRQFlags();

	return nRF24_OK;
}

/**
  * @brief  Check if chip responds.
  * @note	Call this before calling any other function
  * 		to be sure that connection is OK.
  * @retval	nRF24 status
  */
nRF24_StatusTypeDef nRF24_Check(void) {

	uint8_t rxAddr[5];
	uint8_t *txAddr = (uint8_t *)nRF24_TEST_ADDR;

	// Write test TX address and read TX_ADDR register
	nRF24_WriteMBReg(nRF24_CMD_W_REGISTER | nRF24_REG_TX_ADDR, txAddr, 5);
	nRF24_ReadMBReg(nRF24_CMD_R_REGISTER | nRF24_REG_TX_ADDR, rxAddr, 5);

	// Compare buffers, return error on first mismatch
	for (uint8_t i = 0; i < 5; i++) {
		if (rxAddr[i] != *txAddr++) return nRF24_ERROR;
	}

	return nRF24_OK;
}

/**
  * @brief  Set power mode of chip.
  * @param	mode: power mode, value of nRF24_powerMode_t.
  */
void nRF24_SetPowerMode(nRF24_powerMode_t mode) {

	uint8_t reg;

	reg = nRF24_ReadReg(nRF24_REG_CONFIG);
	if (mode == nRF24_PWR_UP) {
		// Set the PWR_UP bit of CONFIG register to wake the transceiver
		// It goes into Stanby-I mode with consumption about 26uA
		reg |= nRF24_CONFIG_PWR_UP;
	} else {
		// Clear the PWR_UP bit of CONFIG register to put the transceiver
		// into power down mode with consumption about 900nA
		reg &= ~nRF24_CONFIG_PWR_UP;
	}
	nRF24_WriteReg(nRF24_REG_CONFIG, reg);
}

/**
  * @brief  Set operational mode of chip.
  * @param	mode: operational mode, value of nRF24_mode_t.
  */
void nRF24_SetOperationalMode(nRF24_mode_t mode) {

	uint8_t reg;

	// Configure PRIM_RX bit of the CONFIG register
	reg  = nRF24_ReadReg(nRF24_REG_CONFIG);
	reg &= ~nRF24_CONFIG_PRIM_RX;
	reg |= (mode & nRF24_CONFIG_PRIM_RX);
	nRF24_WriteReg(nRF24_REG_CONFIG, reg);
}

/**
  * @brief  Configure transceiver CRC scheme.
  * @param	scheme:  CRC scheme, value of nRF24_crcScheme_t.
  * @note	transceiver will forcibly turn on the CRC in case if auto acknowledgment
  * 		enabled for at least one RX pipe.
  */
void nRF24_SetCRCScheme(nRF24_crcScheme_t scheme) {

	uint8_t reg;

	// Configure EN_CRC[3] and CRCO[2] bits of the CONFIG register
	reg  = nRF24_ReadReg(nRF24_REG_CONFIG);
	reg &= ~nRF24_MASK_CRC;
	reg |= (scheme | nRF24_MASK_CRC);
	nRF24_WriteReg(nRF24_REG_CONFIG, reg);
}

/**
  * @brief  Set frequency channele.
  * @param	channel: radio frequency channel, value from 0 to 125.
  * @note	Frequency will be (2400 + channel)MHz.
  * @note	PLOS_CNT[7:4] bits of the OBSERVER_TX register will be reset.
  * @retval	nRF24 status.
  */
nRF24_StatusTypeDef nRF24_SetRFChannel(uint8_t channel) {

	if(channel > 125){
		return nRF24_ERROR;
	}
	nRF24_WriteReg(nRF24_REG_RF_CH, channel);
	return nRF24_OK;
}

/**
  * @brief  Set automatic retransmission parameters.
  * @param	ard: auto retransmit delay, value of nRF24_ardDelay_t.
  * @param	arc: count of auto retransmits, value from 0 to 15.
  * @note	Zero arc value means that the automatic retransmission disabled.
  * @retval	nRF24 status.
  */
nRF24_StatusTypeDef nRF24_SetAutoRetr(nRF24_ardDelay_t ard, uint8_t arc) {

	if(arc > 15){
		return nRF24_ERROR;
	}
	// Set auto retransmit settings (SETUP_RETR register)
	nRF24_WriteReg(nRF24_REG_SETUP_RETR, (uint8_t)((ard << 4) | (arc & nRF24_MASK_RETR_ARC)));
	return nRF24_OK;
}

/**
  * @brief  Set of address width.
  * @param	addrWidth: RX/TX address field width, value from 3 to 5.
  * @note	This setting is common for all pipes.
  * @retval	nRF24 status.
  */
nRF24_StatusTypeDef nRF24_SetAddrWidth(uint8_t addrWidth) {
	if((addrWidth > 5) || (addrWidth < 3)){
		return nRF24_ERROR;
	}
	nRF24_WriteReg(nRF24_REG_SETUP_AW, addrWidth - 2);
	return nRF24_OK;
}

/**
  * @brief  Set static RX address for a specified pipe.
  * @param	pipe: pipe number, value of nRF24_pipe_t.
  * @param	addr: pointer to the buffer with address.
  * @note: 	pipe can be a number from 0 to 5 (RX pipes) and 6 (TX pipe)
  * @note: 	buffer length must be equal to current address width of transceiver
  * @note: 	for pipes[2..5] only first byte of address will be written because
  * 	   	other bytes of address equals to pipe1
  * @note: 	for pipes[2..5] only first byte of address will be written because
  * 	   	pipes 1-5 share the four most significant address bytes
  * @retval	nRF24 status.
  */
nRF24_StatusTypeDef nRF24_SetAddr(nRF24_pipe_t pipe, const uint8_t *addr) {

	uint8_t addrWidth;

	// RX_ADDR_Px register
	switch (pipe) {

	case nRF24_PIPETX:

	case nRF24_PIPE0:

	case nRF24_PIPE1:
		// Get address width
		addrWidth = nRF24_ReadReg(nRF24_REG_SETUP_AW) + 1;
		// Write address in reverse order (LSByte first)
		addr += addrWidth;
		nRF24_CSN_L();
		nRF24_LL_RW(nRF24_CMD_W_REGISTER | nRF24_ADDR_REGS[pipe]);
		do {
			nRF24_LL_RW(*addr--);
		} while (addrWidth--);
		nRF24_CSN_H();
		break;

	case nRF24_PIPE2:

	case nRF24_PIPE3:

	case nRF24_PIPE4:

	case nRF24_PIPE5:
		// Write address LSBbyte (only first byte from the addr buffer)
		nRF24_WriteReg(nRF24_ADDR_REGS[pipe], *addr);
		break;

	default:
		return nRF24_ERROR;
		break;

	}
	return nRF24_OK;
}

/**
  * @brief  Configure RF output power in TX mode.
  * @param	txPwr: RF output power, value of nRF24_txPwr_t.
  */
void nRF24_SetTXPower(nRF24_txPwr_t txPwr) {

	uint8_t reg;

	// Configure RF_PWR[2:1] bits of the RF_SETUP register
	reg  = nRF24_ReadReg(nRF24_REG_RF_SETUP);
	reg &= ~nRF24_MASK_RF_PWR;
	reg |= txPwr;
	nRF24_WriteReg(nRF24_REG_RF_SETUP, reg);
}

/**
  * @brief  Configure transceiver data rate.
  * @param	txPwr: data rate, value of nRF24_DataRate_t.
  */
void nRF24_SetDataRate(nRF24_DataRate_t dataRate) {

	uint8_t reg;

	// Configure RF_DR_LOW[5] and RF_DR_HIGH[3] bits of the RF_SETUP register
	reg  = nRF24_ReadReg(nRF24_REG_RF_SETUP);
	reg &= ~nRF24_MASK_DATARATE;
	reg |= dataRate;
	nRF24_WriteReg(nRF24_REG_RF_SETUP, reg);
}

/**
  * @brief  Configure a specified RX pipe.
  * @param	pipe: number of the RX pipe, value from 0 to 5.
  * @param	aaState: state of auto acknowledgment, value of nRF24_autoAcknowledgment_t.
  * @param	payloadLen: payload length in bytes.
  * @retval	nRF24 status.
  */
nRF24_StatusTypeDef nRF24_SetRXPipe(nRF24_pipe_t pipe, nRF24_autoAcknowledgment_t aaState, uint8_t payloadLen) {

	uint8_t reg;

	if(pipe == nRF24_PIPETX){
		return nRF24_ERROR;
	}
	// Enable the specified pipe (EN_RXADDR register)
	reg = (nRF24_ReadReg(nRF24_REG_EN_RXADDR) | (1 << pipe)) & nRF24_MASK_EN_RX;
	nRF24_WriteReg(nRF24_REG_EN_RXADDR, reg);

	// Set RX payload length (RX_PW_Px register)
	nRF24_WriteReg(nRF24_RX_PW_PIPE[pipe], payloadLen & nRF24_MASK_RX_PW);

	// Set auto acknowledgment for a specified pipe (EN_AA register)
	reg = nRF24_ReadReg(nRF24_REG_EN_AA);
	if (aaState == nRF24_AA_ON) {
		reg |=  (1 << pipe);
	} else {
		reg &= ~(1 << pipe);
	}
	nRF24_WriteReg(nRF24_REG_EN_AA, reg);

	return nRF24_OK;
}

/**
  * @brief  Disable specified RX pipe.
  * @param	pipe: number of the RX pipe, value from 0 to 5.
  * @retval	nRF24 status.
  */
nRF24_StatusTypeDef nRF24_ClosePipe(nRF24_pipe_t pipe) {

	uint8_t reg;

	if(pipe == nRF24_PIPETX){
		return nRF24_ERROR;
	}
	reg  = nRF24_ReadReg(nRF24_REG_EN_RXADDR);
	reg &= ~(1 << pipe);
	reg &= nRF24_MASK_EN_RX;
	nRF24_WriteReg(nRF24_REG_EN_RXADDR, reg);

	return nRF24_OK;
}

/**
  * @brief  Enable the auto retransmit (a.k.a. enhanced ShockBurst) for the specified RX pipe.
  * @param	pipe: number of the RX pipe, value from 0 to 5.
  * @retval	nRF24 status.
  */
nRF24_StatusTypeDef nRF24_EnableAA(uint8_t pipe) {

	uint8_t reg;

	if(pipe == nRF24_PIPETX){
		return nRF24_ERROR;
	}
	// Set bit in EN_AA register
	reg  = nRF24_ReadReg(nRF24_REG_EN_AA);
	reg |= (1 << pipe);
	nRF24_WriteReg(nRF24_REG_EN_AA, reg);

	return nRF24_OK;
}

/**
  * @brief  Disable the auto retransmit (a.k.a. enhanced ShockBurst) for one or all RX pipes.
  * @param	pipe: number of the RX pipe, value from 0 to 5, any other value will disable AA for all RX pipes.
  */
void nRF24_DisableAA(uint8_t pipe) {

	uint8_t reg;

	if (pipe > 5) {
		// Disable Auto-ACK for ALL pipes
		nRF24_WriteReg(nRF24_REG_EN_AA, 0x00);
	} else {
		// Clear bit in the EN_AA register
		reg  = nRF24_ReadReg(nRF24_REG_EN_AA);
		reg &= ~(1 << pipe);
		nRF24_WriteReg(nRF24_REG_EN_AA, reg);
	}
}

/**
  * @brief  Get value of the STATUS register.
  * @retval	STATUS register value.
  */
uint8_t nRF24_GetStatus(void) {
	return nRF24_ReadReg(nRF24_REG_STATUS);
}

/**
  * @brief  Get pending IRQ flags.
  * @retval	Current status of RX_DR, TX_DS and MAX_RT bits of the STATUS register.
  */
uint8_t nRF24_GetIRQFlags(void) {
	return (nRF24_ReadReg(nRF24_REG_STATUS) & nRF24_MASK_STATUS_IRQ);
}

/**
  * @brief  Get status of the RX FIFO.
  * @retval	value of nRF24_STATUS_RXFIFO_t.
  */
uint8_t nRF24_GetStatus_RXFIFO(void) {
	return (nRF24_ReadReg(nRF24_REG_FIFO_STATUS) & nRF24_MASK_RXFIFO);
}

/**
  * @brief  Get status of the TX FIFO.
  * @retval	Value of nRF24_STATUS_RXFIFO_t.
  * @note	The TX_REUSE bit ignored.
  */
uint8_t nRF24_GetStatus_TXFIFO(void) {
	return ((nRF24_ReadReg(nRF24_REG_FIFO_STATUS) & nRF24_MASK_TXFIFO) >> 4);
}

/**
  * @brief  Get pipe number for the payload available for reading from RX FIFO.
  * @retval	Pipe number or 0x07 if the RX FIFO is empty
  */
uint8_t nRF24_GetRXSource(void) {
	return ((nRF24_ReadReg(nRF24_REG_STATUS) & nRF24_MASK_RX_P_NO) >> 1);
}

/**
  * @brief  Get auto retransmit statistic.
  * @retval	Value of OBSERVE_TX register which contains two counters encoded in nibbles:
  * 	  	high - lost packets count (max value 15, can be reseted by write to RF_CH register)
  * 		low  - retransmitted packets count (max value 15, reseted when new transmission starts)
  */
uint8_t nRF24_GetRetransmitCounters(void) {
	return (nRF24_ReadReg(nRF24_REG_OBSERVE_TX));
}


/**
  * @brief  Reset packet lost counter (PLOS_CNT bits in OBSERVER_TX register).
  */
void nRF24_ResetPLOS(void) {

	uint8_t reg;

	// The PLOS counter is reset after write to RF_CH register
	reg = nRF24_ReadReg(nRF24_REG_RF_CH);
	nRF24_WriteReg(nRF24_REG_RF_CH, reg);
}

/**
  * @brief  Flush the TX FIFO.
  */
void nRF24_FlushTX(void) {
	nRF24_WriteReg(nRF24_CMD_FLUSH_TX, nRF24_CMD_NOP);
}

/**
  * @brief  Flush the RX FIFO.
  */
void nRF24_FlushRX(void) {
	nRF24_WriteReg(nRF24_CMD_FLUSH_RX, nRF24_CMD_NOP);
}

/**
  * @brief  Clear any pending IRQ flags.
  */
void nRF24_ClearIRQFlags(void) {

	uint8_t reg;

	// Clear RX_DR, TX_DS and MAX_RT bits of the STATUS register
	reg  = nRF24_ReadReg(nRF24_REG_STATUS);
	reg |= nRF24_MASK_STATUS_IRQ;
	nRF24_WriteReg(nRF24_REG_STATUS, reg);
}

/**
  * @brief  Write TX payload.
  * @param	pBuf: pointer to the buffer with payload data.
  * @param	length: payload length in bytes.
  */
void nRF24_WritePayload(uint8_t *pBuf, uint8_t length) {
	nRF24_WriteMBReg(nRF24_CMD_W_TX_PAYLOAD, pBuf, length);
}

/**
  * @brief  Read top level payload available in the RX FIFO.
  * @param	pBuf: pointer to the buffer to store a payload data.
  * @param	length: pointer to variable to store a payload length.
  * @retval	One of nRF24_RXResult values.
  * 		nRF24_RX_PIPEX - packet has been received from the pipe number X
  * 		nRF24_RX_EMPTY - the RX FIFO is empty
  */
nRF24_RXResult nRF24_ReadPayload(uint8_t *pBuf, uint8_t *length) {
	uint8_t pipe;

	// Extract a payload pipe number from the STATUS register
	pipe = (nRF24_ReadReg(nRF24_REG_STATUS) & nRF24_MASK_RX_P_NO) >> 1;

	// RX FIFO empty?
	if (pipe < 6) {
		// Get payload length
		*length = nRF24_ReadReg(nRF24_RX_PW_PIPE[pipe]);

		// Read a payload from the RX FIFO
		if (*length) {
			nRF24_ReadMBReg(nRF24_CMD_R_RX_PAYLOAD, pBuf, *length);
		}

		return ((nRF24_RXResult)pipe);
	}

	// The RX FIFO is empty
	*length = 0;

	return nRF24_RX_EMPTY;
}

void nRF24_TransmitPacketIRQ(uint8_t *pBuf, uint8_t length){
	nRF24_CE_L();
	// Transfer a data from the specified buffer to the TX FIFO
	nRF24_WritePayload(pBuf, length);
	// Start a transmission by asserting CE pin (must be held at least 10us)
	nRF24_CE_H();
}
