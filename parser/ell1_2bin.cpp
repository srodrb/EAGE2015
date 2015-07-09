/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *	  Description:  Sirve para almacenar en formato binario el formato ELL-1.
 *	  				Este formato almacena la diagonal principal por un lado como un 
 *	  				vector denso complejo y el resto de la matriz, real sparse, en formato
 *	  				ELL.
 *
 *        Version:  1.0
 *        Created:  17/06/15 18:33:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, const char *argv[])
{

	string directory( argv[1] ); // directorio donde estan contenidas las matrices
	string mname    ( argv[2] ); // nombre de la matriz a procesar
	string path   = directory + "/" + mname;

	string descriptor_path = path  + ".ell_des";

	fprintf( stderr, "La ruta completa al fichero descriptor de la matriz es: %s\n", descriptor_path.c_str() ) ;

	// ahora abrimos el fichero
	FILE *fdescriptor = fopen( descriptor_path.c_str() , "r");
	if ( !fdescriptor)
	{
		fprintf(stderr, "No podemos abrir el fichero descriptor\n");
		abort();
	}

	int nrows, ncols, max_len, nelems;
	if( fscanf( fdescriptor, "%d", &nrows) != 1)
	{
		fprintf(stderr, "No podemos leer el numero de filas en la matriz\n");
		abort();
	}

	if( fscanf( fdescriptor, "%d", &ncols) != 1)
	{
		fprintf(stderr, "No podemos leer el numero de columnas en la matriz\n");
		abort();
	}

	if( fscanf( fdescriptor, "%d", &max_len) != 1)
	{
		fprintf(stderr, "No podemos leer el numero maximo de nnz/row\n");
		abort();
	}

	fprintf(stderr, "nrows: %d ncols %d, max nnz/row %d\n", nrows, ncols, max_len);
	fclose( fdescriptor);

	nelems = nrows * max_len;

	// generamos el fichero binario para almacenar la matriz
	string bname = path + ".ell1";
	fprintf(stderr, "La ruta de destino del fichero binario es %s\n", bname.c_str() );
	FILE *fbin = fopen( bname.c_str(), "wb");
    fwrite( &nrows  , sizeof(int), 1, fbin);
    fwrite( &ncols  , sizeof(int), 1, fbin);
    fwrite( &max_len, sizeof(int), 1, fbin);
	
	
	// ahora reservamos memoria y leemos los elementos de la matriz
	int*       ell_nnz  = (int*   ) malloc( nrows   * sizeof(int   ));
	int*       ell_col  = (int*   ) malloc( nelems  * sizeof(int   ));
	double*    ell_re   = (double*) malloc( nelems  * sizeof(double));
	
	// leemos el array de coeficientes no nulos
	string values_path = path + ".val";
    fprintf(stderr, "Abriendo el fichero de valores %s\n", values_path.c_str() );
	FILE *fvalues = fopen( values_path.c_str(), "r"); 
	if( ! fvalues) {
		fprintf(stderr, "ERROR: no podemos abrir el fichero %s correctamente\n", values_path.c_str() );
		abort();
	}
	for (int i = 0; i < nelems; i++) 
	{
		if( fscanf( fvalues, "%lf", &ell_re[i]) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de valores\n");
			abort();
		}
	}
	fclose( fvalues);
	fwrite( ell_re, sizeof(double), nelems, fbin);
	free  ( ell_re);


	// ahora leemos los elementos de la diagonal principal
	double* dia_re   = (double*) malloc( nrows  * sizeof(double));
	double* dia_im   = (double*) malloc( nrows  * sizeof(double));
	
	// leemos el array de coeficientes no nulos
	string diagonal_path = path + ".dia";
    fprintf(stderr, "Abriendo el fichero de diagonal principal %s\n", diagonal_path.c_str() );
	FILE *fdiag = fopen( diagonal_path.c_str(), "r"); 
	if( ! fvalues) {
		fprintf(stderr, "ERROR: no podemos abrir el fichero %s correctamente\n", diagonal_path.c_str() );
		abort();
	}
	for (int i = 0; i < nrows; i++) 
	{
		if( fscanf( fdiag, "%lf  %lf", &dia_re[i], &dia_im[i] ) != 2)
		{
			fprintf(stderr, "Problema al leer el fichero de valores de la diagonal principal\n");
			abort();
		}
	}
	fclose( fdiag);
	fwrite( dia_re, sizeof(double), nrows, fbin);
	fwrite( dia_im, sizeof(double), nrows, fbin);
	free  ( dia_re);
	free  ( dia_im);





	// leemos el array de indices de columna
	string col_path = path + ".col";
  	fprintf(stderr, "Abriendo el fichero de valores %s\n", col_path.c_str() );
	FILE *fcol = fopen( col_path.c_str(), "r"); 
	if( ! fcol) {
		fprintf(stderr, "ERROR: no podemos abrir el fichero %s correctamente\n", col_path.c_str() );
		abort();
	}
	for (int i = 0; i < nelems; i++) 
	{
		if( fscanf( fcol, "%d", &ell_col[i] ) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de indices de columnas\n");
			abort();
		}
	}
	fclose( fcol);
	fwrite( ell_col, sizeof(int), nelems, fbin);
	free( ell_col);

	// leemos el array de nnz/row
	string nnz_path = path + ".nnz";
  	fprintf(stderr, "Abriendo el fichero de elementos por fila %s\n", nnz_path.c_str() );
	FILE *fnnz = fopen( nnz_path.c_str(), "r"); 
	for (int i = 0; i < nrows; i++) 
	{
		if( fscanf( fnnz, "%d", &ell_nnz[i] ) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de elementos no nulos por fila\n");
			abort();
		}
	}
	// display( nptr, row_ptr );
	fclose( fnnz );
	fwrite( ell_nnz, sizeof(int), nrows, fbin);
	free( ell_nnz);


	// leemos el lado derecho del sistema
	double* RHS_re   = (double*) malloc( nrows  * sizeof(double));
	double* RHS_im   = (double*) malloc( nrows  * sizeof(double));
	string rhs_path = path + ".rhs";
    fprintf(stderr, "Abriendo el fichero del vector del lado derecho del sistema %s\n", rhs_path.c_str() );
	FILE *frhs = fopen( rhs_path.c_str(), "r"); 
	for (int i = 0; i < nrows; i++) 
	{
		int res = fscanf( frhs, "%lf %lf", &RHS_re[i], &RHS_im[i] );
		
		if( res !=2)
		{
			fprintf(stderr, "Problema al leer el fichero de coeficientes complejos (valores leidos %d sobre %d)\n", i, nrows );
			abort();
		}

	}
	fclose( frhs);
	fwrite( RHS_re, sizeof(double), nrows, fbin);
	fwrite( RHS_im, sizeof(double), nrows, fbin);
   	free  ( RHS_re );	
   	free  ( RHS_im );	




	// close binary file
	fclose( fbin);

	return 0;
}
