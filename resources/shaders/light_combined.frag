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

//spot light variables
uniform int spotlight_count;
#if USING_LIGHT_COLORS
uniform vec4 spotlight_color[MAX_SPOT_LIGHTS];
#endif
uniform vec2 spotlight_position[MAX_SPOT_LIGHTS];
uniform vec2 spotlight_direction[MAX_SPOT_LIGHTS];
uniform float spotlight_radius[MAX_SPOT_LIGHTS];
uniform float spotlight_luminence[MAX_SPOT_LIGHTS];
uniform float spotlight_attenuation_constant[MAX_SPOT_LIGHTS];
uniform float spotlight_attenuation_linear[MAX_SPOT_LIGHTS];
uniform float spotlight_attenuation_quadratic[MAX_SPOT_LIGHTS];
uniform float spotlight_cutoff[MAX_SPOT_LIGHTS];
uniform float spotlight_outerCutoff[MAX_SPOT_LIGHTS];


// **** end of uniform variables

vec4 source = texture2D(texture, fragTexCoord);

float cross2D(vec2 a, vec2 b) { return a.x * b.y - a.y * b.x; }

vec4 shift(float amount) {
    vec4 pixel = source;

	for (int i = 0; i < palette_size; i++) {
		float fi = float(i);
		vec4 swatch = texture2D(palette, vec2(fi / float(palette_size), 0));
		if (source.rgb == swatch.rgb) { 
            //need something here to clamp this texture pull to the border, if its not already there
            fi = clamp((fi + amount) / float(palette_size), 0.0, 1.0);
			pixel = texture2D(palette, vec2(fi, 0)); 
		}
	}
    return pixel;
}

vec4 saturateColor(vec4 inputColor, float saturationAmount) {
	float gray = dot(inputColor.rgb, vec3(0.2126, 0.7152, 0.0722)); // Using standard luminance weights
	vec3 saturatedColor = mix(vec3(gray), inputColor.rgb, saturationAmount);
	return vec4(clamp(saturatedColor, 0.0, 1.0), inputColor.a);
}

float CalculatePointLightShift(vec2 point, int light) {
    float lightDistance = length(point - pointlight_position[light]);
    if(lightDistance == 0.0 || lightDistance > pointlight_radius[light]) {
        return 0.0;
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
    return pointlight_luminence[light] * attenuation;
}

float CalculateSpotLightShift(vec2 point, int light) {
    vec2 lightDirection = point - spotlight_position[light];
    float lightDistance = length(lightDirection);

    if(lightDistance == 0.0 || lightDistance > spotlight_radius[light]) {
        return 0.0;
    }
    lightDirection = normalize(lightDirection); //normalized
    float spotDot = dot(lightDirection, -normalize(spotlight_direction[light]));
    if(spotDot < spotlight_outerCutoff[light]){
        return 0.0;
    }
    float spotValue = smoothstep(spotlight_outerCutoff[light], spotlight_cutoff[light], spotDot);
    //float spotAttenuation = pow(spotValue, spotlight_exponent[light]); // i need to dick with this to see what it does

    lightDistance = lightDistance / spotlight_radius[light];

    float attenuation = 1.0 / 
                    (
                        spotlight_attenuation_constant[light] +
                        spotlight_attenuation_linear[light] * lightDistance +
                        spotlight_attenuation_quadratic[light] * lightDistance * lightDistance
                    );
    return spotlight_luminence[light] * attenuation * spotValue;
}

void main() {
	float u_px = float(2.0);
	vec2 pixelPoint = gl_FragCoord.xy;
	pixelPoint = floor(pixelPoint / u_px) * u_px;
    //to_discard was giving me trouble

	float total_amount = 0.0;
	for(int light = 0; light < pointlight_count; light++){
        //int localShift = CalculatePointLightShift(pixelPoint, light);
        //if(result.x == 1.0){
            //i think theres a better way to calculate dithering, but ill come back to this
            //localShift = clamp(localShift - 1, 0, localShift);
        //}
        total_amount += CalculatePointLightShift(pixelPoint, light);
    }
    for(int light = 0; light < spotlight_count; light++){
        total_amount += CalculateSpotLightShift(pixelPoint, light);
    }


    
    float fraction = fract(total_amount);
    total_amount = total_amount - fraction;
    
    if(total_amount >= 1.0){
    if(fraction < 0.25){
        if((mod(pixelPoint.x, 2.0) == 0.0) && (mod(pixelPoint.y, 2.0) == 0.0)){
            total_amount -= 1.0;
        }
    }
    else if(fraction < 0.5){
        if((mod(pixelPoint.x, 4.0) == 0.0) && (mod(pixelPoint.y, 4.0) == 0.0)){
            total_amount -= 1.0;
        }
    }
    else {
        if((mod(pixelPoint.x, 8.0) == 0.0) && (mod(pixelPoint.y, 8.0) == 0.0)){
            total_amount -= 1.0;
        }
    }
    }


	gl_FragColor = gl_Color * shift(total_amount);
	//gl_FragColor = gl_Color * source;
}