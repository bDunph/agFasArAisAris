#version 410

in vec4 nearPos;
in vec4 farPos;
//in vec2 texCoordsOut;

out vec4 fragColor;
	
// raymarch basic setup adapted from dila's tutorial
// https://www.youtube.com/watch?v=yxNnRSefK94

#define Iterations 32
#define MAX_ITERATIONS 100
#define Scale 2.0
#define Offset 3.0
#define NUM_NOISE_OCTAVES 5
#define SUN_DIR vec3(0.5, 0.8, 0.0)
#define EPSILON 0.01

int index;
vec4 orbit;

// function from http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

float DE(vec3 p)
{
    
    mat3 rot = rotationMatrix(vec3(0.5, 1.0, 0.0), 45.0);
    
 	// sierpinski fractal from http://blog.hvidtfeldts.net/index.php/2011/08/distance-estimated-3d-fractals-iii-folding-space/
    
    orbit = vec4(1000.0);
    
    int n = 0;
    while(n < Iterations)
    {
    
        p = rot * p;
        
        if(p.x + p.y < 0.0) p.xy = -p.yx; // fold 1
        if(p.x + p.z < 0.0) p.xz = -p.zx; // fold 2
        if(p.y + p.z < 0.0) p.zy = -p.yz; // fold 3
        
        p = rot * p;
        
        p = p * Scale - Offset * (Scale - 1.0);
        
        float orbPoint = dot(p, p);
        orbit = min(orbit, vec4(abs(p), orbPoint));
        
        if(length(p) > float(MAX_ITERATIONS)) break;
        
        n++;
    }
    
    return length(p) * pow(Scale, -float(n));
}

float march(vec3 o, vec3 r)
{
 	float t = 0.0;
    int ind = 0;
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
     	vec3 p = o + r * t;
        float d = DE(p);
        if(d < EPSILON) break;
        t += d * 0.5;
        ind++;
    }
    
    index = ind;
    return t;
}

// finite difference normal from 
// http://blog.hvidtfeldts.net/index.php/2011/08/distance-estimated-3d-fractals-ii-lighting-and-coloring/
vec3 norm(vec3 pos, vec3 dir)
{
	return normalize(vec3(	DE(pos + vec3(EPSILON, 0.0, 0.0)) - DE(pos - vec3(EPSILON, 0.0, 0.0)),
                			DE(pos + vec3(0.0, EPSILON, 0.0)) - DE(pos - vec3(0.0, EPSILON, 0.0)),
                			DE(pos + vec3(0.0, 0.0, EPSILON)) - DE(pos - vec3(0.0, 0.0, EPSILON))));
}

// hash, noise and fbm implementations from morgan3d
// https://www.shadertoy.com/view/4dS3Wd
// By Morgan McGuire @morgan3d, http://graphicscodex.com
// Reuse permitted under the BSD license.

float hash(float p) { p = fract(p * 0.011); p *= p + 7.5; p *= p + p; return fract(p); }

float noise(vec3 x) {
    const vec3 step = vec3(110, 241, 171);

    vec3 i = floor(x);
    vec3 f = fract(x);
 
    // For performance, compute the base input to a 1D hash from the integer part of the argument and the 
    // incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);

    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

float fbm(vec3 x) {
	float v = 0.0;
	float a = 0.5;
	vec3 shift = vec3(100);
	for (int i = 0; i < NUM_NOISE_OCTAVES; ++i) {
		v += a * noise(x);
		x = x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

// ambient occlusion implementation from 
// http://www.pouet.net/topic.php?which=7931&page=1&x=3&y=14

float ao(vec3 p, vec3 n, float d, float i) {
	float o;
	for (o=1.;i>0.;i--) {
		o-=(i*d-abs(DE(p+n*i*d)))/pow(2.,i);
	}
	return o;
}

void main()
{

	//************* ray setup code from 
	//https://encreative.blogspot.com/2019/05/computing-ray-origin-and-direction-from.html*/
	
	
	//******* Perform raymarch *********************//
	vec3 rayOrigin = nearPos.xyz / nearPos.w;
	vec3 rayEnd = farPos.xyz / farPos.w;
	vec3 rayDir = rayEnd - rayOrigin;
	rayDir = normalize(rayDir);	
	
	// raymarch the point
	float dist = march(rayOrigin, rayDir);
	    
	vec3 pos = rayOrigin + dist * rayDir;
	    
	// colouring and shading
	vec3 norm = norm(pos, rayDir);
	    
	// material colour
	float sq = float(Iterations) * float(Iterations);
	float smootherVal = float(index) + log(log(sq)) / log(Scale) - log(log(dot(pos, pos))) / log(Scale);
	vec3 matCol1 = vec3(pow(0.25, log(smootherVal)), pow(0.38, log(smootherVal)), pow(0.08, log(smootherVal)));
	vec3 matCol2 = vec3(pow(0.15, 1.0 / log(smootherVal)), pow(0.15, 1.0 / log(smootherVal)), pow(0.34, 1.0 / log(smootherVal)));
	vec3 matCol3 = vec3(pow(smootherVal, 0.15), pow(smootherVal, 0.15), pow(1.0 / smootherVal, 0.84));
	vec3 totMatCol = mix(matCol1, matCol2, clamp(6.0*orbit.y, 0.0, 1.0));
	totMatCol = mix(totMatCol, matCol3, pow(clamp(1.0 - 2.0 * orbit.z, 0.0, 1.0), 8.0));
	    
	// lighting
	float ao = ao(pos, norm, 0.5, 5.0);
	float sun = clamp(dot(norm, SUN_DIR), 0.0, 1.0);
	float sky = clamp(0.5 + 0.5 * norm.y, 0.0, 1.0);
	float ind = clamp(dot(norm, normalize(SUN_DIR * vec3(-1.0, 0.0, -1.0))), 0.0, 1.0);
	    
	vec3 lightRig = sun * vec3(1.64, 1.27, 0.99);
	lightRig += sky * vec3(0.16, 0.2, 0.28) * ao;
	lightRig += ind * vec3(0.4, 0.28, 0.2) * ao;
	    
	vec3 colour = totMatCol * lightRig;
	    
	float fog = 1.0 / (1.0 + dist * dist * 0.5);
	    
	//colour = pow(colour, vec3(fog));
	colour *= fog;
	    
	// gamma corr
	colour = pow(colour, vec3(1.0/2.2));
	
	// Output to screen
	fragColor = vec4(colour,1.0);
}
