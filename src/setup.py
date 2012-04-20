from distutils.core import setup, Extension

gensafeprime = Extension('gensafeprime', 
	sources = ['primemodule.c'], 
	libraries = ['ssl'])

setup (
	name = 'gensafeprime', 
	packages = ["gensafeprime"],
	version = '1.0', 
	description = 'Generate safe primes using OpenSSL (a prime p = 2*q+1, and q is prime too)', 
	author = "Erik Tews",
	author_email = "erik@datenzone.de",
	url = "",
	keywords = ["prime", "openssl", "bignum", "crypto"],
	classifiers = [
		"Programming Language :: Python"],
	ext_modules = [gensafeprime])
