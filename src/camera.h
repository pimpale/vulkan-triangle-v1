#ifndef SRC_CAMERA_H_
#define SRC_CAMERA_H_

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <linmath.h>

// A set of 3 vectors forming a right handed orthonormal basis for the camera
typedef struct {
  vec3 front;
  vec3 right;
  vec3 up;
} CameraBasis;

// The camera struct
typedef struct {
  // global camera position
  vec3 pos;
  // pitch and yaw values in radians
  float pitch;
  float yaw;
  // the camera's basis
  CameraBasis basis;
  // Projection Matrix
  mat4x4 projection;
} Camera;

Camera new_Camera(const vec3 pos, const VkExtent2D dimensions);

void resizeCamera(Camera *camera, const VkExtent2D dimensions);
void updateCamera(Camera *camera, GLFWwindow *pWindow);
void getMvpCamera(mat4x4 mvp, const Camera *camera);

#endif // SRC_CAMERA_H_
