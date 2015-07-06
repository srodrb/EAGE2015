/*
 * =====================================================================================
 *
 *       Filename:  interfaces.h
 *
 *    Description:  Interfaz de formatos para CSR, ELL y ELL-sigma. 
 *
 *        Version:  1.0
 *        Created:  06/07/2015
 *       Revision:  none
 *       Compiler:  icc
 *
 *         Author:  Samuel Rodriguez Bernabeu (), 
 *   Organization:  Barcelona Supercomputing Center.
 *
 * =====================================================================================
 */

#ifndef _COMMON_H_
#define _COMMON_H_
	#define __ALIGN_BOUNDARY 32

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
    #include <string>
	#include <time.h>
	#include <sys/time.h>
    using namespace std;

	typedef struct
	{
		double re;
		double im;
	} complex32;
	
	typedef struct
	{
		float re;
		float im;
	} complex16;

	double dtime(void);
	
    inline void* __malloc(size_t size)
    {
		void* buffer = _mm_malloc( size, (size_t) __ALIGN_BOUNDARY);
        if( !buffer){
            fprintf(stderr, "Cant allocate memory for buffer (%llu bytes)\n", size);
            abort();
        }
        return buffer;
    };

	typedef struct{
		//
		// El formato esta preparado para trabajar tanto con MKL como con cuSPARSE
		// El formato de Intel emplea 4 arrays para el formato CSR.
		// Doc: https://software.intel.com/es-es/node/522243
		//
		int         nrows;
		int         ncols;
		int 		nnz;
		int 	    nptr;
		int*    	col_ind; 
		int*    	pointerB;      // MKL pointerB, Nvidia row_ptr
		int*        pointerE;      // MKL pointerE
		complex16*  csr_scoef;
		complex32*  csr_dcoef;
	}CSR;

	typedef struct{
		//
		// Formato ELLPACK
		//
		int      nrows;
		int      ncols;
		int      ell_len;
		int*     ell_nnz;
		int*     ell_col;
		
        float*   rval__single;
        float*   ival__single;
        double*  rval__double;
        double*  ival__double;
	}ELL;

	template <typename FORMAT> FORMAT* LoadMatrix(void) {}; 

    template <> inline CSR* LoadMatrix <CSR> (void)
    {
        CSR* Matrix     = new CSR;
		string filepath = "../TestMatrices/em.csr";
        FILE* fbin = fopen( filepath.c_str() , "rb");
        
        if( !fbin )
        {
            fprintf(stderr, "Cant open filename %s correctly!\n", filepath.c_str() );
            abort();
        }

        // read number of nnz and number dimension of row ptr
        fread( &Matrix->nrows , sizeof(int), 1, fbin);
        fread( &Matrix->ncols , sizeof(int), 1, fbin);
        fread( &Matrix->nnz   , sizeof(int), 1, fbin);
        fread( &Matrix->nptr  , sizeof(int), 1, fbin);

        fprintf(stderr, "Dimensiones de la matriz: nnrows%d  ncols %d  nnz %d  nptr %d\n", Matrix->nrows, Matrix->ncols, Matrix->nnz, Matrix->nptr);

        // allocate memory for arrays
        Matrix->csr_dcoef     = (complex32*)  __malloc( Matrix->nnz  * sizeof(complex32));
        Matrix->csr_scoef     = (complex16*)  __malloc( Matrix->nnz  * sizeof(complex16));
        Matrix->col_ind       = (int*   )     __malloc( Matrix->nnz  * sizeof(int   ));
        Matrix->pointerB      = (int*   )     __malloc( Matrix->nptr * sizeof(int   ));
        Matrix->pointerE      = (int*   )     __malloc( Matrix->nptr * sizeof(int   ));

        // read arrays
        fread( &Matrix->csr_dcoef[0].re , sizeof(double), Matrix->nnz , fbin);
        fread( &Matrix->csr_dcoef[0].im , sizeof(double), Matrix->nnz , fbin);
        fread( Matrix->col_ind         , sizeof(int   ), Matrix->nnz , fbin);
        fread( Matrix->pointerB        , sizeof(int   ), Matrix->nptr, fbin);

        // copy double precision array to single precision
        for (int i = 0; i < Matrix->nnz; i++){
            Matrix->csr_scoef[i].re = (float) Matrix->csr_dcoef[i].re;
            Matrix->csr_scoef[i].im = (float) Matrix->csr_dcoef[i].im;
        }

        // el formato de Intel requiere rellenar el cuato array
        for (int i = 0; i < Matrix->nptr -1; i++) 
            Matrix->pointerE[i] = Matrix->pointerB[i+1];		

        // close file
        fclose( fbin );
        return Matrix;
    };


    template <> inline ELL* LoadMatrix <ELL> (void)
    {
        ELL* Matrix = new ELL;
		string filepath = "../TestMatrices/em.ell";
        FILE* fbin = fopen( filepath.c_str()  , "rb");
        if( !fbin )
        {
            fprintf(stderr, "Cant open filename %s correctly!\n", filepath.c_str() );
            abort();
        }

        // read number of nnz and number dimension of row ptr
        fread( &Matrix->nrows     , sizeof(int), 1, fbin);
        fread( &Matrix->ncols     , sizeof(int), 1, fbin);
        fread( &Matrix->ell_len   , sizeof(int), 1, fbin);

        fprintf(stderr, "Dimensiones de la matriz: nnrows%d  ncols %d  max nnz/row %d\n", Matrix->nrows, Matrix->ncols, Matrix->ell_len);

        int nelems = Matrix->ell_len * Matrix->nrows;

        // allocate memory for arrays
        Matrix->rval__double = (double*)  __malloc( nelems         * sizeof(double));
        Matrix->ival__double = (double*)  __malloc( nelems         * sizeof(double));
        Matrix->rval__single = (float* )  __malloc( nelems         * sizeof(float ));
        Matrix->ival__single = (float* )  __malloc( nelems         * sizeof(float ));
        Matrix->ell_col      = (int*   )  __malloc( nelems         * sizeof(int   ));
        Matrix->ell_nnz      = (int*   )  __malloc( Matrix->nrows  * sizeof(int   ));

        // read arrays
        fread( Matrix->rval__double, sizeof(double), nelems, fbin);
        fread( Matrix->rval__single, sizeof(double), nelems, fbin);
        fread( Matrix->ell_col , sizeof(int   ), nelems , fbin);
        fread( Matrix->ell_nnz , sizeof(int   ), Matrix->nrows , fbin);

        // copy double precision array to single precision
        for (int i = 0; i < nelems; i++){
            Matrix->rval__single[i] = (float) Matrix->rval__double[i];
            Matrix->ival__single[i] = (float) Matrix->ival__double[i];
        }

        // close file
        fclose( fbin );
        return Matrix;
    };

    template <typename FORMAT> void ShowMatrix( FORMAT *A) {};
    
    template <> inline void ShowMatrix<CSR> ( CSR *A)
    { 
        fprintf(stderr, "%s() showing CSR matrix\n", __FUNCTION__ ); 
        
        // creamos un buffer para la matriz
        const int rows = A->nrows;
        const int cols = A->ncols;
        const int mn   = rows * cols;

        fprintf(stderr, "Showing values for CSR format\n");
        for (int j = 0; j < rows; j++) {
            fprintf(stderr, "Row[%d]    ", j);
            for (int i = A->pointerB[j]; i < A->pointerB[j+1]; i++) {
                fprintf(stderr, "(%.2f,%.2f)    ", A->csr_dcoef[i].re, A->csr_dcoef[i].im );
            }
            fprintf(stderr, "\n");
        }


        fprintf(stderr, "Showing column indices for CSR format\n");
        for (int j = 0; j < rows; j++) {
            fprintf(stderr, "Row[%d]    ", j);
            for (int i = A->pointerB[j]; i < A->pointerB[j+1]; i++) {
                fprintf(stderr, "%d    ", A->col_ind[i] );
            }
            fprintf(stderr, "\n");
        }
    };
    
    template <> inline void ShowMatrix <ELL> ( ELL *A)
    {

    };
    
	template <typename T> inline T* array_new( int len, T value)
	{
		fprintf(stderr, "Creating array of size: %lu bytes\n", sizeof(T) * len);
		T* buffer = (T*) __malloc( len * sizeof(T));
		
		if( ! buffer ){
			printf("Cant allocate array\n");
			abort();
		}

		for (int i = 0; i < len; i++)
			buffer[i] = (T) value;

		return buffer;
	};

	template <typename T> inline T* array_new( int len)
	{
			T* buffer = (T*) __malloc( len * sizeof(T));
		
		if( ! buffer ){
			printf("Cant allocate array\n");
			abort();
		}
		
		for (int i = 0; i < len; i++)
			buffer[i] = (T) i;

		return buffer;
	};


#endif // _INTERFACES_H_
