#version 110

//the 5 is arbitrary, lights WILL very quickly add up to computational complexity.
//there might be a hardware limitation for the amount of lights you can have, id recommend staying under 8.
//potentially you could go up to 64 of each light. i suspect itll work, but i cant promise
#define MAX_POINT_LIGHTS 5
#define MAX_SPOT_LIGHTS 5

#define USING_LIGHT_COLORS false

varying vec2 fragTexCoord;
varying vec2 texPosition;
uniform sampler2D palette;
uniform sampler2D texture;

uniform int palette_size;

//point light variables
uniform int pointlight_count;
#if USING_LIGHT_COLORS
uniform vec4 pointlight_color[MAX_POINT_LIGHTS];
#endif

uniform vec2 pointlight_position[MAX_POINT_LIGHTS];
uniform float pointlight_radius[MAX_POINT_LIGHTS];
uniform float pointlight_luminence[MAX_POINT_LIGHTS];
uniform float pointlight_attenuation_constant[MAX_POINT_LIGHTS];
uniform float pointlight_attenuation_linear[MAX_POINT_LIGHTS];
uniform float pointlight_attenuation_quadratic[MAX_POINT_LIGHTS];

//uniform vec2 u_offset[MAX_POINT_LIGHTS]; this wasnt used

//spot light, which is kinda just a triangle i guess
uniform int spotlight_count;
#if USING_LIGHT_COLORS
uniform vec4 spotlight_color[MAX_SPOT_LIGHTS];
#endif

//uniform vec2 u_offset[MAX_SPOT_LIGHTS]; //wasnt used

vec4 source = texture2D(texture, fragTexCoord);

float cross2D(vec2 a, vec2 b) { return a.x * b.y - a.y * b.x; }

vec4 shift(float amount) {
    vec4 pixel = source;
	for (int i = 0; i < palette_size; i++) {
		float fi = float(i);
		vec4 swatch = texture2D(palette, vec2(fi / float(palette_size), 0));
		if (source.rgb == swatch.rgb) { 
            //need something here to clamp this texture pull to the border, if its not already there
            
			pixel = texture2D(palette, vec2((fi + amount) / float(palette_size), 0)); 
		}
	}
    return pixel;
}

vec4 saturateColor(vec4 inputColor, float saturationAmount) {
	float gray = dot(inputColor.rgb, vec3(0.2126, 0.7152, 0.0722)); // Using standard luminance weights
	vec3 saturatedColor = mix(vec3(gray), inputColor.rgb, saturationAmount);
	return vec4(clamp(saturatedColor, 0.0, 1.0), inputColor.a);
}

int CalculatePointLightShift(vec2 point, int light) {
    float lightDistance = length(point - pointlight_position[light]);
    if(lightDistance == 0.0 || lightDistance > pointlight_radius[light]) {
        return 0;
    }
    lightDistance = lightDistance / pointlight_radius[light];
    //vec2 lightDir = point - pointlight_position[light];
    //lightDir = lightDir / lightDistance;

    float attenuation = 1.0 / 
                    (
                        pointlight_attenuation_constant[light] +
                        pointlight_attenuation_linear[light] * lightDistance +
                        pointlight_attenuation_quadratic[light] * lightDistance * lightDistance
                    );
    return int(floor(pointlight_luminence[light] * attenuation));
}

void main() {
	float u_px = float(2.0);
	vec2 pixelPoint = gl_FragCoord.xy;
	pixelPoint = floor(pixelPoint / u_px) * u_px;
    //to_discard was giving me trouble

	int total_amount = 0;
	for(int light = 0; light < pointlight_count; light++){
        //int localShift = CalculatePointLightShift(pixelPoint, light);
        //if(result.x == 1.0){
            //i think theres a better way to calculate dithering, but ill come back to this
            //localShift = clamp(localShift - 1, 0, localShift);
        //}
        total_amount += CalculatePointLightShift(pixelPoint, light);
    }
	gl_FragColor = gl_Color * shift(float(total_amount));
	//gl_FragColor = gl_Color * source;
}