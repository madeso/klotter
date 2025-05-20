#pragma once

// conditional compilation
// https://x.com/voxagonlabs/status/1821444613430763778?t=TVzxLcPgauVE1CaDprFfPA&s=19

// todo(Gustav): should we prefix theese with something better?

#define IS_ENABLED +
#define IS_DISABLED -
#define HAS(x) (1 x 1)

// #define FEATURE_A IS_ENABLED
//
// #if HAS(FEATURE_A)
//    ...
// #else
//    ...
// #endif
