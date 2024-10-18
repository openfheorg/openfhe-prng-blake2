External PRNG Engine (Blake2) for OpenFHE
=====================================

This example demonstrates how to develop external PRNG engines as shared objects. It is an experimental feature currently available only on Linux, and g++ is the required compiler for linking the shared object.

Below are the steps to follow to create a new PRNG:
1. Create a repo (or directory) for your custom engine.
2. Create a new class similar to [Blake2Engine](https://github.com/openfheorg/openfhe-prng-blake2/blob/main/src/include/blake2engine.h):
    * copy [prng.h](https://github.com/openfheorg/openfhe-development/blob/main/src/core/include/utils/prng/prng.h) from OpenFHE to your repo. The class PRNG defined in prng.h must be used as the base class for the new class. prng.h is not allowed to be changed.
    * only two public member functions are needed for the class: a constructor with 2 input parameters (seed array and counter) and operator()
    * create extern "C" function "createEngineInstance" returning a dynamically allocated object of the new class. As the function is called from OpenFHE using dlsym() you may not change its current name.
3. Link your new engine library as a shared object. See [CMakeLists.txt](https://github.com/openfheorg/openfhe-prng-blake2/blob/main/CMakeLists.txt) as an example if you use cmake.
4. Add a call to PseudoRandomNumberGenerator::InitPRNGEngine() at the beginning of your application‘s main(). This function takes the absolute path to your shared object. If you call InitPRNGEngine() without argument or do not call it at all, then your application will use the built-in PRNG engine (blake2).

The indication of using an external PRNG engine is a trace from the application "InitPRNGEngine: using external PRNG". There no trace for the built-in PRNG. 

To quickly test if OpenFHE uses your new shared object or/and see how to call InitPRNGEngine() see [external-prng.cpp](https://github.com/openfheorg/openfhe-development/blob/main/src/core/examples/external-prng.cpp). The example prints its usage if run with the option "-h".