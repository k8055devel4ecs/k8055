import sys,os
from distutils.core import setup
from distutils.extension import Extension
from Pyrex.Distutils import build_ext

setup(
	name = 'pyk8055lib',
	version = '1.0.0',
	description = 'Wrapper for K8055',
	url = 'bhennigat.homedns.org',
	author = 'Bjoern Hennig',
	author_email = 'b.hennig@gmx.de',
	license = 'bsd',
	platforms = 'os2',
	long_description = 'Wrapper for K8055 on ecs with adc-,input- aund output-support',
	ext_modules = [
		Extension("pyk8055lib", ["pyk8055lib.pyx"],
		extra_compile_args=["-DOS2","-DTEST_ON=1"],
		extra_link_args=["-lK8055DD.lib"]),
	],
	cmdclass = {'build_ext': build_ext}
)
