uniform sampler2D diffuse;
uniform vec2 lightPosition;

varying vec2 texCoordVar;
varying vec2 varPosition;

float attenuate(float dist, float a, float b) {
    return 1.0 / (1.0 + (a * dist) + (b * dist * dist));
}

void main() {
    float brightness = attenuate(distance(lightPosition, varPosition), 1.0, 0.0);
    vec4 color = texture2D(diffuse, texCoordVar);

    // multiply with a num over 1.0 to make light brighter
    // multiply with a num less than 1.0 to make light darker
    //gl_FragColor = vec4(color.rgb * brightness, color.a);
    //gl_FragColor = vec4(color.rgb * brightness * 1.2f, color.a);
    //gl_FragColor = vec4(color.rgb * brightness * 0.5f, color.a);

    // if want brightness to only affect one color
    gl_FragColor = vec4(color.r * brightness, 0.0f, 0.0f, color.a);     // only red
}