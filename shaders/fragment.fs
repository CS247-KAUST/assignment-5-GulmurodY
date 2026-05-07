#version 410

in vec2 texCoord;

uniform sampler2D txtr;
uniform vec4 vertexColor;

// TODO: add uniform for colormap mode (int) and blend factor (float)

out vec4 fragColor;

// TODO: implement colormap functions
// Hint: define at least two colormaps (e.g. rainbow and cool-warm)
//   - Rainbow: interpolate between blue -> cyan -> green -> yellow -> red
//     using 4 segments at thresholds 0.25, 0.50, 0.75 with mix()
//   - Cool-warm: interpolate blue -> white -> red
//     using 2 segments at threshold 0.5 with mix()

void main() {
    // TODO: apply colormap to the scalar field texture value
    // Hint: sample the texture to get a scalar value in [0,1],
    //   use it to look up the colormap color, then blend between
    //   grayscale and the mapped color using the blend factor.
    //   When colormap mode is off (0), use the original behavior below.
    //   Make sure overlays (glyphs, streamlines) still work by
    //   setting colormapMode to 0 from C++ before drawing them.

    fragColor = vertexColor + texture(txtr, texCoord);
}
