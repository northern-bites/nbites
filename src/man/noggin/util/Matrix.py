
import math
import sys

__author__ = "Mark McGranaghan, Jeremy Fishman"

print >>sys.stderr, "****************************************************"
print >>sys.stderr, "** WARNING - Importing pure-Python Matrix library **"
print >>sys.stderr, "**    Performance may, no, will, be degraded      **"
print >>sys.stderr, "****************************************************"


class Matrix:
    """Lightweight, native Python matrix class supporting the creation of constant-valued, identity, and arbitrary-valued matricies, and their copying, scaling, addition, multiplication, transpose, and inverse.  Also preforms arbitrarily-high-dimension regurlar and inverse least-squares regressions and inverse interpolations for 2-dimensional data.  In general this class does not sanity-check, so if something on the dog breaks you should probably check what its doing with this class.  This non-regression aspects of this class borrow very heavily from the JAMA Java Matrix library's interface and implementation."
    __init__()  ( Matrix() )
    withValues()
    identity()
    copy()
    get()
    getArrayCopy()
    getMatrix()
    set()
    setValues()
    scale()
    scaleEquals()
    plus()
    plusArray()
    plusEquals()
    minus()
    matrixSum()
    matrixDifference()
    times()
    transpose()
    inverse()
    // wrapper methods to present identical API as C++/python implementation
    withvalues()
    add()
    subtract()
    multiply()
    invert()

    __computeLeastSquaresCoefs()
    __applyRegressionCoefs()
    leastSquares()
    inverseLeastSquares()
    interpolation()
    inverseInterpolation()
    __interpolate()
    __between()
    """

    #  m     number of rows in matrix
    #  n     number of columns in matrix
    #  rows  list of individual rows in matrix, which are in turn lists

    def __init__(self, num_rows, num_columns = 1, initialize_value = 0.0):
        """Matrix.__init__(rows, columns): if called with 2 arguments, constructs a 0.0-valued matrix with the specified number of rows and columns.  If a 3rd argument is provided and that value is a scalar, constructs a matrix with all entries set to the constant value.  If the provided 3rd argument is a list, constructs a matrix with the entries drawn from that list, filling the entries in from left to right and then from top to bottom.  If the first argument is a matrix instance, creates a copy of that instance.  Usage: my_new_0_valued_matrix = Matrix(my_num_rows, my_num_columns)  OR  my_new_constant_valued_matrix = Matrix(my_num_rows, my_num_columns, my_constant_value)"""
        # Assumes that initialize_value is either a list or a scalar, and that num_rows is a scalar or a matrix object
        # The unfortunate overloading of num_rows is due to the need to conform to the API specified by the C++/python implementation
        if isinstance(num_rows, Matrix):
            # num_rows is a Matrix instance
            self.m = num_rows.m
            self.n = num_rows.n
            self.rows = num_rows.getArrayCopy()
        elif isinstance(initialize_value, list):
            self.m = num_rows
            self.n = num_columns
            self.rows = [ [initialize_value]*self.n for i in xrange(self.m)]
            for i in xrange(min(len(initialize_value), num_rows*num_columns)):
                setting_at_column = i % num_columns
                setting_at_row = i / num_columns
                self.set(setting_at_row, setting_at_column, initialize_value[i])
        else:
            self.m = num_rows
            self.n = num_columns
            self.rows = [ [initialize_value]*self.n for i in xrange(self.m)]

    def copy(self):
        """copy(): returns a deep copy of self"""
        return self.scale(1.0)

    def copyValues(self, from_matrix):
        """
        copyValues(): copies the values from the argument matrix into self's rows
        """
        for row_index in xrange(self.m):
            for column_index in xrange(self.n):
                self.rows[row_index][column_index] = from_matrix.rows[row_index][column_index]

    def get(self, row_index, column_index):
        """get(row, column): returns the value at a specific location in the matrix"""
        return self.rows[row_index][column_index]

    def getArrayCopy(self):
        return [self.rows[r][:] for r in xrange(self.m)]

    def getMatrix(self, r, j0, j1):
        """Get a copy of a submatrix of self, from the list or row indices r and the initial and final column indicies j0 and j1."""
        X = Matrix(len(r), j1-j0+1)
        for i in xrange(0, len(r)):
            for j in xrange(j0, j1 + 1):
                X.set(i, j-j0, self.rows[r[i]][j])
        return X

    def set(self, row_index, column_index, to_value):
        """set(row, column, value): sets the value at a specific location in the matrix"""
        self.rows[row_index][column_index] = to_value

    def setValues(self, matrix_values):
        """set(values[][]): sets the matrix values to those contained in the 2D list argument"""
        for row_index in xrange(self.m):
            for column_index in xrange(self.n):
                self.rows[row_index][column_index] = matrix_values[row_index][column_index]


    def scale(self, scalar):
        """sclae(c): returns B such that B = c * self"""
        scaled_matrix = Matrix(self.m, self.n)
        for row_index in xrange(self.m):
            for column_index in xrange(self.n):
                scaled_matrix.rows[row_index][column_index] = self.rows[row_index][column_index] * scalar
        return scaled_matrix

    def scaleEquals(self, scalar):
        """
        scaleEquals(c): scale in place, changing the values in self such that equal the previous values times the scalar
        """
        for row_index in xrange(self.m):
            for column_index in xrange(self.n):
                self.rows[row_index][column_index] *= scalar

    def plus(self, adding_matrix):
        """plus(A): returns B such that B = self - A"""
        sum_matrix = Matrix(self.m, self.n)
        for row_index in xrange(self.m):
            for column_index in xrange(self.n):
                sum_matrix.rows[row_index][column_index] = self.rows[row_index][column_index] + adding_matrix.rows[row_index][column_index]
        return sum_matrix


    def plusArray(self, adding_array):
        """
        plusArray(adding_array): adds the values in the 2D list adding_array to self.  Doesen't have the overhead of creating a new matrix like plus() does.
        """
        for row_index in xrange(self.m):
            for column_index in xrange(self.n):
                self.rows[row_index][column_index] += adding_array[row_index][column_index]

    def plusEquals(self, adding_matrix):
        """
        plusEquals(adding_matrix): adds adding_matrix to self in place, overwriting the previous values of self.
        """
        for row_index in xrange(self.m):
            for column_index in xrange(self.n):
                self.rows[row_index][column_index] += adding_matrix.rows[row_index][column_index]


    def matrixSum(self, adding_matrix1, adding_matrix2):
        """
        matrixSum(adding_matrix1, adding_matrix2): updates the values of self to be the the sum of the respective values in the two matrix addends.
        """
        for row_index in xrange(self.m):
            for column_index in xrange(self.n):
                self.rows[row_index][column_index] = adding_matrix1.rows[row_index][column_index] + adding_matrix2.rows[row_index][column_index]


    def minus(self, subtracting_matrix):
        """minus(A): returns B such that B = self - A"""
        return (self.plus(subtracting_matrix.scale(-1.0)))

    def matrixDifference(self, from_matrix, subtract_matrix):
        """matrixDifferenc(from_matrix, subtract_matrix): similarly to matrixSum(), updates the values of self to be the difference between the respective values of the two argument matricies.
        """
        for row_index in xrange(self.m):
            for column_index in xrange(self.n):
                self.rows[row_index][column_index] = from_matrix.rows[row_index][column_index] - subtract_matrix.rows[row_index][column_index]


    def times(self, multiplying_matrix):
        """times(A): returns B such that B = self * A, in the matrix algebraic sense"""
        B = [ [0.0]*multiplying_matrix.n for i in xrange(self.m)]
        mult_matr_colj = [0.0] * self.n
        for j in xrange(multiplying_matrix.n):
            for k in xrange(self.n):
                mult_matr_colj[k] = multiplying_matrix.rows[k][j]
            for i in xrange(self.m):
                selfrowi = self.rows[i]
                s = 0
                for k in xrange(self.n):
                    s += selfrowi[k] * mult_matr_colj[k]
                B[i][j] = s
        return withValues(self.m, multiplying_matrix.n, B)

    def transpose(self):
        """transpose(A): returns the matrix B that is the transpose of A"""
        transposed_matrix = Matrix(self.n, self.m)
        for orig_row_index in xrange(self.m):
            for orig_column_index in xrange(self.n):
                transposed_matrix.rows[orig_column_index][orig_row_index] = self.rows[orig_row_index][orig_column_index]
        return transposed_matrix

    def inverse(self):
        """self(A): returns B such that B = A^-1"""
        # First make sure that all of the entries are in decimal, and not integer, form
        # The algorithms below do not neccisarily work with integral entries
        for i in xrange(self.m):
            for j in xrange(self.n):
                self.rows[i][j] *= 1.0
        # The calculation is simpler if the array is 2x2.  This is usefull becuase it simplifies linear regression calculations
        # http://www.analyzemath.com/Calculators/InverseMatrixCalculator.html
        if self.m == 2 and self.n == 2:
            temp = Matrix(2,2)
            temp.set(0,0, self.get(1,1))
            temp.set(0,1, self.get(0,1)*-1)
            temp.set(1,0, self.get(1,0)*-1)
            temp.set(1,1, self.get(0,0))
            return temp.scale(1.0/((self.get(0,0) * self.get(1,1)) - (self.get(0,1) * self.get(1,0))))
        else:
            identity_matrix = identity(self.m)
            return self.solve(identity_matrix)

    def solve(self, B):
        """solve(B) returns A such that A * self = B"""
        decomposition = LUDecomposition(self)
        return decomposition.solveLU(B)

    # Wrapper methods to present identical API as C++/python implementation
    def add(self, adding_matrix):
        return self.plus(adding_matrix)
    def subtract(self, subtracting_matrix):
        return self.minus(subtracting_matrix)
    def multiply(self, multiplying_matrix):
        return self.times(multiplying_matrix)
    def invert(self):
        return self.inverse()


class LUDecomposition:
    """An LU matrix decomposition helps us to solve systems of linear equations.  If you don't know why that is the case but would like to, please consult a Linear Algebra text book.  See also the notes under the Matrix class heading.
    __init__()
    solveLU()"""

    # m        number of rows
    # n        number of columns
    # LU       list of individual rows in LU matrix
    # piv      pivot vector
    # pivsign  pivot sign

    def __init__(self, A):
        """Preform an LU decomposition on the matrix argument, using Gaussian elimination.  Usage: my_LU_decomposition = LUDecomposition(matrix_to_decompose)"""
        self.LU = A.getArrayCopy()
        self.m = A.m
        self.n = A.n
        self.piv = []
        self.piv = [i for i in xrange(self.m)]
        pivsign = 1

        for k in xrange(self.n):
            p = k
            for i in xrange(k+1, self.m):
                if abs(self.LU[i][k]) > abs(self.LU[p][k]):
                    p = i

            if p != k:
                for j in xrange(self.n):
                    t = self.LU[p][j]
                    self.LU[p][j] = self.LU[k][j]
                    self.LU[k][j] = t
                t = self.piv[p]
                self.piv[p] = self.piv[k]
                self.piv[k] = t
                pivsign = -pivsign
            if self.LU[k][k] != 0.0:
                for i in xrange(k + 1, self.m):
                    self.LU[i][k] /= self.LU[k][k]
                    for j in xrange(k +1, self.n):
                        self.LU[i][j] -= self.LU[i][k]*self.LU[k][j]

    def solveLU(self, B):
        """solveLU(B) Use the LU decomposition to solve the system of linear equations"""
        nx = B.n
        Xmat = B.getMatrix(self.piv, 0, nx-1)
        X = Xmat.getArrayCopy()

        for k in xrange(self.n):
            for i in xrange(k+1, self.n):
                for j in xrange(0, nx):
                    X[i][j] -= X[k][j]*self.LU[i][k]

        for k in xrange(self.n - 1, -1, -1):
            for j in xrange(B.n):
                X[k][j] /= self.LU[k][k]
            for i in xrange(0, k):
                for j in xrange(B.n):
                    X[i][j] -= X[k][j]*self.LU[i][k]
        Xmat.setValues(X)
        return Xmat

def withValues(num_rows, num_cols, values):
    """withValues(rows, columns, values): convenience method that constructs a matrix of the specified size and populates it with the entry values from the 3rd argument, which should be a list of lists which contain the values for each row."""
    X = Matrix(0,0)
    X.m = num_rows
    X.n = num_cols
    X.rows = values
    return X

def withvalues(num_rows, num_cols, values):
    """Similar to origional withValues, only here the argument values is in the form of a single, list of scalar values.  See the called function for more information."""
    return Matrix(num_rows, num_cols, values)

def identity(num_rows):
    """identity(rows): convenience method that constructs a square identity matrix with the specified number of rows."""
    identity_matrix = Matrix(num_rows, num_rows)
    for diagonal_index in xrange(identity_matrix.n):
        identity_matrix.rows[diagonal_index][diagonal_index] = 1.0
    return identity_matrix

def __computeLeastSquaresCoefs(x_values, y_values, regression_dimension):
    """__computeLeastSquaresCoefs(x_data, y_data, regression_dimension): returns a list of regression_dimension+1 coefficients for a leastSquares regression of the specified dimension, with the first coefficient in the list being the constant term, the second in the list being for the x^1 term, the third for the x^2 term, etc.  Usage for, e.g., a cubic regression on 5 (x,y) points: my_4_coefficients = leastSquares([x1,x2,x3,x4,x5],[y1,y2,y3,y4,y5],3)."""
    observation_vector = Matrix(len(y_values), 1)
    for obs_row in xrange(0, len(y_values)):
        observation_vector.set(obs_row, 0, y_values[obs_row])
    input_matrix = Matrix(len(x_values), regression_dimension + 1)
    for input_row in xrange(0, len(x_values)):
        for input_column in xrange(0, regression_dimension + 1):
            input_matrix.set(input_row, input_column, pow(x_values[input_row], input_column))
    regression_coefs = ((input_matrix.transpose().times(input_matrix)).inverse()).times((input_matrix.transpose()).times(observation_vector))
    return regression_coefs.transpose().getArrayCopy()[0]

def leastSquaresCoefs(x_values, y_values, regression_dimension):
    """
    Included for compatibility with the C matrix API.

    __computeLeastSquaresCoefs(x_data, y_data, regression_dimension):
    returns a list of length regression_dimension+1 coefficients for a
    leastSquares regression of the specified dimension, with the first
    coefficient in the list being the constant term, the second in the list
    being for the x^1 term, the third for the x^2 term, etc.
    Usage for, e.g., a cubic regression on 5 (x,y) points:
    my_4_coefficients = leastSquares([x1,x2,x3,x4,x5],[y1,y2,y3,y4,y5],3).
    """
    observation_vector = Matrix(len(y_values), 1, 0.)
    for obs_row in xrange(0, len(y_values)):
        observation_vector.set(obs_row, 0, y_values[obs_row])
    input_matrix = Matrix(len(x_values), regression_dimension + 1, 0.)
    for input_row in xrange(0, len(x_values)):
        for input_column in xrange(0, regression_dimension + 1):
            input_matrix.set(input_row, input_column,
                             pow(x_values[input_row], input_column))
    regression_coefs_vector = ((input_matrix.transpose().multiply(
        input_matrix)).invert()).multiply(
        (input_matrix.transpose()).multiply(observation_vector))

    # Extract the coefficients from the matrix into a list
    regression_coefs_list = \
        [regression_coefs_vector.get(c,0) for c in range(0,regression_dimension + 1)]
    return regression_coefs_list

def __applyRegressionCoefs(coefs, input_value):
    """applyRegressionCoefs(): Computes the value at input_value of a polynomial function characatersized be the coefficients in the first argument"""
    output_value = 0
    for power in xrange(0, len(coefs)):
        output_value += coefs[power] * pow(input_value, power)
    return output_value

def leastSquares(x_values, y_values, regression_dimension):
    """leastSquares(x_values, y_values, regression_dimension): Provides a funciton mapping from x values to y values based on a regression of the provided data points.  to assure that you only ask for the image of reasonable x values, the funciton also returned list also includes the minumum and maxium x values for which the function is valid.  """
    # Compute the regression coefs from the provided data points
    coefs = __computeLeastSquaresCoefs(x_values, y_values, regression_dimension)
    # Generate and return the regression function and minumum and maximum valid x values
    regression = [lambda input_value:__applyRegressionCoefs(coefs, input_value), min(x_values), max(x_values)]
    return regression

def inverseLeastSquares(x_values, y_values, regression_dimension):
    """inverseLeastSquares(): Provides an inverse function mapping from y values to x values based on a regresion of the provided data points.  For example, if the x_values represent odemetry raw values and the y_values represent actual speeds, inverseLeastSquares() provides a function mapping from actual speeds to y_value.   To assure that you only ask for inverse of reasonable y values, the function also provides the minimum and maximum y values that the mapping should be valid for."""
    # Find the minumum and maximum y values in the data set, and the list indecies for their data points
    min_y, min_y_index = y_values[0], 0
    max_y, min_y_index = y_values[0], 0
    for data_index in xrange(1, len(y_values)):
        if y_values[data_index] < min_y:
            min_y, min_y_index = y_values[data_index], data_index
        if y_values[data_index] > max_y:
            max_y, max_y_index = y_values[data_index], data_index
    # Slice the data lists to include only those points between the min and max y values
    # First, check if the min y value has a higher list index than the max y value
    min_list_index = min(min_y_index, max_y_index)
    max_list_index = max(min_y_index, max_y_index)
    use_x_values, use_y_values = x_values[min_list_index:max_list_index+1], y_values[min_list_index:max_list_index+1]
    # Return the triple, switch the x and y values in the applyRegressionCoefs() arguments becuase we want to get an inverse function
    coefs = __computeLeastSquaresCoefs(use_y_values, use_x_values, regression_dimension)
    inverseRegression = [lambda input_value:__applyRegressionCoefs(coefs, input_value), min_y, max_y]
    return inverseRegression

def interpolation(x_values, y_values):
    """interpolation(x_values, y_values): provies a function mapping from x values to y values based on linear interpolation between the provided data points.  Additionaly returns the minumum and maximum x values for witch the funciton is valid."""
    interp = [lambda input_value: __interpolate(x_values, y_values, input_value), min(x_values), max(x_values)]
    return interp

def inverseInterpolation(x_values, y_values):
    """inverseInterpolation(): provides an inverse function mapping from y vlaues to x values based on linear interpolation between the provided data points.  Like inverseLeastSquares(), inverseInterpolate() returns the suggested minimum and maximum y values to ask for the inverse of, along with the inverse funciton itself."""
    # Find the minumum and maximum y values in the data set, and the list indicies for their data points
    min_y, min_y_index = y_values[0], 0
    max_y, max_y_index = y_values[0], 0
    for data_index in xrange(1, len(y_values)):
        if y_values[data_index] < min_y:
            min_y, min_y_index = y_values[data_index], data_index
        if y_values[data_index] > max_y:
            max_y, max_y_index = y_values[data_index], data_index
    # Slice the data lists to include only those points between the min and max y values
    # First, check if the min y value has a higher list index than the max y value
    min_list_index = min(min_y_index, max_y_index)
    max_list_index = max(min_y_index, max_y_index)
    use_x_values, use_y_values = x_values[min_list_index:max_list_index+1], y_values[min_list_index:max_list_index+1]
    # Return the triple, switching the x and y values in the interpolate() arguments becuase we want to get an inverse function
    inverseInterpolation = [lambda input_value: __interpolate(use_y_values, use_x_values, input_value), min_y, max_y]
    return inverseInterpolation

def __interpolate(x_values, y_values, for_x_value):
    """__interpolate(): uses the (x, y) data points given by the first two arguments to linearly interpolate the y value for the argument for_x_value.  Note that this approximation method is only valid when the given data points are reasonably reflective of the underlying function and do not include excessive noise.  For approximating data points given many noisy values, a least squares regression is more appropriate.  Note that it is only possible to interpolate for x values that lie between 2 x values given in the data arguments"""
    #Ensure that the x input is valid
    assert (for_x_value >= min(x_values)) and (for_x_value <= max(x_values)), "Cannot interpolate: input value not within data domain"
    #Find a pair of data points to intepolate between
    left_index = 0
    while not __between(for_x_value, x_values[left_index], x_values[left_index + 1]):
        left_index += 1
    # Interpolate
    interpolated_slope = (y_values[left_index] - y_values[left_index + 1]) / (x_values[left_index] - x_values[left_index + 1])
    x_displacement = for_x_value - x_values[left_index]
    return y_values[left_index] + (interpolated_slope * x_displacement)

def __between(for_value, a, b):
    """ __between(for_value, a, b): Checks if for_value is between the values a and b.  Note that a is not neccisarily smaller than b.  If for_value = a or =b, then we consider it to be between."""
    if (for_value >= a) and (for_value <=b):
        return True
    elif (for_value <= a) and (for_value >=b):
        return True
    else:
        return False

# Tests
# To check the state of any of the test matricies, insert print(vars(<matrix_name>)) after the operation in question

def test(verbose=False):

    if verbose:
        print '---Matrix Tests---\n'

    if verbose:
        print 'Constructing empty matrix'
    test1 = Matrix(3,3)

    if verbose:
        print 'Creating arbitrary constant-valued matrix'
    test2 = Matrix(1, 4, 3.1415)

    if verbose:
        print 'Getting previosly set value'
    test2.get(0, 2)

    if verbose:
        print 'Changing single value in matrix'
    test2.set(0,0, 7.0)

    if verbose:
        print 'Creating additional matrix as a scaling of existing matrix'
    test3 = test2.scale(4)

    if verbose:
        print 'Creating new scaled matrix and replincing existing matrix'
    test3 = test3.scale(2)

    if verbose:
        print 'Constructing matrix with arbitrary values'
    test4 = withValues(2, 2, [[1,2],[3,4]])

    if verbose:
        print 'Creating an identity matrix'
    test5 = identity(2)

    if verbose:
        print 'Adding matricies'
    test6 = withValues(2,2,[[5,6],[7,8]])
    sum_matrix = test6.plus(test4)

    if verbose:
        print 'Subtracting matricies'
    test7 = test6.minus(test4)

    if verbose:
        print 'Multiplying matricies'
    test7half = test6.times(test4)

    if verbose:
        print 'Performing transpose'
    test8 = withValues(2, 3, [[1,2,3,4],[5,6,7,8]])
    test9 = test8.transpose()

    if verbose:
        print 'Calculating LU of a matrix'
    test10 = withValues(3, 3, [[6., -2., 0.],[9., -1., 1.],[3., 7., 5.]])
    test11 = LUDecomposition(test10)

    if verbose:
        print 'Using LU decomposition to calculate inverse of a square matrix'
    test12 = withValues(3, 3, [[1.,-1.,3.],[2.,1.,2.],[-2.,-2.,1.]])
    test13 = test12.inverse()
    if verbose:
        print test13.rows

    test14 = withValues(3, 3, [[8., 36., 204.], [36., 204., 1296.], [204., 1296., 8772.]])
    test15 = test14.inverse()
    if verbose:
        print test15.rows

    test16 = withValues(3, 3, [[8, 36, 204], [36, 204, 1296], [204, 1296, 8772]])
    test17 = test16.inverse()
    if verbose:
        print test17.rows

    print '\n\n'

    """if verbose:
    print 'Speed test: Performing 10,000 3x3 matrix inversions'
    for i in xrange(0, 10000):
        testi = test12.inverse()"""

    if verbose:
        print '---Approximation Function Tests---\n'

    x_values = [1290., 1350., 1470., 1600., 1710., 1840., 1980., 2230., 2400., 2930.]
    y_values = [1182., 1172., 1264., 1493., 1571., 1711., 1804., 1840., 1956., 1954.]

    if verbose:
        print 'Using cubic regression to approximate mapping of x to y values'
    cubic_function, min_x, max_x = leastSquares(x_values, y_values, 3)
    if verbose:
        print 'We can apply this regression to x values between', min_x, 'and', max_x
    if verbose:
        print 'For example, the image of 1600 is', cubic_function(1600), '(should be about 1500)\n'

    if verbose:
        print 'Using inverse cubic regression to approximate mapping of y to x values'
    inverse_polynomial, min_y, max_y = inverseLeastSquares(x_values, y_values, 3)
    if verbose:
        print 'We can apply this regression to values between', min_y, 'and', max_y
    if verbose:
        print 'For example, the inverse of 1600 is', inverse_polynomial(1600), '(should be about 1720)\n'

    if verbose:
        print 'Testing effect of order on polynomial dimension'
    for dimension in xrange(1, 5):
        regression_function, min_x, max_x = leastSquares(x_values, y_values, dimension)
        sum_SE = 0.0
        for residual_num in xrange(0, len(y_values)):
            estimate = regression_function(x_values[residual_num])
            square_error = pow(y_values[residual_num] - (estimate),2.0)
            sum_SE += square_error
        RMSE = math.sqrt(sum_SE / len(x_values))
        if verbose:
            print 'RMSE for order', dimension, 'is', RMSE
    if verbose:
        print ''

    if verbose:
        print 'Using interpolation to approximate mapping of x to y values'
    interpolation_function, min_x, min_y = interpolation(x_values, y_values)
    if verbose:
        print 'We can interpolate for x values between', min_x, 'and', max_x
    if verbose:
        print 'For example, the image of 1290 is', interpolation_function(1290), '(should be 1182)\n'

    if verbose:
        print 'Using inverse interpolation to approximate mapping of y to x values'
    inverse_interpolation_function, min_y, max_y = inverseInterpolation(x_values, y_values)
    if verbose:
        print 'We can interpolate for y values between', min_y, 'and', max_y
    if verbose:
        print 'For example, the inverse of 1290 is', inverse_interpolation_function(1290), '(should be about 1500)\n'
    if verbose:
        x_values = [1,2,3,4,5]
        #y_values = [0,20,60,80,100]
        y_values = [100,100,100,100,100]
        lin_func, min_x, max_x = leastSquares(x_values, y_values, 1)
        for x in x_values:
            print lin_func(x)
    if verbose:
        print '---No crashes - Yeah!---\n\n'

if __name__ == '__main__':
    test(True)
