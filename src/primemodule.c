#include <Python.h>
#include <openssl/bn.h>

// Generate a safe prime using OpenSSL
static PyObject * gensafeprime_generate(PyObject *self, PyObject *args) {
	PyObject * result; // Return value
	int bitlength; // Desired bitlength of prime
	char * r; // temporary representation as string
	BIGNUM * n; // OpenSSL bignum object holding the prime

	if (!PyArg_ParseTuple(args, "i", &bitlength)) {
		PyErr_SetString(PyExc_TypeError, "parameter must be an integer");
		return NULL;
	}
	
	// Generate the prime
	n = BN_generate_prime(NULL, bitlength, 1, NULL, NULL, NULL, NULL);
	if (n == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "call to BN_generate_prime failed");
		return NULL;
	}

	// Convert it to a hex string
	r = BN_bn2hex(n);

	// Convert it to a python integer
	result = PyLong_FromString(r, NULL, 16);

	// Free the hex string
	OPENSSL_free(r);

	// Free the bignum representation
	BN_clear_free(n);

	// return the result
	return result;
}


// There is only one method in this module
static PyMethodDef GenPrime_Methods[] = {
  {"generate", gensafeprime_generate, METH_VARARGS, "generate(n)\n\nGenerate a safe prime of bitlength 'n'."},
  {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initgensafeprime(void)
{
    (void) Py_InitModule("gensafeprime", GenPrime_Methods);
}
