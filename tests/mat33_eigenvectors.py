# Python code to generate expected values for eigen vectors and values
import numpy as np

a_mat = np.array ([[1.00671141, -0.11835884,  0.87760447],
                   [-0.11835884,  1.00671141, -0.43131554],
                   [0.87760447, -0.43131554,  1.00671141]])
print ('a matrix: {}'.format (a_mat))

# Eigendecomposition of covariance matrix
eig_vals, eig_vecs = np.linalg.eig (a_mat)

print('Eigenvalues \n', eig_vals)
print('Eigenvectors \n', eig_vecs)

for i in range (len (eig_vals)):
    print ('Eigenvalue {} corresponds to Eigenvector {}'.format (eig_vals[i], eig_vecs[:,i]))

# Next test
a_mat = np.diag((1, 2, 3))
print ('\n\na matrix: {}'.format (a_mat))

# Eigendecomposition of covariance matrix
eig_vals, eig_vecs = np.linalg.eig (a_mat)

print('Eigenvalues \n', eig_vals)
print('Eigenvectors \n', eig_vecs)

for i in range (len (eig_vals)):
    print ('Eigenvalue {} corresponds to Eigenvector {}'.format (eig_vals[i], eig_vecs[:,i]))
