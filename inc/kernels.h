/*
 * =====================================================================================
 *
 *       Filename:  kernels.h
 *
 *    Description:  spmv kernel implementations
 *
 *        Version:  1.0
 *        Created:  07/06/2015 03:40:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Samuel Rodriguez Bernabeu (), samuel.rodriguez@bsc.es
 *   Organization:  Barcelona Supercomputing Center (BSC)
 *
 * =====================================================================================
 */

#include "./interfaces.h"

enum ARCHICTECTURE {XEON, KNC};

template <int ARCH, typename FORMAT, typename PREC> void spmv( FORMAT* M) { /*   */};

template <> void spmv <XEON, CSR, float> (CSR* M)
{
	
};
