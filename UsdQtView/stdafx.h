#pragma once

#define NOMINMAX
#pragma warning(push)
#pragma warning(disable: 4244 4267 4305 6011 6319 26439 26451 26495 26812)
#include <pxr/base/gf/vec2f.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec4f.h>
#include <pxr/base/gf/matrix4d.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/variantSets.h>
#include <pxr/usd/usd/editContext.h>
#include <pxr/usd/usd/modelApi.h>
#include <pxr/usd/sdf/types.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#pragma warning(pop)
#undef NOMINMAX

using namespace PXR_INTERNAL_NS;

#include <QOpenGLWidget>
#include <QVector3D>
#include <QtOpenGL>
#include <QtCore/QString>
#include <QtWidgets/QWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QBoxLayout>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <conio.h>

#pragma comment(lib, "pcp.lib")
#pragma comment(lib, "sdf.lib")
#pragma comment(lib, "vt.lib")
#pragma comment(lib, "gf.lib")
#pragma comment(lib, "tf.lib")
#pragma comment(lib, "usd.lib")
#pragma comment(lib, "usdGeom.lib")
#pragma comment(lib, "usdShade.lib")
#pragma comment(lib, "usdSkel.lib")
#pragma comment(lib, "usdLux.lib")
#pragma comment(lib, "glm_static.lib")
