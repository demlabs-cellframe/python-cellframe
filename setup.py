import sys

from skbuild import setup

# Require pytest-runner only when running tests
#pytest_runner = (['pytest-runner>=2.0,<3dev']
#                 if any(arg in sys.argv for arg in ('pytest', 'test'))
#                 else [])

#setup_requires = pytest_runner

setup(
    name="CellFrame",
    version="0.9-0",
    description="CellFrame SDK",
    author='DEMLABS Inc. (2017-2020)',
    license="GNU GPLv3",
    packages=['CellFrame'],
#    tests_require=['pytest'],
#    setup_requires=setup_requires
)

