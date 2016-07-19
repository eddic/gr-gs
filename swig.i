/* -*- c++ -*- */

#define GS_API

%include "gnuradio.i"

%include "swig_doc.i"

%{
#include "gr-gs/GuidedScrambler.h"
#include "gr-gs/Descrambler.h"
#include "gr-gs/PulseGenerator.h"
#include "gr-gs/SymbolGenerator.h"
#include "gr-gs/exceptions.h"
%}


%include "gr-gs/GuidedScrambler.h"
GR_SWIG_BLOCK_MAGIC2(gr_gs, GuidedScrambler);
%include "gr-gs/Descrambler.h"
GR_SWIG_BLOCK_MAGIC2(gr_gs, Descrambler);
%include "gr-gs/PulseGenerator.h"
GR_SWIG_BLOCK_MAGIC2(gr_gs, PulseGenerator);
%include "gr-gs/SymbolGenerator.h"
GR_SWIG_BLOCK_MAGIC2(gr_gs, SymbolGenerator);
