#version 330 core 
 
// Defines an output variable to return the pixel's color //
out vec4 FragColor;

// Get the aspect ratio of the window from the CPU //
uniform float uWidth;
uniform float uHeight;
uniform float uAspectRatio;

// Also take some camera settings from the CPU //
uniform vec3 uCameraPosition;
uniform vec3 uCameraRotation;

// Calculate the ray's origin and direction from the screen dimensions and camera settings //
vec3 RayOrigin = uCameraPosition;
vec3 RayDirection = vec3(0) + vec3(0) + 1 * uCameraDirection;

void main() {
	FragColor = vec4(gl_FragCoord.x / uWidth, gl_FragCoord.y / uHeight, uAspectRatio, 1.0);
}