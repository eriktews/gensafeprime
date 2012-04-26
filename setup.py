from distutils.core import setup, Extension

setup (
	name = 'gensafeprime', 
	version = '1.0', 
	description = 'Generate safe primes using OpenSSL (a prime p = 2*q+1, and q is prime too)', 
	author = 'Erik Tews',
	author_email = 'erik@datenzone.de',
	url = 'https://github.com/eriktews/gensafeprime',
	keywords = ['prime', 'openssl', 'bignum', 'crypto'],
	classifiers = [
		'Programming Language :: Python',
		'Intended Audience :: Developers'],
	ext_modules = [Extension('gensafeprime',sources = ['src/primemodule.c'],libraries = ['ssl'])])
