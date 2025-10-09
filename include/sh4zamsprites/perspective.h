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
#endif // PERSPECTIVE_H