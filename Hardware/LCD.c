#include "LCD.h"

#define ILI9341_Delay(ms)  DWT_Delay(ms)

// ���ȫ�ֱ���
volatile bool spi_tx_complete = false;

// ˽�к�������
void WriteCommand(uint8_t cmd);
void WriteData(uint8_t data);
void SPI_Transmit(uint8_t data);
static void HardwareReset(void);

// DMA��ʼ��
void DMA_Init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    LL_DMA_InitTypeDef DMA_InitStruct = {0};
    DMA_InitStruct.PeriphOrM2MSrcAddress  = (uint32_t)&(LCD_SPI->DR);
    DMA_InitStruct.MemoryOrM2MDstAddress  = 0; // ��̬����
    DMA_InitStruct.Direction              = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    DMA_InitStruct.Mode                   = LL_DMA_MODE_NORMAL;
    DMA_InitStruct.PeriphOrM2MSrcIncMode  = LL_DMA_PERIPH_NOINCREMENT;
    DMA_InitStruct.MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_INCREMENT;
    DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
    DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
    DMA_InitStruct.NbData                 = 0; // ��̬����
    DMA_InitStruct.Priority               = LL_DMA_PRIORITY_HIGH;
    
    /* �ؼ��޸ģ�����SPI1_TX��DMAӳ����������� */
    #if defined(DMAMUX1)
    DMA_InitStruct.PeriphRequest          = LL_DMAMUX_REQ_SPI1_TX; // ʹ��DMAMUXʱ�������
    #else
    DMA_InitStruct.PeriphRequest          = LL_DMA_REQUEST_3;      // ��DMAMUXʱ�������
    #endif

    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_3, &DMA_InitStruct);

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);
    NVIC_SetPriority(DMA1_Channel3_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    LL_SPI_EnableDMAReq_TX(LCD_SPI);
}

void SPI_Transmit_DMA(uint8_t *data, uint32_t size)
{
    spi_tx_complete = false;
    
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)data);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, size);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
    //while(!spi_tx_complete); // �ȴ��������
}

void DMA1_Channel3_IRQHandler(void)
{
    if(LL_DMA_IsActiveFlag_TC3(DMA1))
    {
        LL_DMA_ClearFlag_TC3(DMA1);
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
        spi_tx_complete = true;
				fr_printf("DMA Transfer Complete\r\n");
    }
}

void ILI9341_Init(void)
{
  // ��ʼ��GPIO
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  // ʹ��SPI1��GPIOAʱ��
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

  // ���ÿ������ţ�CS, DC, RESET��
  GPIO_InitStruct.Pin = LCD_CS_PIN | LCD_DC_PIN | LCD_RESET_PIN;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LCD_CS_PORT, &GPIO_InitStruct);

  // ����SPI���Ÿ��ù��ܣ�SCK��MOSI��
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ALTERNATE); // SCK
  LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_5, LL_GPIO_AF_5);
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE); // MOSI
  LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_7, LL_GPIO_AF_5);

  // Ӳ����λ
  HardwareReset();

  // ʹ�ñ�׼������SPI
  LL_SPI_StructInit(&SPI_InitStruct);
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;       // ȫ˫��ģʽ
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;                    // ��ģʽ
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;            // 8λ����
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;          // CPOL = 0
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;              // CPHA = 0
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;                        // ���NSS����
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;     // �����ʷ�Ƶ
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;                  // MSB��ǰ
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE; // ����CRC
  
  LL_SPI_Init(LCD_SPI, &SPI_InitStruct);
  LL_SPI_SetNSSMode(LCD_SPI, LL_SPI_NSS_SOFT);                 // ���NSSģʽ
  LL_SPI_Enable(LCD_SPI);                                      // ʹ��SPI

  DMA_Init();

  // ���ͳ�ʼ������
  WriteCommand(0xCF);
  WriteData(0x00);
  WriteData(0xC1);
  WriteData(0x30);

  WriteCommand(0xED);
  WriteData(0x64);
  WriteData(0x03);
  WriteData(0x12);
  WriteData(0x81);

  WriteCommand(0xE8);
  WriteData(0x85);
  WriteData(0x10);
  WriteData(0x7A);

  WriteCommand(0x36);  // �ڴ���ʿ���
  WriteData(0x36);		//0x28����0��,0x48����180��,0xE8����180��,0x88����180��

  WriteCommand(0x3A);  // ���ظ�ʽ
  WriteData(0x55);     // 16λ����

  // �˳�˯�߲�������ʾ
  WriteCommand(0x11);
  ILI9341_Delay(120);
  WriteCommand(0x29);
}

void ILI9341_FillScreen(uint16_t color)
{
    uint32_t i;
    ILI9341_SetWindow(0, 0, ILI9341_WIDTH-1, ILI9341_HEIGHT-1);

    WriteCommand(0x2C);  // �ڴ�д������
    LL_GPIO_ResetOutputPin(LCD_CS_PORT, LCD_CS_PIN);
    LL_GPIO_SetOutputPin(LCD_DC_PORT, LCD_DC_PIN);

    uint8_t data[2];
    data[0] = (color >> 8) & 0xFF;
    data[1] = color & 0xFF;

    for(i = 0; i < ILI9341_WIDTH * ILI9341_HEIGHT; i++) {
        SPI_Transmit(data[0]);
        SPI_Transmit(data[1]);
    }

    LL_GPIO_SetOutputPin(LCD_CS_PORT, LCD_CS_PIN);
}

void WriteCommand(uint8_t cmd)
{
  LL_GPIO_ResetOutputPin(LCD_CS_PORT, LCD_CS_PIN);
  LL_GPIO_ResetOutputPin(LCD_DC_PORT, LCD_DC_PIN);
  SPI_Transmit(cmd);
  LL_GPIO_SetOutputPin(LCD_CS_PORT, LCD_CS_PIN);
}

void WriteData(uint8_t data)
{
  LL_GPIO_ResetOutputPin(LCD_CS_PORT, LCD_CS_PIN);
  LL_GPIO_SetOutputPin(LCD_DC_PORT, LCD_DC_PIN);
  SPI_Transmit(data);
  LL_GPIO_SetOutputPin(LCD_CS_PORT, LCD_CS_PIN);
}

void SPI_Transmit(uint8_t data)
{
    LL_SPI_TransmitData8(LCD_SPI, data);
    while(LL_SPI_IsActiveFlag_BSY(LCD_SPI));
}

static void HardwareReset(void)
{
  LL_GPIO_ResetOutputPin(LCD_RESET_PORT, LCD_RESET_PIN);
  ILI9341_Delay(20);
  LL_GPIO_SetOutputPin(LCD_RESET_PORT, LCD_RESET_PIN);
  ILI9341_Delay(20);
}

void ILI9341_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  WriteCommand(0x2A);  // �е�ַ����
  WriteData(x0 >> 8);
  WriteData(x0 & 0xFF);
  WriteData(x1 >> 8);
  WriteData(x1 & 0xFF);

  WriteCommand(0x2B);  // �е�ַ����
  WriteData(y0 >> 8);
  WriteData(y0 & 0xFF);
  WriteData(y1 >> 8);
  WriteData(y1 & 0xFF);
}
