#version 330 core
    out vec4 fragColor;
	in vec3 ourColor;
    in vec2 TexCoord;

    uniform float mixVal;

    uniform sampler2D texture1;
    uniform sampler2D texture2;

    void main()
    {
        vec2 hCoord = vec2(1.0-TexCoord.x, TexCoord.y);
        vec4 happy = texture(texture2, hCoord);
        fragColor = mix(texture(texture1, TexCoord), happy, mixVal);
    //    fragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
    }
