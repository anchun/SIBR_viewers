/** \file ibr.vp
 *
 * Vertex shader WITHOUT projection and modelview transformations.
 */

#version 420

layout(location = 0) in vec4 in_vertex;   /**< Input vertex coordinates */
layout(location = 1) in vec4 in_texcoord; /**< Input texture coordinates */
layout(location = 2) in vec4 in_color;    /**< Input colour value */

out vec4 texcoord;                        /**< Output texture coordinates */
out vec4 color;                           /**< Output color value */

void main(void) {
  gl_Position = in_vertex;
  texcoord    = in_texcoord;
  color       = in_color;
}
