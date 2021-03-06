/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2010 Soeren Sonnenburg
 * Copyright (C) 2010 Berlin Institute of Technology
 */

#ifndef _SPARSESPATIALSAMPLESTRINGKERNEL_H___
#define _SPARSESPATIALSAMPLESTRINGKERNEL_H___

#include "lib/common.h"
#include "kernel/StringKernel.h"

namespace shogun
{
	struct SSKFeatures
	{
		int *features;
		int *group;
		int n; 
	};

/** @brief Sparse Spatial Sample String Kernel
 * by Pavel Kuksa <pkuksa@cs.rutgers.edu> and
 * Vladimir Pavlovic <vladimir@cs.rutgers.edu>
 */
class CSparseSpatialSampleStringKernel: public CStringKernel<char>
{
	public:
		/** constructor
		 */
		CSparseSpatialSampleStringKernel();

		/** constructor
		 *
		 * @param l features of left-hand side
		 * @param r features of right-hand side
		 */
		CSparseSpatialSampleStringKernel(CStringFeatures<char>* l, CStringFeatures<char>* r);

		virtual ~CSparseSpatialSampleStringKernel();

		/** initialize kernel
		 *
		 * @param l features of left-hand side
		 * @param r features of right-hand side
		 * @return if initializing was successful
		 */
		virtual bool init(CFeatures* l, CFeatures* r);

		/** clean up kernel */
		virtual void cleanup();

		/** return what type of kernel we are
		 *
		 * @return kernel type SPARSESPATIALSAMPLE
		 */
		virtual EKernelType get_kernel_type()
		{
			return K_SPARSESPATIALSAMPLE;
		}

		void set_d(int32_t max_distance)
		{
			ASSERT(d>0);
			d=max_distance;
		}

		int32_t get_d()
		{
			return d;
		}

		void set_t(int32_t sequence_length)
		{
			ASSERT(t==2 || t==3);
			t=sequence_length;
		}

		int32_t get_t()
		{
			return t;
		}

		/** return the kernel's name
		 *
		 * @return name SparseSpatialSample
		 */
		virtual const char* get_name() const { return "SparseSpatialSampleStringKernel"; }

	protected:
		/** compute kernel function for features a and b
		 * idx_{a,b} denote the index of the feature vectors
		 * in the corresponding feature object
		 *
		 * @param idx_a index a
		 * @param idx_b index b
		 * @return computed kernel function at indices a,b
		 */
		virtual float64_t compute(int32_t idx_a, int32_t idx_b);

		SSKFeatures *extractTriple(int **S, int *len, int nStr, int d1, int d2);
		SSKFeatures *extractDouble(int **S, int *len, int nStr, int d1);
		void compute_double(int32_t idx_a, int32_t idx_b);
		void compute_triple(int32_t idx_a, int32_t idx_b);
		int* cntsrtna(int *sx, int k, int r, int na);
		void countAndUpdate(int *outK, int *sx, int *g, int k, int r, int nStr);

	protected:
		/** parameter t of the SSSK denotes how many words are considered in the
		 * sequence (valid are only 2 or 3) */
		int32_t t;

		/** parameter d of the SSSK denotes maximum allowed distance between
		 * words in the sequence */
		int32_t d;
		bool isVerbose;
};
}
#endif /* _SPARSESPATIALSAMPLESTRINGKERNEL_H___ */
