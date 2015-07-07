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
 *   Organization:  Barcelona Supercomputing Center.
 *
 *          Usage: ./2bin matrix_path
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
    string path       = string( argv[1] ) + "/" + string( argv[2])  + "/" + name;

    // ahora abrimos el fichero descriptor de la matriz
    string descriptor = path + ".des";
	FILE *fdescriptor = fopen( descriptor.c_str() , "r");
	if ( !fdescriptor)
	{
		fprintf(stderr, "No podemos abrir el fichero descriptor\n");
		abort();
	}


	int nrows, ncols, nnz, nptr;
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

	
	if( fscanf( fdescriptor, "%d", &nnz) != 1)
	{
		fprintf(stderr, "No podemos leer el numero de elementos no nulos en la matriz\n");
		abort();
	}

	if( fscanf( fdescriptor, "%d", &nptr) != 1)
	{
		fprintf(stderr, "No podemos leer el numero de elementos en el array de indices\n");
		abort();
	}
	fprintf(stderr, "nnz: %d nptr %d\n", nnz, nptr);
	fclose( fdescriptor);

	// generamos el fichero binario para almacenar la matriz
	fprintf(stderr, "La ruta de destino del fichero binario es %s\n", binary.c_str() );
	FILE *fbin = fopen( bname.c_str(), "wb");
    fwrite( &nrows , sizeof(int), 1, fbin);
    fwrite( &ncols , sizeof(int), 1, fbin);
    fwrite( &nnz   , sizeof(int), 1, fbin);
    fwrite( &nptr  , sizeof(int), 1, fbin);
	
	
	// ahora reservamos memoria y leemos los elementos de la matriz
	int*       row_ptr  = (int*   ) malloc( nptr * sizeof(int   ));
	int*       col_ind  = (int*   ) malloc( nnz  * sizeof(int   ));
	double*    re_val   = (double*) malloc( nnz  * sizeof(double));
	double*    im_val   = (double*) malloc( nnz  * sizeof(double));
	
	// leemos el array de valores REALES
	string real_file = path + ".re";
    fprintf(stderr, "Abriendo el fichero de valores reales %s\n", real_file.c_str() );
	FILE *freal = fopen( real_file.c_str(), "r"); 
	for (int i = 0; i < nnz; i++) 
	{
		if( fscanf( fvalues, "%lf", &re_val[i] ) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de valores reales\n");
			abort();
		}
	}
	fclose( freal);
	fwrite( re_val, sizeof(double), nnz, fbin);
	free  ( re_val); 

    // leemos el array de valores IMAGINARIOS
	string imag_file = path + ".im";
    fprintf(stderr, "Abriendo el fichero de valores imaginarios %s\n", imag_file.c_str() );
	FILE *fimag = fopen( imag_file.c_str(), "r"); 
	for (int i = 0; i < nnz; i++) 
	{
		if( fscanf( fvalues, "%lf", &im_val[i] ) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de valores imaginarios\n");
			abort();
		}
	}
	fclose( fimag );
	fwrite( im_val, sizeof(double), nnz, fbin);
	free( im_val); 
	
    // leemos el array de indices de columna
	string colind_path = path + ".col_ind";
    fprintf(stderr, "Abriendo el fichero de valores %s\n", colind_path.c_str() );
	FILE *findices = fopen( colind_path.c_str(), "r"); 
	for (int i = 0; i < nnz; i++) 
	{
		if( fscanf( findices, "%d", &col_ind[i] ) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de indices de columnas\n");
			abort();
		}
	}
	// display( nnz, col_ind);
	fclose( findices);
	fwrite( col_ind, sizeof(int), nnz, fbin);
	free( col_ind);

	// leemos el array de punteros a fila
	string rowptr_path = path + ".row_ptr";
    fprintf(stderr, "Abriendo el fichero de valores %s\n", rowptr_path.c_str() );
	FILE *frowptr = fopen( rowptr_path.c_str(), "r"); 
	for (int i = 0; i < nptr; i++) 
	{
		if( fscanf( frowptr, "%d", &row_ptr[i] ) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de punteros a filas\n");
			abort();
		}
	}
	// display( nptr, row_ptr );
	fclose( frowptr);
	fwrite( row_ptr, sizeof(int), nptr, fbin);
	free( row_ptr);

	// close binary file
	fclose( fbin);

	return 0;
}
