// sub1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

struct v3f
{
	float x, y, z;
};

typedef struct v3f v3f;

struct v4f
{
	float x, y, z, w;
};

typedef struct v4f v4f;

#define NX	10

v4f gp1[NX][NX][NX];
float gv1[NX-1][NX-1][NX-1][6];
v3f gs1;
float gsv1;
v4f gp2[NX][NX][NX];
float gv2[NX - 1][NX - 1][NX - 1][6];
v3f gs2;
float gsv2;


void v3fmul(v3f *v, v3f f, float s)
{
	v->x = f.x * s;
	v->y = f.y * s;
	v->z = f.z * s;
}

void v4fmul(v4f *v, v4f f, float s)
{
	v->x = f.x * s;
	v->y = f.y * s;
	v->z = f.z * s;
	v->w = f.w * s;
}

void v3fsub(v3f *v, v3f f, v3f s)
{
	v->x = f.x - s.x;
	v->y = f.y - s.y;
	v->z = f.z - s.z;
}

void v4fsub(v4f *v, v4f f, v4f s)
{
	v->x = f.x - s.x;
	v->y = f.y - s.y;
	v->z = f.z - s.z;
	v->w = f.w - s.w;
}

void v3fadd(v3f *v, v3f f, v3f s)
{
	v->x = f.x + s.x;
	v->y = f.y + s.y;
	v->z = f.z + s.z;
}

void v4fadd(v4f *v, v4f f, v4f s)
{
	v->x = f.x + s.x;
	v->y = f.y + s.y;
	v->z = f.z + s.z;
	v->w = f.w + s.w;
}

float mag3f(v3f v)
{
	return (float)sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

float mag4f(v4f v)
{
	return (float)sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
}

float dot3f(v3f v1, v3f v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

float dot4f(v4f v1, v4f v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w));
}

v3f cross3f(v3f v1, v3f v2)
{
	v3f n;

	n.x = ((v1.y * v2.z) - (v1.z * v2.y));
	n.y = ((v1.z * v2.x) - (v1.x * v2.z));
	n.z = ((v1.x * v2.y) - (v1.y * v2.x));

	return n;
}

float vol3f2(v3f a, v3f b, v3f c)
{
	float f;
	b = cross3f(b, c);
	f = dot3f(a, b);
	f = fabs(f);
	f *= 1.0 / (2.0 * 3.0);
	return f;
}

float vol3f(v3f a, v3f b, v3f c, v3f d)
{
	v3fsub(&a, a, d);
	v3fsub(&b, b, d);
	v3fsub(&c, c, d);
	return vol3f2(a, b, c);
}

float sa3f(v3f a, v3f b, v3f c)
{
	v3f ab;
	v3f ac;
	float f;
	v3fsub(&ab, a, b);
	v3fsub(&ac, a, c);
	f = mag3f(ab);
	return 0.5 * f * (1.0 - dot3f(ab, ac) / f)*f;
}

float vol4f2(float s1, float s2, float s3, float s4, float a1, float a2, float a3, float a4, float a5, float a6)
{
	return sqrtf(s1*(s1 - a1)*(s1 - a2)*(s1 - a4)) +
		sqrtf(s2*(s2 - a2)*(s2 - a3)*(s2 - a5)) +
		sqrtf(s3*(s3 - a3)*(s3 - a6)*(s3 - a1)) +
		sqrtf(s4*(s4 - a4)*(s4 - a5)*(s4 - a6));
}

float vol4f(v4f a, v4f b, v4f c, v4f d)
{
	float s1, s2, s3, s4,
		a1, a2, a3, a4, a5, a6;
	v4f ab, ac, ad, bc, bd, cd;

	v4fsub(&ab, a, b);
	v4fsub(&ac, a, c);
	v4fsub(&ad, a, d);
	v4fsub(&bc, b, c);
	v4fsub(&bd, b, d);
	v4fsub(&cd, c, d);

	a1 = mag4f(ab);
	a2 = mag4f(ac);
	a3 = mag4f(ad);
	a4 = mag4f(bc);
	a5 = mag4f(bd);
	a6 = mag4f(cd);

	s1 = (a1 + a2 + a4) / 2.0f;
	s1 = (a2 + a3 + a5) / 2.0f;
	s1 = (a3 + a6 + a1) / 2.0f;
	s1 = (a4 + a5 + a6) / 2.0f;

	return vol4f2(s1, s2, s3, s4, a1, a2, a3, a4, a5, a6);
}

void topl(v4f n, float d, v4f v, v4f *r)
{
	float dot = dot4f(n, v);
	v4fmul(r, n, dot);
	v4fsub(r, v, *r);
}

void tobr(v4f v, v3f *r)
{
	int cx = -1, cy = -1, cz = -1;
	float cd = -1;
	int x, y, z;
	float d;

	for (x = 0; x < NX - 1; ++x)
	{
		for (y = 0; y < NX - 1; ++y)
		{
			for (z = 0; z < NX - 1; ++z)
			{

			}
		}
	}
}

v4f cvto4(v3f a)
{
	v4f r;
	return r;
}

void plat(v3f v, v4f *n, float *d)
{
}

float shortdist(v3f a, v3f b)
{
	float d = 0;
	float dc;
	v3f from3, to3;
	v4f from4, to4;
	v3f mid3;
	v4f mid4;
	v3f move3;
	v4f move4;
	v4f pln;
	float pld;

	from3 = b;
	from4 = cvto4(from3);
	to3 = a;
	to4 = cvto4(to3);

	while (1)
	{
		v4fadd(&mid4, from4, to4);
		v4fmul(&mid4, mid4, 0.5f);
		plat(from3, &pln, &pld);
		topl(pln, pld, mid4, &mid4);
		v4fsub(&move4, mid4, from4);
		v4fmul(&move4, move4, 0.1f / mag4f(move4));
		v4fadd(&mid4, move4, from4);
		tobr(mid4, &mid3);
		v3fsub(&move3, mid3, from3);
		dc = mag4f(move4);
		from3 = mid3;
		from4 = mid4;
		v4fsub(&move4, from4, to4);
		d += dc;
		if (mag4f(move4) < 1.0f)
			break;
	}

	return d;
}

void init()
{
	int x, y, z;

	for (x = 0; x < NX; ++x)
	{
		for (y = 0; y < NX; ++y)
		{
			for (z = 0; z < NX; ++z)
			{
				gp1[x][y][z].x = x;
				gp1[x][y][z].y = y;
				gp1[x][y][z].z = z;
				gp1[x][y][z].w = 0;

				gp2[x][y][z].x = x;
				gp2[x][y][z].y = y;
				gp2[x][y][z].z = z;
				gp2[x][y][z].w = 0;
			}
		}
	}

	for (x = 0; x < NX-1; ++x)
	{
		for (y = 0; y < NX-1; ++y)
		{
			for (z = 0; z < NX-1; ++z)
			{
				gv1[x][y][z][0] = vol4f(gp1[x][y][z], gp1[x][y][z], gp1[x][y][z], gp1[x][y][z]);
				gv1[x][y][z][1] = vol4f(gp1[x][y][z], gp1[x][y][z], gp1[x][y][z], gp1[x][y][z]);
				gv1[x][y][z][2] = vol4f(gp1[x][y][z], gp1[x][y][z], gp1[x][y][z], gp1[x][y][z]);
				gv1[x][y][z][3] = vol4f(gp1[x][y][z], gp1[x][y][z], gp1[x][y][z], gp1[x][y][z]);
				gv1[x][y][z][4] = vol4f(gp1[x][y][z], gp1[x][y][z], gp1[x][y][z], gp1[x][y][z]);
				gv1[x][y][z][5] = vol4f(gp1[x][y][z], gp1[x][y][z], gp1[x][y][z], gp1[x][y][z]);

				gv2[x][y][z][0] = vol4f(gp2[x][y][z], gp2[x][y][z], gp2[x][y][z], gp2[x][y][z]);
				gv2[x][y][z][1] = vol4f(gp2[x][y][z], gp2[x][y][z], gp2[x][y][z], gp2[x][y][z]);
				gv2[x][y][z][2] = vol4f(gp2[x][y][z], gp2[x][y][z], gp2[x][y][z], gp2[x][y][z]);
				gv2[x][y][z][3] = vol4f(gp2[x][y][z], gp2[x][y][z], gp2[x][y][z], gp2[x][y][z]);
				gv2[x][y][z][4] = vol4f(gp2[x][y][z], gp2[x][y][z], gp2[x][y][z], gp2[x][y][z]);
				gv2[x][y][z][5] = vol4f(gp2[x][y][z], gp2[x][y][z], gp2[x][y][z], gp2[x][y][z]);
			}
		}
	}
}

void bal()
{
}

int main()
{
    return 0;
}

