#include <Python.h>
#include <openssl/conf.h>
#include <openssl/bn.h>
#include <openssl/rand.h>

// Compiler macro for checking python version, for python 3 compatibility
#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif


// By default, use 32 BYTE of randomness
#define RAND_LEN 32

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
	n = BN_new();
	if (n == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "call to BN_new failed");
		return NULL;
	}

	if (BN_generate_prime_ex(n, bitlength, 1, NULL, NULL, NULL) != 1) {
		BN_free(n);
		PyErr_SetString(PyExc_RuntimeError, "call to BN_generate_prime_ex failed");
		return NULL;
	}

	// Convert it to a hex string
	r = BN_bn2hex(n);
	if (r == NULL) {
		BN_clear_free(n);
		PyErr_SetString(PyExc_RuntimeError, "call to BN_bn2hex failed");
		return NULL;
	}

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
	{NULL, NULL, 0, NULL}  /* Sentinel */
};

#ifdef IS_PY3K
static struct PyModuleDef gensafeprimemod =
{
	PyModuleDef_HEAD_INIT,
	"gensafeprime", /* name of module */
	"This module generates safe prime numbers using the OpenSSL library. A number p is a safe prime number, if p is a prime number, and p = 2*q+1, and q is a prime number too. These numbers are sometimes used in cryptography.",          /* module documentation, may be NULL */
	-1,          /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	GenPrime_Methods
};
#endif


#ifdef IS_PY3K
PyMODINIT_FUNC PyInit_gensafeprime(void)
#else
PyMODINIT_FUNC initgensafeprime(void)
#endif
{
	PyObject *modname, *mod, *mdict, *func, *args, *rslt;
	char * rnd;
	// int i;

	// first, standard initialization
#ifndef IS_PY3K
	if (!Py_InitModule("gensafeprime", GenPrime_Methods)) {
		PyErr_SetString(PyExc_RuntimeError, "cannot init module");
		return;
	}
#else
	return PyModule_Create(&gensafeprimemod);
#endif

	// seed the random number generator of OpenSSL using os.urandom(RAND_LEN)
	// According to https://docs.python.org/2.7/library/random.html os.urandom
	// generates secure random numbers for cryptographic applications.

	// Code based on
	// https://www.daniweb.com/software-development/python/threads/31682/calling-python-function-from-cc-
	// but has been heavily modified.

	// Import os and get a reference to os.urandom
#ifndef IS_PY3K
	modname = PyString_FromString("os");
#else
	modname = PyBytes_FromString("os");
#endif

	mod = PyImport_Import(modname);
	if (mod) {
		mdict = PyModule_GetDict(mod);
		func = PyDict_GetItemString(mdict, "urandom");
		if (func) {
			// Call urandom(RAND_LEN)
			if (PyCallable_Check(func)) {
				args = Py_BuildValue("(i)", RAND_LEN);
				rslt = PyObject_CallObject(func, args);
				Py_XDECREF(args);
				// Check the result, when the call failed, rslt will be NULL
				if (rslt) {
					// Get random bytes as char* representation
#ifndef IS_PY3K
					rnd = PyString_AsString(rslt);
#else
					rnd = PyBytes_AsString(rslt);
#endif
					if (rnd) {
						// Check that really RAND_LEN bytes have been returned
#ifndef IS_PY3K
						if (PyString_Size(rslt) == RAND_LEN) {
#else
						if (PyBytes_Size(rslt) == RAND_LEN) {
#endif
							// rnd should now contain RAND_LEN securely random characters
							// terminated by a 0 byte, pass it to OpenSSL RAND_seed
							RAND_seed(rnd, RAND_LEN);

							// DEBUG code, uncomment to see the seed
							//printf("prng has been seeded with");
							//for (i = 0; i < RAND_LEN; i++) {
							//    printf(" %02x", 0xff & rnd[i]);
							//}
							//printf("\n");
						} else {
							PyErr_SetString(PyExc_RuntimeError, "wrong length of random data");
						}
					} else {
						PyErr_SetString(PyExc_RuntimeError, "parsing result failed, rnd == NULL");
					}
					Py_XDECREF(rslt);
					} else {
						PyErr_SetString(PyExc_RuntimeError, "call to urandom returned NULL pointer");
					}
				} else {
					PyErr_SetString(PyExc_RuntimeError, "urandom not callable");
				}
			} else {
				PyErr_SetString(PyExc_RuntimeError, "could not get reference to os.urandom");
			}
			Py_XDECREF(mod);
		} else {
			PyErr_SetString(PyExc_RuntimeError, "could not import os");
		}
		Py_XDECREF(modname);

	}
