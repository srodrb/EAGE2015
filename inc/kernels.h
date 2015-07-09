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

template <> void spmv <XEON, CSR, float> (CSR* M, Vector* X, Vector* B)
{
    char trans = 'n';
    char matdescra[6];
    matdescra[0] = 'g';
    matdescra[1] = 'n';
    matdescra[2] = 'n';
    matdescra[3] = 'c';

    // mkl_zcsrmv( &trans, &A->nrows, );
};

template <> void spmv <XEON,ELL,float> (ELL* M, Vector* X, Vector* B)
{

    // aqui tengo que poner todo el rollo de alineamiento
    __assume_aligned( ELL->ell_sreal, 32);
    __assume_aligned( ELL->ell_simag, 32);
    
    __assume_aligned( ELL->rhs_sreal, 32);
    __assume_aligned( ELL->rhs_sreal, 32);
    
    __assume_aligned( X->sreal, 32);
    __assume_aligned( X->simag, 32);

    __assume_aligned( B->sreal, 32);
    __assume_aligned( B->simag, 32);


    const size_t rows    = M->ell_rows;
    const size_t cols    = M->ell_cols;
    const size_t ell_len = M->ell_len;


};
