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
    string path = string( argv[1] ) + "/" + string( argv[2]);
	int count   = 0;

    // ahora abrimos el fichero descriptor de la matriz
    string descriptor = path + ".csr_des";
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
	string binary = path + ".csr";
	fprintf(stderr, "La ruta de destino del fichero binario es %s\n", binary.c_str() );
	FILE *fbin = fopen( binary.c_str(), "wb");
    fwrite( &nrows , sizeof(int), 1, fbin);
    fwrite( &ncols , sizeof(int), 1, fbin);
    fwrite( &nnz   , sizeof(int), 1, fbin);
    fwrite( &nptr  , sizeof(int), 1, fbin);
	
	
	// ahora reservamos memoria y leemos los elementos de la matriz
	// en este punto los leo segun he definido la estructura para complejos
	double*    coefficients   = (double*) malloc( 2 * nnz  * sizeof(double));
	
	// leemos el array de valores COMPLEJOS
	string coefs_path = path + ".val";
    fprintf(stderr, "Abriendo el fichero de coeficientes complejos %s\n", coefs_path.c_str() );
	FILE *fcoefs = fopen( coefs_path.c_str(), "r"); 
	count = 0;
	for (int i = 0; i < nnz; i++) 
	{
		int res = fscanf( fcoefs, "%lf %lf", &coefficients[count], &coefficients[count+1] );
		
		if( i < 10){
			fprintf(stderr, "Valores leidos %.5f,%5fi\n", coefficients[count], coefficients[count +1]);
		}
	
		if( res !=2)
		{
				fprintf(stderr, "Problema al leer el fichero de coeficientes complejos\n");
			abort();
		}

		count += 2;
	}
	fclose( fcoefs);
	fwrite( coefficients, sizeof(double), 2*nnz, fbin);
   	free  ( coefficients );	

	int*       col_ind  = (int*   ) malloc( nnz  * sizeof(int   ));
    // leemos el array de indices de columna
	string cols_path = path + ".col";
    fprintf(stderr, "Abriendo el fichero de valores %s\n", cols_path.c_str() );
	FILE *fcols = fopen( cols_path.c_str(), "r"); 
	for (int i = 0; i < nnz; i++) 
	{
		if( fscanf( fcols, "%d", &col_ind[i] ) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de indices de columnas\n");
			abort();
		}
	}
	// display( nnz, col_ind);
	fclose( fcols);
	fwrite( col_ind, sizeof(int), nnz, fbin);
	free( col_ind);

	// leemos el array de punteros a fila
	int*       row_ptr  = (int*   ) malloc( nptr * sizeof(int   ));
	string rows_path = path + ".row";
    fprintf(stderr, "Abriendo el fichero de punteros a filas %s\n", rows_path.c_str() );
	FILE *frows = fopen( rows_path.c_str(), "r"); 
	for (int i = 0; i < nptr; i++) 
	{
		if( fscanf( frows, "%d", &row_ptr[i] ) != 1)
		{
			fprintf(stderr, "Problema al leer el fichero de punteros a filas\n");
			abort();
		}
	}
	// display( nptr, row_ptr );
	fclose( frows);
	fwrite( row_ptr, sizeof(int), nptr, fbin);
	free( row_ptr);

	

	// leemos el lado derecho del sistema
	double* RHS   = (double*) malloc( 2 * nnz  * sizeof(double));
	string rhs_path = path + ".rhs";
    fprintf(stderr, "Abriendo el fichero del vector del lado derecho de la ecuacion %s\n", rhs_path.c_str() );
	FILE *frhs = fopen( rhs_path.c_str(), "r"); 
	if( !frhs)
	{
		fprintf(stderr, "No podemos abrir el fichero %s\n", rhs_path.c_str() );
		abort();
	}
	count = 0;
	for (int i = 0; i < nrows; i++) 
	{
		int res = fscanf( frhs, "%lf %lf", &RHS[count], &RHS[count+1] );
		
		if( i < 10){
			fprintf(stderr, "Valores leidos %.5f,%5fi\n", RHS[count], RHS[count +1]);
		}
	
		if( res !=2)
		{
			fprintf(stderr, "Problema al leer el fichero de coeficientes complejos (valores leidos %d sobre %d)\n", i, nrows );
			abort();
		}

		count += 2;
	}
	fclose( frhs);
	fwrite( RHS, sizeof(double), 2*nnz, fbin);
   	free  ( RHS );	




	// close binary file
	fclose( fbin);

	return 0;
}
