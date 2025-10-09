#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#include <dc/matrix.h> /* Matrix library headers for handling matrix operations */
#include <dc/matrix3d.h> /* Matrix3D library headers for handling 3D matrix operations */
#include <sh4zam/shz_sh4zam.h>
#include <stdio.h>

#ifndef XSCALE
#define XSCALE 1.0f
#endif

void print_matrix(const char *label) {
  alignas(32) shz_mat4x4_t mtrx = {0};
  shz_xmtrx_store_4x4(&mtrx);

  printf("%s\n", label);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printf("%f ", mtrx.elem2D[i][j]);
    }
    printf("\n");
  }
}

alignas(32) shz_mat4x4_t stored_projection_view = {0};
void update_projection_view(float fovy) {
  mat_identity();
  float radians = fovy * F_PI / 180.0f;
  float cot_fovy_2 = 1.0f / ftan(radians * 0.5f);
  mat_perspective(XSCALE * 320.0f, 240.0f, cot_fovy_2, -10.f, +10.0f);

  point_t eye = {0.f, -0.00001f, 20.0f};
  point_t center = {0.f, 0.f, 0.f};
  vector_t up = {0.f, 0.f, 1.f};
  mat_lookat(&eye, &center, &up);
  mat_store((matrix_t*)(&stored_projection_view));
}

// TODO: get this code to work the same way as the old one above

// shz_mat4x4_t stored_projection_view alignas(32) = {0};
// void update_projection_view(float fovy) {
//   float radians = fovy * F_PI / 180.0f;
//   float cot_fovy_2 = 1.0f / ftan(radians * 0.5f);

//   shz_xmtrx_init_identity();
//   // print_matrix("Identity Matrix:");

//   shz_xmtrx_apply_perspective(radians, 320.0f * XSCALE / 240.0f, 10.0f);
//   // print_matrix("After Perspective Matrix:");

//   shz_xmtrx_apply_screen(320.0f * XSCALE, 240.0f);
//   // print_matrix("After Screen Matrix:");

//   const float eye[3] = {0.f, -0.1f, 20.0f};
//   const float center[3] = {320.0f * XSCALE, 240.0f, 0.f};
//   const float up[3] = {0.f, 0.f, 1.f};
//   shz_xmtrx_apply_lookat(eye, center, up);
//   // print_matrix("After LookAt Matrix:");

//   shz_xmtrx_store_4x4(&stored_projection_view);

//   // TODO: get values more like these in my transformation matrix
//   memcpy(&stored_projection_view.elem, &(float[16]){
//     312.815460f, 0.0f, 0.0f, 0.0f,
//     0.000320f, 312.815582f, 0.0f, 0.0f,
//     -640.0f,-239.999847f, 0.0f,-1.0f,
//     13440.0f, 5040.0f, 10.0f,21.0f
//   }, sizeof(stored_projection_view));
//   shz_xmtrx_load_4x4(&stored_projection_view);
//   // // print_matrix("Manually Set Projection-View Matrix:");

// // dc-tool-ip: mat[0]: 312.815460,0.000000,0.000000,0.000000,
// // dc-tool-ip: mat[1]: 0.000320,312.815582,0.000000,0.000000,
// // dc-tool-ip: mat[2]: -640.000000,-239.999847,0.000000,-1.000000,
// // dc-tool-ip: mat[3]: 13440.000000,5040.000000,10.000000,21.000000,}
// }
#endif // PERSPECTIVE_H