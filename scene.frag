#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3  lightPos;
uniform vec3  viewPos;
uniform vec3  lightColor;
uniform vec3  ambientColor;
uniform vec3  objectColor;
uniform vec3  spotColor;
uniform sampler2D texture1;
uniform sampler2D reflectionTex;
uniform bool  useTexture;
uniform bool  useSpots;
uniform bool  useScales;
uniform bool  isSand;
uniform bool  isGlass;
uniform vec2  screenSize;
uniform float time;

float hash(vec2 p) { return fract(sin(dot(p, vec2(127.1,311.7)))*43758.5453); }
float smoothNoise(vec2 p) {
    vec2 i=floor(p); vec2 f=fract(p); f=f*f*(3.-2.*f);
    return mix(mix(hash(i),hash(i+vec2(1,0)),f.x),mix(hash(i+vec2(0,1)),hash(i+vec2(1,1)),f.x),f.y);
}

float caustics(vec3 pos) {
    vec2 p = pos.xz*2.0 + vec2(time*0.10, time*0.07);
    return pow(smoothNoise(p)*smoothNoise(p*1.8+vec2(1.3,2.7)), 0.35)*0.6+0.4;
}

float spotPattern(vec2 uv) {
    vec2 cell = fract(uv*vec2(6.0,3.0))-0.5;
    return 1.0 - smoothstep(0.18,0.28,length(cell));
}

float scalePattern(vec2 uv) {
    vec2 sc = uv * vec2(18.0, 10.0);
    sc.x += floor(sc.y) * 0.5;
    vec2 cell = fract(sc) - 0.5;
    float d     = length(cell);
    float edge  = smoothstep(0.35, 0.45, d);
    float shine = smoothstep(0.15, 0.0,  d) * 0.35;
    return -edge * 0.22 + shine;
}

float sandRipple(vec2 uv) {
    float r1 = sin(uv.x * 18.0 + time * 0.30) * 0.5 + 0.5;
    float r2 = sin(uv.y * 14.0 + time * 0.22) * 0.5 + 0.5;
    float r3 = sin((uv.x + uv.y) * 10.0 + time * 0.18) * 0.5 + 0.5;
    float r4 = smoothNoise(uv * 8.0 + vec2(time * 0.08, time * 0.06));
    return (r1 * 0.25 + r2 * 0.20 + r3 * 0.15 + r4 * 0.40);
}

void main() {
    vec3 norm     = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir  = normalize(viewPos  - FragPos);
    vec3 halfDir  = normalize(lightDir + viewDir);

    vec3  ambient = ambientColor * 1.4;
    float diff    = max(dot(norm,lightDir), 0.0);
    vec3  diffuse = diff * lightColor;
    float spec    = pow(max(dot(norm,halfDir),0.0), 64.0);
    vec3  specular= 0.8 * spec * lightColor;

    vec3 baseColor = useTexture ? texture(texture1, TexCoords).rgb : objectColor;

    if (isSand) {
        float ripple = sandRipple(TexCoords);
        baseColor *= (0.82 + ripple * 0.26);
        float streak = smoothNoise(TexCoords * vec2(3.0, 12.0) + vec2(time * 0.05, 0.0));
        baseColor *= (0.88 + streak * 0.18);
        baseColor *= vec3(0.72, 0.64, 0.50);
    }

    if (useSpots) {
        float mask = smoothstep(0.45,0.65,TexCoords.y);
        baseColor  = mix(baseColor, spotColor, spotPattern(TexCoords)*mask*0.85);
    }

    if (useScales) {
        float sc = scalePattern(TexCoords);
        baseColor = baseColor + baseColor * sc;
        float scEdge = smoothstep(0.38,0.48, length(fract(TexCoords*vec2(18.,10.)+vec2(floor(fract(TexCoords.y*10.))*0.5,0.))-0.5));
        specular = specular * (1.0 + (1.0-scEdge)*0.6);
    }

    float caustMask = clamp(1.0-(FragPos.y+1.5)*0.6,0.0,1.0);
    float caustStrength = isSand ? 0.65 : 0.45;
    vec3  caustColor= vec3(0.4,0.75,1.0)*caustics(FragPos)*caustMask*caustStrength;

    vec3 result = (ambient+diffuse+specular)*baseColor + caustColor;

    float alpha = 1.0f;
    if (isGlass) {
        float fresnel = pow(1.0 - abs(dot(norm, viewDir)), 3.0);
        fresnel = 0.05 + 0.85 * fresnel;

        vec2 scrUV = gl_FragCoord.xy / screenSize;

        vec2 reflUV;
        if (abs(norm.x) > 0.5) {
            reflUV = vec2(1.0 - scrUV.x, scrUV.y);
        } else if (abs(norm.z) > 0.5) {
            reflUV = vec2(scrUV.x, 1.0 - scrUV.y);
        } else {
            reflUV = vec2(scrUV.x, 1.0 - scrUV.y);
        }
        reflUV += norm.xz * 0.02 * fresnel;
        reflUV  = clamp(reflUV, 0.001, 0.999);

        vec3 reflScene = texture(reflectionTex, reflUV).rgb;

        float spec = pow(max(dot(norm, halfDir), 0.0), 128.0);
        vec3 reflColor = vec3(0.85, 0.95, 1.0) * spec * 2.0;

        float reflStrength, alphaMax;
        if (abs(norm.y) > 0.5) {
            reflStrength = 0.20;
            alphaMax     = 0.25;
        } else {
            reflStrength = 0.70;
            alphaMax     = 0.70;
        }

        result = mix(result * 0.05, reflScene, fresnel * reflStrength);
        result += reflColor * (abs(norm.y) > 0.5 ? 0.3 : 0.8);
        result += objectColor * fresnel * 0.15;

        alpha = mix(0.10, alphaMax, fresnel);
    }

    float fog = clamp(exp(-length(viewPos-FragPos)*0.038),0.0,1.0);
    result = mix(vec3(0.0,0.06,0.18), result, fog);

    result = mix(result, result*vec3(0.82,0.93,1.0), 0.25);

    FragColor = vec4(result, alpha);
}
