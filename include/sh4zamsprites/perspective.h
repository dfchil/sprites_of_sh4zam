#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#include <sh4zam/shz_sh4zam.h>
#include <stdio.h>

#ifndef XSCALE
#define XSCALE 1.0f
#endif

void print_matrix(const char* label) {
  shz_mat4x4_t mtrx __attribute__((aligned(32))) = {0};
  shz_xmtrx_store_4x4(&mtrx);

  printf("%s\n", label);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printf("%f ", mtrx.elem2D[i][j]);
    }
    printf("\n");
  }
}


shz_mat4x4_t stored_projection_view __attribute__((aligned(32))) = {0};
void update_projection_view(float fovy) {
  float radians = fovy * F_PI / 180.0f;
  float cot_fovy_2 = 1.0f / ftan(radians * 0.5f);

  shz_xmtrx_init_identity();  
  print_matrix("Identity Matrix:");
    
  shz_xmtrx_apply_perspective(radians, 320.0f * XSCALE / 240.0f, 10.0f);
  print_matrix("After Perspective Matrix:");
  
  const float eye[3] = {0.f, -0.00001f, 20.0f};
  const float  center[3] = {0.f, 0.f, 0.f};
  const float up[3] = {0.f, 0.f, 1.f};
  shz_xmtrx_apply_lookat(eye, center, up);
  print_matrix("After LookAt Matrix:");

  shz_xmtrx_apply_screen(320.0f * XSCALE, 240.0f);
  print_matrix("After Screen Matrix:");

  shz_xmtrx_store_4x4(&stored_projection_view);

// dc-tool-ip: mat[0]: 312.815460,0.000000,0.000000,0.000000,
// dc-tool-ip: mat[1]: 0.000160,312.815582,0.000000,0.000000,
// dc-tool-ip: mat[2]: -320.000000,-239.999847,0.000000,-1.000000,
// dc-tool-ip: mat[3]: 6720.000000,5040.000000,10.000000,21.000000,


}
#endif // PERSPECTIVE_H