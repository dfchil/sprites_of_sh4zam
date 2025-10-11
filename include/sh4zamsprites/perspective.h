#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#include <dc/matrix.h> /* Matrix library headers for handling matrix operations */
#include <dc/matrix3d.h> /* Matrix3D library headers for handling 3D matrix operations */
#include <sh4zam/shz_sh4zam.h>
#include <stdio.h>

#ifndef XSCALE
#define XSCALE 1.0f
#endif

#define XCENTER 0.0f
#define YCENTER 0.0f
#define COT_FOVY_2 1.0f
#define ZNEAR 1.0f
#define ZFAR 100.0f

/* Screen view matrix template (used to transform to screen space) */
static alignas(32) shz_mat4x4_t scrn_mtrx = {
    .elem2D = {
      { YCENTER, 0.0f,    0.0f, 0.0f},
      { 0.0f,    YCENTER, 0.0f, 0.0f},
      { 0.0f,    0.0f,    1.0f, 0.0f},
      { XCENTER, YCENTER, 0.0f, 1.0f}
    }
  };

/* Frustum matrix template (does perspective) */
static alignas(32) shz_mat4x4_t p_mtrx = {
  .elem2D = {
    { COT_FOVY_2, 0.0f,       0.0f,                            0.0f},
    { 0.0f,       COT_FOVY_2, 0.0f,                            0.0f},
    { 0.0f,       0.0f,       (ZFAR + ZNEAR) / (ZNEAR - ZFAR), -1.0f},
    { 0.0f,       0.0f,       2 * ZFAR *ZNEAR / (ZNEAR - ZFAR), 1.0f}
  }
};

/**
 * Create and apply a perspective projection matrix to the current matrix
 * stack.
 * My backport of the mat_perspective function from KallistiOS's matrix3d.h
 * 
 * \param screen_width The width of the screen, in pixels.
 * \param screen_height The height of the screen, in pixels.
 * \param fovy The field of view angle, in degrees, in the y direction.
 * \param near_z The distance to the near clipping plane.
 * \param far_z The distance to the far clipping plane.
 */
void kos_perspective(float screen_width, float screen_height, float fovy,
                    float near_z, float far_z) {
  scrn_mtrx.elem2D[0][0] = scrn_mtrx.elem2D[1][1] = scrn_mtrx.elem2D[3][1] =
      screen_height * 0.5f;
  scrn_mtrx.elem2D[3][0] = screen_width * 0.5f;
  shz_xmtrx_apply_4x4(&scrn_mtrx);

  float radians = fovy * F_PI / 180.0f;
  float cot_fovy_2 = 1.0f / shz_tanf(-radians * 0.5f);

  p_mtrx.elem2D[0][0] = cot_fovy_2;
  p_mtrx.elem2D[1][1] = cot_fovy_2;
  p_mtrx.elem2D[2][2] = (far_z + near_z) / (near_z - far_z);
  p_mtrx.elem2D[3][2] = (2.0f * far_z * near_z) / (near_z - far_z);

  shz_xmtrx_apply_4x4(&p_mtrx);
}

#define SHZ_IDENTITY_MAT4X4_TMPLATE { \
  .elem2D = { \
    { 1.0f, 0.0f, 0.0f, 0.0f }, \
    { 0.0f, 1.0f, 0.0f, 0.0f }, \
    { 0.0f, 0.0f, 1.0f, 0.0f }, \
    { 0.0f, 0.0f, 0.0f, 1.0f } \
} } 

/* lookAt matrix template */
static alignas(32) shz_mat4x4_t lookAt_mtrx = SHZ_IDENTITY_MAT4X4_TMPLATE;

/* translation matrix template */
static alignas(32) shz_mat4x4_t translation_mtrx = SHZ_IDENTITY_MAT4X4_TMPLATE;

/**
 * Create and apply a lookAt matrix to the current matrix stack.
 * My backport of the mat_lookAt function from KallistiOS's matrix3d.h
 * 
 * \param eye The position of the camera.
 * \param center The point the camera is looking at.
 * \param upi The up vector.
 */
void kos_lookAt(const shz_vec3_t eye, const shz_vec3_t center,
               const shz_vec3_t upi) {
  shz_vec3_t forward = shz_vec3_normalize(shz_vec3_sub(center, eye));
  shz_vec3_t side = shz_vec3_normalize(shz_vec3_cross(forward, upi));
  shz_vec3_t up = shz_vec3_cross(side, forward);
  
  lookAt_mtrx.elem2D[0][0] = side.x;
  lookAt_mtrx.elem2D[1][0] = side.y;
  lookAt_mtrx.elem2D[2][0] = side.z;

  lookAt_mtrx.elem2D[0][1] = up.x;
  lookAt_mtrx.elem2D[1][1] = up.y;
  lookAt_mtrx.elem2D[2][1] = up.z;

  lookAt_mtrx.elem2D[0][2] = -forward.x;
  lookAt_mtrx.elem2D[1][2] = -forward.y;
  lookAt_mtrx.elem2D[2][2] = -forward.z;

  shz_xmtrx_apply_4x4(&lookAt_mtrx);

  translation_mtrx.elem2D[3][0] = -eye.x;
  translation_mtrx.elem2D[3][1] = -eye.y;
  translation_mtrx.elem2D[3][2] = -eye.z;
  shz_xmtrx_apply_4x4(&translation_mtrx);
}

alignas(32) shz_mat4x4_t stored_projection_view = {0};
void update_projection_view(float fovy) {
  shz_xmtrx_init_identity();
  kos_perspective(640.0f * XSCALE, 480.0f, fovy, 0.f, -10.0f);

  kos_lookAt(
            (shz_vec3_t){0.f, -0.00001f, 20.0f}, 
            (shz_vec3_t){0.f, 0.f, 0.f},
            (shz_vec3_t){0.f, 0.f, 1.f});

  shz_xmtrx_store_4x4(&stored_projection_view);
}
#endif // PERSPECTIVE_H