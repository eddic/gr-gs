#define GS_API

%include "gnuradio.i"
%module "gr_gs"

%{
#include "gr-gs/config.h"
#include "gr-gs/GuidedScrambler.h"
#include "gr-gs/Descrambler.h"
#include "gr-gs/SymbolMapper.h"
#include "gr-gs/SymbolGenerator.h"
#include "gr-gs/Average.h"
#include "gr-gs/Terminator.h"
#include "gr-gs/Distribution.h"
#include "gr-gs/Integrate.h"
#include "gr-gs/Autocovariance.h"
#include "gr-gs/ErrorCount.h"
#include "gr-gs/Primitives.h"
#include "gr-gs/exceptions.h"
%}

%include "gr-gs/config.h"
%include "gr-gs/Primitives.h"

%include "gr-gs/GuidedScrambler.h"
%template(GuidedScrambler_bb) gr::gs::GuidedScrambler<unsigned char>;
%template(GuidedScrambler_ss) gr::gs::GuidedScrambler<unsigned short>;
%template(GuidedScrambler_ii) gr::gs::GuidedScrambler<unsigned int>;
GR_SWIG_BLOCK_MAGIC2(gs, GuidedScrambler_bb);
GR_SWIG_BLOCK_MAGIC2(gs, GuidedScrambler_ss);
GR_SWIG_BLOCK_MAGIC2(gs, GuidedScrambler_ii);

%include "gr-gs/Descrambler.h"
%template(Descrambler_bb) gr::gs::Descrambler<unsigned char>;
%template(Descrambler_ss) gr::gs::Descrambler<unsigned short>;
%template(Descrambler_ii) gr::gs::Descrambler<unsigned int>;
GR_SWIG_BLOCK_MAGIC2(gs, Descrambler_bb);
GR_SWIG_BLOCK_MAGIC2(gs, Descrambler_ss);
GR_SWIG_BLOCK_MAGIC2(gs, Descrambler_ii);

%include "gr-gs/SymbolMapper.h"
%template(SymbolMapper_bc) gr::gs::SymbolMapper<unsigned char>;
%template(SymbolMapper_sc) gr::gs::SymbolMapper<unsigned short>;
%template(SymbolMapper_ic) gr::gs::SymbolMapper<unsigned int>;
GR_SWIG_BLOCK_MAGIC2(gs, SymbolMapper_bc);
GR_SWIG_BLOCK_MAGIC2(gs, SymbolMapper_sc);
GR_SWIG_BLOCK_MAGIC2(gs, SymbolMapper_ic);

%include "gr-gs/SymbolGenerator.h"
%template(SymbolGenerator_b) gr::gs::SymbolGenerator<unsigned char>;
%template(SymbolGenerator_s) gr::gs::SymbolGenerator<unsigned short>;
%template(SymbolGenerator_i) gr::gs::SymbolGenerator<unsigned int>;
GR_SWIG_BLOCK_MAGIC2(gs, SymbolGenerator_b);
GR_SWIG_BLOCK_MAGIC2(gs, SymbolGenerator_s);
GR_SWIG_BLOCK_MAGIC2(gs, SymbolGenerator_i);

%include "gr-gs/ErrorCount.h"
%template(ErrorCount_bf) gr::gs::ErrorCount<unsigned char>;
%template(ErrorCount_sf) gr::gs::ErrorCount<unsigned short>;
%template(ErrorCount_if) gr::gs::ErrorCount<unsigned int>;
GR_SWIG_BLOCK_MAGIC2(gs, ErrorCount_bf);
GR_SWIG_BLOCK_MAGIC2(gs, ErrorCount_sf);
GR_SWIG_BLOCK_MAGIC2(gs, ErrorCount_if);

%include "gr-gs/Autocovariance.h"
%template(Autocovariance_ff) gr::gs::Autocovariance<float>;
%template(Autocovariance_cc) gr::gs::Autocovariance<std::complex<float>>;
GR_SWIG_BLOCK_MAGIC2(gs, Autocovariance_ff);
GR_SWIG_BLOCK_MAGIC2(gs, Autocovariance_cc);

%include "gr-gs/Average.h"
%template(Average_ff) gr::gs::Average<double, float>;
%template(Average_cc) gr::gs::Average<std::complex<double>, std::complex<float>>;
GR_SWIG_BLOCK_MAGIC2(gs, Average_ff);
GR_SWIG_BLOCK_MAGIC2(gs, Average_cc);

%include "gr-gs/Integrate.h"
%template(Integrate_ff) gr::gs::Integrate<double, float>;
%template(Integrate_cc) gr::gs::Integrate<std::complex<double>, std::complex<float>>;
GR_SWIG_BLOCK_MAGIC2(gs, Integrate_ff);
GR_SWIG_BLOCK_MAGIC2(gs, Integrate_cc);

%include "gr-gs/Terminator.h"
GR_SWIG_BLOCK_MAGIC2(gs, Terminator);
%include "gr-gs/Distribution.h"
GR_SWIG_BLOCK_MAGIC2(gs, Distribution);
