#ifndef ASMARM_DMA_IOMMU_H
#define ASMARM_DMA_IOMMU_H

#ifdef __KERNEL__

#include <linux/mm_types.h>
#include <linux/scatterlist.h>
#include <linux/dma-debug.h>
#include <linux/kmemcheck.h>
<<<<<<< HEAD
=======
#include <linux/kref.h>
>>>>>>> android-4.9

struct dma_iommu_mapping {
	/* iommu specific data */
	struct iommu_domain	*domain;

<<<<<<< HEAD
	void			*bitmap;
	size_t			bits;
	unsigned int		order;
=======
	unsigned long		**bitmaps;	/* array of bitmaps */
	unsigned int		nr_bitmaps;	/* nr of elements in array */
	unsigned int		extensions;
	size_t			bitmap_size;	/* size of a single bitmap */
	size_t			bits;		/* per bitmap */
>>>>>>> android-4.9
	dma_addr_t		base;

	spinlock_t		lock;
	struct kref		kref;
};

struct dma_iommu_mapping *
<<<<<<< HEAD
arm_iommu_create_mapping(struct bus_type *bus, dma_addr_t base, size_t size,
			 int order);
=======
arm_iommu_create_mapping(struct bus_type *bus, dma_addr_t base, u64 size);
>>>>>>> android-4.9

void arm_iommu_release_mapping(struct dma_iommu_mapping *mapping);

int arm_iommu_attach_device(struct device *dev,
					struct dma_iommu_mapping *mapping);
<<<<<<< HEAD
=======
void arm_iommu_detach_device(struct device *dev);
>>>>>>> android-4.9

#endif /* __KERNEL__ */
#endif
