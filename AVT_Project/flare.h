#ifndef __FLARE_H
#define __FLARE_H

#include <iostream>
#include <sstream>

#include <string>
#include "VSShaderlib.h"

/* --- Defines --- */

#define FLARE_MAXELEMENTSPERFLARE 15
#define NTEXTURES 5

/* --- Types --- */

typedef struct FLARE_ELEMENT_DEF {

	float			fDistance;        // Distance along ray from source (0.0-1.0)
	float           fSize;            // Size relative to flare envelope (0.0-1.0)
	float			matDiffuse[4];  // color
	int				textureId;
} FLARE_ELEMENT_DEF;

typedef struct FLARE_DEF {
	float           fScale;     // Scale factor for adjusting overall size of flare elements.
	float           fMaxSize;   // Max size of largest element, as proportion of screen width (0.0-1.0)
	int             nPieces;    // Number of elements in use
	FLARE_ELEMENT_DEF    element[FLARE_MAXELEMENTSPERFLARE];
} FLARE_DEF;


inline double clamp(const double x, const double min, const double max) {
	return (x < min ? min : (x > max ? max : x));
}

inline int clampi(const int x, const int min, const int max) {
	return (x < min ? min : (x > max ? max : x));
}

char* flareTextureNames[NTEXTURES] = { "crcl", "flar", "hxgn", "ring", "sun" };

void render_flare(FLARE_DEF* flare, int lx, int ly, int* m_viewport, VSShaderLib* shader);
void loadFlareFile(FLARE_DEF* flare, char* filename);

unsigned int getTextureId(char* name) {
	int i;

	for (i = 0; i < NTEXTURES; ++i)
	{
		if (strncmp(name, flareTextureNames[i], strlen(name)) == 0)
			return i;
	}
	return -1;
}
void loadFlareFile(FLARE_DEF* flare, char* filename)
{
	int     n = 0;
	FILE* f;
	char    buf[256];
	int fields;

	memset(flare, 0, sizeof(FLARE_DEF));

	f = fopen(filename, "r");
	if (f)
	{
		fgets(buf, sizeof(buf), f);
		sscanf(buf, "%f %f", &flare->fScale, &flare->fMaxSize);

		while (!feof(f))
		{
			char name[8] = { '\0', };
			double dDist = 0.0, dSize = 0.0;
			float			color[4];
			int				id;

			fgets(buf, sizeof(buf), f);
			fields = sscanf(buf, "%4s %lf %lf ( %f %f %f %f )", name, &dDist, &dSize, &color[3], &color[0], &color[1], &color[2]);
			if (fields == 7)
			{
				for (int i = 0; i < 4; ++i) color[i] = clamp(color[i] / 255.0f, 0.0f, 1.0f);
				id = getTextureId(name);
				if (id < 0) printf("Texture name not recognized\n");
				else
					flare->element[n].textureId = id;
				flare->element[n].fDistance = (float)dDist;
				flare->element[n].fSize = (float)dSize;
				memcpy(flare->element[n].matDiffuse, color, 4 * sizeof(float));
				++n;
			}
		}

		flare->nPieces = n;
		fclose(f);
	}
	else printf("Flare file opening error\n");
}

#endif
