#pragma once

#define DEBUG_LABEL_ARG_MANY const std::string& debug_label,
#define DEBUG_LABEL_ARG_SINGLE const std::string &debug_label
#define DEBUG_LABEL_EXPLICIT_NO_EXTRA explicit
#define SEND_DEBUG_LABEL(X) X
#define SEND_DEBUG_LABEL_MANY(X) X,

#define USE_DEBUG_LABEL(name) name
#define USE_DEBUG_LABEL_MANY(name) name,

// debug labels
#define USE_TEXTURE_LABEL(name) USE_DEBUG_LABEL(name)
#define USE_TEXTURE_LABEL_MANY(name) USE_DEBUG_LABEL_MANY(name)
#define USE_PROGRAM_LABEL_MANY(name) USE_DEBUG_LABEL_MANY(name)

