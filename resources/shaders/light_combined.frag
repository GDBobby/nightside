#version 110

//the 5 is arbitrary, lights WILL very quickly add up to computational complexity.
//there might be a hardware limitation for the amount of lights you can have, id recommend staying under 8.
//potentially you could go up to 64 of each light. i suspect itll work, but i cant promise
#define MAX_POINT_LIGHTS 5
#define MAX_SPOT_LIGHTS 5

varying vec2 fragTexCoord;
varying vec2 texPosition;
uniform sampler2D palette;
uniform sampler2D texture;

uniform int palette_size;

//point light variables
uniform int pointlight_count;
uniform float pointlight_key[MAX_POINT_LIGHTS]; //this is always 0 in the source
uniform vec2 pointlight_position[MAX_POINT_LIGHTS];
uniform int pointlight_luminosity[MAX_POINT_LIGHTS];
uniform float pointlight_radius[MAX_POINT_LIGHTS];
//uniform vec2 u_offset[MAX_POINT_LIGHTS]; this wasnt used

//spot light, which is kinda just a triangle i guess
uniform int spotlight_count;
uniform float spotlight_key[MAX_SPOT_LIGHTS];
uniform vec2 spotlight_vertex0[MAX_SPOT_LIGHTS];
uniform vec2 spotlight_vertex1[MAX_SPOT_LIGHTS];
uniform vec2 spotlight_vertex2[MAX_SPOT_LIGHTS];
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


bool isPointInConvexPolygon(vec2 point, int light) {

    vec2 edge = spotlight_vertex1[light] - spotlight_vertex0[light];
    vec2 toPoint = point - spotlight_vertex0[light];
    if(cross2D(edge, toPoint) < 0.0){
        return false;
    }

    edge = spotlight_vertex2[light] - spotlight_vertex1[light];
    toPoint = point - spotlight_vertex1[light];
    if(cross2D(edge, toPoint) < 0.0){
        return false;
    }

    edge = spotlight_vertex0[light] - spotlight_vertex2[light];
    toPoint = point - spotlight_vertex2[light];
    if(cross2D(edge, toPoint) < 0.0){
        return false;
    }

    return true;
}

vec4 saturateColor(vec4 inputColor, float saturationAmount) {
	float gray = dot(inputColor.rgb, vec3(0.2126, 0.7152, 0.0722)); // Using standard luminance weights
	vec3 saturatedColor = mix(vec3(gray), inputColor.rgb, saturationAmount);
	return vec4(clamp(saturatedColor, 0.0, 1.0), inputColor.a);
}

//im pretty sure this can be simplified
vec2 isPointInRadius(vec2 point, float dist, vec2 position, float radius) {
	float inside = 1.0;
	float dither = 0.0;
	float currdist = radius * dist * dist;
	float dither_threshold = currdist * 0.4;
	vec2 diff = vec2(position - point);
	if (length(diff) > (currdist + dither_threshold)) {
		inside = 0.0;
	} 
    else if (length(diff) > currdist) {
		float t = mod(point.x, 4.0);
		float u = mod(point.y, 4.0);
		if ((t == 0.0 && u == 2.0) || (t == 2.0 && u == 0.0)) { 
            dither = 1.0; 
        }
	}

	return vec2(inside, dither);
}

void main() {
	float u_px = float(2.0);
	vec2 pixelPoint = gl_FragCoord.xy;
	pixelPoint = floor(pixelPoint / u_px) * u_px;
	bool to_discard = true;
	//if to_discard would be the same light to light, but it inside the light for loop

	float total_amount = 0.0;
	for(int light = 0; light < pointlight_count; light++){
        for (int i = pointlight_luminosity[light]; i >= 0; i--) {


            float fi = float(i);
            vec2 result = isPointInRadius(pixelPoint, fi, pointlight_position[light], pointlight_radius[light]);
            if (result.x == 1.0) {
                float dither = fi + result.y;
                total_amount += pointlight_key[light] - dither;
                to_discard = false;
            }
        }
    }
	if (to_discard) { discard; }



    for(int light = 0; light < spotlight_count; light++){
        if (isPointInConvexPolygon(pixelPoint, light)) { 
            total_amount += spotlight_key[light];
/* i dont really understand what this does, ill need to understand it so i can reimplement it
            if(!isPointInConvexPolygon(pixelPoint + 2.0, light)){
                pixel = saturateColor(pixel, 1.5); 
            }
            
            if (!isPointInConvexPolygon(pixelPoint + 8.0, light)) { 
                pixel = saturateColor(pixel, 1.25); 
            }
*/
        }
    }
	gl_FragColor = gl_Color * shift(total_amount);
	//gl_FragColor = gl_Color * pixel;
}