/* Copyright (c) 2010-2012, The Linux Foundation. All rights reserved.  *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/iommu.h>
#include <linux/clk.h>
<<<<<<< HEAD
#include <linux/scatterlist.h>
=======
#include <linux/err.h>
#include <linux/of_iommu.h>
>>>>>>> android-4.9

#include <asm/cacheflush.h>
#include <asm/sizes.h>

<<<<<<< HEAD
#include <mach/iommu_hw-8xxx.h>
#include <mach/iommu.h>
#include <mach/msm_smsm.h>
=======
#include "msm_iommu_hw-8xxx.h"
#include "msm_iommu.h"
#include "io-pgtable.h"
>>>>>>> android-4.9

#define MRC(reg, processor, op1, crn, crm, op2)				\
__asm__ __volatile__ (							\
"   mrc   "   #processor "," #op1 ", %0,"  #crn "," #crm "," #op2 "\n"  \
: "=r" (reg))

<<<<<<< HEAD
#define RCP15_PRRR(reg)		MRC(reg, p15, 0, c10, c2, 0)
#define RCP15_NMRR(reg)		MRC(reg, p15, 0, c10, c2, 1)

/* Sharability attributes of MSM IOMMU mappings */
#define MSM_IOMMU_ATTR_NON_SH		0x0
#define MSM_IOMMU_ATTR_SH		0x4

/* Cacheability attributes of MSM IOMMU mappings */
#define MSM_IOMMU_ATTR_NONCACHED	0x0
#define MSM_IOMMU_ATTR_CACHED_WB_WA	0x1
#define MSM_IOMMU_ATTR_CACHED_WB_NWA	0x2
#define MSM_IOMMU_ATTR_CACHED_WT	0x3


static int msm_iommu_unmap_range(struct iommu_domain *domain, unsigned int va,
				 unsigned int len);

static int msm_iommu_unmap_range(struct iommu_domain *domain, unsigned int va,
				 unsigned int len);

static inline void clean_pte(unsigned long *start, unsigned long *end,
			     int redirect)
{
	if (!redirect)
		dmac_flush_range(start, end);
}

/* bitmap of the page sizes currently supported */
#define MSM_IOMMU_PGSIZES	(SZ_4K | SZ_64K | SZ_1M | SZ_16M)

static int msm_iommu_tex_class[4];

DEFINE_MUTEX(msm_iommu_lock);

/**
 * Remote spinlock implementation based on Peterson's algorithm to be used
 * to synchronize IOMMU config port access between CPU and GPU.
 * This implements Process 0 of the spin lock algorithm. GPU implements
 * Process 1. Flag and turn is stored in shared memory to allow GPU to
 * access these.
 */
struct msm_iommu_remote_lock {
	int initialized;
	struct remote_iommu_petersons_spinlock *lock;
};

static struct msm_iommu_remote_lock msm_iommu_remote_lock;

#ifdef CONFIG_MSM_IOMMU_GPU_SYNC
static void _msm_iommu_remote_spin_lock_init(void)
{
	msm_iommu_remote_lock.lock = smem_alloc(SMEM_SPINLOCK_ARRAY, 32);
	memset(msm_iommu_remote_lock.lock, 0,
			sizeof(*msm_iommu_remote_lock.lock));
}

void msm_iommu_remote_p0_spin_lock(void)
{
	msm_iommu_remote_lock.lock->flag[PROC_APPS] = 1;
	msm_iommu_remote_lock.lock->turn = 1;

	smp_mb();

	while (msm_iommu_remote_lock.lock->flag[PROC_GPU] == 1 &&
	       msm_iommu_remote_lock.lock->turn == 1)
		cpu_relax();
}

void msm_iommu_remote_p0_spin_unlock(void)
{
	smp_mb();

	msm_iommu_remote_lock.lock->flag[PROC_APPS] = 0;
}
#endif

inline void msm_iommu_mutex_lock(void)
{
	mutex_lock(&msm_iommu_lock);
}

inline void msm_iommu_mutex_unlock(void)
{
	mutex_unlock(&msm_iommu_lock);
}

void *msm_iommu_lock_initialize(void)
{
	mutex_lock(&msm_iommu_lock);
	if (!msm_iommu_remote_lock.initialized) {
		msm_iommu_remote_lock_init();
		msm_iommu_remote_lock.initialized = 1;
	}
	mutex_unlock(&msm_iommu_lock);
	return msm_iommu_remote_lock.lock;
}

struct msm_priv {
	unsigned long *pgtable;
	int redirect;
=======
/* bitmap of the page sizes currently supported */
#define MSM_IOMMU_PGSIZES	(SZ_4K | SZ_64K | SZ_1M | SZ_16M)

DEFINE_SPINLOCK(msm_iommu_lock);
static LIST_HEAD(qcom_iommu_devices);
static struct iommu_ops msm_iommu_ops;

struct msm_priv {
>>>>>>> android-4.9
	struct list_head list_attached;
	struct iommu_domain domain;
	struct io_pgtable_cfg	cfg;
	struct io_pgtable_ops	*iop;
	struct device		*dev;
	spinlock_t		pgtlock; /* pagetable lock */
};

static struct msm_priv *to_msm_priv(struct iommu_domain *dom)
{
	return container_of(dom, struct msm_priv, domain);
}

static int __enable_clocks(struct msm_iommu_dev *iommu)
{
	int ret;

<<<<<<< HEAD
	ret = clk_prepare_enable(drvdata->pclk);
	if (ret)
		goto fail;

	if (drvdata->clk) {
		ret = clk_prepare_enable(drvdata->clk);
		if (ret)
			clk_disable_unprepare(drvdata->pclk);
=======
	ret = clk_enable(iommu->pclk);
	if (ret)
		goto fail;

	if (iommu->clk) {
		ret = clk_enable(iommu->clk);
		if (ret)
			clk_disable(iommu->pclk);
>>>>>>> android-4.9
	}
fail:
	return ret;
}

static void __disable_clocks(struct msm_iommu_dev *iommu)
{
<<<<<<< HEAD
	if (drvdata->clk)
		clk_disable_unprepare(drvdata->clk);
	clk_disable_unprepare(drvdata->pclk);
}

static int __flush_iotlb_va(struct iommu_domain *domain, unsigned int va)
{
	struct msm_priv *priv = domain->priv;
	struct msm_iommu_drvdata *iommu_drvdata;
	struct msm_iommu_ctx_drvdata *ctx_drvdata;
	int ret = 0;
	int asid;

	list_for_each_entry(ctx_drvdata, &priv->list_attached, attached_elm) {
		if (!ctx_drvdata->pdev || !ctx_drvdata->pdev->dev.parent)
			BUG();

		iommu_drvdata = dev_get_drvdata(ctx_drvdata->pdev->dev.parent);
		if (!iommu_drvdata)
			BUG();

		ret = __enable_clocks(iommu_drvdata);
		if (ret)
			goto fail;

		msm_iommu_remote_spin_lock();

		asid = GET_CONTEXTIDR_ASID(iommu_drvdata->base,
					   ctx_drvdata->num);

		SET_TLBIVA(iommu_drvdata->base, ctx_drvdata->num,
			   asid | (va & TLBIVA_VA));
		mb();

		msm_iommu_remote_spin_unlock();

		__disable_clocks(iommu_drvdata);
	}
fail:
	return ret;
}

static int __flush_iotlb(struct iommu_domain *domain)
{
	struct msm_priv *priv = domain->priv;
	struct msm_iommu_drvdata *iommu_drvdata;
	struct msm_iommu_ctx_drvdata *ctx_drvdata;
	int ret = 0;
	int asid;
=======
	if (iommu->clk)
		clk_disable(iommu->clk);
	clk_disable(iommu->pclk);
}

static void msm_iommu_reset(void __iomem *base, int ncb)
{
	int ctx;

	SET_RPUE(base, 0);
	SET_RPUEIE(base, 0);
	SET_ESRRESTORE(base, 0);
	SET_TBE(base, 0);
	SET_CR(base, 0);
	SET_SPDMBE(base, 0);
	SET_TESTBUSCR(base, 0);
	SET_TLBRSW(base, 0);
	SET_GLOBAL_TLBIALL(base, 0);
	SET_RPU_ACR(base, 0);
	SET_TLBLKCRWE(base, 1);

	for (ctx = 0; ctx < ncb; ctx++) {
		SET_BPRCOSH(base, ctx, 0);
		SET_BPRCISH(base, ctx, 0);
		SET_BPRCNSH(base, ctx, 0);
		SET_BPSHCFG(base, ctx, 0);
		SET_BPMTCFG(base, ctx, 0);
		SET_ACTLR(base, ctx, 0);
		SET_SCTLR(base, ctx, 0);
		SET_FSRRESTORE(base, ctx, 0);
		SET_TTBR0(base, ctx, 0);
		SET_TTBR1(base, ctx, 0);
		SET_TTBCR(base, ctx, 0);
		SET_BFBCR(base, ctx, 0);
		SET_PAR(base, ctx, 0);
		SET_FAR(base, ctx, 0);
		SET_CTX_TLBIALL(base, ctx, 0);
		SET_TLBFLPTER(base, ctx, 0);
		SET_TLBSLPTER(base, ctx, 0);
		SET_TLBLKCR(base, ctx, 0);
		SET_CONTEXTIDR(base, ctx, 0);
	}
}

static void __flush_iotlb(void *cookie)
{
	struct msm_priv *priv = cookie;
	struct msm_iommu_dev *iommu = NULL;
	struct msm_iommu_ctx_dev *master;
	int ret = 0;
>>>>>>> android-4.9

	list_for_each_entry(iommu, &priv->list_attached, dom_node) {
		ret = __enable_clocks(iommu);
		if (ret)
			goto fail;

<<<<<<< HEAD
		iommu_drvdata = dev_get_drvdata(ctx_drvdata->pdev->dev.parent);
		if (!iommu_drvdata)
			BUG();
=======
		list_for_each_entry(master, &iommu->ctx_list, list)
			SET_CTX_TLBIALL(iommu->base, master->num, 0);
>>>>>>> android-4.9

		__disable_clocks(iommu);
	}
fail:
	return;
}

static void __flush_iotlb_range(unsigned long iova, size_t size,
				size_t granule, bool leaf, void *cookie)
{
	struct msm_priv *priv = cookie;
	struct msm_iommu_dev *iommu = NULL;
	struct msm_iommu_ctx_dev *master;
	int ret = 0;
	int temp_size;

	list_for_each_entry(iommu, &priv->list_attached, dom_node) {
		ret = __enable_clocks(iommu);
		if (ret)
			goto fail;

<<<<<<< HEAD
		msm_iommu_remote_spin_lock();

		asid = GET_CONTEXTIDR_ASID(iommu_drvdata->base,
					   ctx_drvdata->num);

		SET_TLBIASID(iommu_drvdata->base, ctx_drvdata->num, asid);
		mb();

		msm_iommu_remote_spin_unlock();

		__disable_clocks(iommu_drvdata);
=======
		list_for_each_entry(master, &iommu->ctx_list, list) {
			temp_size = size;
			do {
				iova &= TLBIVA_VA;
				iova |= GET_CONTEXTIDR_ASID(iommu->base,
							    master->num);
				SET_TLBIVA(iommu->base, master->num, iova);
				iova += granule;
			} while (temp_size -= granule);
		}

		__disable_clocks(iommu);
>>>>>>> android-4.9
	}

fail:
	return;
}

static void __flush_iotlb_sync(void *cookie)
{
	/*
	 * Nothing is needed here, the barrier to guarantee
	 * completion of the tlb sync operation is implicitly
	 * taken care when the iommu client does a writel before
	 * kick starting the other master.
	 */
}

static const struct iommu_gather_ops msm_iommu_gather_ops = {
	.tlb_flush_all = __flush_iotlb,
	.tlb_add_flush = __flush_iotlb_range,
	.tlb_sync = __flush_iotlb_sync,
};

static int msm_iommu_alloc_ctx(unsigned long *map, int start, int end)
{
	int idx;

	do {
		idx = find_next_zero_bit(map, end, start);
		if (idx == end)
			return -ENOSPC;
	} while (test_and_set_bit(idx, map));

	return idx;
}

static void msm_iommu_free_ctx(unsigned long *map, int idx)
{
	clear_bit(idx, map);
}

static void config_mids(struct msm_iommu_dev *iommu,
			struct msm_iommu_ctx_dev *master)
{
	int mid, ctx, i;

	for (i = 0; i < master->num_mids; i++) {
		mid = master->mids[i];
		ctx = master->num;

		SET_M2VCBR_N(iommu->base, mid, 0);
		SET_CBACR_N(iommu->base, ctx, 0);

		/* Set VMID = 0 */
		SET_VMID(iommu->base, mid, 0);

		/* Set the context number for that MID to this context */
		SET_CBNDX(iommu->base, mid, ctx);

		/* Set MID associated with this context bank to 0*/
		SET_CBVMID(iommu->base, ctx, 0);

		/* Set the ASID for TLB tagging for this context */
		SET_CONTEXTIDR_ASID(iommu->base, ctx, ctx);

		/* Set security bit override to be Non-secure */
		SET_NSCFG(iommu->base, mid, 3);
	}
}

static void __reset_context(void __iomem *base, int ctx)
{
	SET_BPRCOSH(base, ctx, 0);
	SET_BPRCISH(base, ctx, 0);
	SET_BPRCNSH(base, ctx, 0);
	SET_BPSHCFG(base, ctx, 0);
	SET_BPMTCFG(base, ctx, 0);
	SET_ACTLR(base, ctx, 0);
	SET_SCTLR(base, ctx, 0);
	SET_FSRRESTORE(base, ctx, 0);
	SET_TTBR0(base, ctx, 0);
	SET_TTBR1(base, ctx, 0);
	SET_TTBCR(base, ctx, 0);
	SET_BFBCR(base, ctx, 0);
	SET_PAR(base, ctx, 0);
	SET_FAR(base, ctx, 0);
	SET_TLBFLPTER(base, ctx, 0);
	SET_TLBSLPTER(base, ctx, 0);
	SET_TLBLKCR(base, ctx, 0);
<<<<<<< HEAD
	SET_PRRR(base, ctx, 0);
	SET_NMRR(base, ctx, 0);
	mb();
}

static void __program_context(void __iomem *base, int ctx, int ncb,
			      phys_addr_t pgtable, int redirect,
			      int ttbr_split)
{
	unsigned int prrr, nmrr;
	int i, j, found;

	msm_iommu_remote_spin_lock();

=======
}

static void __program_context(void __iomem *base, int ctx,
			      struct msm_priv *priv)
{
>>>>>>> android-4.9
	__reset_context(base, ctx);

	/* Turn on TEX Remap */
	SET_TRE(base, ctx, 1);
	SET_AFE(base, ctx, 1);

	/* Set up HTW mode */
	/* TLB miss configuration: perform HTW on miss */
	SET_TLBMCFG(base, ctx, 0x3);

	/* V2P configuration: HTW for access */
	SET_V2PCFG(base, ctx, 0x3);

<<<<<<< HEAD
	SET_TTBCR(base, ctx, ttbr_split);
	SET_TTBR0_PA(base, ctx, (pgtable >> TTBR0_PA_SHIFT));
	if (ttbr_split)
		SET_TTBR1_PA(base, ctx, (pgtable >> TTBR1_PA_SHIFT));
=======
	SET_TTBCR(base, ctx, priv->cfg.arm_v7s_cfg.tcr);
	SET_TTBR0(base, ctx, priv->cfg.arm_v7s_cfg.ttbr[0]);
	SET_TTBR1(base, ctx, priv->cfg.arm_v7s_cfg.ttbr[1]);

	/* Set prrr and nmrr */
	SET_PRRR(base, ctx, priv->cfg.arm_v7s_cfg.prrr);
	SET_NMRR(base, ctx, priv->cfg.arm_v7s_cfg.nmrr);

	/* Invalidate the TLB for this context */
	SET_CTX_TLBIALL(base, ctx, 0);

	/* Set interrupt number to "secure" interrupt */
	SET_IRPTNDX(base, ctx, 0);
>>>>>>> android-4.9

	/* Enable context fault interrupt */
	SET_CFEIE(base, ctx, 1);

	/* Stall access on a context fault and let the handler deal with it */
	SET_CFCFG(base, ctx, 1);

	/* Redirect all cacheable requests to L2 slave port. */
	SET_RCISH(base, ctx, 1);
	SET_RCOSH(base, ctx, 1);
	SET_RCNSH(base, ctx, 1);

	/* Turn on BFB prefetch */
	SET_BFBDFE(base, ctx, 1);

<<<<<<< HEAD
	/* Configure page tables as inner-cacheable and shareable to reduce
	 * the TLB miss penalty.
	 */
	if (redirect) {
		SET_TTBR0_SH(base, ctx, 1);
		SET_TTBR1_SH(base, ctx, 1);

		SET_TTBR0_NOS(base, ctx, 1);
		SET_TTBR1_NOS(base, ctx, 1);

		SET_TTBR0_IRGNH(base, ctx, 0); /* WB, WA */
		SET_TTBR0_IRGNL(base, ctx, 1);

		SET_TTBR1_IRGNH(base, ctx, 0); /* WB, WA */
		SET_TTBR1_IRGNL(base, ctx, 1);

		SET_TTBR0_ORGN(base, ctx, 1); /* WB, WA */
		SET_TTBR1_ORGN(base, ctx, 1); /* WB, WA */
	}

	/* Find if this page table is used elsewhere, and re-use ASID */
	found = 0;
	for (i = 0; i < ncb; i++)
		if (GET_TTBR0_PA(base, i) == (pgtable >> TTBR0_PA_SHIFT) &&
		    i != ctx) {
			SET_CONTEXTIDR_ASID(base, ctx, \
					    GET_CONTEXTIDR_ASID(base, i));
			found = 1;
			break;
		}

	/* If page table is new, find an unused ASID */
	if (!found) {
		for (i = 0; i < ncb; i++) {
			found = 0;
			for (j = 0; j < ncb; j++) {
				if (GET_CONTEXTIDR_ASID(base, j) == i &&
				    j != ctx)
					found = 1;
			}

			if (!found) {
				SET_CONTEXTIDR_ASID(base, ctx, i);
				break;
			}
		}
		BUG_ON(found);
	}

=======
>>>>>>> android-4.9
	/* Enable the MMU */
	SET_M(base, ctx, 1);
	mb();

	msm_iommu_remote_spin_unlock();
}

<<<<<<< HEAD
static int msm_iommu_domain_init(struct iommu_domain *domain, int flags)
=======
static struct iommu_domain *msm_iommu_domain_alloc(unsigned type)
>>>>>>> android-4.9
{
	struct msm_priv *priv;

	if (type != IOMMU_DOMAIN_UNMANAGED)
		return NULL;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		goto fail_nomem;

	INIT_LIST_HEAD(&priv->list_attached);

	priv->domain.geometry.aperture_start = 0;
	priv->domain.geometry.aperture_end   = (1ULL << 32) - 1;
	priv->domain.geometry.force_aperture = true;

<<<<<<< HEAD
#ifdef CONFIG_IOMMU_PGTABLES_L2
	priv->redirect = flags & MSM_IOMMU_DOMAIN_PT_CACHEABLE;
#endif

	memset(priv->pgtable, 0, SZ_16K);
	domain->priv = priv;

	clean_pte(priv->pgtable, priv->pgtable + NUM_FL_PTE, priv->redirect);

	return 0;
=======
	return &priv->domain;
>>>>>>> android-4.9

fail_nomem:
	kfree(priv);
	return NULL;
}

static void msm_iommu_domain_free(struct iommu_domain *domain)
{
	struct msm_priv *priv;
<<<<<<< HEAD
	unsigned long *fl_table;
	int i;

	mutex_lock(&msm_iommu_lock);
	priv = domain->priv;
	domain->priv = NULL;

	if (priv) {
		fl_table = priv->pgtable;

		for (i = 0; i < NUM_FL_PTE; i++)
			if ((fl_table[i] & 0x03) == FL_TYPE_TABLE)
				free_page((unsigned long) __va(((fl_table[i]) &
								FL_BASE_MASK)));
=======
	unsigned long flags;

	spin_lock_irqsave(&msm_iommu_lock, flags);
	priv = to_msm_priv(domain);
	kfree(priv);
	spin_unlock_irqrestore(&msm_iommu_lock, flags);
}
>>>>>>> android-4.9

static int msm_iommu_domain_config(struct msm_priv *priv)
{
	spin_lock_init(&priv->pgtlock);

	priv->cfg = (struct io_pgtable_cfg) {
		.quirks = IO_PGTABLE_QUIRK_TLBI_ON_MAP,
		.pgsize_bitmap = msm_iommu_ops.pgsize_bitmap,
		.ias = 32,
		.oas = 32,
		.tlb = &msm_iommu_gather_ops,
		.iommu_dev = priv->dev,
	};

	priv->iop = alloc_io_pgtable_ops(ARM_V7S, &priv->cfg, priv);
	if (!priv->iop) {
		dev_err(priv->dev, "Failed to allocate pgtable\n");
		return -EINVAL;
	}

<<<<<<< HEAD
	kfree(priv);
	mutex_unlock(&msm_iommu_lock);
=======
	msm_iommu_ops.pgsize_bitmap = priv->cfg.pgsize_bitmap;

	return 0;
>>>>>>> android-4.9
}

static int msm_iommu_attach_dev(struct iommu_domain *domain, struct device *dev)
{
	int ret = 0;
<<<<<<< HEAD

	mutex_lock(&msm_iommu_lock);

	priv = domain->priv;
=======
	unsigned long flags;
	struct msm_iommu_dev *iommu;
	struct msm_priv *priv = to_msm_priv(domain);
	struct msm_iommu_ctx_dev *master;

	priv->dev = dev;
	msm_iommu_domain_config(priv);
>>>>>>> android-4.9

	spin_lock_irqsave(&msm_iommu_lock, flags);
	list_for_each_entry(iommu, &qcom_iommu_devices, dev_node) {
		master = list_first_entry(&iommu->ctx_list,
					  struct msm_iommu_ctx_dev,
					  list);
		if (master->of_node == dev->of_node) {
			ret = __enable_clocks(iommu);
			if (ret)
				goto fail;

			list_for_each_entry(master, &iommu->ctx_list, list) {
				if (master->num) {
					dev_err(dev, "domain already attached");
					ret = -EEXIST;
					goto fail;
				}
				master->num =
					msm_iommu_alloc_ctx(iommu->context_map,
							    0, iommu->ncb);
					if (IS_ERR_VALUE(master->num)) {
						ret = -ENODEV;
						goto fail;
					}
				config_mids(iommu, master);
				__program_context(iommu->base, master->num,
						  priv);
			}
			__disable_clocks(iommu);
			list_add(&iommu->dom_node, &priv->list_attached);
		}
<<<<<<< HEAD

	ret = __enable_clocks(iommu_drvdata);
	if (ret)
		goto fail;

	__program_context(iommu_drvdata->base, ctx_dev->num, iommu_drvdata->ncb,
			  __pa(priv->pgtable), priv->redirect,
			  iommu_drvdata->ttbr_split);

	__disable_clocks(iommu_drvdata);
	list_add(&(ctx_drvdata->attached_elm), &priv->list_attached);
=======
	}
>>>>>>> android-4.9

	ctx_drvdata->attached_domain = domain;
fail:
<<<<<<< HEAD
	mutex_unlock(&msm_iommu_lock);
=======
	spin_unlock_irqrestore(&msm_iommu_lock, flags);

>>>>>>> android-4.9
	return ret;
}

static void msm_iommu_detach_dev(struct iommu_domain *domain,
				 struct device *dev)
{
<<<<<<< HEAD
	struct msm_priv *priv;
	struct msm_iommu_ctx_dev *ctx_dev;
	struct msm_iommu_drvdata *iommu_drvdata;
	struct msm_iommu_ctx_drvdata *ctx_drvdata;
	int ret;

	mutex_lock(&msm_iommu_lock);
	priv = domain->priv;

	if (!priv || !dev)
		goto fail;

	iommu_drvdata = dev_get_drvdata(dev->parent);
	ctx_drvdata = dev_get_drvdata(dev);
	ctx_dev = dev->platform_data;

	if (!iommu_drvdata || !ctx_drvdata || !ctx_dev)
		goto fail;

	ret = __enable_clocks(iommu_drvdata);
	if (ret)
		goto fail;

	msm_iommu_remote_spin_lock();

	SET_TLBIASID(iommu_drvdata->base, ctx_dev->num,
		     GET_CONTEXTIDR_ASID(iommu_drvdata->base, ctx_dev->num));

	__reset_context(iommu_drvdata->base, ctx_dev->num);

	msm_iommu_remote_spin_unlock();

	__disable_clocks(iommu_drvdata);
	list_del_init(&ctx_drvdata->attached_elm);
	ctx_drvdata->attached_domain = NULL;
fail:
	mutex_unlock(&msm_iommu_lock);
}

static int __get_pgprot(int prot, int len)
{
	unsigned int pgprot;
	int tex;

	if (!(prot & (IOMMU_READ | IOMMU_WRITE))) {
		prot |= IOMMU_READ | IOMMU_WRITE;
		WARN_ONCE(1, "No attributes in iommu mapping; assuming RW\n");
	}

	if ((prot & IOMMU_WRITE) && !(prot & IOMMU_READ)) {
		prot |= IOMMU_READ;
		WARN_ONCE(1, "Write-only iommu mappings unsupported; falling back to RW\n");
	}

	if (prot & IOMMU_CACHE)
		tex = (pgprot_kernel >> 2) & 0x07;
	else
		tex = msm_iommu_tex_class[MSM_IOMMU_ATTR_NONCACHED];

	if (tex < 0 || tex > NUM_TEX_CLASS - 1)
		return 0;

	if (len == SZ_16M || len == SZ_1M) {
		pgprot = FL_SHARED;
		pgprot |= tex & 0x01 ? FL_BUFFERABLE : 0;
		pgprot |= tex & 0x02 ? FL_CACHEABLE : 0;
		pgprot |= tex & 0x04 ? FL_TEX0 : 0;
		pgprot |= FL_AP0 | FL_AP1;
		pgprot |= prot & IOMMU_WRITE ? 0 : FL_AP2;
	} else	{
		pgprot = SL_SHARED;
		pgprot |= tex & 0x01 ? SL_BUFFERABLE : 0;
		pgprot |= tex & 0x02 ? SL_CACHEABLE : 0;
		pgprot |= tex & 0x04 ? SL_TEX0 : 0;
		pgprot |= SL_AP0 | SL_AP1;
		pgprot |= prot & IOMMU_WRITE ? 0 : SL_AP2;
	}

	return pgprot;
}

static unsigned long *make_second_level(struct msm_priv *priv,
					unsigned long *fl_pte)
{
	unsigned long *sl;
	sl = (unsigned long *) __get_free_pages(GFP_KERNEL,
			get_order(SZ_4K));

	if (!sl) {
		pr_debug("Could not allocate second level table\n");
		goto fail;
	}
	memset(sl, 0, SZ_4K);
	clean_pte(sl, sl + NUM_SL_PTE, priv->redirect);

	*fl_pte = ((((int)__pa(sl)) & FL_BASE_MASK) | \
			FL_TYPE_TABLE);

	clean_pte(fl_pte, fl_pte + 1, priv->redirect);
fail:
	return sl;
}

static int sl_4k(unsigned long *sl_pte, phys_addr_t pa, unsigned int pgprot)
{
	int ret = 0;

	if (*sl_pte) {
		ret = -EBUSY;
		goto fail;
	}

	*sl_pte = (pa & SL_BASE_MASK_SMALL) | SL_NG | SL_SHARED
		| SL_TYPE_SMALL | pgprot;
fail:
	return ret;
}

static int sl_64k(unsigned long *sl_pte, phys_addr_t pa, unsigned int pgprot)
{
	int ret = 0;

	int i;

	for (i = 0; i < 16; i++)
		if (*(sl_pte+i)) {
			ret = -EBUSY;
			goto fail;
		}

	for (i = 0; i < 16; i++)
		*(sl_pte+i) = (pa & SL_BASE_MASK_LARGE) | SL_NG
				| SL_SHARED | SL_TYPE_LARGE | pgprot;
=======
	struct msm_priv *priv = to_msm_priv(domain);
	unsigned long flags;
	struct msm_iommu_dev *iommu;
	struct msm_iommu_ctx_dev *master;
	int ret;

	free_io_pgtable_ops(priv->iop);

	spin_lock_irqsave(&msm_iommu_lock, flags);
	list_for_each_entry(iommu, &priv->list_attached, dom_node) {
		ret = __enable_clocks(iommu);
		if (ret)
			goto fail;
>>>>>>> android-4.9

		list_for_each_entry(master, &iommu->ctx_list, list) {
			msm_iommu_free_ctx(iommu->context_map, master->num);
			__reset_context(iommu->base, master->num);
		}
		__disable_clocks(iommu);
	}
fail:
	return ret;
}


static inline int fl_1m(unsigned long *fl_pte, phys_addr_t pa, int pgprot)
{
	if (*fl_pte)
		return -EBUSY;

	*fl_pte = (pa & 0xFFF00000) | FL_NG | FL_TYPE_SECT | FL_SHARED
		| pgprot;

	return 0;
}


static inline int fl_16m(unsigned long *fl_pte, phys_addr_t pa, int pgprot)
{
	int i;
	int ret = 0;
	for (i = 0; i < 16; i++)
		if (*(fl_pte+i)) {
			ret = -EBUSY;
			goto fail;
		}
	for (i = 0; i < 16; i++)
		*(fl_pte+i) = (pa & 0xFF000000) | FL_SUPERSECTION
			| FL_TYPE_SECT | FL_SHARED | FL_NG | pgprot;
fail:
	return ret;
}

static int msm_iommu_map(struct iommu_domain *domain, unsigned long iova,
			 phys_addr_t pa, size_t len, int prot)
{
<<<<<<< HEAD
	struct msm_priv *priv;
	unsigned long *fl_table;
	unsigned long *fl_pte;
	unsigned long fl_offset;
	unsigned long *sl_table;
	unsigned long *sl_pte;
	unsigned long sl_offset;
	unsigned int pgprot;
	int ret = 0;

	mutex_lock(&msm_iommu_lock);

	priv = domain->priv;
	if (!priv) {
		ret = -EINVAL;
		goto fail;
	}

	fl_table = priv->pgtable;

	if (len != SZ_16M && len != SZ_1M &&
	    len != SZ_64K && len != SZ_4K) {
		pr_debug("Bad size: %d\n", len);
		ret = -EINVAL;
		goto fail;
	}

	if (!fl_table) {
		pr_debug("Null page table\n");
		ret = -EINVAL;
		goto fail;
	}

	pgprot = __get_pgprot(prot, len);

	if (!pgprot) {
		ret = -EINVAL;
		goto fail;
	}

	fl_offset = FL_OFFSET(va);	/* Upper 12 bits */
	fl_pte = fl_table + fl_offset;	/* int pointers, 4 bytes */

	if (len == SZ_16M) {
		ret = fl_16m(fl_pte, pa, pgprot);
		if (ret)
			goto fail;
		clean_pte(fl_pte, fl_pte + 16, priv->redirect);
	}

	if (len == SZ_1M) {
		ret = fl_1m(fl_pte, pa, pgprot);
		if (ret)
			goto fail;
		clean_pte(fl_pte, fl_pte + 1, priv->redirect);
	}

	/* Need a 2nd level table */
	if (len == SZ_4K || len == SZ_64K) {

		if (*fl_pte == 0) {
			if (make_second_level(priv, fl_pte) == NULL) {
				ret = -ENOMEM;
				goto fail;
			}
		}

		if (!(*fl_pte & FL_TYPE_TABLE)) {
			ret = -EBUSY;
			goto fail;
		}
	}

	sl_table = (unsigned long *) __va(((*fl_pte) & FL_BASE_MASK));
	sl_offset = SL_OFFSET(va);
	sl_pte = sl_table + sl_offset;

	if (len == SZ_4K) {
		ret = sl_4k(sl_pte, pa, pgprot);
		if (ret)
			goto fail;

		clean_pte(sl_pte, sl_pte + 1, priv->redirect);
	}

	if (len == SZ_64K) {
		ret = sl_64k(sl_pte, pa, pgprot);
		if (ret)
			goto fail;
		clean_pte(sl_pte, sl_pte + 16, priv->redirect);
	}

	ret = __flush_iotlb_va(domain, va);
fail:
	mutex_unlock(&msm_iommu_lock);
=======
	struct msm_priv *priv = to_msm_priv(domain);
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&priv->pgtlock, flags);
	ret = priv->iop->map(priv->iop, iova, pa, len, prot);
	spin_unlock_irqrestore(&priv->pgtlock, flags);

>>>>>>> android-4.9
	return ret;
}

static size_t msm_iommu_unmap(struct iommu_domain *domain, unsigned long iova,
			      size_t len)
{
<<<<<<< HEAD
	struct msm_priv *priv;
	unsigned long *fl_table;
	unsigned long *fl_pte;
	unsigned long fl_offset;
	unsigned long *sl_table;
	unsigned long *sl_pte;
	unsigned long sl_offset;
	int i, ret = 0;

	mutex_lock(&msm_iommu_lock);

	priv = domain->priv;

	if (!priv)
		goto fail;
=======
	struct msm_priv *priv = to_msm_priv(domain);
	unsigned long flags;
>>>>>>> android-4.9

	spin_lock_irqsave(&priv->pgtlock, flags);
	len = priv->iop->unmap(priv->iop, iova, len);
	spin_unlock_irqrestore(&priv->pgtlock, flags);

<<<<<<< HEAD
	if (len != SZ_16M && len != SZ_1M &&
	    len != SZ_64K && len != SZ_4K) {
		pr_debug("Bad length: %d\n", len);
		goto fail;
	}

	if (!fl_table) {
		pr_debug("Null page table\n");
		goto fail;
	}

	fl_offset = FL_OFFSET(va);	/* Upper 12 bits */
	fl_pte = fl_table + fl_offset;	/* int pointers, 4 bytes */

	if (*fl_pte == 0) {
		pr_debug("First level PTE is 0\n");
		goto fail;
	}

	/* Unmap supersection */
	if (len == SZ_16M) {
		for (i = 0; i < 16; i++)
			*(fl_pte+i) = 0;

		clean_pte(fl_pte, fl_pte + 16, priv->redirect);
	}

	if (len == SZ_1M) {
		*fl_pte = 0;

		clean_pte(fl_pte, fl_pte + 1, priv->redirect);
	}

	sl_table = (unsigned long *) __va(((*fl_pte) & FL_BASE_MASK));
	sl_offset = SL_OFFSET(va);
	sl_pte = sl_table + sl_offset;

	if (len == SZ_64K) {
		for (i = 0; i < 16; i++)
			*(sl_pte+i) = 0;

		clean_pte(sl_pte, sl_pte + 16, priv->redirect);
	}

	if (len == SZ_4K) {
		*sl_pte = 0;

		clean_pte(sl_pte, sl_pte + 1, priv->redirect);
	}

	if (len == SZ_4K || len == SZ_64K) {
		int used = 0;

		for (i = 0; i < NUM_SL_PTE; i++)
			if (sl_table[i])
				used = 1;
		if (!used) {
			free_page((unsigned long)sl_table);
			*fl_pte = 0;

			clean_pte(fl_pte, fl_pte + 1, priv->redirect);
		}
	}

	ret = __flush_iotlb_va(domain, va);

fail:
	mutex_unlock(&msm_iommu_lock);

	/* the IOMMU API requires us to return how many bytes were unmapped */
	len = ret ? 0 : len;
=======
>>>>>>> android-4.9
	return len;
}

static unsigned int get_phys_addr(struct scatterlist *sg)
{
	/*
	 * Try sg_dma_address first so that we can
	 * map carveout regions that do not have a
	 * struct page associated with them.
	 */
	unsigned int pa = sg_dma_address(sg);
	if (pa == 0)
		pa = sg_phys(sg);
	return pa;
}

static inline int is_fully_aligned(unsigned int va, phys_addr_t pa, size_t len,
				   int align)
{
	return  IS_ALIGNED(va, align) && IS_ALIGNED(pa, align)
		&& (len >= align);
}

static int check_range(unsigned long *fl_table, unsigned int va,
				 unsigned int len)
{
	unsigned int offset = 0;
	unsigned long *fl_pte;
	unsigned long fl_offset;
	unsigned long *sl_table;
	unsigned long sl_start, sl_end;
	int i;

	fl_offset = FL_OFFSET(va);	/* Upper 12 bits */
	fl_pte = fl_table + fl_offset;	/* int pointers, 4 bytes */

	while (offset < len) {
		if (*fl_pte & FL_TYPE_TABLE) {
			sl_start = SL_OFFSET(va);
			sl_table =  __va(((*fl_pte) & FL_BASE_MASK));
			sl_end = ((len - offset) / SZ_4K) + sl_start;

			if (sl_end > NUM_SL_PTE)
				sl_end = NUM_SL_PTE;

			for (i = sl_start; i < sl_end; i++) {
				if (sl_table[i] != 0) {
					pr_err("%08x - %08x already mapped\n",
						va, va + SZ_4K);
					return -EBUSY;
				}
				offset += SZ_4K;
				va += SZ_4K;
			}


			sl_start = 0;
		} else {
			if (*fl_pte != 0) {
				pr_err("%08x - %08x already mapped\n",
				       va, va + SZ_1M);
				return -EBUSY;
			}
			va += SZ_1M;
			offset += SZ_1M;
			sl_start = 0;
		}
		fl_pte++;
	}
	return 0;
}

static int msm_iommu_map_range(struct iommu_domain *domain, unsigned int va,
			       struct scatterlist *sg, unsigned int len,
			       int prot)
{
	unsigned int pa;
	unsigned int start_va = va;
	unsigned int offset = 0;
	unsigned long *fl_table;
	unsigned long *fl_pte;
	unsigned long fl_offset;
	unsigned long *sl_table = NULL;
	unsigned long sl_offset, sl_start;
	unsigned int chunk_size, chunk_offset = 0;
	int ret = 0;
	struct msm_priv *priv;
	unsigned int pgprot4k, pgprot64k, pgprot1m, pgprot16m;

	mutex_lock(&msm_iommu_lock);

	BUG_ON(len & (SZ_4K - 1));

	priv = domain->priv;
	fl_table = priv->pgtable;

	pgprot4k = __get_pgprot(prot, SZ_4K);
	pgprot64k = __get_pgprot(prot, SZ_64K);
	pgprot1m = __get_pgprot(prot, SZ_1M);
	pgprot16m = __get_pgprot(prot, SZ_16M);

	if (!pgprot4k || !pgprot64k || !pgprot1m || !pgprot16m) {
		ret = -EINVAL;
		goto fail;
	}
	ret = check_range(fl_table, va, len);
	if (ret)
		goto fail;

	fl_offset = FL_OFFSET(va);	/* Upper 12 bits */
	fl_pte = fl_table + fl_offset;	/* int pointers, 4 bytes */
	pa = get_phys_addr(sg);

	while (offset < len) {
		chunk_size = SZ_4K;

		if (is_fully_aligned(va, pa, sg->length - chunk_offset,
				     SZ_16M))
			chunk_size = SZ_16M;
		else if (is_fully_aligned(va, pa, sg->length - chunk_offset,
					  SZ_1M))
			chunk_size = SZ_1M;
		/* 64k or 4k determined later */

		/* for 1M and 16M, only first level entries are required */
		if (chunk_size >= SZ_1M) {
			if (chunk_size == SZ_16M) {
				ret = fl_16m(fl_pte, pa, pgprot16m);
				if (ret)
					goto fail;
				clean_pte(fl_pte, fl_pte + 16, priv->redirect);
				fl_pte += 16;
			} else if (chunk_size == SZ_1M) {
				ret = fl_1m(fl_pte, pa, pgprot1m);
				if (ret)
					goto fail;
				clean_pte(fl_pte, fl_pte + 1, priv->redirect);
				fl_pte++;
			}

			offset += chunk_size;
			chunk_offset += chunk_size;
			va += chunk_size;
			pa += chunk_size;

			if (chunk_offset >= sg->length && offset < len) {
				chunk_offset = 0;
				sg = sg_next(sg);
				pa = get_phys_addr(sg);
			}
			continue;
		}
		/* for 4K or 64K, make sure there is a second level table */
		if (*fl_pte == 0) {
			if (!make_second_level(priv, fl_pte)) {
				ret = -ENOMEM;
				goto fail;
			}
		}
		if (!(*fl_pte & FL_TYPE_TABLE)) {
			ret = -EBUSY;
			goto fail;
		}
		sl_table = __va(((*fl_pte) & FL_BASE_MASK));
		sl_offset = SL_OFFSET(va);
		/* Keep track of initial position so we
		 * don't clean more than we have to
		 */
		sl_start = sl_offset;

		/* Build the 2nd level page table */
		while (offset < len && sl_offset < NUM_SL_PTE) {

			/* Map a large 64K page if the chunk is large enough and
			 * the pa and va are aligned
			 */

			if (is_fully_aligned(va, pa, sg->length - chunk_offset,
					     SZ_64K))
				chunk_size = SZ_64K;
			else
				chunk_size = SZ_4K;

			if (chunk_size == SZ_4K) {
				sl_4k(&sl_table[sl_offset], pa, pgprot4k);
				sl_offset++;
			} else {
				BUG_ON(sl_offset + 16 > NUM_SL_PTE);
				sl_64k(&sl_table[sl_offset], pa, pgprot64k);
				sl_offset += 16;
			}


			offset += chunk_size;
			chunk_offset += chunk_size;
			va += chunk_size;
			pa += chunk_size;

			if (chunk_offset >= sg->length && offset < len) {
				chunk_offset = 0;
				sg = sg_next(sg);
				pa = get_phys_addr(sg);
			}
		}

		clean_pte(sl_table + sl_start, sl_table + sl_offset,
				priv->redirect);

		fl_pte++;
		sl_offset = 0;
	}
	__flush_iotlb(domain);
fail:
	mutex_unlock(&msm_iommu_lock);
	if (ret && offset > 0)
		msm_iommu_unmap_range(domain, start_va, offset);
	return ret;
}


static int msm_iommu_unmap_range(struct iommu_domain *domain, unsigned int va,
				 unsigned int len)
{
	unsigned int offset = 0;
	unsigned long *fl_table;
	unsigned long *fl_pte;
	unsigned long fl_offset;
	unsigned long *sl_table;
	unsigned long sl_start, sl_end;
	int used, i;
	struct msm_priv *priv;

	mutex_lock(&msm_iommu_lock);

	BUG_ON(len & (SZ_4K - 1));

	priv = domain->priv;
	fl_table = priv->pgtable;

	fl_offset = FL_OFFSET(va);	/* Upper 12 bits */
	fl_pte = fl_table + fl_offset;	/* int pointers, 4 bytes */

	while (offset < len) {
		if (*fl_pte & FL_TYPE_TABLE) {
			sl_start = SL_OFFSET(va);
			sl_table =  __va(((*fl_pte) & FL_BASE_MASK));
			sl_end = ((len - offset) / SZ_4K) + sl_start;

			if (sl_end > NUM_SL_PTE)
				sl_end = NUM_SL_PTE;

			memset(sl_table + sl_start, 0, (sl_end - sl_start) * 4);
			clean_pte(sl_table + sl_start, sl_table + sl_end,
					priv->redirect);

			offset += (sl_end - sl_start) * SZ_4K;
			va += (sl_end - sl_start) * SZ_4K;

			/* Unmap and free the 2nd level table if all mappings
			 * in it were removed. This saves memory, but the table
			 * will need to be re-allocated the next time someone
			 * tries to map these VAs.
			 */
			used = 0;

			/* If we just unmapped the whole table, don't bother
			 * seeing if there are still used entries left.
			 */
			if (sl_end - sl_start != NUM_SL_PTE)
				for (i = 0; i < NUM_SL_PTE; i++)
					if (sl_table[i]) {
						used = 1;
						break;
					}
			if (!used) {
				free_page((unsigned long)sl_table);
				*fl_pte = 0;

				clean_pte(fl_pte, fl_pte + 1, priv->redirect);
			}

			sl_start = 0;
		} else {
			*fl_pte = 0;
			clean_pte(fl_pte, fl_pte + 1, priv->redirect);
			va += SZ_1M;
			offset += SZ_1M;
			sl_start = 0;
		}
		fl_pte++;
	}

	__flush_iotlb(domain);
	mutex_unlock(&msm_iommu_lock);
	return 0;
}

static phys_addr_t msm_iommu_iova_to_phys(struct iommu_domain *domain,
					  dma_addr_t va)
{
	struct msm_priv *priv;
	struct msm_iommu_dev *iommu;
	struct msm_iommu_ctx_dev *master;
	unsigned int par;
<<<<<<< HEAD
	void __iomem *base;
=======
	unsigned long flags;
>>>>>>> android-4.9
	phys_addr_t ret = 0;

	mutex_lock(&msm_iommu_lock);

	priv = to_msm_priv(domain);
	iommu = list_first_entry(&priv->list_attached,
				 struct msm_iommu_dev, dom_node);

	if (list_empty(&iommu->ctx_list))
		goto fail;

	master = list_first_entry(&iommu->ctx_list,
				  struct msm_iommu_ctx_dev, list);
	if (!master)
		goto fail;

	ret = __enable_clocks(iommu);
	if (ret)
		goto fail;

<<<<<<< HEAD
	msm_iommu_remote_spin_lock();

	SET_V2PPR(base, ctx, va & V2Pxx_VA);

	mb();
	par = GET_PAR(base, ctx);
=======
	/* Invalidate context TLB */
	SET_CTX_TLBIALL(iommu->base, master->num, 0);
	SET_V2PPR(iommu->base, master->num, va & V2Pxx_VA);

	par = GET_PAR(iommu->base, master->num);
>>>>>>> android-4.9

	/* We are dealing with a supersection */
	if (GET_NOFAULT_SS(iommu->base, master->num))
		ret = (par & 0xFF000000) | (va & 0x00FFFFFF);
	else	/* Upper 20 bits from PAR, lower 12 from VA */
		ret = (par & 0xFFFFF000) | (va & 0x00000FFF);

	if (GET_FAULT(iommu->base, master->num))
		ret = 0;

<<<<<<< HEAD
	msm_iommu_remote_spin_unlock();

	__disable_clocks(iommu_drvdata);
=======
	__disable_clocks(iommu);
>>>>>>> android-4.9
fail:
	mutex_unlock(&msm_iommu_lock);
	return ret;
}

static bool msm_iommu_capable(enum iommu_cap cap)
{
	return false;
}

static void print_ctx_regs(void __iomem *base, int ctx)
{
	unsigned int fsr = GET_FSR(base, ctx);
	pr_err("FAR    = %08x    PAR    = %08x\n",
	       GET_FAR(base, ctx), GET_PAR(base, ctx));
	pr_err("FSR    = %08x [%s%s%s%s%s%s%s%s%s%s]\n", fsr,
			(fsr & 0x02) ? "TF " : "",
			(fsr & 0x04) ? "AFF " : "",
			(fsr & 0x08) ? "APF " : "",
			(fsr & 0x10) ? "TLBMF " : "",
			(fsr & 0x20) ? "HTWDEEF " : "",
			(fsr & 0x40) ? "HTWSEEF " : "",
			(fsr & 0x80) ? "MHF " : "",
			(fsr & 0x10000) ? "SL " : "",
			(fsr & 0x40000000) ? "SS " : "",
			(fsr & 0x80000000) ? "MULTI " : "");

	pr_err("FSYNR0 = %08x    FSYNR1 = %08x\n",
	       GET_FSYNR0(base, ctx), GET_FSYNR1(base, ctx));
	pr_err("TTBR0  = %08x    TTBR1  = %08x\n",
	       GET_TTBR0(base, ctx), GET_TTBR1(base, ctx));
	pr_err("SCTLR  = %08x    ACTLR  = %08x\n",
	       GET_SCTLR(base, ctx), GET_ACTLR(base, ctx));
}

static void insert_iommu_master(struct device *dev,
				struct msm_iommu_dev **iommu,
				struct of_phandle_args *spec)
{
	struct msm_iommu_ctx_dev *master = dev->archdata.iommu;
	int sid;

	if (list_empty(&(*iommu)->ctx_list)) {
		master = kzalloc(sizeof(*master), GFP_ATOMIC);
		master->of_node = dev->of_node;
		list_add(&master->list, &(*iommu)->ctx_list);
		dev->archdata.iommu = master;
	}

	for (sid = 0; sid < master->num_mids; sid++)
		if (master->mids[sid] == spec->args[0]) {
			dev_warn(dev, "Stream ID 0x%hx repeated; ignoring\n",
				 sid);
			return;
		}

	master->mids[master->num_mids++] = spec->args[0];
}

static int qcom_iommu_of_xlate(struct device *dev,
			       struct of_phandle_args *spec)
{
	struct msm_iommu_dev *iommu;
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&msm_iommu_lock, flags);
	list_for_each_entry(iommu, &qcom_iommu_devices, dev_node)
		if (iommu->dev->of_node == spec->np)
			break;

	if (!iommu || iommu->dev->of_node != spec->np) {
		ret = -ENODEV;
		goto fail;
	}

	insert_iommu_master(dev, &iommu, spec);
fail:
	spin_unlock_irqrestore(&msm_iommu_lock, flags);

	return ret;
}

irqreturn_t msm_iommu_fault_handler(int irq, void *dev_id)
{
<<<<<<< HEAD
	struct msm_iommu_ctx_drvdata *ctx_drvdata = dev_id;
	struct msm_iommu_drvdata *drvdata;
	void __iomem *base;
	unsigned int fsr, num;
	int ret;
=======
	struct msm_iommu_dev *iommu = dev_id;
	unsigned int fsr;
	int i, ret;
>>>>>>> android-4.9

	mutex_lock(&msm_iommu_lock);
	BUG_ON(!ctx_drvdata);

<<<<<<< HEAD
	drvdata = dev_get_drvdata(ctx_drvdata->pdev->dev.parent);
	BUG_ON(!drvdata);

	base = drvdata->base;
	num = ctx_drvdata->num;
=======
	if (!iommu) {
		pr_err("Invalid device ID in context interrupt handler\n");
		goto fail;
	}

	pr_err("Unexpected IOMMU page fault!\n");
	pr_err("base = %08x\n", (unsigned int)iommu->base);
>>>>>>> android-4.9

	ret = __enable_clocks(iommu);
	if (ret)
		goto fail;

<<<<<<< HEAD
	msm_iommu_remote_spin_lock();

	fsr = GET_FSR(base, num);

	if (fsr) {
		if (!ctx_drvdata->attached_domain) {
			pr_err("Bad domain in interrupt handler\n");
			ret = -ENOSYS;
		} else
			ret = report_iommu_fault(ctx_drvdata->attached_domain,
						&ctx_drvdata->pdev->dev,
						GET_FAR(base, num), 0);

		if (ret == -ENOSYS) {
			pr_err("Unexpected IOMMU page fault!\n");
			pr_err("name    = %s\n", drvdata->name);
			pr_err("context = %s (%d)\n", ctx_drvdata->name, num);
			pr_err("Interesting registers:\n");
			print_ctx_regs(base, num);
		}

		SET_FSR(base, num, fsr);
		/*
		 * Only resume fetches if the registered fault handler
		 * allows it
		 */
		if (ret != -EBUSY)
			SET_RESUME(base, num, 1);

		ret = IRQ_HANDLED;
	} else
		ret = IRQ_NONE;

	msm_iommu_remote_spin_unlock();

	__disable_clocks(drvdata);
=======
	for (i = 0; i < iommu->ncb; i++) {
		fsr = GET_FSR(iommu->base, i);
		if (fsr) {
			pr_err("Fault occurred in context %d.\n", i);
			pr_err("Interesting registers:\n");
			print_ctx_regs(iommu->base, i);
			SET_FSR(iommu->base, i, 0x4000000F);
		}
	}
	__disable_clocks(iommu);
>>>>>>> android-4.9
fail:
	mutex_unlock(&msm_iommu_lock);
	return ret;
}

static phys_addr_t msm_iommu_get_pt_base_addr(struct iommu_domain *domain)
{
	struct msm_priv *priv = domain->priv;
	return __pa(priv->pgtable);
}

static struct iommu_ops msm_iommu_ops = {
	.capable = msm_iommu_capable,
	.domain_alloc = msm_iommu_domain_alloc,
	.domain_free = msm_iommu_domain_free,
	.attach_dev = msm_iommu_attach_dev,
	.detach_dev = msm_iommu_detach_dev,
	.map = msm_iommu_map,
	.unmap = msm_iommu_unmap,
<<<<<<< HEAD
	.map_range = msm_iommu_map_range,
	.unmap_range = msm_iommu_unmap_range,
	.iova_to_phys = msm_iommu_iova_to_phys,
	.domain_has_cap = msm_iommu_domain_has_cap,
	.get_pt_base_addr = msm_iommu_get_pt_base_addr,
=======
	.map_sg = default_iommu_map_sg,
	.iova_to_phys = msm_iommu_iova_to_phys,
>>>>>>> android-4.9
	.pgsize_bitmap = MSM_IOMMU_PGSIZES,
	.of_xlate = qcom_iommu_of_xlate,
};

static int msm_iommu_probe(struct platform_device *pdev)
{
	struct resource *r;
	struct msm_iommu_dev *iommu;
	int ret, par, val;

	iommu = devm_kzalloc(&pdev->dev, sizeof(*iommu), GFP_KERNEL);
	if (!iommu)
		return -ENODEV;

	iommu->dev = &pdev->dev;
	INIT_LIST_HEAD(&iommu->ctx_list);

	iommu->pclk = devm_clk_get(iommu->dev, "smmu_pclk");
	if (IS_ERR(iommu->pclk)) {
		dev_err(iommu->dev, "could not get smmu_pclk\n");
		return PTR_ERR(iommu->pclk);
	}

	ret = clk_prepare(iommu->pclk);
	if (ret) {
		dev_err(iommu->dev, "could not prepare smmu_pclk\n");
		return ret;
	}

	iommu->clk = devm_clk_get(iommu->dev, "iommu_clk");
	if (IS_ERR(iommu->clk)) {
		dev_err(iommu->dev, "could not get iommu_clk\n");
		clk_unprepare(iommu->pclk);
		return PTR_ERR(iommu->clk);
	}

	ret = clk_prepare(iommu->clk);
	if (ret) {
		dev_err(iommu->dev, "could not prepare iommu_clk\n");
		clk_unprepare(iommu->pclk);
		return ret;
	}

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	iommu->base = devm_ioremap_resource(iommu->dev, r);
	if (IS_ERR(iommu->base)) {
		dev_err(iommu->dev, "could not get iommu base\n");
		ret = PTR_ERR(iommu->base);
		goto fail;
	}

	iommu->irq = platform_get_irq(pdev, 0);
	if (iommu->irq < 0) {
		dev_err(iommu->dev, "could not get iommu irq\n");
		ret = -ENODEV;
		goto fail;
	}

	ret = of_property_read_u32(iommu->dev->of_node, "qcom,ncb", &val);
	if (ret) {
		dev_err(iommu->dev, "could not get ncb\n");
		goto fail;
	}
	iommu->ncb = val;

	msm_iommu_reset(iommu->base, iommu->ncb);
	SET_M(iommu->base, 0, 1);
	SET_PAR(iommu->base, 0, 0);
	SET_V2PCFG(iommu->base, 0, 1);
	SET_V2PPR(iommu->base, 0, 0);
	par = GET_PAR(iommu->base, 0);
	SET_V2PCFG(iommu->base, 0, 0);
	SET_M(iommu->base, 0, 0);

	if (!par) {
		pr_err("Invalid PAR value detected\n");
		ret = -ENODEV;
		goto fail;
	}

	ret = devm_request_threaded_irq(iommu->dev, iommu->irq, NULL,
					msm_iommu_fault_handler,
					IRQF_ONESHOT | IRQF_SHARED,
					"msm_iommu_secure_irpt_handler",
					iommu);
	if (ret) {
		pr_err("Request IRQ %d failed with ret=%d\n", iommu->irq, ret);
		goto fail;
	}

	list_add(&iommu->dev_node, &qcom_iommu_devices);
	of_iommu_set_ops(pdev->dev.of_node, &msm_iommu_ops);

	pr_info("device mapped at %p, irq %d with %d ctx banks\n",
		iommu->base, iommu->irq, iommu->ncb);

	return ret;
fail:
	clk_unprepare(iommu->clk);
	clk_unprepare(iommu->pclk);
	return ret;
}

static const struct of_device_id msm_iommu_dt_match[] = {
	{ .compatible = "qcom,apq8064-iommu" },
	{}
};

static int msm_iommu_remove(struct platform_device *pdev)
{
	struct msm_iommu_dev *iommu = platform_get_drvdata(pdev);

	clk_unprepare(iommu->clk);
	clk_unprepare(iommu->pclk);
	return 0;
}

static struct platform_driver msm_iommu_driver = {
	.driver = {
		.name	= "msm_iommu",
		.of_match_table = msm_iommu_dt_match,
	},
	.probe		= msm_iommu_probe,
	.remove		= msm_iommu_remove,
};

static int __init msm_iommu_driver_init(void)
{
	int ret;

	ret = platform_driver_register(&msm_iommu_driver);
	if (ret != 0)
		pr_err("Failed to register IOMMU driver\n");

	return ret;
}

static void __exit msm_iommu_driver_exit(void)
{
	platform_driver_unregister(&msm_iommu_driver);
}

subsys_initcall(msm_iommu_driver_init);
module_exit(msm_iommu_driver_exit);

static int __init msm_iommu_init(void)
{
<<<<<<< HEAD
	if (!msm_soc_version_supports_iommu_v1())
		return -ENODEV;

	msm_iommu_lock_initialize();

	setup_iommu_tex_classes();
=======
>>>>>>> android-4.9
	bus_set_iommu(&platform_bus_type, &msm_iommu_ops);
	return 0;
}

static int __init msm_iommu_of_setup(struct device_node *np)
{
	msm_iommu_init();
	return 0;
}

IOMMU_OF_DECLARE(msm_iommu_of, "qcom,apq8064-iommu", msm_iommu_of_setup);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Stepan Moskovchenko <stepanm@codeaurora.org>");
