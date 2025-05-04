#include "gpio.h"

/**
  * @brief GPIO初始化函数
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; // GPIO配置结构体
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC); // 启用GPIOC时钟

  /* 配置PC0为输出模式 */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0; // 选择PC0引脚
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT; // 设置为输出模式
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW; // 设置引脚速度
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; // 设置输出类型为推挽
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO; // 设置无上下拉
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct); // 初始化GPIOC的PC0引脚

  /* 初始化PC0为低电平 */
  LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0);
}

void My_GPIO_Init(void)
{
  MX_GPIO_Init();
}
