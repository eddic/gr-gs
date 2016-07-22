#define GS_API

%include "gnuradio.i"
%module "gr_gs"

%{
#include "gr-gs/config.h"
#include "gr-gs/GuidedScrambler.h"
#include "gr-gs/Descrambler.h"
#include "gr-gs/PulseGenerator.h"
#include "gr-gs/SymbolGenerator.h"
#include "gr-gs/exceptions.h"
typedef gr::gs::Symbol Symbol;
%}

typedef gr::gs::Symbol Symbol;
%include "gr-gs/GuidedScrambler.h"
GR_SWIG_BLOCK_MAGIC2(gs, GuidedScrambler);
%include "gr-gs/Descrambler.h"
GR_SWIG_BLOCK_MAGIC2(gs, Descrambler);
%include "gr-gs/PulseGenerator.h"
GR_SWIG_BLOCK_MAGIC2(gs, PulseGenerator);
%include "gr-gs/SymbolGenerator.h"
GR_SWIG_BLOCK_MAGIC2(gs, SymbolGenerator);
