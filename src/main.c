/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2019 Western Digital Corporation or its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http:*www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file   main.c
 * @author Nati Rapaport
 * @date   23.10.2019
 * @brief  main C file of the demonstration application
 */

/**
 * include files
 */
#include <rtthread.h>
#include <rthw.h>
#include "demo_platform_al.h"
#include "board.c"

#include "kalman_filter.h"
#include <stdlib.h> // for rand()
#include <stdio.h>  // for printf
#include "sm3.h"

#define SegEn_ADDR      0x80001038
#define SegDig_ADDR     0x8000103C

#define GPIO_SWs        0x80001400
#define GPIO_LEDs       0x80001404
#define GPIO_INOUT      0x80001408
#define RGPIO_INTE      0x8000140C
#define RGPIO_PTRIG     0x80001410
#define RGPIO_CTRL      0x80001418
#define RGPIO_INTS      0x8000141C

#define RPTC_CNTR       0x80001200
#define RPTC_HRC        0x80001204
#define RPTC_LRC        0x80001208
#define RPTC_CTRL       0x8000120c

#define Select_INT      0x80001018

#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_GPIO(dir, value)             \
  {                                        \
    (*(volatile unsigned *)dir) = (value); \
  }

extern void tick_init();
extern void thread_sample();
extern void msgq_sample();
extern void mempool_sample();
extern void interrupt_sample();
extern void mutex_sample();


/** led 线程 :Seg显示0x02111212(我的学号末位数字），同时LED灯双数位置和单数位置交替
*/
void thread_led_entry()
{
    WRITE_GPIO(SegDig_ADDR, 0x02111212)
    u32_t count = 0;
    while (1)
    {
      if (count % 2 == 0)
      {
        WRITE_GPIO(GPIO_LEDs, 0xaaaa);
      }
      else
      {
        WRITE_GPIO(GPIO_LEDs, 0x5555);
      }
      count ++;
      rt_thread_mdelay(2000);
    }

}


int main(void)
 {
    rt_thread_t thread_led, thread_entry = NULL;

    /* 创建 led 线程 */
    thread_led = rt_thread_create("thread1", thread_led_entry, RT_NULL,
                              512,
                              1, 5);
    rt_thread_startup(thread_led);

    u32_t sw_v;

    // 无限循环，用于处理按键操作
    while (1)
    {
      // 如果按键值没变化
      if (sw_v == (READ_GPIO(GPIO_SWs) >> 16)) 
      {
        // 延迟5秒
        rt_thread_mdelay(5000);
        continue;
      }
      // 获取按键值，获取的值需要右移16位，板子只有16位，u32_t有32位
      sw_v = (READ_GPIO(GPIO_SWs) >> 16);
      // 如果按键值为0
      if (sw_v == 0)
      {
        if (thread_entry != NULL) 
        {
          // 如果之前存在线程，则挂起
          rt_thread_suspend(thread_entry);
          // 删除线程
          rt_thread_delete(thread_entry);
        }
      }
      else if (sw_v == 1)
      {
        // 创建并启动名为"thread_m"的线程，使用msgq_sample函数作为线程入口
        thread_entry = rt_thread_create("thread_m", msgq_sample, RT_NULL,
                                  512,
                                  9, 5);
        rt_thread_startup(thread_entry);
      }
      else if (sw_v == 2)
      {
        // 创建并启动名为"thread_t"的线程，使用thread_sample函数作为线程入口
        thread_entry = rt_thread_create("thread_t", thread_sample, RT_NULL,
                                  512,
                                  9, 5);
        rt_thread_startup(thread_entry);
      }
      else if (sw_v == 4)
      {
        // 创建并启动名为"thread_p"的线程，使用mempool_sample函数作为线程入口
        thread_entry = rt_thread_create("thread_p", mempool_sample, RT_NULL,
                                  512,
                                  9, 5);
        rt_thread_startup(thread_entry);
      }
      else if (sw_v == 8)
      {
        // 创建并启动名为"thread_i"的线程，使用interrupt_sample函数作为线程入口
        thread_entry = rt_thread_create("thread_i", interrupt_sample, RT_NULL,
                                  512,
                                  9, 5);
        rt_thread_startup(thread_entry);
      }
      else if (sw_v == 16)
      {
        // 创建并启动名为"thread_mt"的线程，使用mutex_sample函数作为线程入口
        thread_entry = rt_thread_create("thread_mt", mutex_sample, RT_NULL,
                                  512,
                                  9, 5);
        rt_thread_startup(thread_entry);
      }
      // 延迟5秒
      rt_thread_mdelay(5000);
    }

    for (;;)
    {
        rt_thread_delay(50);
    }
    return 0;
}
