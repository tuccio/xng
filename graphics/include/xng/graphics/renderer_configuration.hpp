#pragma once

#include <set>
#include <cstdint>
#include <mutex>

#include <xng/math.hpp>

#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/seq.hpp>

#include <xng/os.hpp>

#define XNG_MAKE_SEQUENCE_OF_TUPLES_PAR1(A, B, C, D) ((A, B, C, D)) XNG_MAKE_SEQUENCE_OF_TUPLES_PAR2
#define XNG_MAKE_SEQUENCE_OF_TUPLES_PAR2(A, B, C, D) ((A, B, C, D)) XNG_MAKE_SEQUENCE_OF_TUPLES_PAR1
#define XNG_MAKE_SEQUENCE_OF_TUPLES_PAR1_END
#define XNG_MAKE_SEQUENCE_OF_TUPLES_PAR2_END
#define XNG_MAKE_SEQUENCE_OF_TUPLES(Input) BOOST_PP_CAT(XNG_MAKE_SEQUENCE_OF_TUPLES_PAR1 Input, _END) 

#define XNG_GRAPHICS_RENDER_VARIABLE_DECLARE(Type, Name, Enum, Default) Type Name;
#define XNG_GRAPHICS_RENDER_VARIABLE_GET_SET(Type, Name, Enum, Default) Type BOOST_PP_CAT(get_, Name) (void) const { return BOOST_PP_CAT(m_vars., Name); } void BOOST_PP_CAT(set_, Name) (const Type & x) { BOOST_PP_CAT(m_vars., Name) = x; m_updates.insert(Enum); }
#define XNG_GRAPHICS_RENDER_VARIABLE_SET_DEFAULT(Type, Name, Enum, Default) BOOST_PP_CAT(rv., Name) = Default;
#define XNG_GRAPHICS_RENDER_VARIABLE_ENUM(Type, Name, Enum, Default) BOOST_PP_CAT(Enum, BOOST_PP_COMMA())

#define XNG_GRAPHICS_RENDER_VARIABLES_DECLARE_MACRO(R, Data, Element) XNG_GRAPHICS_RENDER_VARIABLE_DECLARE(BOOST_PP_TUPLE_ELEM(4, 0, Element), BOOST_PP_TUPLE_ELEM(4, 1, Element), BOOST_PP_TUPLE_ELEM(4, 2, Element), BOOST_PP_TUPLE_ELEM(4, 3, Element))
#define XNG_GRAPHICS_RENDER_VARIABLES_GET_SET_MACRO(R, Data, Element) XNG_GRAPHICS_RENDER_VARIABLE_GET_SET(BOOST_PP_TUPLE_ELEM(4, 0, Element), BOOST_PP_TUPLE_ELEM(4, 1, Element), BOOST_PP_TUPLE_ELEM(4, 2, Element), BOOST_PP_TUPLE_ELEM(4, 3, Element))
#define XNG_GRAPHICS_RENDER_VARIABLES_SET_DEFAULT_MACRO(R, Data, Element) XNG_GRAPHICS_RENDER_VARIABLE_SET_DEFAULT(BOOST_PP_TUPLE_ELEM(4, 0, Element), BOOST_PP_TUPLE_ELEM(4, 1, Element), BOOST_PP_TUPLE_ELEM(4, 2, Element), BOOST_PP_TUPLE_ELEM(4, 3, Element))
#define XNG_GRAPHICS_RENDER_VARIABLES_ENUM_MACRO(R, Data, Element) XNG_GRAPHICS_RENDER_VARIABLE_ENUM(BOOST_PP_TUPLE_ELEM(4, 0, Element), BOOST_PP_TUPLE_ELEM(4, 1, Element), BOOST_PP_TUPLE_ELEM(4, 2, Element), BOOST_PP_TUPLE_ELEM(4, 3, Element))

#define XNG_GRAPHICS_RENDER_VARIABLES(Variables)\
    enum xng_render_variable { BOOST_PP_SEQ_FOR_EACH(XNG_GRAPHICS_RENDER_VARIABLES_ENUM_MACRO, 0, XNG_MAKE_SEQUENCE_OF_TUPLES(Variables)) XNG_RV_COUNT };\
    namespace xng { namespace graphics {\
    typedef std::set<xng_render_variable> render_variables_updates;\
    struct render_variables { BOOST_PP_SEQ_FOR_EACH(XNG_GRAPHICS_RENDER_VARIABLES_DECLARE_MACRO, 0, XNG_MAKE_SEQUENCE_OF_TUPLES(Variables)) };\
    class renderer_configuration\
    {\
        public:\
            renderer_configuration(void);\
            BOOST_PP_SEQ_FOR_EACH(XNG_GRAPHICS_RENDER_VARIABLES_GET_SET_MACRO, 0, XNG_MAKE_SEQUENCE_OF_TUPLES(Variables))\
            void get_render_variables(render_variables * vars, std::set<xng_render_variable> * updates);\
        private:\
            render_variables m_vars;\
            std::set<xng_render_variable> m_updates;\
    };\
    inline render_variables make_default_render_variables(void) { render_variables rv; BOOST_PP_SEQ_FOR_EACH(XNG_GRAPHICS_RENDER_VARIABLES_SET_DEFAULT_MACRO, 0, XNG_MAKE_SEQUENCE_OF_TUPLES(Variables)) return rv; }\
    }} // Close namespace xng::graphics

// Types

enum xng_culling_mode
{
    XNG_CULLING_MODE_NONE,
    XNG_CULLING_MODE_BACK,
    XNG_CULLING_MODE_FRONT
};

// Definition

XNG_GRAPHICS_RENDER_VARIABLES(
    (xng::math::uint2, render_resolution,       XNG_RV_RENDER_RESOLUTION,       xng::math::uint2(1280, 720))
    (int,              shadow_map_resolution,   XNG_RV_SHADOW_MAP_RESOLUTION,   1024)
    (float,            shadow_map_bias,         XNG_RV_SHADOW_MAP_BIAS,         0.f)
    (xng_culling_mode, culling_mode,            XNG_RV_CULLING_MODE,            XNG_CULLING_MODE_BACK)
    (xng_culling_mode, shadow_map_culling_mode, XNG_RV_SHADOW_MAP_CULLING_MODE, XNG_CULLING_MODE_FRONT)
    (bool,             forward_depth_prepass,   XNG_RV_FORWARD_DEPTH_PREPASS,   false)
    (bool,             deferred_depth_prepass,  XNG_RV_DEFERRED_DEPTH_PREPASS,  false)
    (bool,             debug_normals,           XNG_RV_DEBUG_NORMALS,           false)
    (bool,             debug_camera_frustum,    XNG_RV_DEBUG_CAMERA_FRUSTUM,    false)
)