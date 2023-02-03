#version 460 core

in vec2 fragmentCoords;

out vec3 color;

uniform dvec4 lbrt;

double dist(dvec2 a, dvec2 b)
{
	dvec2 c = a - b;
	return dot(c, c);
}

void main()
{
	dvec2 lb = lbrt.xy, rt = lbrt.zw;
	dvec2 c = lb + (rt - lb) * dvec2(fragmentCoords);
	dvec2 z = dvec2(0, 0);
	dvec2 o = dvec2(0, 0);

	float it = 0.0f;
	float itMax = 1000.0f;
	for (; it < itMax && dist(o, z) <= 4.0; it++)
	{
		z = dvec2(z.x * z.x - z.y * z.y + c.x, z.x * z.y + z.y * z.x + c.y);
	}

	float gray = sin(it / 10.0f) / 2.0f + 0.5f;
	color = vec3(gray);
}
