uniform sampler2D backGround;
uniform vec3 iResolution;
uniform float iTime;

//gl_Color is send by the vertexBuffer
void main()
{
     // Sawtooth calc of time
    float offset = (iTime - floor(iTime)) / iTime;
	float time = iTime * offset;

    // Wave design params
	vec3 waveParams = vec3(10.0, 0.8, 0.1 );
    
    // Find coordinate, flexible to different resolutions
    vec2 uv = gl_TexCoord[0].xy / iResolution.xy;
    
    // Find center, flexible to different resolutions
    vec2 center = iResolution.xy  / iResolution.x / 2.;

    // Distance to the center
    float dist = distance(uv, center);
    
    // Original color
	vec4 c = texture(backGround, uv);
    
    // Limit to waves
	if (time > 0. && dist <= time + waveParams.z && dist >= time - waveParams.z) {
        // The pixel offset distance based on the input parameters
		float diff = (dist - time);
		float diffPow = (1.0 - pow(abs(diff * waveParams.x), waveParams.y));
		float diffTime = (diff  * diffPow);

        // The direction of the distortion
		vec2 dir = normalize(uv - center);
        
        // Perform the distortion and reduce the effect over time
		uv += ((dir * diffTime) / (time * dist * 80.0));
        
        // Grab color for the new coord
		c = texture(backGround, uv);

        // Optionally: Blow out the color for brighter-energy origin
        //c += (c * diffPow) / (time * dist * 40.0);
	}
   
    gl_FragColor = c;
}