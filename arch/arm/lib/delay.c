/*
 * Delay loops based on the OpenRISC implementation.
 *
 * Copyright (C) 2012 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Will Deacon <will.deacon@arm.com>
 */

<<<<<<< HEAD
=======
#include <linux/clocksource.h>
>>>>>>> android-4.9
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timex.h>

/*
 * Default to the loop-based delay implementation.
 */
<<<<<<< HEAD
struct arm_delay_ops arm_delay_ops = {
=======
struct arm_delay_ops arm_delay_ops __ro_after_init = {
>>>>>>> android-4.9
	.delay		= __loop_delay,
	.const_udelay	= __loop_const_udelay,
	.udelay		= __loop_udelay,
};

static const struct delay_timer *delay_timer;
static bool delay_calibrated;
<<<<<<< HEAD
=======
static u64 delay_res;
>>>>>>> android-4.9

int read_current_timer(unsigned long *timer_val)
{
	if (!delay_timer)
		return -ENXIO;

	*timer_val = delay_timer->read_current_timer();
	return 0;
}
EXPORT_SYMBOL_GPL(read_current_timer);

<<<<<<< HEAD
=======
static inline u64 cyc_to_ns(u64 cyc, u32 mult, u32 shift)
{
	return (cyc * mult) >> shift;
}

>>>>>>> android-4.9
static void __timer_delay(unsigned long cycles)
{
	cycles_t start = get_cycles();

	while ((get_cycles() - start) < cycles)
		cpu_relax();
}

static void __timer_const_udelay(unsigned long xloops)
{
	unsigned long long loops = xloops;
<<<<<<< HEAD
	loops *= loops_per_jiffy;
=======
	loops *= arm_delay_ops.ticks_per_jiffy;
>>>>>>> android-4.9
	__timer_delay(loops >> UDELAY_SHIFT);
}

static void __timer_udelay(unsigned long usecs)
{
	__timer_const_udelay(usecs * UDELAY_MULT);
}

void __init register_current_timer_delay(const struct delay_timer *timer)
{
<<<<<<< HEAD
	if (!delay_calibrated) {
		pr_info("Switching to timer-based delay loop\n");
		delay_timer			= timer;
		lpj_fine			= timer->freq / HZ;
		loops_per_jiffy			= lpj_fine;
		arm_delay_ops.delay		= __timer_delay;
		arm_delay_ops.const_udelay	= __timer_const_udelay;
		arm_delay_ops.udelay		= __timer_udelay;
		delay_calibrated		= true;
=======
	u32 new_mult, new_shift;
	u64 res;

	clocks_calc_mult_shift(&new_mult, &new_shift, timer->freq,
			       NSEC_PER_SEC, 3600);
	res = cyc_to_ns(1ULL, new_mult, new_shift);

	if (res > 1000) {
		pr_err("Ignoring delay timer %ps, which has insufficient resolution of %lluns\n",
			timer, res);
		return;
	}

	if (!delay_calibrated && (!delay_res || (res < delay_res))) {
		pr_info("Switching to timer-based delay loop, resolution %lluns\n", res);
		delay_timer			= timer;
		lpj_fine			= timer->freq / HZ;
		delay_res			= res;

		/* cpufreq may scale loops_per_jiffy, so keep a private copy */
		arm_delay_ops.ticks_per_jiffy	= lpj_fine;
		arm_delay_ops.delay		= __timer_delay;
		arm_delay_ops.const_udelay	= __timer_const_udelay;
		arm_delay_ops.udelay		= __timer_udelay;
>>>>>>> android-4.9
	} else {
		pr_info("Ignoring duplicate/late registration of read_current_timer delay\n");
	}
}

<<<<<<< HEAD
unsigned long __cpuinit calibrate_delay_is_known(void)
=======
unsigned long calibrate_delay_is_known(void)
>>>>>>> android-4.9
{
	delay_calibrated = true;
	return lpj_fine;
}
<<<<<<< HEAD
=======

void calibration_delay_done(void)
{
	delay_calibrated = true;
}
>>>>>>> android-4.9
