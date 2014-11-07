"											\n\
#ifdef GL_ES \n\
precision lowp float; \n\
#endif \n\
 \n\
varying vec4 v_fragmentColor; \n\
varying vec2 v_texCoord; \n\
uniform sampler2D CC_Texture0; \n\
uniform vec4 v_effectColor; \n\
uniform vec4 v_textColor; \n\
 \n\
void main() \n\
{	\n\
  float dist = texture2D(CC_Texture0, v_texCoord).a; \n\
  //todo:Implementation 'fwidth' for glsl 1.0 \n\
  //float width = fwidth(dist); \n\
  //assign width for constant will lead to a little bit fuzzy,it's temporary measure.\n\
  float width = 0.1; \n\
  float alpha = smoothstep(0.37, 0.5, dist); \n\
  alpha = alpha < 1.0 ? alpha*0.75 : alpha; \n\
  //glow \n\
  vec4 baseColor = alpha < 1.0 ? vec4(255.0/255.0, 255.0/255.0, 255.0/255.0, 1.0) : v_fragmentColor; \n\
  vec4 leftColor = vec4(255.0/255.0, 0.0/255.0, 0.0/255.0, 1); \n\
  vec4 color = vec4(leftColor.rgb*(1.0-alpha*alpha) + baseColor.rgb*alpha*alpha*alpha, alpha); \n\
  gl_FragColor = color; \n\
} \n\
";