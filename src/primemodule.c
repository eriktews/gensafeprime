#include <Python.h>
#include <openssl/conf.h>
#include <openssl/bn.h>
#include <openssl/rand.h>

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
    PyObject *modname, *mod, *mdict, *func, *args, *rslt;
    char * rnd;
    // int i;

    // first, standard initialization
    if (!Py_InitModule("gensafeprime", GenPrime_Methods)) {
        PyErr_SetString(PyExc_RuntimeError, "cannot init module");
        return;
    }

    // seed the random number generator of OpenSSL using os.urandom(RAND_LEN)
    // According to https://docs.python.org/2.7/library/random.html os.urandom
    // generates secure random numbers for cryptographic applications.

    // Code based on 
    // https://www.daniweb.com/software-development/python/threads/31682/calling-python-function-from-cc-
    // but has been heavily modified.

    // Import os and get a reference to os.urandom
    modname = PyString_FromString("os");
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
                    rnd = PyString_AsString(rslt);
                    if (rnd) {
                        // Check that really RAND_LEN bytes have been returned
                        if (PyString_Size(rslt) == RAND_LEN) {
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
