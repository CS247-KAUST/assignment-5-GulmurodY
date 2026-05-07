#version 410

in vec2 texCoord;

uniform sampler2D txtr;
uniform vec4 vertexColor;

// TODO: add uniform for colormap mode (int) and blend factor (float)
uniform int   colormapMode;  // 0 = off, 1 = rainbow, 2 = cool-warm
uniform float blendFactor;   // 0.0 = grayscale, 1.0 = full colormap

out vec4 fragColor;

// TODO: implement colormap functions
// Hint: define at least two colormaps (e.g. rainbow and cool-warm)
//   - Rainbow: interpolate between blue -> cyan -> green -> yellow -> red
//     using 4 segments at thresholds 0.25, 0.50, 0.75 with mix()
//   - Cool-warm: interpolate blue -> white -> red
//     using 2 segments at threshold 0.5 with mix()
vec3 rainbow(float t) {
    if (t < 0.25) return mix(vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0), t / 0.25);
    if (t < 0.50) return mix(vec3(0.0, 1.0, 1.0), vec3(0.0, 1.0, 0.0), (t - 0.25) / 0.25);
    if (t < 0.75) return mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0), (t - 0.50) / 0.25);
                  return mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), (t - 0.75) / 0.25);
}

vec3 coolWarm(float t) {
    if (t < 0.5) return mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), t / 0.5);
                 return mix(vec3(1.0, 1.0, 1.0), vec3(1.0, 0.0, 0.0), (t - 0.5) / 0.5);
}

void main() {
    // TODO: apply colormap to the scalar field texture value
    // Hint: sample the texture to get a scalar value in [0,1],
    //   use it to look up the colormap color, then blend between
    //   grayscale and the mapped color using the blend factor.
    //   When colormap mode is off (0), use the original behavior below.
    //   Make sure overlays (glyphs, streamlines) still work by
    //   setting colormapMode to 0 from C++ before drawing them.
    float scalar = texture(txtr, texCoord).r;
    vec3  gray   = vec3(scalar);

    vec3 mapped;
    if      (colormapMode == 1) mapped = rainbow(scalar);
    else if (colormapMode == 2) mapped = coolWarm(scalar);
    else                        mapped = gray;

    vec3 blended = mix(gray, mapped, blendFactor);

    fragColor = vertexColor + vec4(blended, 1.0);
}
