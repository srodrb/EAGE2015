import sys
import numpy as np
import matplotlib.pylab as plt
import scipy.sparse as sp

def main(argv):
    print 'Generando matriz en formato ELL'
    
    directory = argv[0];
    mname     = argv[1];
    fname     = directory + '/' + mname

    M = np.loadtxt( fname + '.mat')
    dim = 97104

    print 'Matriz cargada en memoria'

    # cargamos los elementos de la matriz por separado
    _rows = M[:,0] -1
    _cols = M[:,1] -1
    _real = M[:,2] 
    _imag = M[:,3] 

    coeffs = _real + 1j * _imag

    # assembly CSR matrix
    M = sp.csr_matrix( (coeffs, (_rows,_cols)), shape=(dim,dim))

    print 'Matriz ensamblada correctamente en formato CSR'
    
    # calculamos las dimensiones de la matriz
    rows,cols = M.shape

    # ahora calculamos el numero maximo de elementos por fila
    ELL_nnz = np.zeros( rows )
    for i in range( rows):
        ELL_nnz[i] = M.indptr[i+1] - M.indptr[i]

    nnz_max = np.max(ELL_nnz)

    # ahora tenemos que crear el formato ELLPACK
    ELL_real = np.zeros( shape=(rows, nnz_max))
    ELL_imag = np.zeros( shape=(rows, nnz_max))
    ELL_col  = np.zeros( shape=(rows, nnz_max)) 

    for i in range(rows):
        elems = M.indptr[i+1] - M.indptr[i]

        # numero de elementos no nulos por fila
        ELL_nnz[i] = elems

        if(elems):
            # copia de coeficientes no nulos en la fila
            ELL_real[i,0:elems] = M.data[ M.indptr[i] : M.indptr[i+1] ].real
            ELL_imag[i,0:elems] = M.data[ M.indptr[i] : M.indptr[i+1] ].imag

            # copia de los indices de columna de los elementos no nulos
            ELL_col[i,0:elems] = M.indices[ M.indptr[i] : M.indptr[i+1] ]

    # damos formato a los coeficientes
    coeffs = np.column_stack( Ell_real.flatten(),T, ELL_imag.flatten().T) 

    # Guardamos los arrays planos
    np.savetxt( fname + '.dat',   coeffs                                   )
    np.savetxt( fname + '.nnz',  (ELL_nnz).astype(int)          , fmt='%d' )
    np.savetxt( fname + '.col',  (ELL_col.flatten()).astype(int), fmt='%d' )
    
    # Generamos el fichero descriptor de la matriz
    # filas, columnas, nnz, len(row_ptr) 
    dims = np.array([ rows, cols, nnz_max] )
    np.savetxt( fname + '.ell_des', dims , fmt='%d')
    
    print 'Matriz generada en formato ELL'


if __name__ == '__main__':
    main( sys.argv[1:] )
