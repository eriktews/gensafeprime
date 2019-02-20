from distutils.core import setup, Extension

setup (
	name = 'gensafeprime', 
	version = '1.6', 
	description = 'Generate safe primes using OpenSSL (a prime p = 2*q+1, and q is prime too)', 
	long_description = open('README.txt').read(),
	author = 'Erik Tews',
	author_email = 'erik@datenzone.de',
	url = 'https://github.com/eriktews/gensafeprime',
	keywords = ['prime', 'openssl', 'bignum', 'crypto'],
	license = 'MIT',
	platforms = ['Unix', 'Windows', 'Posix', 'Linux'],
	classifiers = [
		'Development Status :: 4 - Beta',
		'Environment :: No Input/Output (Daemon)',
		'Programming Language :: Python',
		'Programming Language :: Python :: 2.7',
		'License :: OSI Approved :: MIT License',
		'Topic :: Scientific/Engineering :: Mathematics',
		'Topic :: Software Development :: Libraries',
		'Operating System :: POSIX',
		'Operating System :: Microsoft :: Windows',
		'Operating System :: Unix',
		'Intended Audience :: Developers'],
	ext_modules = [Extension('gensafeprime',sources = ['src/primemodule.c'],libraries = ['ssl','crypto'])])
