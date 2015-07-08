/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  El programa transforma los ficheros en texto plano que salen de
 *    				convertir el formato Matrix Market en formato CSR usando el script
 *    				de Python.
 *
 *    				Todos los ficheros generados se convierten en un solo formato binario
 *    				que podemos leer para cargar la matriz.
 *
 *    				Entrada: 1.-  directorio de las matrices
 *    						 2.-  d
 *
 *    				En el fichero de salida se guardan los siguientes valores:
 *    					- nrows (int)
 *    					- ncols (int)
 *    					- nnz  (int )
 *    					- nptr (int )
 *    					- val  (array double; size nnz )
 *    					- col_ind (array int; size nnz )
 *    					- row_ptr (array int; size nptr)
 *
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
	string bname = path + ".ell";
	fprintf(stderr, "La ruta de destino del fichero binario es %s\n", bname.c_str() );
	FILE *fbin = fopen( bname.c_str(), "wb");
    fwrite( &nrows  , sizeof(int), 1, fbin);
    fwrite( &ncols  , sizeof(int), 1, fbin);
    fwrite( &max_len, sizeof(int), 1, fbin);
	
	
	// ahora reservamos memoria y leemos los elementos de la matriz
	int*       ell_nnz  = (int*   ) malloc(   nrows   * sizeof(int   ));
	int*       ell_col  = (int*   ) malloc(   nelems  * sizeof(int   ));
	double*    ell_dat  = (double*) malloc( 2*nelems  * sizeof(double));
	
	// leemos el array de coeficientes no nulos
	string values_path = path + ".dat";
    fprintf(stderr, "Abriendo el fichero de valores %s\n", values_path.c_str() );
	FILE *fvalues = fopen( values_path.c_str(), "r"); 
    int count = 0;
	for (int i = 0; i < nelems; i++) 
	{
		if( fscanf( fvalues, "%lf  %lf", &ell_dat[count], &ell_dat[count+1]) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de valores\n");
			abort();
		}
        count += 2;
	}
	fclose( fvalues);
	fwrite( ell_dat, sizeof(double), 2*nelems, fbin);
	free  ( ell_dat); 

	// leemos el array de indices de columna
	string colind_path = path + ".col";
  	fprintf(stderr, "Abriendo el fichero de valores %s\n", colind_path.c_str() );
	FILE *findices = fopen( colind_path.c_str(), "r"); 
	for (int i = 0; i < nelems; i++) 
	{
		if( fscanf( findices, "%d", &ell_col[i] ) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de indices de columnas\n");
			abort();
		}
	}
	fclose( findices);
	fwrite( ell_col, sizeof(int), nelems, fbin);
	free( ell_col);

	// leemos el array de nnz/row
	string rowptr_path = path + ".nnz";
  	fprintf(stderr, "Abriendo el fichero de valores %s\n", rowptr_path.c_str() );
	FILE *frowptr = fopen( rowptr_path.c_str(), "r"); 
	for (int i = 0; i < nrows; i++) 
	{
		if( fscanf( frowptr, "%d", &ell_nnz[i] ) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de punteros a filas\n");
			abort();
		}
	}
	// display( nptr, row_ptr );
	fclose( frowptr);
	fwrite( ell_nnz, sizeof(int), nrows, fbin);
	free( ell_nnz);

	// close binary file
	fclose( fbin);

	return 0;
}
