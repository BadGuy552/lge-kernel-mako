#ifndef __ARM_MMU_H
#define __ARM_MMU_H

#ifdef CONFIG_MMU

typedef struct {
#ifdef CONFIG_CPU_HAS_ASID
	atomic64_t	id;
<<<<<<< HEAD
#endif
	unsigned int kvm_seq;
	unsigned long	sigpage;
=======
#else
	int		switch_pending;
#endif
	unsigned int	vmalloc_seq;
	unsigned long	sigpage;
#ifdef CONFIG_VDSO
	unsigned long	vdso;
#endif
>>>>>>> android-4.9
} mm_context_t;

#ifdef CONFIG_CPU_HAS_ASID
#define ASID_BITS	8
#define ASID_MASK	((~0ULL) << ASID_BITS)
<<<<<<< HEAD
#define ASID(mm)	((mm)->context.id.counter & ~ASID_MASK)
=======
#define ASID(mm)	((unsigned int)((mm)->context.id.counter & ~ASID_MASK))
>>>>>>> android-4.9
#else
#define ASID(mm)	(0)
#endif

#else

/*
 * From nommu.h:
 *  Copyright (C) 2002, David McCullough <davidm@snapgear.com>
 *  modified for 2.6 by Hyok S. Choi <hyok.choi@samsung.com>
 */
typedef struct {
	unsigned long	end_brk;
} mm_context_t;

#endif

<<<<<<< HEAD
/*
 * switch_mm() may do a full cache flush over the context switch,
 * so enable interrupts over the context switch to avoid high
 * latency.
 */
#ifndef CONFIG_CPU_HAS_ASID
#define __ARCH_WANT_INTERRUPTS_ON_CTXSW
#endif

=======
>>>>>>> android-4.9
#endif
