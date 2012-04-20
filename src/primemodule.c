#include <Python.h>
#include <openssl/bn.h>

static PyObject * gensafeprime_generate(PyObject *self, PyObject *args) {
	PyObject * result;
	int bitlength;
	char * r;
	BIGNUM * n;

	if (!PyArg_ParseTuple(args, "i", &bitlength)) {
		PyErr_SetString(PyExc_TypeError, "parameter must be an integer");
		return NULL;
	}
	
	n = BN_generate_prime(NULL, bitlength, 1, NULL, NULL, NULL, NULL);
	if (n == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "call to BN_generate_prime failed");
		return NULL;
	}
	r = BN_bn2hex(n);
	result = PyLong_FromString(r, NULL, 16);
	OPENSSL_free(r);
	return result;
}


static PyMethodDef GenPrime_Methods[] = {
  {"generate", gensafeprime_generate, METH_VARARGS, "Generate a safe prime for a given bitlength."},
  {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initgensafeprime(void)
{
    (void) Py_InitModule("gensafeprime", GenPrime_Methods);
}
