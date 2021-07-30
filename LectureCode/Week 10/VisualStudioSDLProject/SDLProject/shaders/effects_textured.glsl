
uniform sampler2D diffuse;
varying vec2 texCoordVar;

void main() {
    vec4 color = texture2D(diffuse, texCoordVar);
    //gl_FragColor = vec4(color.r, 0.0f, 0.0f, color.a);  // turns everything textured to reddish
    //gl_FragColor = vec4(0.0f, color.g, 0.0f, color.a);  // turns everything textured to greenish
    //gl_FragColor = vec4(0.0f, 0.0f, color.b, color.a);  // turns everything textured to blueish

    // can divide to make darker, decrease brightness of color
    //gl_FragColor = vec4(0.0f, 0.0f, color.b / 4.0f, color.a);

    // invert color
    //gl_FragColor = vec4(1.0f - color.r, 1.0f - color.g, 1.0f - color.b, color.a);


    /*
    ----------------    Grayscale   ----------------
    */
    // VERSION 1 = using RGB
    //float c = (color.r + color.g + color.b) / 3.0;
    //gl_FragColor = vec4(c, c, c, color.a);

    // VERSION 2 = using luminousity, more realistic grayscale
    //vec3 luminance = vec3(dot(vec3(0.2126,0.7152,0.0722), color.rgb));
    //gl_FragColor = vec4(luminance, color.a);


    /*
    ----------------    Change Saturation   ----------------
    */
    vec3 luminance = vec3(dot(vec3(0.2126,0.7152,0.0722), color.rgb));
    
    // change 1.0f to 2.0f = saturate, 0.5 = desaturate
    vec3 m = mix(luminance, color.rgb, 1.0f);    // normal
    //vec3 m = mix(luminance, color.rgb, 0.5f);   // desaturate
    //vec3 m = mix(luminance, color.rgb, 2.0f);   // saturate

    gl_FragColor = vec4(m, color.a);

}
