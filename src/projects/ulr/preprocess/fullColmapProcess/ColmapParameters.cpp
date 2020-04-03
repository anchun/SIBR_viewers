#include "ColmapParameters.h"


ColmapParameters::ColmapParameters(ColmapParameters::Quality q):
_siftExtraction_ImageSize                            (initSiftExtraction_ImageSize(q)),
_siftExtraction_EstimateAffineShape                  (initSiftExtraction_EstimateAffineShape(q)),
_siftExtraction_DomainSizePooling                    (initSiftExtraction_DomainSizePooling(q)),
_siftExtraction_MaxNumFeatures                       (initSiftExtraction_MaxNumFeatures(q)),
                                                     
_exhaustiveMatcher_ExhaustiveMatchingBlockSize       (initExhaustiveMatcher_ExhaustiveMatchingBlockSize(q)),
                                                     
_mapper_MapperDotbaLocalMaxNumIterations             (initMapper_MapperDotbaLocalMaxNumIterations(q)),
_mapper_MapperDotbaGlobalMaxNumIterations            (initMapper_MapperDotbaGlobalMaxNumIterations(q)),
_mapper_MapperDotbaGlobalImagesRatio                 (initMapper_MapperDotbaGlobalImagesRatio(q)),
_mapper_MapperDotbaGlobalPointsRatio                 (initMapper_MapperDotbaGlobalPointsRatio(q)),
_mapper_MapperDotbaGlobalMaxRefinements              (initMapper_MapperDotbaGlobalMaxRefinements(q)),
_mapper_MapperDotbaLocalMaxRefinements               (initMapper_MapperDotbaLocalMaxRefinements(q)),
                                                     
_patchMatchStereo_PatchMatchStereoDotMaxImageSize    (initMatchMatchStereo_PatchMatchStereoDotMaxImageSize(q)),
_patchMatchStereo_PatchMatchStereoDotWindowRadius    (initMatchMatchStereo_PatchMatchStereoDotWindowRadius(q)),
_patchMatchStereo_PatchMatchStereoDotWindowStep      (initMatchMatchStereo_PatchMatchStereoDotWindowStep(q)),
_patchMatchStereo_PatchMatchStereoDotNumSamples      (initMatchMatchStereo_PatchMatchStereoDotNumSamples(q)),
_patchMatchStereo_PatchMatchStereoDotNumIterations   (initMatchMatchStereo_PatchMatchStereoDotNumIterations(q)),
_patchMatchStereo_PatchMatchStereoDotGeomConsistency (initMatchMatchStereo_PatchMatchStereoDotGeomConsistency(q)),

_stereoFusion_CheckNumImages                         (initStereoFusion_CheckNumImages(q)),
_stereoFusion_MaxImageSize                           (initStereoFusion_MaxImageSize(q))
{
}

uint ColmapParameters::initSiftExtraction_ImageSize(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 3200; break;
	case ColmapParameters::Quality::LOW:		return 1000; break;
	case ColmapParameters::Quality::MEDIUM:		return 1600; break;
	case ColmapParameters::Quality::HIGH:		return 2400; break;
	case ColmapParameters::Quality::EXTREME:	return 3200; break;
	default:									return 3200; break;
	}
}

bool ColmapParameters::initSiftExtraction_EstimateAffineShape(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return false; break;
	case ColmapParameters::Quality::LOW:		return false; break;
	case ColmapParameters::Quality::MEDIUM:		return false; break;
	case ColmapParameters::Quality::HIGH:		return true; break;
	case ColmapParameters::Quality::EXTREME:	return true; break;
	default:									return false; break;
	}

}
bool ColmapParameters::initSiftExtraction_DomainSizePooling(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return false; break;
	case ColmapParameters::Quality::LOW:		return false; break;
	case ColmapParameters::Quality::MEDIUM:		return false; break;
	case ColmapParameters::Quality::HIGH:		return false; break;
	case ColmapParameters::Quality::EXTREME:	return true; break;
	default:									return false; break;
	}
}

bool ColmapParameters::initSiftExtraction_MaxNumFeatures(ColmapParameters::Quality q) {
	return 8192;
}

uint ColmapParameters::initExhaustiveMatcher_ExhaustiveMatchingBlockSize(ColmapParameters::Quality q) {
	return 50;
}

uint ColmapParameters::initMapper_MapperDotbaLocalMaxNumIterations(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 25; break;
	case ColmapParameters::Quality::LOW:		return 12; break;
	case ColmapParameters::Quality::MEDIUM:		return 16; break;
	case ColmapParameters::Quality::HIGH:		return 30; break;
	case ColmapParameters::Quality::EXTREME:	return 40; break;
	default:									return 25; break;
	}
}
uint ColmapParameters::initMapper_MapperDotbaGlobalMaxNumIterations(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 50; break;
	case ColmapParameters::Quality::LOW:		return 25; break;
	case ColmapParameters::Quality::MEDIUM:		return 33; break;
	case ColmapParameters::Quality::HIGH:		return 75; break;
	case ColmapParameters::Quality::EXTREME:	return 100; break;
	default:									return 50; break;
	}
}
float ColmapParameters::initMapper_MapperDotbaGlobalImagesRatio(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 1.100001; break;
	case ColmapParameters::Quality::LOW:		return 1.32; break;
	case ColmapParameters::Quality::MEDIUM:		return 1.21; break;
	case ColmapParameters::Quality::HIGH:		return 1.100001; break;
	case ColmapParameters::Quality::EXTREME:	return 1.100001; break;
	default:									return 1.100001; break;
	}
}
float ColmapParameters::initMapper_MapperDotbaGlobalPointsRatio(ColmapParameters::Quality q) {
	return initMapper_MapperDotbaGlobalImagesRatio(q);
}

uint ColmapParameters::initMapper_MapperDotbaGlobalMaxRefinements(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 5; break;
	case ColmapParameters::Quality::LOW:		return 2; break;
	case ColmapParameters::Quality::MEDIUM:		return 2; break;
	case ColmapParameters::Quality::HIGH:		return 5; break;
	case ColmapParameters::Quality::EXTREME:	return 5; break;
	default:									return 5; break;
	}
}
uint ColmapParameters::initMapper_MapperDotbaLocalMaxRefinements(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 2; break;
	case ColmapParameters::Quality::LOW:		return 2; break;
	case ColmapParameters::Quality::MEDIUM:		return 2; break;
	case ColmapParameters::Quality::HIGH:		return 3; break;
	case ColmapParameters::Quality::EXTREME:	return 3; break;
	default:									return 2; break;
	}
}

int ColmapParameters::initMatchMatchStereo_PatchMatchStereoDotMaxImageSize(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return -1; break;
	case ColmapParameters::Quality::LOW:		return 1000; break;
	case ColmapParameters::Quality::MEDIUM:		return 1600; break;
	case ColmapParameters::Quality::HIGH:		return 2400; break;
	case ColmapParameters::Quality::EXTREME:	return -1; break;
	default:									return -1; break;
	}
}
uint ColmapParameters::initMatchMatchStereo_PatchMatchStereoDotWindowRadius(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 5; break;
	case ColmapParameters::Quality::LOW:		return 4; break;
	case ColmapParameters::Quality::MEDIUM:		return 4; break;
	case ColmapParameters::Quality::HIGH:		return 5; break;
	case ColmapParameters::Quality::EXTREME:	return 5; break;
	default:									return 5; break;
	}
}

uint ColmapParameters::initMatchMatchStereo_PatchMatchStereoDotWindowStep(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 1; break;
	case ColmapParameters::Quality::LOW:		return 2; break;
	case ColmapParameters::Quality::MEDIUM:		return 2; break;
	case ColmapParameters::Quality::HIGH:		return 1; break;
	case ColmapParameters::Quality::EXTREME:	return 1; break;
	default:									return 1; break;
	}
}
uint ColmapParameters::initMatchMatchStereo_PatchMatchStereoDotNumSamples(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 15; break;
	case ColmapParameters::Quality::LOW:		return 7; break;
	case ColmapParameters::Quality::MEDIUM:		return 10; break;
	case ColmapParameters::Quality::HIGH:		return 15; break;
	case ColmapParameters::Quality::EXTREME:	return 15; break;
	default:									return 15; break;
	}
}
uint ColmapParameters::initMatchMatchStereo_PatchMatchStereoDotNumIterations(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 5; break;
	case ColmapParameters::Quality::LOW:		return 3; break;
	case ColmapParameters::Quality::MEDIUM:		return 5; break;
	case ColmapParameters::Quality::HIGH:		return 5; break;
	case ColmapParameters::Quality::EXTREME:	return 5; break;
	default:									return 5; break;
	}

}
bool ColmapParameters::initMatchMatchStereo_PatchMatchStereoDotGeomConsistency(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 5; break;
	case ColmapParameters::Quality::LOW:		return 3; break;
	case ColmapParameters::Quality::MEDIUM:		return 5; break;
	case ColmapParameters::Quality::HIGH:		return 5; break;
	case ColmapParameters::Quality::EXTREME:	return 5; break;
	default:									return 5; break;
	}
}

uint ColmapParameters::initStereoFusion_CheckNumImages(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return 50; break;
	case ColmapParameters::Quality::LOW:		return 25; break;
	case ColmapParameters::Quality::MEDIUM:		return 33; break;
	case ColmapParameters::Quality::HIGH:		return 50; break;
	case ColmapParameters::Quality::EXTREME:	return 50; break;
	default:									return 50; break;
	}
}
int ColmapParameters::initStereoFusion_MaxImageSize(ColmapParameters::Quality q) {
	switch (q) {
	case ColmapParameters::Quality::DEFAULT:	return -1; break;
	case ColmapParameters::Quality::LOW:		return 1000; break;
	case ColmapParameters::Quality::MEDIUM:		return 1600; break;
	case ColmapParameters::Quality::HIGH:		return 2400; break;
	case ColmapParameters::Quality::EXTREME:	return -1; break;
	default:									return -1; break;
	}
}

uint	ColmapParameters::siftExtractionImageSize() const {
	return _siftExtraction_ImageSize;
}

bool	ColmapParameters::siftExtractionEstimateAffineShape() const {
	return _siftExtraction_EstimateAffineShape;
}

bool	ColmapParameters::siftExtractionDomainSizePooling() const {
	return _siftExtraction_DomainSizePooling;
}

bool	ColmapParameters::siftExtractionMaxNumFeatures() const {
	return _siftExtraction_MaxNumFeatures;
}

uint	ColmapParameters::exhaustiveMatcherExhaustiveMatchingBlockSize() const {
	return _exhaustiveMatcher_ExhaustiveMatchingBlockSize;
}

uint	ColmapParameters::mapperMapperDotbaLocalMaxNumIterations() const {
	return _mapper_MapperDotbaLocalMaxNumIterations;
}

uint	ColmapParameters::mapperMapperDotbaGlobalMaxNumIterations() const {
	return _mapper_MapperDotbaGlobalMaxNumIterations;
}

float	ColmapParameters::mapperMapperDotbaGlobalImagesRatio() const {
	return _mapper_MapperDotbaGlobalImagesRatio;
}

float	ColmapParameters::mapperMapperDotbaGlobalPointsRatio() const {
	return _mapper_MapperDotbaGlobalPointsRatio;
}

uint	ColmapParameters::mapperMapperDotbaGlobalMaxRefinements() const {
	return _mapper_MapperDotbaGlobalMaxRefinements;
}

uint	ColmapParameters::mapperMapperDotbaLocalMaxRefinements() const {
	return _mapper_MapperDotbaLocalMaxRefinements;
}

int		ColmapParameters::patchMatchStereoPatchMatchStereoDotMaxImageSize() const {
	return _patchMatchStereo_PatchMatchStereoDotMaxImageSize;
}

uint	ColmapParameters::patchMatchStereoPatchMatchStereoDotWindowRadius() const {
	return _patchMatchStereo_PatchMatchStereoDotWindowRadius;
}

uint	ColmapParameters::patchMatchStereoPatchMatchStereoDotWindowStep() const {
	return _patchMatchStereo_PatchMatchStereoDotWindowStep;
}

uint	ColmapParameters::patchMatchStereoPatchMatchStereoDotNumSamples() const {
	return _patchMatchStereo_PatchMatchStereoDotNumSamples;
}

uint	ColmapParameters::patchMatchStereoPatchMatchStereoDotNumIterations() const {
	return _patchMatchStereo_PatchMatchStereoDotNumIterations;
}

bool	ColmapParameters::patchMatchStereoPatchMatchStereoDotGeomConsistency() const {
	return _patchMatchStereo_PatchMatchStereoDotGeomConsistency;
}

uint	ColmapParameters::stereoFusionCheckNumImages() const {
	return _stereoFusion_CheckNumImages;
}

int		ColmapParameters::stereoFusionMaxImageSize() const {
	return _stereoFusion_MaxImageSize;
}

void	ColmapParameters::siftExtractionImageSize(uint value) {
	_siftExtraction_ImageSize = value;
}
void	ColmapParameters::siftExtractionEstimateAffineShape(bool value) {
	_siftExtraction_EstimateAffineShape = value;
}
void	ColmapParameters::siftExtractionDomainSizePooling(bool value) {
	_siftExtraction_DomainSizePooling = value;
}
void	ColmapParameters::siftExtractionMaxNumFeatures(bool value) {
	_siftExtraction_MaxNumFeatures = value;
}
void	ColmapParameters::exhaustiveMatcherExhaustiveMatchingBlockSize(uint value) {
	_exhaustiveMatcher_ExhaustiveMatchingBlockSize = value;
}
void	ColmapParameters::mapperMapperDotbaLocalMaxNumIterations(uint value) {
	_mapper_MapperDotbaLocalMaxNumIterations = value;
}
void	ColmapParameters::mapperMapperDotbaGlobalMaxNumIterations(uint value) {
	_mapper_MapperDotbaGlobalMaxNumIterations = value;
}
void	ColmapParameters::mapperMapperDotbaGlobalImagesRatio(float value) {
	_mapper_MapperDotbaGlobalImagesRatio = value;
}
void	ColmapParameters::mapperMapperDotbaGlobalPointsRatio(float value) {
	_mapper_MapperDotbaGlobalPointsRatio = value;
}
void	ColmapParameters::mapperMapperDotbaGlobalMaxRefinements(uint value) {
	_mapper_MapperDotbaGlobalMaxRefinements = value;
}
void	ColmapParameters::mapperMapperDotbaLocalMaxRefinements(uint value) {
	_mapper_MapperDotbaGlobalMaxRefinements = value;
}
void	ColmapParameters::patchMatchStereoPatchMatchStereoDotMaxImageSize(int value) {
	_patchMatchStereo_PatchMatchStereoDotMaxImageSize = value;
}
void	ColmapParameters::patchMatchStereoPatchMatchStereoDotWindowRadius(uint value) {
	_patchMatchStereo_PatchMatchStereoDotWindowRadius = value;
}
void	ColmapParameters::patchMatchStereoPatchMatchStereoDotWindowStep(uint value) {
	_patchMatchStereo_PatchMatchStereoDotWindowStep = value;
}
void	ColmapParameters::patchMatchStereoPatchMatchStereoDotNumSamples(uint value) {
	_patchMatchStereo_PatchMatchStereoDotNumSamples = value;
}
void	ColmapParameters::patchMatchStereoPatchMatchStereoDotNumIterations(uint value) {
	_patchMatchStereo_PatchMatchStereoDotNumIterations = value;
}
void	ColmapParameters::patchMatchStereoPatchMatchStereoDotGeomConsistency(bool value) {
	_patchMatchStereo_PatchMatchStereoDotGeomConsistency = value;
}
void	ColmapParameters::stereoFusionCheckNumImages(uint value) {
	_stereoFusion_CheckNumImages;
}
void	ColmapParameters::stereoFusionMaxImageSize(int value) {
	_stereoFusion_MaxImageSize;
}

std::shared_ptr<ColmapParameters::Quality> ColmapParameters::stringToQuality(

	const std::string& sQuality) {
	std::shared_ptr<ColmapParameters::Quality> quality;

	if (sQuality.compare("low") == 0) {
		quality = std::make_shared<ColmapParameters::Quality>(
			ColmapParameters::Quality::LOW);
	}
	else if (sQuality.compare("medium") == 0) {
		quality = std::make_shared<ColmapParameters::Quality>(
			ColmapParameters::Quality::MEDIUM);
	}
	else if (sQuality.compare("high") == 0) {
		quality = std::make_shared<ColmapParameters::Quality>(
			ColmapParameters::Quality::HIGH);
	}
	else if (sQuality.compare("extreme") == 0) {
		quality = std::make_shared<ColmapParameters::Quality>(
			ColmapParameters::Quality::EXTREME);
	}
	else {
		SIBR_WRG << "The quality: " << sQuality << " is unknown..." << std::endl
			<< " Possible values : low medium high extreme" << std::endl;
		quality = nullptr;
	}
	return quality;
}
