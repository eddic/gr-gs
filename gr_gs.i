#define GS_API

%include "gnuradio.i"
%module "gr_gs"

%{
#include "gr-gs/config.h"
#include "gr-gs/GuidedScrambler.h"
#include "gr-gs/Descrambler.h"
#include "gr-gs/SymbolMapper.h"
#include "gr-gs/SymbolGenerator.h"
#include "gr-gs/InfiniteAverage.h"
#include "gr-gs/InfiniteDistribution.h"
#include "gr-gs/InfiniteIntegrate.h"
#include "gr-gs/ErrorCount.h"
#include "gr-gs/Primitives.h"
#include "gr-gs/exceptions.h"
%}

%include "gr-gs/config.h"
%include "gr-gs/Primitives.h"

%include "gr-gs/GuidedScrambler.h"
GR_SWIG_BLOCK_MAGIC2(gs, GuidedScrambler);
%include "gr-gs/Descrambler.h"
GR_SWIG_BLOCK_MAGIC2(gs, Descrambler);
%include "gr-gs/SymbolMapper.h"
GR_SWIG_BLOCK_MAGIC2(gs, SymbolMapper);
%include "gr-gs/SymbolGenerator.h"
GR_SWIG_BLOCK_MAGIC2(gs, SymbolGenerator);
%include "gr-gs/InfiniteAverage.h"
GR_SWIG_BLOCK_MAGIC2(gs, InfiniteAverage);
%include "gr-gs/InfiniteDistribution.h"
GR_SWIG_BLOCK_MAGIC2(gs, InfiniteDistribution);
%include "gr-gs/InfiniteIntegrate.h"
GR_SWIG_BLOCK_MAGIC2(gs, InfiniteIntegrate);
%include "gr-gs/ErrorCount.h"
GR_SWIG_BLOCK_MAGIC2(gs, ErrorCount);
