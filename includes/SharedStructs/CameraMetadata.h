#ifndef METADATA_H
# define METADATA_H

# include "SharedStructs/Vec3.h"

struct CameraMetadata {
	Vec3	pixelDeltaU;
	Vec3	pixelDeltaV;
	Vec3	pixel00Loc;
	Vec3	camCenter;
};

#endif
