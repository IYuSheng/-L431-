#include "SystemClock.h"

void DWT_Init(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/**
  * @brief 系统时钟配置函数
  */
void SystemClock_Config(void)
{
  /* 初始化硬件外设时钟 */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG); // 启用SYSCFG时钟
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR); // 启用电源接口时钟

  /* 配置中断优先级分组 */
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);

  /* 配置SysTick中断优先级 */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));

  /*--------------------------------------------------------------------------------------------------------------------------------*/

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4); // 设置FLASH延迟
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4); // 等待延迟设置完成

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1); // 设置电压调节
  while (LL_PWR_IsActiveFlag_VOS() != 0); // 等待电压调节设置完成

  LL_RCC_HSE_Enable(); // 启用外部高速时钟
  while(LL_RCC_HSE_IsReady() != 1); // 等待HSE准备好

  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_1, 20, LL_RCC_PLLR_DIV_2); // 配置PLL
  LL_RCC_PLL_EnableDomain_SYS(); // 启用PLL系统时钟
  LL_RCC_PLL_Enable(); // 启用PLL
  while(LL_RCC_PLL_IsReady() != 1); // 等待PLL准备好

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL); // 设置系统时钟源为PLL
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL); // 等待时钟源切换完成

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1); // 设置AHB总线预分频
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1); // 设置APB1总线预分频
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1); // 设置APB2总线预分频

  LL_Init1msTick(80000000); // 初始化SysTick，每1毫秒产生一次中断
  LL_SetSystemCoreClock(80000000); // 设置系统核心时钟频率

  DWT_Init();
}

void DWT_Delay(uint32_t ms)
{
  uint32_t start = DWT->CYCCNT;
  uint32_t cycles = ms * (SystemCoreClock / 1000);
  while((DWT->CYCCNT - start) < cycles);
}
