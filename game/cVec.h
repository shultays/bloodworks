#pragma once


#define G_TYPE_INT 0
#define G_TYPE_FLOAT 1
#define G_TYPE_LONG 2
#define G_TYPE_DOUBLE 3

// float
#define G_VEC_TYPE_ID G_TYPE_FLOAT

#define GVEC_N 2
#define G_VEC_IMP_NAME Vec2
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME

#define GVEC_N 3
#define G_VEC_IMP_NAME Vec3
#define G_VEC_PREV_1_CLASS Vec2
#define G_VEC_PREV_1_NAME vec2
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME
#undef G_VEC_PREV_1_CLASS
#undef G_VEC_PREV_1_NAME

#define GVEC_N 4
#define G_VEC_IMP_NAME Vec4
#define G_VEC_PREV_1_CLASS Vec3
#define G_VEC_PREV_1_NAME vec3
#define G_VEC_PREV_2_CLASS Vec2
#define G_VEC_PREV_2_NAME vec2
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME
#undef G_VEC_PREV_1_CLASS
#undef G_VEC_PREV_1_NAME
#undef G_VEC_PREV_2_CLASS
#undef G_VEC_PREV_2_NAME


#undef G_VEC_TYPE_ID

// double
#define G_VEC_TYPE_ID G_TYPE_DOUBLE

#define GVEC_N 2
#define G_VEC_IMP_NAME DoubleVec2
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME

#define GVEC_N 3
#define G_VEC_IMP_NAME DoubleVec3
#define G_VEC_PREV_1_CLASS DoubleVec2
#define G_VEC_PREV_1_NAME doubleVec2
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME
#undef G_VEC_PREV_1_CLASS
#undef G_VEC_PREV_1_NAME

#define GVEC_N 4
#define G_VEC_IMP_NAME DoubleVec4
#define G_VEC_PREV_1_CLASS DoubleVec3
#define G_VEC_PREV_1_NAME doubleVec3
#define G_VEC_PREV_2_CLASS DoubleVec2
#define G_VEC_PREV_2_NAME doubleVec2
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME
#undef G_VEC_PREV_1_CLASS
#undef G_VEC_PREV_1_NAME
#undef G_VEC_PREV_2_CLASS
#undef G_VEC_PREV_2_NAME


#undef G_VEC_TYPE_ID


// int
#define G_VEC_TYPE_ID G_TYPE_INT

#define GVEC_N 2
#define G_VEC_IMP_NAME IntVec2
#define G_VEC_FLOATING_IMP_NAME Vec2
#define G_VEC_FLOATING_TYPE float
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME
#undef G_VEC_FLOATING_IMP_NAME
#undef G_VEC_FLOATING_TYPE

#define GVEC_N 3
#define G_VEC_IMP_NAME IntVec3
#define G_VEC_PREV_1_CLASS IntVec2
#define G_VEC_PREV_1_NAME intVec2
#define G_VEC_FLOATING_IMP_NAME Vec3
#define G_VEC_FLOATING_TYPE float
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME
#undef G_VEC_PREV_1_CLASS
#undef G_VEC_PREV_1_NAME
#undef G_VEC_FLOATING_IMP_NAME
#undef G_VEC_FLOATING_TYPE

#define GVEC_N 4
#define G_VEC_IMP_NAME IntVec4
#define G_VEC_PREV_1_CLASS IntVec3
#define G_VEC_PREV_1_NAME intVec3
#define G_VEC_PREV_2_CLASS IntVec2
#define G_VEC_PREV_2_NAME intVec2
#define G_VEC_FLOATING_IMP_NAME Vec4
#define G_VEC_FLOATING_TYPE float
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME
#undef G_VEC_PREV_1_CLASS
#undef G_VEC_PREV_1_NAME
#undef G_VEC_PREV_2_CLASS
#undef G_VEC_PREV_2_NAME
#undef G_VEC_FLOATING_IMP_NAME
#undef G_VEC_FLOATING_TYPE


#undef G_VEC_TYPE_ID

// long long
#define G_VEC_TYPE_ID G_TYPE_LONG

#define GVEC_N 2
#define G_VEC_IMP_NAME LongVec2
#define G_VEC_FLOATING_IMP_NAME DoubleVec2
#define G_VEC_FLOATING_TYPE double
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME
#undef G_VEC_FLOATING_IMP_NAME
#undef G_VEC_FLOATING_TYPE

#define GVEC_N 3
#define G_VEC_IMP_NAME LongVec3
#define G_VEC_PREV_1_CLASS LongVec2
#define G_VEC_PREV_1_NAME longVec2
#define G_VEC_FLOATING_IMP_NAME DoubleVec3
#define G_VEC_FLOATING_TYPE double
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME
#undef G_VEC_PREV_1_CLASS
#undef G_VEC_PREV_1_NAME
#undef G_VEC_FLOATING_IMP_NAME
#undef G_VEC_FLOATING_TYPE

#define GVEC_N 4
#define G_VEC_IMP_NAME LongVec4
#define G_VEC_PREV_1_CLASS LongVec3
#define G_VEC_PREV_1_NAME longVec3
#define G_VEC_PREV_2_CLASS LongVec2
#define G_VEC_PREV_2_NAME longVec2
#define G_VEC_FLOATING_IMP_NAME DoubleVec4
#define G_VEC_FLOATING_TYPE double
#include "cVecImp.h"
#undef GVEC_N
#undef G_VEC_IMP_NAME
#undef G_VEC_PREV_1_CLASS
#undef G_VEC_PREV_1_NAME
#undef G_VEC_PREV_2_CLASS
#undef G_VEC_PREV_2_NAME
#undef G_VEC_FLOATING_IMP_NAME
#undef G_VEC_FLOATING_TYPE


#undef G_VEC_TYPE_ID