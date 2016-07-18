/*
    Copyright (c) 2008-2013 NetAllied Systems GmbH

	This file is part of COLLADAStreamWriter.
	
    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/

#include "COLLADASWConstants.h"

namespace COLLADASW
{

	const String CSWC::EMPTY_STRING = "";
    const String CSWC::CSW_ERR_UNKNOWN_INPUT = "UNKNOWN INPUT ERROR";

    const String CSWC::XML_START_ELEMENT = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    const String CSWC::CSW_NAMESPACE_1_4_1 = "http://www.collada.org/2005/11/COLLADASchema";
    const String CSWC::CSW_VERSION_1_4_1 = "1.4.1";

	const String CSWC::CSW_NAMESPACE_1_5_0 = "http://www.collada.org/2008/03/COLLADASchema";
	const String CSWC::CSW_VERSION_1_5_0 = "1.5.0";
	const String CSWC::CSW_SCHEMALOCATION_1_5_0 = "http://www.collada.org/2008/03/COLLADASchema http://www.khronos.org/files/collada_schema_1_5";
	const String CSWC::CSW_XMLNS_XSI_1_5_0 = "http://www.w3.org/2001/XMLSchema-instance";

    const String CSWC::CSW_PLATFORM_PC_OGL               = "PC-OGL";

    const String CSWC::CSW_ELEMENT_ACCESSOR = "accessor";
    const String CSWC::CSW_ELEMENT_ALL = "all";
    const String CSWC::CSW_ELEMENT_AMBIENT = "ambient";
    const String CSWC::CSW_ELEMENT_ANIMATION = "animation";
    const String CSWC::CSW_ELEMENT_ANIMATION_CLIP = "animation_clip";
    const String CSWC::CSW_ELEMENT_ANNOTATE = "annotate";
    const String CSWC::CSW_ELEMENT_ASPECT_RATIO = "aspect_ratio";
    const String CSWC::CSW_ELEMENT_ASSET = "asset";
    const String CSWC::CSW_ELEMENT_AUTHOR = "author";
    const String CSWC::CSW_ELEMENT_AUTHORING_TOOL = "authoring_tool";
    const String CSWC::CSW_ELEMENT_BLINN = "blinn";
    const String CSWC::CSW_ELEMENT_BIND = "bind";
    const String CSWC::CSW_ELEMENT_BIND_MATERIAL = "bind_material";
	const String CSWC::CSW_ELEMENT_BIND_SHAPE_MATRIX = "bind_shape_matrix";
	const String CSWC::CSW_ELEMENT_BIND_VERTEX_INPUT = "bind_vertex_input";
    const String CSWC::CSW_ELEMENT_BORDER_COLOR = "border_color";
    const String CSWC::CSW_ELEMENT_CAMERA = "camera";
    const String CSWC::CSW_ELEMENT_CAMERA_ORTHOGRAPHIC = "orthographic";
    const String CSWC::CSW_ELEMENT_CAMERA_PERSPECTIVE = "perspective";
    const String CSWC::CSW_ELEMENT_CHANNEL = "channel";
    const String CSWC::CSW_ELEMENT_CHANNELS = "channels";
    const String CSWC::CSW_ELEMENT_CODE = "code";
    const String CSWC::CSW_ELEMENT_COMMENTS = "comments";
    const String CSWC::CSW_ELEMENT_COMPILER_TARGET = "compiler_target";
    const String CSWC::CSW_ELEMENT_COMPILER_OPTIONS = "compiler_options";
    const String CSWC::CSW_ELEMENT_CONSTANT = "constant";
	const String CSWC::CSW_ELEMENT_CONSTANT_ATTENUATION = "constant_attenuation";
    const String CSWC::CSW_ELEMENT_CONTRIBUTOR = "contributor";
	const String CSWC::CSW_ELEMENT_CONTROLLER = "controller";
	const String CSWC::CSW_ELEMENT_CONTROL_VERTICES = "control_vertices";
    const String CSWC::CSW_ELEMENT_COPYRIGHT = "copyright";
    const String CSWC::CSW_ELEMENT_CREATED = "created";
    const String CSWC::CSW_ELEMENT_COLLADA = "COLLADA";
    const String CSWC::CSW_ELEMENT_COLOR = "color";
    const String CSWC::CSW_ELEMENT_COLOR_CLEAR = "color_clear";
	const String CSWC::CSW_ELEMENT_COLOR_TARGET = "color_target";
	const String CSWC::CSW_ELEMENT_CREATE_2D = "create_2d";
    const String CSWC::CSW_ELEMENT_DATA = "data";
    const String CSWC::CSW_ELEMENT_DEPTH_CLEAR = "depth_clear";
    const String CSWC::CSW_ELEMENT_DEPTH_TARGET = "depth_target";
    const String CSWC::CSW_ELEMENT_DIFFUSE = "diffuse";
	const String CSWC::CSW_ELEMENT_DIRECTIONAL = "directional";
    const String CSWC::CSW_ELEMENT_DRAW = "draw";
    const String CSWC::CSW_ELEMENT_EFFECT = "effect";
    const String CSWC::CSW_ELEMENT_EMISSION = "emission";
    const String CSWC::CSW_ELEMENT_EXTRA = "extra";
    const String CSWC::CSW_ELEMENT_FACE = "face";
    const String CSWC::CSW_ELEMENT_FALLOFF_ANGLE = "falloff_angle";
    const String CSWC::CSW_ELEMENT_FALLOFF_EXPONENT = "falloff_exponent";
    const String CSWC::CSW_ELEMENT_FLOAT = "float";
    const String CSWC::CSW_ELEMENT_FLOAT_ARRAY = "float_array";
    const String CSWC::CSW_ELEMENT_FORMAT = "format";
    const String CSWC::CSW_ELEMENT_FORMAT_HINT = "format_hint";
    const String CSWC::CSW_ELEMENT_GEOMETRY = "geometry";
    const String CSWC::CSW_ELEMENT_H = "h";
    const String CSWC::CSW_ELEMENT_IDREF_ARRAY = "IDREF_array";
    const String CSWC::CSW_ELEMENT_IMAGE = "image";
    const String CSWC::CSW_ELEMENT_INCLUDE = "include";
    const String CSWC::CSW_ELEMENT_INDEX_OF_REFRACTION = "index_of_refraction";
    const String CSWC::CSW_ELEMENT_INIT_FROM = "init_from";
    const String CSWC::CSW_ELEMENT_INIT_AS_TARGET = "init_as_target";
    const String CSWC::CSW_ELEMENT_INIT_CUBE = "init_cube";
    const String CSWC::CSW_ELEMENT_INIT_VOLUME = "init_volume";
    const String CSWC::CSW_ELEMENT_INIT_PLANAR = "init_planar";
    const String CSWC::CSW_ELEMENT_INIT_AS_NULL = "init_as_null";
    const String CSWC::CSW_ELEMENT_INPUT = "input";
    const String CSWC::CSW_ELEMENT_INSTANCE_ANIMATION = "instance_animation";
    const String CSWC::CSW_ELEMENT_INSTANCE_CAMERA = "instance_camera";
    const String CSWC::CSW_ELEMENT_INSTANCE_CONTROLLER = "instance_controller";
    const String CSWC::CSW_ELEMENT_INSTANCE_EFFECT = "instance_effect";
	const String CSWC::CSW_ELEMENT_INSTANCE_GEOMETRY = "instance_geometry";
	const String CSWC::CSW_ELEMENT_INSTANCE_IMAGE = "instance_image";
    const String CSWC::CSW_ELEMENT_INSTANCE_LIGHT = "instance_light";
    const String CSWC::CSW_ELEMENT_INSTANCE_MATERIAL = "instance_material";
	const String CSWC::CSW_ELEMENT_INSTANCE_RIGID_BODY = "instance_rigid_body";
    const String CSWC::CSW_ELEMENT_INSTANCE_RIGID_CONSTRAINT = "instance_rigid_constraint";
	const String CSWC::CSW_ELEMENT_INSTANCE_PHYSICS_MODEL = "instance_physics_model";
    const String CSWC::CSW_ELEMENT_INSTANCE_NODE = "instance_node";
    const String CSWC::CSW_ELEMENT_INSTANCE_VISUAL_SCENE = "instance_visual_scene";
	const String CSWC::CSW_ELEMENT_INSTANCE_PHYSICS_SCENE = "instance_physics_scene";
    const String CSWC::CSW_ELEMENT_JOINTS = "joints";
    const String CSWC::CSW_ELEMENT_KEYWORDS = "keywords";
    const String CSWC::CSW_ELEMENT_LAMBERT = "lambert";
    const String CSWC::CSW_ELEMENT_LIBRARY_ANIMATIONS = "library_animations";
    const String CSWC::CSW_ELEMENT_LIBRARY_ANIMATION_CLIPS = "library_animation_clips";
    const String CSWC::CSW_ELEMENT_LIBRARY_CAMERAS = "library_cameras";
    const String CSWC::CSW_ELEMENT_LIBRARY_CONTROLLERS = "library_controllers";
    const String CSWC::CSW_ELEMENT_LIBRARY_EFFECTS = "library_effects";
    const String CSWC::CSW_ELEMENT_LIBRARY_GEOMETRIES = "library_geometries";
	const String CSWC::CSW_ELEMENT_LIBRARY_PHYSICS_MODELS = "library_physics_models";
    const String CSWC::CSW_ELEMENT_LIBRARY_IMAGES = "library_images";
    const String CSWC::CSW_ELEMENT_LIBRARY_LIGHTS = "library_lights";
    const String CSWC::CSW_ELEMENT_LIBRARY_MATERIALS = "library_materials";
    const String CSWC::CSW_ELEMENT_LIBRARY_VISUAL_SCENES = "library_visual_scenes";
	const String CSWC::CSW_ELEMENT_LIBRARY_PHYSICS_SCENES = "library_physics_scenes";
	const String CSWC::CSW_ELEMENT_LIBRARY_NODES = "library_nodes";
    const String CSWC::CSW_ELEMENT_LIGHT = "light";
	const String CSWC::CSW_ELEMENT_LINEAR_ATTENUATION = "linear_attenuation";
	const String CSWC::CSW_ELEMENT_LINES = "lines";
	const String CSWC::CSW_ELEMENT_LINE_STRIPS = "linestrips";
    const String CSWC::CSW_ELEMENT_LOOKAT = "lookat";
    const String CSWC::CSW_ELEMENT_MAGFILTER = "magfilter";
    const String CSWC::CSW_ELEMENT_MATERIAL = "material";
    const String CSWC::CSW_ELEMENT_MATRIX = "matrix";
    const String CSWC::CSW_ELEMENT_MESH = "mesh";
	const String CSWC::CSW_ELEMENT_CONVEX_MESH = "convex_mesh";
    const String CSWC::CSW_ELEMENT_PHYSICS_MATERIAL = "physics_material";
	const String CSWC::CSW_ELEMENT_PHYSICS_MODEL = "physics_model";
    const String CSWC::CSW_ELEMENT_RESTITUTION = "restitution";
    const String CSWC::CSW_ELEMENT_DYNAMIC_FRICTION = "dynamic_friction";
    const String CSWC::CSW_ELEMENT_STATIC_FRICTION = "static_friction";
	const String CSWC::CSW_ELEMENT_FRICTION_COMBINE_MODE = "friction_combine_mode";
	const String CSWC::CSW_ELEMENT_RESTITUTION_COMBINE_MODE = "restitution_combine_mode";
    const String CSWC::CSW_ELEMENT_HOLLOW = "hollow";
    const String CSWC::CSW_ELEMENT_DENSITY = "density";
	const String CSWC::CSW_ELEMENT_SIMULATION_FILTER_DATA = "simulation_filter_data";
	const String CSWC::CSW_ELEMENT_QUERY_FILTER_DATA = "query_filter_data";
	const String CSWC::CSW_ELEMENT_CONTACT_OFFSET = "contact_offset";
	const String CSWC::CSW_ELEMENT_REST_OFFSET = "rest_offset";
	const String CSWC::CSW_ELEMENT_SHAPE_FLAGS = "shape_flags";
	const String CSWC::CSW_ELEMENT_ACTOR_FLAGS = "actor_flags";
	const String CSWC::CSW_ELEMENT_DOMINANCE_GROUP = "dominance_group";
	const String CSWC::CSW_ELEMENT_OWNER_CLIENT = "owner_client";
	const String CSWC::CSW_ELEMENT_CMASS_LOCAL_POSE = "cmass_local_pose";
	const String CSWC::CSW_ELEMENT_RIGID_BODY_FLAGS = "rigid_body_flags";
	const String CSWC::CSW_ELEMENT_MIN_CCD_ADVANCE_COEFFICIENT = "min_ccd_advance_coefficient";
	const String CSWC::CSW_ELEMENT_MAX_DEPENETRATION_VELOCITY = "max_depenetration_velocity";
	const String CSWC::CSW_ELEMENT_LINEAR_DAMPING = "linear_damping";
	const String CSWC::CSW_ELEMENT_ANGULAR_DAMPING = "angular_damping";
	const String CSWC::CSW_ELEMENT_MAX_ANGULAR_VELOCITY = "max_angular_velocity";
	const String CSWC::CSW_ELEMENT_SLEEP_THRESHOLD = "sleep_threshold";
	const String CSWC::CSW_ELEMENT_STABILIZATION_THRESHOLD = "stabilization_threshold";
	const String CSWC::CSW_ELEMENT_WAKE_COUNTER = "wake_counter";
	const String CSWC::CSW_ELEMENT_MIN_POSITION_ITERS = "min_position_iters";
	const String CSWC::CSW_ELEMENT_MIN_VELOCITY_ITERS = "min_velocity_iters";
	const String CSWC::CSW_ELEMENT_CONTACT_REPORT_THRESHOLD = "contact_report_threshold";
	const String CSWC::CSW_ELEMENT_RIGID_BODY = "rigid_body";
	const String CSWC::CSW_ELEMENT_RIGID_BODY_DYNAMIC = "dynamic";
	const String CSWC::CSW_ELEMENT_RIGID_BODY_MASS = "mass";
    const String CSWC::CSW_ELEMENT_RIGID_BODY_MASS_FRAME = "mass_frame";
	const String CSWC::CSW_ELEMENT_RIGID_BODY_INERTIA = "inertia";
	const String CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE = "shape";
	const String CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_BOX = "box";
	const String CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_CAPSULE = "capsule";
    const String CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_PLANE = "plane";
    const String CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_PLANE_EQUATION = "equation";
    const String CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_SPHERE = "sphere";
	const String CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_CAPSULE_RADIUS = "radius";
	const String CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_CAPSULE_HEIGHT = "height";
	const String CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_BOX_EXTENTS = "half_extents";
    const String CSWC::CSW_ELEMENT_RIGID_CONSTRAINT = "rigid_constraint";
    const String CSWC::CSW_ELEMENT_REF_ATTACHMENT = "ref_attachment";
    const String CSWC::CSW_ELEMENT_ATTACHMENT = "attachment";
    const String CSWC::CSW_ELEMENT_ENABLED = "enabled";
    const String CSWC::CSW_ELEMENT_INTERPENETRATE = "interpenetrate";
    const String CSWC::CSW_ELEMENT_LIMITS = "limits";
    const String CSWC::CSW_ELEMENT_SWING_CONE_AND_TWIST = "swing_cone_and_twist";
    const String CSWC::CSW_ELEMENT_LINEAR = "linear";
    const String CSWC::CSW_ELEMENT_ANGULAR = "angular";
    const String CSWC::CSW_ELEMENT_MIN = "min";
    const String CSWC::CSW_ELEMENT_MAX = "max";
    const String CSWC::CSW_ELEMENT_ANGULAR_VELOCITY = "angular_velocity";
    const String CSWC::CSW_ELEMENT_VELOCITY = "velocity";
    const String CSWC::CSW_ELEMENT_SPRING = "spring";
    const String CSWC::CSW_ELEMENT_STIFFNESS = "stiffness";
    const String CSWC::CSW_ELEMENT_DAMPING = "damping";
    const String CSWC::CSW_ELEMENT_TARGET_VALUE = "target_value";
	const String CSWC::CSW_ELEMENT_BREAK_FORCE = "break_force";
	const String CSWC::CSW_ELEMENT_BREAK_TORQUE = "break_torque";
	const String CSWC::CSW_ELEMENT_CONSTRAINT_FLAGS = "constraint_flags";
	const String CSWC::CSW_ELEMENT_INV_MASS_SCALE_0 = "inv_mass_scale0";
	const String CSWC::CSW_ELEMENT_INV_INERTIA_SCALE_0 = "inv_interia_scale0";
	const String CSWC::CSW_ELEMENT_INV_MASS_SCALE_1 = "inv_mass_scale1";
	const String CSWC::CSW_ELEMENT_INV_INERTIA_SCALE_1 = "inv_inertia_scale1";
	const String CSWC::CSW_ELEMENT_PROJECTION_LINEAR_TOLERANCE = "projection_linear_tolerance";
	const String CSWC::CSW_ELEMENT_PROJECTION_ANGULAR_TOLERANCE = "projection_angular_tolerance";
	const String CSWC::CSW_ELEMENT_BOUNCE_THRESHOLD = "bounce_threshold";
	const String CSWC::CSW_ELEMENT_CONTACT_DISTANCE = "contact_distance";
	const String CSWC::CSW_ELEMENT_LIMITS_EXTRA = "limits_extra";
	const String CSWC::CSW_ELEMENT_LINEAR_EXTRA = "linear_extra";
	const String CSWC::CSW_ELEMENT_SWING_CONE_AND_TWIST_EXTRA = "swing_cone_and_twist_extra";
	const String CSWC::CSW_ELEMENT_TARGET_VALUE_EXTRA = "target_value_extra";
	const String CSWC::CSW_ELEMENT_ANGULAR_EXTRA = "angular_extra";
	const String CSWC::CSW_ELEMENT_SPRING_EXTRA = "spring_extra";
	const String CSWC::CSW_ELEMENT_DRIVE = "drive";
	const String CSWC::CSW_ELEMENT_LINEAR_X = "linear_x";
	const String CSWC::CSW_ELEMENT_LINEAR_Y = "linear_y";
	const String CSWC::CSW_ELEMENT_LINEAR_Z = "linear_z";
	const String CSWC::CSW_ELEMENT_SWING = "swing";
	const String CSWC::CSW_ELEMENT_TWIST = "twist";
	const String CSWC::CSW_ELEMENT_SLERP = "slerp";
	const String CSWC::CSW_ELEMENT_LINEAR_VELOCITY = "linear_velocity";
	const String CSWC::CSW_ELEMENT_FORCE_LIMIT = "force_limit";
	const String CSWC::CSW_ELEMENT_DRIVE_FLAGS = "drive_flags";
    const String CSWC::CSW_ELEMENT_MINFILTER = "minfilter";
    const String CSWC::CSW_ELEMENT_MIPFILTER = "mipfilter";
    const String CSWC::CSW_ELEMENT_MIP_LEVELS = "mip_levels";
    const String CSWC::CSW_ELEMENT_MIPMAP_BIAS = "mipmap_bias";
    const String CSWC::CSW_ELEMENT_MIPMAP_GENERATE = "mipmap_generate";
    const String CSWC::CSW_ELEMENT_MIPMAP_MAXLEVEL = "mipmap_maxlevel";
    const String CSWC::CSW_ELEMENT_MODIFIED = "modified";
    const String CSWC::CSW_ELEMENT_MODIFIER = "modifier";
    const String CSWC::CSW_ELEMENT_MORPH = "morph";
    const String CSWC::CSW_ELEMENT_NAME = "name";
    const String CSWC::CSW_ELEMENT_NAME_ARRAY = "Name_array";
    const String CSWC::CSW_ELEMENT_NEWPARAM = "newparam";
    const String CSWC::CSW_ELEMENT_NODE = "node";
    const String CSWC::CSW_ELEMENT_OPTICS = "optics";
    const String CSWC::CSW_ELEMENT_OPTION = "option";
    const String CSWC::CSW_ELEMENT_ORDER = "order";
    const String CSWC::CSW_ELEMENT_P = "p";
    const String CSWC::CSW_ELEMENT_PARAM = "param";
    const String CSWC::CSW_ELEMENT_PASS = "pass";
    const String CSWC::CSW_ELEMENT_PH = "ph";
	const String CSWC::CSW_ELEMENT_PLUGIN_VERSION = "pluginVersion";
	const String CSWC::CSW_ELEMENT_SHA1 = "sha1";
    const String CSWC::CSW_ELEMENT_PHONG = "phong";
	const String CSWC::CSW_ELEMENT_POINT = "point";
    const String CSWC::CSW_ELEMENT_POLYLIST = "polylist";
    const String CSWC::CSW_ELEMENT_POLYGONS = "polygons";
    const String CSWC::CSW_ELEMENT_PRECISION = "precision";
    const String CSWC::CSW_ELEMENT_PRIMARY = "primary";
    const String CSWC::CSW_ELEMENT_PROFILE_COMMON = "profile_COMMON";
    const String CSWC::CSW_ELEMENT_PROFILE_CG = "profile_CG";
    const String CSWC::CSW_ELEMENT_PROFILE_GLES = "profile_GLES";
    const String CSWC::CSW_ELEMENT_PROFILE_GLSL = "profile_GLSL";
	const String CSWC::CSW_ELEMENT_RANGE = "range";
	const String CSWC::CSW_ELEMENT_REF = "ref";
	const String CSWC::CSW_ELEMENT_QUADRATIC_ATTENUATION = "quadratic_attenuation";
    const String CSWC::CSW_ELEMENT_REFLECTIVE = "reflective";
    const String CSWC::CSW_ELEMENT_REFLECTIVITY = "reflectivity";
    const String CSWC::CSW_ELEMENT_REVISION = "revision";
    const String CSWC::CSW_ELEMENT_ROTATE = "rotate";
    const String CSWC::CSW_ELEMENT_SAMPLER = "sampler";
    const String CSWC::CSW_ELEMENT_SAMPLER2D = "sampler2D";
    const String CSWC::CSW_ELEMENT_SCALE = "scale";
    const String CSWC::CSW_ELEMENT_SCENE = "scene";
    const String CSWC::CSW_ELEMENT_SETPARAM = "setparam";
    const String CSWC::CSW_ELEMENT_SEMANTIC = "semantic";
    const String CSWC::CSW_ELEMENT_SHADER = "shader";
    const String CSWC::CSW_ELEMENT_SHININESS = "shininess";
    const String CSWC::CSW_ELEMENT_SIZE = "size";
    const String CSWC::CSW_ELEMENT_SKELETON = "skeleton";
    const String CSWC::CSW_ELEMENT_SKEW = "skew";
    const String CSWC::CSW_ELEMENT_SKIN = "skin";
    const String CSWC::CSW_ELEMENT_SPECULAR = "specular";
    const String CSWC::CSW_ELEMENT_SOURCE = "source";
    const String CSWC::CSW_ELEMENT_SOURCE_DATA = "source_data";
	const String CSWC::CSW_ELEMENT_SPLINE = "spline";
	const String CSWC::CSW_ELEMENT_SPOT = "spot";
    const String CSWC::CSW_ELEMENT_STENCIL_CLEAR = "stencil_clear";
    const String CSWC::CSW_ELEMENT_STENCIL_TARGET = "stencil_target";
    const String CSWC::CSW_ELEMENT_SUBJECT = "subject";
    const String CSWC::CSW_ELEMENT_SURFACE = "surface";
    const String CSWC::CSW_ELEMENT_TARGETS = "targets";
    const String CSWC::CSW_ELEMENT_TECHNIQUE = "technique";
    const String CSWC::CSW_ELEMENT_TECHNIQUE_HINT = "technique_hint";
    const String CSWC::CSW_ELEMENT_TECHNIQUE_COMMON = "technique_common";
    const String CSWC::CSW_ELEMENT_TEXTURE = "texture";
    const String CSWC::CSW_ELEMENT_TITLE = "title";
    const String CSWC::CSW_ELEMENT_TRANSLATE = "translate";
    const String CSWC::CSW_ELEMENT_TRANSPARENCY = "transparency";
    const String CSWC::CSW_ELEMENT_TRANSPARENT = "transparent";	
    const String CSWC::CSW_ELEMENT_TRIANGLES = "triangles";
	const String CSWC::CSW_ELEMENT_TRIFANS = "trifans";
	const String CSWC::CSW_ELEMENT_TRISTRIPS = "tristrips";
    const String CSWC::CSW_ELEMENT_UNIT = "unit";
    const String CSWC::CSW_ELEMENT_UP_AXIS = "up_axis";
    const String CSWC::CSW_ELEMENT_V = "v";
    const String CSWC::CSW_ELEMENT_VCOUNT = "vcount";
    const String CSWC::CSW_ELEMENT_VERTEX_WEIGHTS = "vertex_weights";
    const String CSWC::CSW_ELEMENT_VERTICES = "vertices";
    const String CSWC::CSW_ELEMENT_VIEWPORT_RATIO = "viewport_ratio";
    const String CSWC::CSW_ELEMENT_VISUAL_SCENE = "visual_scene";
	const String CSWC::CSW_ELEMENT_PHYSICS_SCENE = "physics_scene";
    const String CSWC::CSW_ELEMENT_WRAP_S = "wrap_s";
    const String CSWC::CSW_ELEMENT_WRAP_T = "wrap_t";
    const String CSWC::CSW_ELEMENT_WRAP_P = "wrap_p";
    const String CSWC::CSW_ELEMENT_XFOV = "xfov";
    const String CSWC::CSW_ELEMENT_XMAG = "xmag";
    const String CSWC::CSW_ELEMENT_YFOV = "yfov";
    const String CSWC::CSW_ELEMENT_YMAG = "ymag";
    const String CSWC::CSW_ELEMENT_ZFAR = "zfar";
    const String CSWC::CSW_ELEMENT_ZNEAR = "znear";
	const String CSWC::CSW_ELEMENT_GRAVITY = "gravity";
	const String CSWC::CSW_ELEMENT_STEP = "step";

	const String CSWC::CSW_ATTRIBUTE_CLOSED = "closed";
    const String CSWC::CSW_ATTRIBUTE_COUNT = "count";
    const String CSWC::CSW_ATTRIBUTE_END = "end";
    const String CSWC::CSW_ATTRIBUTE_FACE = "face";
    const String CSWC::CSW_ATTRIBUTE_ID = "id";
	const String CSWC::CSW_ATTRIBUTE_INDEX = "index";
	const String CSWC::CSW_ATTRIBUTE_INPUT_SEMANTIC = "input_semantic";
	const String CSWC::CSW_ATTRIBUTE_INPUT_SET = "input_set";
    const String CSWC::CSW_ATTRIBUTE_MATERIAL = "material";
    const String CSWC::CSW_ATTRIBUTE_METER = "meter";
    const String CSWC::CSW_ATTRIBUTE_METHOD = "method";
    const String CSWC::CSW_ATTRIBUTE_MIP = "mip";
    const String CSWC::CSW_ATTRIBUTE_NAME = "name";
    const String CSWC::CSW_ATTRIBUTE_OFFSET = "offset";
    const String CSWC::CSW_ATTRIBUTE_OPAQUE = "opaque";
    const String CSWC::CSW_ATTRIBUTE_PLATFORM = "platform";
    const String CSWC::CSW_ATTRIBUTE_PROFILE = "profile";
    const String CSWC::CSW_ATTRIBUTE_PROGRAM = "program";
    const String CSWC::CSW_ATTRIBUTE_REF = "ref";
    const String CSWC::CSW_ATTRIBUTE_RIGID_BODY = "rigid_body";
    const String CSWC::CSW_ATTRIBUTE_SEMANTIC = "semantic";
    const String CSWC::CSW_ATTRIBUTE_SET = "set";
    const String CSWC::CSW_ATTRIBUTE_SID = "sid";
    const String CSWC::CSW_ATTRIBUTE_SLICE = "slice";
    const String CSWC::CSW_ATTRIBUTE_SOURCE = "source";
    const String CSWC::CSW_ATTRIBUTE_STAGE = "stage";
    const String CSWC::CSW_ATTRIBUTE_START = "start";
    const String CSWC::CSW_ATTRIBUTE_STRIDE = "stride";
    const String CSWC::CSW_ATTRIBUTE_SYMBOL = "symbol";
	const String CSWC::CSW_ATTRIBUTE_BODY = "body";
    const String CSWC::CSW_ATTRIBUTE_TARGET = "target";
    const String CSWC::CSW_ATTRIBUTE_CONSTRAINT = "constraint";
    const String CSWC::CSW_ATTRIBUTE_TEXCOORD = "texcoord";
    const String CSWC::CSW_ATTRIBUTE_TEXTURE = "texture";
    const String CSWC::CSW_ATTRIBUTE_TYPE = "type";
    const String CSWC::CSW_ATTRIBUTE_URL = "url";
    const String CSWC::CSW_ATTRIBUTE_VALUE = "value";
    const String CSWC::CSW_ATTRIBUTE_VERSION = "version";
	const String CSWC::CSW_ATTRIBUTE_XMLNS = "xmlns";
	const String CSWC::CSW_ATTRIBUTE_XMLNS_XSI = "xmlns:xsi";
	const String CSWC::CSW_ATTRIBUTE_XSI_SCHEMALOCATION = "xsi:schemaLocation";
	const String CSWC::CSW_ATTRIBUTE_CONVEX_HULL_OF = "convex_hull_of";

    const String CSWC::CSW_SEMANTIC_BINDMATRIX = "INV_BIND_MATRIX";
    const String CSWC::CSW_SEMANTIC_COLOR = "COLOR";
    const String CSWC::CSW_SEMANTIC_CONTINUITY = "CONTINUITY";
    const String CSWC::CSW_SEMANTIC_INTERPOLATION = "INTERPOLATION";
    const String CSWC::CSW_SEMANTIC_INPUT = "INPUT";
    const String CSWC::CSW_SEMANTIC_IN_TANGENT = "IN_TANGENT";
    const String CSWC::CSW_SEMANTIC_JOINT = "JOINT";
    const String CSWC::CSW_SEMANTIC_LINEAR_STEPS = "LINEAR_STEPS";
    const String CSWC::CSW_SEMANTIC_MORPH_WEIGHT = "MORPH_WEIGHT";
    const String CSWC::CSW_SEMANTIC_MORPH_TARGET = "MORPH_TARGET";
    const String CSWC::CSW_SEMANTIC_NORMAL = "NORMAL";
    const String CSWC::CSW_SEMANTIC_OUTPUT = "OUTPUT";
    const String CSWC::CSW_SEMANTIC_OUT_TANGENT = "OUT_TANGENT";
    const String CSWC::CSW_SEMANTIC_POSITION = "POSITION";
	const String CSWC::CSW_SEMANTIC_TEXCOORD = "TEXCOORD";
	const String CSWC::CSW_SEMANTIC_TANGENT = "TANGENT";
	const String CSWC::CSW_SEMANTIC_BINORMAL = "BINORMAL";
    const String CSWC::CSW_SEMANTIC_TEXTANGENT = "TEXTANGENT";
    const String CSWC::CSW_SEMANTIC_TEXBINORMAL = "TEXBINORMAL";
    const String CSWC::CSW_SEMANTIC_VERTEX = "VERTEX";
    const String CSWC::CSW_SEMANTIC_WEIGHT = "WEIGHT";

    const String CSWC::CSW_TEXT_X_UP = "X_UP";
    const String CSWC::CSW_TEXT_Y_UP = "Y_UP";
    const String CSWC::CSW_TEXT_Z_UP = "Z_UP";

    const String CSWC::CSW_NODE_TYPE_JOINT = "JOINT";
    const String CSWC::CSW_NODE_TYPE_NODE = "NODE";

    const String CSWC::CSW_VALUE_TYPE_BOOL = "bool";
    const String CSWC::CSW_VALUE_TYPE_BOOL2 = "bool2";
    const String CSWC::CSW_VALUE_TYPE_BOOL3 = "bool3";
    const String CSWC::CSW_VALUE_TYPE_BOOL4 = "bool4";
    const String CSWC::CSW_VALUE_TYPE_INT = "int";
    const String CSWC::CSW_VALUE_TYPE_INT2 = "int2";
    const String CSWC::CSW_VALUE_TYPE_INT3 = "int3";
    const String CSWC::CSW_VALUE_TYPE_INT4 = "int4";
	const String CSWC::CSW_VALUE_TYPE_DOUBLE = "double";
    const String CSWC::CSW_VALUE_TYPE_DOUBLE2 = "double2";
    const String CSWC::CSW_VALUE_TYPE_DOUBLE3 = "double3";
    const String CSWC::CSW_VALUE_TYPE_DOUBLE4 = "double4";
    const String CSWC::CSW_VALUE_TYPE_FLOAT = "float";
    const String CSWC::CSW_VALUE_TYPE_FLOAT2 = "float2";
    const String CSWC::CSW_VALUE_TYPE_FLOAT3 = "float3";
    const String CSWC::CSW_VALUE_TYPE_FLOAT4 = "float4";
    const String CSWC::CSW_VALUE_TYPE_FLOAT2x2 = "float2x2";
    const String CSWC::CSW_VALUE_TYPE_FLOAT3x3 = "float3x3";
    const String CSWC::CSW_VALUE_TYPE_FLOAT4x4 = "float4x4";
    const String CSWC::CSW_VALUE_TYPE_STRING = "string";
    const String CSWC::CSW_VALUE_TYPE_SURFACE = "surface";
    const String CSWC::CSW_VALUE_TYPE_SAMPLER_1D = "sampler1D";
    const String CSWC::CSW_VALUE_TYPE_SAMPLER_2D = "sampler2D";
    const String CSWC::CSW_VALUE_TYPE_SAMPLER_3D = "sampler3D";
    const String CSWC::CSW_VALUE_TYPE_SAMPLER_CUBE = "samplerCUBE";
    const String CSWC::CSW_VALUE_TYPE_SAMPLER_RECT = "samplerRECT";
    const String CSWC::CSW_VALUE_TYPE_SAMPLER_DEPTH = "samplerDEPTH";
    const String CSWC::CSW_VALUE_TYPE_SAMPLER_STATE = "sampler_state";
    const String CSWC::CSW_VALUE_TYPE_NAME = "name";
    const String CSWC::CSW_VALUE_TYPE_IDREF = "IDREF";

    const String CSWC::CSW_SAMPLER_FILTER_LINEAR = "LINEAR";
    const String CSWC::CSW_SAMPLER_FILTER_LINEAR_MIPMAP_LINEAR = "LINEAR_MIPMAP_LINEAR" ;
    const String CSWC::CSW_SAMPLER_FILTER_LINEAR_MIPMAP_NEAREST = "LINEAR_MIPMAP_NEAREST";
    const String CSWC::CSW_SAMPLER_FILTER_NEAREST = "NEAREST";
    const String CSWC::CSW_SAMPLER_FILTER_NEAREST_MIPMAP_NEAREST = "NEAREST_MIPMAP_NEAREST";
    const String CSWC::CSW_SAMPLER_FILTER_NEAREST_MIPMAP_LINEAR = "NEAREST_MIPMAP_LINEAR";
    const String CSWC::CSW_SAMPLER_FILTER_NONE = "NONE";

    const String CSWC::CSW_CHANNEL_RGB = "RGB";
    const String CSWC::CSW_CHANNEL_RGBA = "RGBA";
    const String CSWC::CSW_CHANNEL_L = "L";
    const String CSWC::CSW_CHANNEL_LA = "LA";
    const String CSWC::CSW_CHANNEL_D = "D";
    const String CSWC::CSW_CHANNEL_XYZ = "XYZ";
    const String CSWC::CSW_CHANNEL_XYZW = "XYZW";

    const String CSWC::CSW_RANGE_SNORM = "SNORM";
    const String CSWC::CSW_RANGE_UNORM = "UNORM";
    const String CSWC::CSW_RANGE_SINT = "SINT";
    const String CSWC::CSW_RANGE_UINT = "UINT";
    const String CSWC::CSW_RANGE_FLOAT = "FLOAT";

    const String CSWC::CSW_PRECISION_LOW = "LOW";
    const String CSWC::CSW_PRECISION_MID = "MID";
    const String CSWC::CSW_PRECISION_HIGH = "HIGH";

    const String CSWC::CSW_OPTION_SRGB_GAMMA = "SRGB_GAMMA";
    const String CSWC::CSW_OPTION_NORMALIZED3 = "NORMALIZED3";
    const String CSWC::CSW_OPTION_NORMALIZED4 = "NORMALIZED4";
    const String CSWC::CSW_OPTION_COMPRESSABLE = "COMPRESSABLE";

    // texture wrap modes
    const String CSWC::CSW_TEXTURE_WRAP_NONE = "NONE";
    const String CSWC::CSW_TEXTURE_WRAP_WRAP = "WRAP";
    const String CSWC::CSW_TEXTURE_WRAP_MIRROR = "MIRROR";
    const String CSWC::CSW_TEXTURE_WRAP_CLAMP = "CLAMP";
    const String CSWC::CSW_TEXTURE_WRAP_BORDER = "BORDER";
    const String CSWC::CSW_TEXTURE_WRAP_UNKNOWN = "UNKNOWN";

    const String CSWC::CSW_SURFACE_TYPE_UNTYPED = "UNTYPED";
    const String CSWC::CSW_SURFACE_TYPE_1D = "1D";
    const String CSWC::CSW_SURFACE_TYPE_2D = "2D";
    const String CSWC::CSW_SURFACE_TYPE_3D = "3D";
    const String CSWC::CSW_SURFACE_TYPE_RECT = "RECT";
    const String CSWC::CSW_SURFACE_TYPE_CUBE = "CUBE";
    const String CSWC::CSW_SURFACE_TYPE_DEPTH = "DEPTH";


    const String CSWC::CSW_OPAQUE_TYPE_A_ONE = "A_ONE";
    const String CSWC::CSW_OPAQUE_TYPE_RGB_ONE = "RGB_ONE";
    const String CSWC::CSW_OPAQUE_TYPE_RGB_ZERO = "RGB_ZERO";
    const String CSWC::CSW_OPAQUE_TYPE_A_ZERO = "A_ZERO";

    const String CSWC::CSW_FACE_POSITIVE_X = "POSITIVE_X";
    const String CSWC::CSW_FACE_NEGATIVE_X = "NEGATIVE_X";
    const String CSWC::CSW_FACE_POSITIVE_Y = "POSITIVE_Y";
    const String CSWC::CSW_FACE_NEGATIVE_Y = "NEGATIVE_Y";
    const String CSWC::CSW_FACE_POSITIVE_Z = "POSITIVE_Z";
    const String CSWC::CSW_FACE_NEGATIVE_Z = "NEGATIVE_Z";

    const String CSWC::CSW_MODIFIER_CONST = "CONST";
    const String CSWC::CSW_MODIFIER_UNIFORM = "UNIFORM";
    const String CSWC::CSW_MODIFIER_VARYING = "VARYING";
    const String CSWC::CSW_MODIFIER_STATIC = "STATIC";
    const String CSWC::CSW_MODIFIER_VOLATILE = "VOLATILE";
    const String CSWC::CSW_MODIFIER_EXTERN = "EXTERN";
    const String CSWC::CSW_MODIFIER_SHARED = "SHARED";

    const String CSWC::CSW_FX_FUNCTION_NEVER = "NEVER";
    const String CSWC::CSW_FX_FUNCTION_LESS = "LESS";
    const String CSWC::CSW_FX_FUNCTION_EQUAL = "EQUAL";
    const String CSWC::CSW_FX_FUNCTION_LEQUAL = "LEQUAL";
    const String CSWC::CSW_FX_FUNCTION_GREATER = "GREATER";
    const String CSWC::CSW_FX_FUNCTION_NEQUAL = "NOTEQUAL";
    const String CSWC::CSW_FX_FUNCTION_GEQUAL = "GEQUAL";
    const String CSWC::CSW_FX_FUNCTION_ALWAYS = "ALWAYS";

    const String CSWC::CSW_FX_ANNOTATION_RESOURCE_NAME = "ResourceName";
    const String CSWC::CSW_FX_ANNOTATION_RESOURCE_TYPE = "ResourceType";
    const String CSWC::CSW_FX_SHADER_STAGE_VERTEX = "VERTEX";
    const String CSWC::CSW_FX_SHADER_STAGE_VERTEXPROGRAM = "VERTEXPROGRAM";
    const String CSWC::CSW_FX_SHADER_STAGE_FRAGMENT = "FRAGMENT";
    const String CSWC::CSW_FX_SHADER_STAGE_FRAGMENTPROGRAM = "FRAGMENTPROGRAM";

    const String CSWC::CSW_FX_STATE_ALPHA_FUNC = "alpha_func";
    const String CSWC::CSW_FX_STATE_BLEND_FUNC = "blend_func";
    const String CSWC::CSW_FX_STATE_BLEND_FUNC_SEPARATE = "blend_func_separate";
    const String CSWC::CSW_FX_STATE_BLEND_EQUATION = "blend_equation";
    const String CSWC::CSW_FX_STATE_BLEND_EQUATION_SEPARATE = "blend_equation_separate";
    const String CSWC::CSW_FX_STATE_COLOR_MATERIAL = "color_material";
    const String CSWC::CSW_FX_STATE_CULL_FACE = "cull_face";
    const String CSWC::CSW_FX_STATE_DEPTH_FUNC = "depth_func";
    const String CSWC::CSW_FX_STATE_FOG_MODE = "fog_mode";
    const String CSWC::CSW_FX_STATE_FOG_COORD_SRC = "fog_coord_src";
    const String CSWC::CSW_FX_STATE_FRONT_FACE = "front_face";
    const String CSWC::CSW_FX_STATE_LIGHT_MODEL_COLOR_CONTROL = "light_model_color_control";
    const String CSWC::CSW_FX_STATE_LOGIC_OP = "logic_op";
    const String CSWC::CSW_FX_STATE_POLYGON_MODE = "polygon_mode";
    const String CSWC::CSW_FX_STATE_SHADE_MODEL = "shade_model";
    const String CSWC::CSW_FX_STATE_STENCIL_FUNC = "stencil_func";
    const String CSWC::CSW_FX_STATE_STENCIL_OP = "stencil_op";
    const String CSWC::CSW_FX_STATE_STENCIL_FUNC_SEPARATE = "stencil_func_separate";
    const String CSWC::CSW_FX_STATE_STENCIL_OP_SEPARATE = "stencil_op_separate";
    const String CSWC::CSW_FX_STATE_STENCIL_MASK_SEPARATE = "stencil_mask_separate";
    const String CSWC::CSW_FX_STATE_LIGHT_ENABLE = "light_enable";
    const String CSWC::CSW_FX_STATE_LIGHT_AMBIENT = "light_ambient";
    const String CSWC::CSW_FX_STATE_LIGHT_DIFFUSE = "light_diffuse";
    const String CSWC::CSW_FX_STATE_LIGHT_SPECULAR = "light_specular";
    const String CSWC::CSW_FX_STATE_LIGHT_POSITION = "light_position";
    const String CSWC::CSW_FX_STATE_LIGHT_CONSTANT_ATTENUATION = "light_constant_attenuation";
    const String CSWC::CSW_FX_STATE_LIGHT_LINEAR_ATTENUATION = "light_linear_attenuation";
    const String CSWC::CSW_FX_STATE_LIGHT_QUADRATIC_ATTENUATION = "light_quadratic_attenuation";
    const String CSWC::CSW_FX_STATE_LIGHT_SPOT_CUTOFF = "light_spot_cutoff";
    const String CSWC::CSW_FX_STATE_LIGHT_SPOT_DIRECTION = "light_spot_direction";
    const String CSWC::CSW_FX_STATE_LIGHT_SPOT_EXPONENT = "light_spot_exponent";
    const String CSWC::CSW_FX_STATE_TEXTURE1D = "texture1D";
    const String CSWC::CSW_FX_STATE_TEXTURE2D = "texture2D";
    const String CSWC::CSW_FX_STATE_TEXTURE3D = "texture3D";
    const String CSWC::CSW_FX_STATE_TEXTURECUBE = "textureCUBE";
    const String CSWC::CSW_FX_STATE_TEXTURERECT = "textureRECT";
    const String CSWC::CSW_FX_STATE_TEXTUREDEPTH = "textureDEPTH";
    const String CSWC::CSW_FX_STATE_TEXTURE1D_ENABLE = "texture1D_enable";
    const String CSWC::CSW_FX_STATE_TEXTURE2D_ENABLE = "texture2D_enable";
    const String CSWC::CSW_FX_STATE_TEXTURE3D_ENABLE = "texture3D_enable";
    const String CSWC::CSW_FX_STATE_TEXTURECUBE_ENABLE = "textureCUBE_enable";
    const String CSWC::CSW_FX_STATE_TEXTURERECT_ENABLE = "textureRECT_enable";
    const String CSWC::CSW_FX_STATE_TEXTUREDEPTH_ENABLE = "textureDEPTH_enable";
    const String CSWC::CSW_FX_STATE_TEXTURE_ENV_COLOR = "texture_env_color";
    const String CSWC::CSW_FX_STATE_TEXTURE_ENV_MODE = "texture_env_mode";
    const String CSWC::CSW_FX_STATE_CLIP_PLANE = "clip_plane";
    const String CSWC::CSW_FX_STATE_CLIP_PLANE_ENABLE = "clip_plane_enable";
    const String CSWC::CSW_FX_STATE_BLEND_COLOR = "blend_color";
    const String CSWC::CSW_FX_STATE_CLEAR_COLOR = "clear_color";
    const String CSWC::CSW_FX_STATE_CLEAR_STENCIL = "clear_stencil";
    const String CSWC::CSW_FX_STATE_CLEAR_DEPTH = "clear_depth";
    const String CSWC::CSW_FX_STATE_COLOR_MASK = "color_mask";
    const String CSWC::CSW_FX_STATE_DEPTH_BOUNDS = "depth_bounds";
    const String CSWC::CSW_FX_STATE_DEPTH_MASK = "depth_mask";
    const String CSWC::CSW_FX_STATE_DEPTH_RANGE = "depth_range";
    const String CSWC::CSW_FX_STATE_FOG_DENSITY = "fog_density";
    const String CSWC::CSW_FX_STATE_FOG_START = "fog_start";
    const String CSWC::CSW_FX_STATE_FOG_END = "fog_end";
    const String CSWC::CSW_FX_STATE_FOG_COLOR = "fog_color";
    const String CSWC::CSW_FX_STATE_LIGHT_MODEL_AMBIENT = "light_model_ambient";
    const String CSWC::CSW_FX_STATE_LIGHTING_ENABLE = "lighting_enable";
    const String CSWC::CSW_FX_STATE_LINE_STIPPLE = "line_stipple";
    const String CSWC::CSW_FX_STATE_LINE_STIPPLE_ENABLE = "line_stipple_enable";
    const String CSWC::CSW_FX_STATE_LINE_WIDTH = "line_width";
    const String CSWC::CSW_FX_STATE_MATERIAL_AMBIENT = "material_ambient";
    const String CSWC::CSW_FX_STATE_MATERIAL_DIFFUSE = "material_diffuse";
    const String CSWC::CSW_FX_STATE_MATERIAL_EMISSION = "material_emission";
    const String CSWC::CSW_FX_STATE_MATERIAL_SHININESS = "material_shininess";
    const String CSWC::CSW_FX_STATE_MATERIAL_SPECULAR = "material_specular";
    const String CSWC::CSW_FX_STATE_MODEL_VIEW_MATRIX = "model_view_matrix";
    const String CSWC::CSW_FX_STATE_POINT_DISTANCE_ATTENUATION = "point_distance_attenuation";
    const String CSWC::CSW_FX_STATE_POINT_FADE_THRESHOLD_SIZE = "point_fade_threshold_size";
    const String CSWC::CSW_FX_STATE_POINT_SIZE = "point_size";
    const String CSWC::CSW_FX_STATE_POINT_SIZE_MIN = "point_size_min";
    const String CSWC::CSW_FX_STATE_POINT_SIZE_MAX = "point_size_max";
    const String CSWC::CSW_FX_STATE_POLYGON_OFFSET = "polygon_offset";
    const String CSWC::CSW_FX_STATE_PROJECTION_MATRIX = "projection_matrix";
    const String CSWC::CSW_FX_STATE_SCISSOR = "scissor";
    const String CSWC::CSW_FX_STATE_STENCIL_MASK = "stencil_mask";
    const String CSWC::CSW_FX_STATE_ALPHA_TEST_ENABLE = "alpha_test_enable";
    const String CSWC::CSW_FX_STATE_AUTO_NORMAL_ENABLE = "auto_normal_enable";
    const String CSWC::CSW_FX_STATE_BLEND_ENABLE = "blend_enable";
    const String CSWC::CSW_FX_STATE_COLOR_LOGIC_OP_ENABLE = "color_logic_op_enable";
    const String CSWC::CSW_FX_STATE_COLOR_MATERIAL_ENABLE = "color_material_enable";
    const String CSWC::CSW_FX_STATE_CULL_FACE_ENABLE = "cull_face_enable";
    const String CSWC::CSW_FX_STATE_DEPTH_BOUNDS_ENABLE = "depth_bounds_enable";
    const String CSWC::CSW_FX_STATE_DEPTH_CLAMP_ENABLE = "depth_clamp_enable";
    const String CSWC::CSW_FX_STATE_DEPTH_TEST_ENABLE = "depth_test_enable";
    const String CSWC::CSW_FX_STATE_DITHER_ENABLE = "dither_enable";
    const String CSWC::CSW_FX_STATE_FOG_ENABLE = "fog_enable";
    const String CSWC::CSW_FX_STATE_LIGHT_MODEL_LOCAL_VIEWER_ENABLE = "light_model_local_viewer_enable";
    const String CSWC::CSW_FX_STATE_LIGHT_MODEL_TWO_SIDE_ENABLE = "light_model_two_side_enable";
    const String CSWC::CSW_FX_STATE_LINE_SMOOTH_ENABLE = "line_smooth_enable";
    const String CSWC::CSW_FX_STATE_LOGIC_OP_ENABLE = "logic_op_enable";
    const String CSWC::CSW_FX_STATE_MULTISAMPLE_ENABLE = "multisample_enable";
    const String CSWC::CSW_FX_STATE_NORMALIZE_ENABLE = "normalize_enable";
    const String CSWC::CSW_FX_STATE_POINT_SMOOTH_ENABLE = "point_smooth_enable";
    const String CSWC::CSW_FX_STATE_POLYGON_OFFSET_FILL_ENABLE = "polygon_offset_fill_enable";
    const String CSWC::CSW_FX_STATE_POLYGON_OFFSET_LINE_ENABLE = "polygon_offset_line_enable";
    const String CSWC::CSW_FX_STATE_POLYGON_OFFSET_POINT_ENABLE = "polygon_offset_point_enable";
    const String CSWC::CSW_FX_STATE_POLYGON_SMOOTH_ENABLE = "polygon_smooth_enable";
    const String CSWC::CSW_FX_STATE_POLYGON_STIPPLE_ENABLE = "polygon_stipple_enable";
    const String CSWC::CSW_FX_STATE_RESCALE_NORMAL_ENABLE = "rescale_normal_enable";
    const String CSWC::CSW_FX_STATE_SAMPLE_ALPHA_TO_COVERAGE_ENABLE = "sample_alpha_to_coverage_enable";
    const String CSWC::CSW_FX_STATE_SAMPLE_ALPHA_TO_ONE_ENABLE = "sample_alpha_to_one_enable";
    const String CSWC::CSW_FX_STATE_SAMPLE_COVERAGE_ENABLE = "sample_coverage_enable";
    const String CSWC::CSW_FX_STATE_SCISSOR_TEST_ENABLE = "scissor_test_enable";
    const String CSWC::CSW_FX_STATE_STENCIL_TEST_ENABLE = "stencil_test_enable";

	const String CSWC::VALUE_TYPE_BOOL = "bool";
	const String CSWC::VALUE_TYPE_INTEGER = "integer";
	const String CSWC::VALUE_TYPE_FLOAT = "float";
	const String CSWC::VALUE_TYPE_DOUBLE = "double";
	const String CSWC::VALUE_TYPE_STRING = "string";
}
