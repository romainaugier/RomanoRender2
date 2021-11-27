// Copyright Contributors to the Open Shading Language project.
// SPDX-License-Identifier: BSD-3-Clause
// https://github.com/AcademySoftwareFoundation/OpenShadingLanguage

#pragma once

#include <OSL/platform.h>
#include <OSL/export.h>
#include <OSL/oslversion.h>

#include "common/math/vec3.h"
#include "common/math/col3.h"


// All the things we need from Imath.
#define OSL_USING_IMATH 2
#if OSL_USING_IMATH >= 3
#   include <Imath/ImathVec.h>
#   include <Imath/ImathMatrix.h>
#   include <Imath/ImathColor.h>
#else
    // OpenEXR 2.x lacks the Cuda decorators we need, so we replicated some
    // Imath files in OSL/Imathx, adding the decorations needed for them to
    // be safe to compile with Cuda (and sometimes other improvements).
#   include <OSL/Imathx/ImathLimits.h>
#   include <OSL/Imathx/ImathVec.h>
#   include <OSL/Imathx/ImathMatrix.h>
#   include <OSL/Imathx/ImathColor.h>
#   include <OSL/matrix22.h>
#endif

// The fmt library causes trouble for Cuda. Work around by disabling it from
// oiio headers (OIIO <= 2.1) or telling fmt not to use the troublesome
// int128 that Cuda doesn't understand (OIIO >= 2.2).
#ifdef __CUDA_ARCH__
#    define OIIO_USE_FMT 0
#    ifndef FMT_USE_INT128
#        define FMT_USE_INT128 0
#    endif
#endif

// All the things we need from OpenImageIO
#include <OpenImageIO/oiioversion.h>
#include <OpenImageIO/errorhandler.h>
#include <OpenImageIO/texture.h>
#include <OpenImageIO/typedesc.h>
#include <OpenImageIO/ustring.h>
#include <OpenImageIO/platform.h>
#include <OpenImageIO/span.h>


OSL_NAMESPACE_ENTER

#define OSL_USE_BATCHED False

/// Various compile-time defaults are defined here that could, in
/// principle, be redefined if you are using OSL in some particular
/// renderer that wanted things a different way.

/// By default, we operate with single precision float.  Change this
/// definition to make a shading system that fundamentally operates
/// on doubles.
/// FIXME: it's very likely that all sorts of other things will break
/// if you do this, but eventually we should make sure it works.
typedef float Float;

/// By default, use the excellent Imath vector, matrix, and color types
/// from the IlmBase package from: http://www.openexr.com
///
/// It's permissible to override these types with the vector, matrix,
/// and color classes of your choice, provided that (a) your vectors
/// have the same data layout as a simple Float[n]; (b) your
/// matrices have the same data layout as Float[n][n]; and (c) your
/// classes have most of the obvious constructors and overloaded
/// operators one would expect from a C++ vector/matrix/color class.
// using Vec2     = Imath::Vec2<Float>;
// using Vec3     = Imath::Vec3<Float>;
// using Color3   = Imath::Color3<Float>;

using Vec2        = embree::Vec2f;
using Vec3        = embree::Vec3f;
using Color3      = embree::Vec3f;

using Matrix33 = Imath::Matrix33<Float>;
using Matrix44 = Imath::Matrix44<Float>;

#if OSL_USING_IMATH >= 3
using Matrix22 = Imath::Matrix22<Float>;
#else
using Matrix22 = Imathx::Matrix22<Float>;
#endif


/// Assume that we are dealing with OpenImageIO's texture system.  It
/// doesn't literally have to be OIIO's... it just needs to have the
/// same API as OIIO's TextureSystem class, it's a purely abstract class
/// anyway.
using OIIO::TextureSystem;
using OIIO::TextureOpt;

// And some other things we borrow from OIIO...
using OIIO::ErrorHandler;
using OIIO::ustring;
using OIIO::ustringHash;
using OIIO::string_view;
using OIIO::span;
using OIIO::cspan;

using OIIO::TypeDesc;
using OIIO::TypeUnknown;
using OIIO::TypeFloat;
using OIIO::TypeColor;
using OIIO::TypePoint;
using OIIO::TypeVector;
using OIIO::TypeNormal;
using OIIO::TypeMatrix;
using OIIO::TypeFloat4;
using OIIO::TypeString;
using OIIO::TypeInt;
#if OIIO_VERSION >= 20203
using OIIO::TypeFloat2;
using OIIO::TypeVector2;
using OIIO::TypeVector4;
#endif


// N.B. SymArena is not really "configuration", but we cram it here for
// lack of a better home.

/// The different memory arenas where symbols may live on the app side
enum class SymArena {
    Unknown = 0,        // Unknown/uninitialized value
    Absolute,           // Absolute address
    Heap,               // Belongs to context heap
    Outputs,            // Belongs to output arena
    UserData,           // UserData arena
    // ShaderGlobals,   // RESERVED
};



/////////////////////////////////////////////////////////////////////////
// Define various macros and templates that need to be different for
// various C++ versions or compilers, or to make up for things not
// yet available in earlier C++ standards.

#if OSL_CPLUSPLUS_VERSION >= 17
    // fold expression to expand
    #define __OSL_EXPAND_PARAMETER_PACKS(EXPRESSION) \
        (void((EXPRESSION)) , ... );
#else
    // Choose to use initializer list to expand our parameter pack so that
    // the order they are evaluated in the order they appear
    #define __OSL_EXPAND_PARAMETER_PACKS(EXPRESSION) \
        { \
            using expander = int [sizeof...(IntListT)]; \
            (void)expander{((EXPRESSION),0)...}; \
        }
#endif


namespace pvt {

template<int... IntegerListT>
using int_sequence = std::integer_sequence<int, IntegerListT...>;

template<int EndBeforeT>
using make_int_sequence =  std::make_integer_sequence<int, EndBeforeT>;

 template<bool... BoolListT >
using bool_sequence = std::integer_sequence<bool, BoolListT... >;

#if (OSL_CPLUSPLUS_VERSION >= 17)
    template<class... ListT>
    using conjunction = std::conjunction<ListT...>;
#else
    template<class... ListT>
    using conjunction = std::is_same<bool_sequence<true,ListT::value...>, bool_sequence<ListT::value..., true>>;
#endif

    // We need the SFINAE type to be different for
    // enable_if_type from disable_if_type so that we can apply both to
    // the same template signature to avoid
    // "error: invalid redeclaration of member function template"
    // NOTE: std::enable_if_t is a c++14 library feature, our baseline
    // and we wish to remain compatible with c++11 header libraries.
    // Also we are using std::true_type vs. void as the default type
    template <bool TestT, typename TypeT = std::true_type>
    using enable_if_type = typename std::enable_if<TestT, TypeT>::type;

} // namespace pvt

// Instead of relying on compiler loop unrolling, we can statically call functor
// for each integer in a sequence
template <template<int> class ConstantWrapperT, int... IntListT, typename FunctorT>
static OSL_FORCEINLINE OSL_HOSTDEVICE void static_foreach(pvt::int_sequence<IntListT...>, const FunctorT &iFunctor) {
     __OSL_EXPAND_PARAMETER_PACKS( iFunctor(ConstantWrapperT<IntListT>{}) );
}

template <template<int> class ConstantWrapperT, int N, typename FunctorT>
static OSL_FORCEINLINE OSL_HOSTDEVICE void static_foreach(const FunctorT &iFunctor) {
    static_foreach<ConstantWrapperT>(pvt::make_int_sequence<N>(), iFunctor);
}

template<int N>
using ConstIndex = std::integral_constant<int, N>;


// During development it can be useful to output extra information
// NOTE:  internal use only
#ifdef OSL_DEV
    #define OSL_DEV_ONLY(...) __VA_ARGS__
#else
    #define OSL_DEV_ONLY(...)
#endif


OSL_NAMESPACE_EXIT
