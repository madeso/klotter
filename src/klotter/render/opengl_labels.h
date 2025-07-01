#pragma once

#include "klotter/feature_flags.h"

#if FF_HAS(ENABLE_GL_DEBUG)
	#define DEBUG_LABEL_ARG_MANY const std::string& debug_label,
	#define DEBUG_LABEL_ARG_SINGLE const std::string &debug_label
	#define DEBUG_LABEL_EXPLICIT_MANY
	#define SEND_DEBUG_LABEL_MANY(X) X,

	#define USE_DEBUG_LABEL(name) name
	#define USE_DEBUG_LABEL_MANY(name) name,
#else
	#define DEBUG_LABEL_ARG_MANY
	#define DEBUG_LABEL_ARG_SINGLE
	#define DEBUG_LABEL_EXPLICIT_MANY explicit
	#define SEND_DEBUG_LABEL_MANY(X)

	#define USE_DEBUG_LABEL(name)
	#define USE_DEBUG_LABEL_MANY(name)
#endif
