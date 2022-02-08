#version 410  
// raymarch basic setup adapted from dila's tutorial
// https://www.youtube.com/watch?v=yxNnRSefK94

#define ITERATIONS 10 
#define MAX_ITERATIONS 35 
#define FOLD_CUTOFF 50
#define SCALE 2.0
#define OFFSET 2.0
//#define NUM_NOISE_OCTAVES 2
#define SUN_DIR vec3(0.5, 0.8, 0.0)
#define EPSILON 0.01
#define NUM_FFT_BINS 512
#define PLANE_NORMAL vec4(0.0, 1.0, 0.0, 0.0)
#define SPHERE_RAD 10.0
#define FACTOR 5.0

uniform mat4 MVEPMat;
//uniform float specCentVal;
//uniform float timeVal;
//uniform float rmsModVal;

uniform vec2 dispRes;
uniform float time;
uniform float fbmAmp;
uniform float fbmSpeed;

in vec4 nearPos;
in vec4 farPos;
//in vec2 texCoordsOut;

layout(location = 0) out vec4 fragColor; 
layout(location = 1) out vec4 indexOut;

int index;
vec4 orbit;

float fbmVal;

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

//----------------------------------------------------------------------------------------
// Sphere SDF from https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
//----------------------------------------------------------------------------------------
float sphereSDF(vec3 p, float radius)
{
	return abs(length(p) - radius);
}

//----------------------------------------------------------------------------------------
// Ground plane SDF from https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
//----------------------------------------------------------------------------------------

float planeSDF(vec3 p, vec4 normal)
{
	int n = 0;
	while(n < ITERATIONS)
    {
	if(p.x + p.y < 0.0) p.xy = -p.yx; // fold 1
        if(p.x + p.z < 0.0) p.xz = -p.zx; // fold 2
        if(p.y + p.z < 0.0) p.zy = -p.yz; // fold 3

        p = p * SCALE - OFFSET * (SCALE - 1.0);
        
        if(length(p) > float(FOLD_CUTOFF)) break;
        
        n++;
    }

	return dot(p, normal.xyz) + normal.w;
}

float kifSDF(vec3 p)
{
	mat3 rot = rotationMatrix(vec3(0.5, 1.0, 0.0), fbmVal);

 	// sierpinski fractal from http://blog.hvidtfeldts.net/index.php/2011/08/distance-estimated-3d-fractals-iii-folding-space/
    
    orbit = vec4(10.0, 10.0, 10.0, 1.0);
    
    int n = 0;
    while(n < ITERATIONS)
    {
    
        p = rot * p;
        
        if(p.x + p.y < 0.0) p.xy = -p.yx; // fold 1
        if(p.x + p.z < 0.0) p.xz = -p.zx; // fold 2
        if(p.y + p.z < 0.0) p.zy = -p.yz; // fold 3
        
        p = rot * p;
        
        p = p * SCALE - OFFSET * (SCALE - 1.0);
        
        orbit = min(orbit, vec4(abs(p), 1.0));
        
        if(length(p) > float(FOLD_CUTOFF)) break;
        
        n++;
    }
    return length(p) * pow(SCALE, -float(n));
}


float recVal = 0.0;

float DE(vec3 p)
{
	float rad = SPHERE_RAD + recVal;

	//float specFactor = sin(specCentVal) * 0.25; 
	float specFactor = sin(0.43) * 0.25; 
  	float specDisp = sin(specFactor * p.x) * sin(specFactor * p.y) * sin(specFactor * p.z);

	//float ampDisp = sin(rmsModVal * 5.0);
	float ampDisp = sin(5.0);
	
	float sphereDist = sphereSDF(p + ampDisp + specDisp, rad);

	if(length(p) > rad) 
	{
		recVal += FACTOR;
		rad += recVal;
		sphereDist = sphereSDF(p + ampDisp + specDisp, rad);
	}	

	float kifDist = kifSDF(p);
	float planeDist = planeSDF(p + specDisp, PLANE_NORMAL);

	return min(kifDist, min(sphereDist, planeDist));
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
vec3 norm(vec3 pos)
{
	vec3 xDir = vec3(EPSILON, 0.0, 0.0);
	vec3 yDir = vec3(0.0, EPSILON, 0.0);
	vec3 zDir = vec3(0.0, 0.0, EPSILON);

	return normalize(vec3(	DE(pos + xDir) - DE(pos - xDir),
                		DE(pos + yDir) - DE(pos - yDir),
                		DE(pos + zDir) - DE(pos - zDir)));
}

// ambient occlusion implementation from 
// http://www.pouet.net/topic.php?which=7931&page=1&x=3&y=14
float ao(vec3 p, vec3 n, float d, float i) 
{
	float o;
	for (o=1.;i>0.;i--) {
		o-=(i*d-abs(DE(p+n*i*d)))/pow(2.,i);
	}
	return o;
}

//----------------------------------------------------------------------------------------
// Fog based on https://www.iquilezles.org/www/articles/fog/fog.htm
//----------------------------------------------------------------------------------------
vec3 fog(in vec3 col, in float dist, in vec3 rayDir, in vec3 lightDir)
{
	float fogAmount = 1.0 - exp(-dist * 0.1);

	vec3 normLightDir = normalize(lightDir);
	float lightAmount = max(dot(rayDir, normLightDir), 0.0);
	vec3 fogColour = mix(vec3(0.1, 0.12, 0.14), vec3(0.2, 0.18, 0.14), pow(lightAmount, 8.0));

	vec3 bExt = vec3(0.05, 0.03, 0.06);
	vec3 bIns = vec3(0.12, 0.05, 0.05);

	vec3 extCol = vec3(exp(-dist * bExt.x), exp(-dist * bExt.y), exp(-dist * bExt.z));
	vec3 insCol = vec3(exp(-dist * bIns.x), exp(-dist * bIns.y), exp(-dist * bIns.z));

	col += vec3(col * (vec3(1.0) - extCol) + fogColour * (vec3(1.0) - insCol));

	return mix(col, fogColour, fogAmount);
}

//------------------------------------------------------------------------------------------
// fBM implementation from Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
//------------------------------------------------------------------------------------------
#define NUM_NOISE_OCTAVES 5 

// Precision-adjusted variations of https://www.shadertoy.com/view/4djSRW
float hash(float p) { p = fract(p * 0.011); p *= p + 7.5; p *= p + p; return fract(p); }
float hash(vec2 p) {vec3 p3 = fract(vec3(p.xyx) * 0.13); p3 += dot(p3, p3.yzx + 3.333); return fract((p3.x + p3.y) * p3.z); }

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
	float a = fbmAmp;
	vec3 shift = vec3(100);
	for (int i = 0; i < NUM_NOISE_OCTAVES; ++i) {
		v += a * noise(x);
		x = x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

void main()
{

	//************* calculate fBM *******************************//
	// st calculation taken from https://thebookofshaders.com/13/ 
	// Author @patriciogv - 2015
	// http:patriciogonzalezvivo.com 

	vec2 st = gl_FragCoord.xy / dispRes.xy;
	st.x *= dispRes.x / dispRes.y;

	vec3 animFBM = vec3(st.x, st.y, time * fbmSpeed);
	fbmVal = fbm(animFBM);
	
	//************* ray setup code from **************************//
	//https://encreative.blogspot.com/2019/05/computing-ray-origin-and-direction-from.html*/
	
	
	//******* Perform raymarch *********************//
	vec3 rayOrigin = nearPos.xyz / nearPos.w;
	vec3 rayEnd = farPos.xyz / farPos.w;
	vec3 rayDir = rayEnd - rayOrigin;
	rayDir = normalize(rayDir);	
	
	// raymarch the point
	float dist = march(rayOrigin, rayDir);
	vec3 pos = rayOrigin + dist * rayDir;		    

	// material colour
	vec3 colour = vec3(0.0);
	vec3 totMatCol = vec3(0.0);

	if(index < MAX_ITERATIONS)
	{
		// material colour
		float sq = float(ITERATIONS) * float(ITERATIONS);
		float smootherVal = float(index) + log(log(sq)) / log(SCALE) - log(log(dot(pos, pos))) / log(SCALE);
		vec3 matCol1 = vec3(pow(0.392, log(smootherVal)), pow(0.19, log(smootherVal)), pow(0.04, log(smootherVal)));
		vec3 matCol2 = vec3(pow(0.333, 1.0 / log(smootherVal)), pow(0.417, 1.0 / log(smootherVal)), pow(0.184, 1.0 / log(smootherVal)));
		totMatCol = mix(matCol1, matCol2, clamp(6.0*orbit.x, 0.0, 1.0));

		// lighting
		vec3 norm = norm(pos);

		float ambOcc = ao(pos, norm, 0.5, 5.0);
		float sun = clamp(dot(norm, SUN_DIR), 0.0, 1.0);
		float sky = clamp(0.5 + 0.5 * norm.y, 0.0, 1.0);
		float ind = clamp(dot(norm, normalize(SUN_DIR * vec3(-1.0, -1.0, 0.0))), 0.0, 1.0);
		    
		vec3 lightRig = sun * vec3(1.64, 1.27, 0.99);
		lightRig += sky * vec3(0.32, 0.4, 0.56) * ambOcc;
		lightRig += ind * vec3(0.4, 0.28, 0.2) * ambOcc;
		    
		colour = totMatCol * lightRig;
	}
	else
	{
		colour = vec3(0.16, 0.2, 0.28);
	}
	
	colour = fog(colour, dist, rayDir, SUN_DIR);

	// gamma corr
	colour = pow(colour, vec3(1.0/2.2));
	
	// Output to screen
	fragColor = vec4(colour,1.0);

	// Output to PBO
	// convert index from range 0 - 100 to 0 - 1
	//float indOut = float(index) / 100.0;
	//indexOut = vec4(indOut);

//-----------------------------------------------------------------------------
// To calculate depth for use with rasterized material
//-----------------------------------------------------------------------------
	vec4 pClipSpace =  MVEPMat * vec4(pos, 1.0);
	vec3 pNdc = vec3(pClipSpace.x / pClipSpace.w, pClipSpace.y / pClipSpace.w, pClipSpace.z / pClipSpace.w);
	float ndcDepth = pNdc.z;
	
	float d = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0; 
	gl_FragDepth = d;

}
