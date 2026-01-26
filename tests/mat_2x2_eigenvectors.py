# Python code to generate expected values for eigen vectors and values
import numpy as np

a_mat = np.array ([[1.00671141, -0.11835884],
                   [-0.11835884,  1.00671141]])
print ('\nExample matrix: {}\n'.format (a_mat))

# Eigendecomposition of covariance matrix
eig_vals, eig_vecs = np.linalg.eig (a_mat)

print('Eigenvalues \n{}\n'.format(eig_vals))
#print('Eigenvectors \n{}\n'.format(eig_vecs))

for i in range (len (eig_vals)):
    print ('Eigenvalue {} corresponds to Eigenvector {}'.format (eig_vals[i], eig_vecs[:,i]))

a_mat = np.array ([[1, 0],
                   [0, 2]])
print ('\nExample matrix: {}\n'.format (a_mat))

# Eigendecomposition of covariance matrix
eig_vals, eig_vecs = np.linalg.eig (a_mat)

print('Eigenvalues \n{}\n'.format(eig_vals))
#print('Eigenvectors \n{}\n'.format(eig_vecs))

for i in range (len (eig_vals)):
    print ('Eigenvalue {} corresponds to Eigenvector {}'.format (eig_vals[i], eig_vecs[:,i]))
