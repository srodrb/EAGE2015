import sys
import numpy as np
from scipy.sparse import csr_matrix

def main(argv):
    print 'Generando matriz en formato CSR'
    directory = argv[0];
    mname     = argv[1];
    fname     = directory + '/' + mname

    # load plain text matrix
    M = np.loadtxt( fname + '.mat' )
    dim = 97104

    print 'Matriz cargada en memoria'
        
    # load data separately
    _rows = M[:,0] -1
    _cols = M[:,1] -1
    _real = M[:,2]
    _imag = M[:,3]

    coeffs  = _real + 1j * _imag

    M = csr_matrix( (coeffs, (_rows, _cols)), shape=(dim,dim))

    print 'Matriz convertida a formato CSR'

    np.savetxt( fname + '.val',   np.column_stack((M.data.real,M.data.imag)))
    np.savetxt( fname + '.row',  (M.indptr ).astype(int), fmt='%d' )
    np.savetxt( fname + '.col',  (M.indices).astype(int), fmt='%d' )

    print 'Matriz guardada'

    dims = np.array([ M.shape[0], M.shape[1], M.nnz, len(M.indptr)])

    print 'Descriptores de la matriz', dims

    np.savetxt( fname + '.csr_des', dims , fmt='%d')

    print 'Creado fichero descriptor de la matriz'

    rhs = np.loadtxt( fname + ".rhs")
    np.savetxt( fname + ".rhs", np.column_stack((rhs[:,0], rhs[:,1])) )

    print 'Creado el fichero para el lado derecho del sistema'


if __name__ == '__main__':
    main( sys.argv[1:] )
