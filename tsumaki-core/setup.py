#!/usr/bin/env python

from setuptools import dist
dist.Distribution().fetch_build_eggs(['cython', 'numpy'])

import sys
import os
import platform
import numpy as np

from Cython.Build import cythonize
from setuptools import find_packages
from distutils.core import setup
from distutils.extension import Extension

install_requires = ["numpy", "Pillow", "click", "plaidml-keras", "protobuf"]
if sys.version_info[0] >= 3 and sys.version_info[0] < 7:
    install_requires.append('dataclasses')


setup(
    name="tsumaki-core",
    version="0.0.1",
    description="Tsumaki: detect human in blink",
    author="Alchan Kim",
    author_email="a9413miky@gmail.com",
    setup_requires = ["cython", "numpy"],
    install_requires=install_requires,
    python_requires=">=3.6",
    license="MIT",
    classifiers=[
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: Implementation :: CPython",
    ],
    scripts=['bin/tsumaki'],
    packages=find_packages(),
    include_package_data=True,
    ext_modules=cythonize([])
)

