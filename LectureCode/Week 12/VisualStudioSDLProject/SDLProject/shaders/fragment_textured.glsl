
uniform sampler2D diffuse;
varying vec2 texCoordVar;

void main() {
    gl_FragColor = texture2D(diffuse, texCoordVar);

    // if is transparent, discard the pixel
    if (gl_FragColor.a == 0.0) {
        discard;
    }
}
