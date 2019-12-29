import sys

from skbuild import setup

# Require pytest-runner only when running tests
#pytest_runner = (['pytest-runner>=2.0,<3dev']
#                 if any(arg in sys.argv for arg in ('pytest', 'test'))
#                 else [])

#setup_requires = pytest_runner

setup(
    name="CellFrame",
    version="0.5-1",
    description="SDK CellFrame network",
    author='DEMLABS Inc. (2017-2019)',
    license="GNU GPL",
    packages=['CellFrame'],
#    tests_require=['pytest'],
#    setup_requires=setup_requires
)

