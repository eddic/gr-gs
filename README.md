# Guided Scrambling GNU Radio Module #

**Author:** Eddie Carle

**Version:** 1.0alpha

## About ##

The aim of this project is a fully functional GNU Radio implementation of the
Guided Scrambling channel coding technique first described by Dr. Ivan Fair at
The University of Alberta in [1]. Extension of the technique into the realm of
RF communications is described in [2]. The performance level of this module
opens the door to both large scale simulations and deployment in software
defined radio systems. It provides easy to use mechanisms for experimenting with
different selection methods, scrambling polynomials and field sizes.

 1. Ivan J. Fair, Wayne D. Grover, Witold A. Krymien, and R. Ian
    MacDonald, “Guided scrambling: A new line coding technique for
    high bit rate fiber optic transmission systems,” *IEEE
    Transaction on Communications*, vol. 39, no. 2, pp. 289-297,
    February 1991.
 2. I. Fair and D. Martin, “Generation of balanced QPSK sequence
    through guided scrambling,” *IET Communications*, vol. 9, no. 11,
    pp. 1404-1411, July 2015.

## Building and Installing ##

This should provide you with all the basic stuff you need to do to get the
module built and installed. The build system is CMake and the following
instructions assume you are in Bash.

First we need to clone.

    git clone https://github.com/eddic/gr-gs.git gr-gs

Then we make a build directory.

    mkdir gr-gs.build
    cd gr-gs.build

Now we need run cmake.

    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=RELEASE ../gr-gs

Note that that was to do a release build. That means heavily optimized and not
good for debugging. If you want to do some debugging to the module, do a debug
build.

    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=DEBUG ../gr-gs

Now let's build the library itself.

    make

Then we can build the documentation if we so desire.

    make doc

Now let's install it all (doc included if it was built).

    make install

Maybe we should build the unit tests?

    make tests

And of course we should run them as well.

    make test

## Demos and Simulators ##

If you'd like to see Guided Scrambling in action, check out the GNU Radio
Companion flowgraph, gs_demo.grc, included in the project.
