#version 460 core

in vec2 fragmentCoords;

out vec4 color;

uniform dvec4 lbrt;
uniform float colorPeriod;
uniform float maxIt;

uniform sampler1D textureSampler;

void main()
{
	dvec2 lb = lbrt.xy, rt = lbrt.zw;
	dvec2 c  = lb + (rt - lb) * dvec2(fragmentCoords);
	dvec2 z  = dvec2(0, 0);
	dvec2 z2 = dvec2(0, 0);

	float it = 0.0f;
	for (; it < maxIt && z2.x + z2.y <= (1 << 16); it++) {
		z  = dvec2(z2.x - z2.y + c.x, 2 * z.x * z.y + c.y);
		z2 = dvec2(z.x * z.x, z.y * z.y);
	}

	if (it >= maxIt)
	{
		color = vec4(0.0f);
	}
	else
	{
		float log_zn = log(float(z2.x + z2.y)) / 2;
		float nu = log(log_zn / log(2)) / log(2);
		it = it + 1 - nu;
		color = texture(textureSampler, it / colorPeriod);
	}
}
