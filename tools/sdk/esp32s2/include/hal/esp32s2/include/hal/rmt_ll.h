/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "hal/misc.h"
#include "soc/rmt_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RMT_LL_MAX_LOOP_COUNT          (1023)/*!< Max loop count that hardware is supported */

#define RMT_LL_HW_BASE  (&RMT)
#define RMT_LL_MEM_BASE (&RMTMEM)

// Note: TX and RX channel number are all index from zero in the LL driver
// i.e. tx_channel belongs to [0,3], and rx_channel belongs to [0,3]

__attribute__((always_inline))
static inline void rmt_ll_enable_drive_clock(rmt_dev_t *dev, bool enable)
{
    dev->apb_conf.clk_en = enable; // register clock gating
    dev->apb_conf.mem_clk_force_on = enable; // memory clock gating
}

__attribute__((always_inline))
static inline void rmt_ll_power_down_mem(rmt_dev_t *dev, bool enable)
{
    dev->apb_conf.mem_force_pu = !enable;
    dev->apb_conf.mem_force_pd = enable;
}

__attribute__((always_inline))
static inline bool rmt_ll_is_mem_power_down(rmt_dev_t *dev)
{
    // the RTC domain can also power down RMT memory
    // so it's probably not enough to detect whether it's powered down or not
    // mem_force_pd has higher priority than mem_force_pu
    return (dev->apb_conf.mem_force_pd) || !(dev->apb_conf.mem_force_pu);
}

__attribute__((always_inline))
static inline void rmt_ll_enable_mem_access(rmt_dev_t *dev, bool enable)
{
    dev->apb_conf.apb_fifo_mask = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_set_group_clock_src(rmt_dev_t *dev, uint32_t channel, uint8_t src, uint8_t div_num, uint8_t div_a, uint8_t div_b)
{
    dev->conf_ch[channel].conf1.ref_always_on = src;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_group_clock_src(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.ref_always_on;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_reset_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    dev->ref_cnt_rst.val |= (1 << channel);
}

__attribute__((always_inline))
static inline void rmt_ll_tx_reset_channels_clock_div(rmt_dev_t *dev, uint32_t channel_mask)
{
    dev->ref_cnt_rst.val |= channel_mask;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_reset_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    dev->ref_cnt_rst.val |= (1 << channel);
}

__attribute__((always_inline))
static inline void rmt_ll_tx_reset_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.mem_rd_rst = 1;
    dev->conf_ch[channel].conf1.mem_rd_rst = 0;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_reset_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.mem_wr_rst = 1;
    dev->conf_ch[channel].conf1.mem_wr_rst = 0;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_start(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.tx_start = 1;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_stop(rmt_dev_t *dev, uint32_t channel)
{
    dev->conf_ch[channel].conf1.tx_stop = 1;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_enable(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.rx_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->conf_ch[channel].conf0.mem_size = block_num;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->conf_ch[channel].conf0.mem_size = block_num;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf0.mem_size;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf0.mem_size;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_channel_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt, div);
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_channel_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt, div);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    uint32_t div = HAL_FORCE_READ_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt);
    return div == 0 ? 256 : div;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    uint32_t div = HAL_FORCE_READ_U32_REG_FIELD(dev->conf_ch[channel].conf0, div_cnt);
    return div == 0 ? 256 : div;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_pingpong(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->apb_conf.mem_tx_wrap_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_idle_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf0, idle_thres, thres);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_idle_thres(rmt_dev_t *dev, uint32_t channel)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->conf_ch[channel].conf0, idle_thres);
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_mem_owner(rmt_dev_t *dev, uint32_t channel, uint8_t owner)
{
    dev->conf_ch[channel].conf1.mem_owner = owner;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_mem_owner(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.mem_owner;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_loop(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.tx_conti_mode = enable;
}

__attribute__((always_inline))
static inline bool rmt_ll_is_tx_loop_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.tx_conti_mode;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_loop_count(rmt_dev_t *dev, uint32_t channel, uint32_t count)
{
    dev->tx_lim_ch[channel].tx_loop_num = count;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_reset_loop(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_lim_ch[channel].loop_count_reset = 1;
    dev->tx_lim_ch[channel].loop_count_reset = 0;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_loop_count(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_lim_ch[channel].tx_loop_cnt_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_sync(rmt_dev_t *dev, bool enable)
{
    dev->tx_sim.en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_add_to_sync_group(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_sim.val |= 1 << channel;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_remove_from_sync_group(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_sim.val &= ~(1 << channel);
}

__attribute__((always_inline))
static inline void rmt_ll_rx_enable_filter(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.rx_filter_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_filter_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->conf_ch[channel].conf1, rx_filter_thres, thres);
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_idle(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf1.idle_out_en = enable;
}

__attribute__((always_inline))
static inline bool rmt_ll_is_tx_idle_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.idle_out_en;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_idle_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->conf_ch[channel].conf1.idle_out_lv = level;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_idle_level(rmt_dev_t *dev, uint32_t channel)
{
    return dev->conf_ch[channel].conf1.idle_out_lv;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_channel_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->status_ch[channel].val;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_channel_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->status_ch[channel].val;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->tx_lim_ch[channel].tx_lim = limit;
}

__attribute__((always_inline))
static inline void rmt_ll_enable_interrupt(rmt_dev_t *dev, uint32_t mask, bool enable)
{
    if (enable) {
        dev->int_ena.val |= mask;
    } else {
        dev->int_ena.val &= ~mask;
    }
}

__attribute__((always_inline))
static inline void rmt_ll_enable_tx_end_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel * 3));
    dev->int_ena.val |= (enable << (channel * 3));
}

__attribute__((always_inline))
static inline void rmt_ll_enable_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel * 3 + 1));
    dev->int_ena.val |= (enable << (channel * 3 + 1));
}

__attribute__((always_inline))
static inline void rmt_ll_enable_tx_err_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel * 3 + 2));
    dev->int_ena.val |= (enable << (channel * 3 + 2));
}

__attribute__((always_inline))
static inline void rmt_ll_enable_rx_err_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel * 3 + 2));
    dev->int_ena.val |= (enable << (channel * 3 + 2));
}

__attribute__((always_inline))
static inline void rmt_ll_enable_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel + 12));
    dev->int_ena.val |= (enable << (channel + 12));
}

__attribute__((always_inline))
static inline void rmt_ll_enable_tx_loop_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->int_ena.val &= ~(1 << (channel + 16));
    dev->int_ena.val |= (enable << (channel + 16));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_tx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3 + 1));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_tx_err_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3 + 2));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_rx_err_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel * 3 + 2));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 12));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_tx_loop_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 16));
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_end_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status = dev->int_st.val;
    return ((status & 0x01) >> 0) | ((status & 0x08) >> 2) | ((status & 0x40) >> 4) | ((status & 0x200) >> 6);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_rx_end_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status = dev->int_st.val;
    return ((status & 0x02) >> 1) | ((status & 0x10) >> 3) | ((status & 0x80) >> 5) | ((status & 0x400) >> 7);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_err_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return ((status & 0x04) >> 2) | ((status & 0x20) >> 4) | ((status & 0x100) >> 6) | ((status & 0x800) >> 8);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_rx_err_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return ((status & 0x04) >> 2) | ((status & 0x20) >> 4) | ((status & 0x100) >> 6) | ((status & 0x800) >> 8);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_thres_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return (status & 0xF000) >> 12;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_loop_interrupt_status(rmt_dev_t *dev)
{
    uint32_t status =  dev->int_st.val;
    return (status & 0xF0000) >> 16;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    // In case the compiler optimise a 32bit instruction (e.g. s32i) into two 16bit instruction (e.g. s16i, which is not allowed to access a register)
    // We take care of the "read-modify-write" procedure by ourselves.
    typeof(dev->carrier_duty_ch[0]) reg;
    reg.high = high_ticks;
    reg.low = low_ticks;
    dev->carrier_duty_ch[channel].val = reg.val;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    typeof(dev->ch_rx_carrier_rm[0]) reg;
    reg.carrier_high_thres_ch = high_ticks;
    reg.carrier_low_thres_ch = low_ticks;
    dev->ch_rx_carrier_rm[channel].val = reg.val;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_get_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t *high_ticks, uint32_t *low_ticks)
{
    *high_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->carrier_duty_ch[channel], high);
    *low_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->carrier_duty_ch[channel], low);
}

__attribute__((always_inline))
static inline void rmt_ll_rx_get_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t *high_ticks, uint32_t *low_ticks)
{
    *high_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->ch_rx_carrier_rm[channel], carrier_high_thres_ch);
    *low_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->ch_rx_carrier_rm[channel], carrier_low_thres_ch);
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_carrier_modulation(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf0.carrier_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_enable_carrier_demodulation(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf0.carrier_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_carrier_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->conf_ch[channel].conf0.carrier_out_lv = level;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_carrier_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->conf_ch[channel].conf0.carrier_out_lv = level;
}

// set true, enable carrier in all RMT state (idle, reading, sending)
// set false, enable carrier only in sending state (i.e. there're effective data in RAM to be sent)
__attribute__((always_inline))
static inline void rmt_ll_tx_set_carrier_always_on(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->conf_ch[channel].conf0.carrier_eff_en = !enable;
}

//Writes items to the specified TX channel memory with the given offset and length.
//the caller should ensure that (length + off) <= (memory block * SOC_RMT_MEM_WORDS_PER_CHANNEL)
__attribute__((always_inline))
static inline void rmt_ll_write_memory(rmt_mem_t *mem, uint32_t channel, const void *data, size_t length_in_words, size_t off)
{
    volatile uint32_t *to = (volatile uint32_t *)&mem->chan[channel].data32[off];
    uint32_t *from = (uint32_t *)data;
    while (length_in_words--) {
        *to++ = *from++;
    }
}

#ifdef __cplusplus
}
#endif
