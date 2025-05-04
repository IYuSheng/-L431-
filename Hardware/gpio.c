#include "gpio.h"

/**
  * @brief GPIO��ʼ������
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; // GPIO���ýṹ��
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC); // ����GPIOCʱ��

  /* ����PC0Ϊ���ģʽ */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0; // ѡ��PC0����
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT; // ����Ϊ���ģʽ
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW; // ���������ٶ�
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; // �����������Ϊ����
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO; // ������������
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct); // ��ʼ��GPIOC��PC0����

  /* ��ʼ��PC0Ϊ�͵�ƽ */
  LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0);
}

void My_GPIO_Init(void)
{
  MX_GPIO_Init();
}
