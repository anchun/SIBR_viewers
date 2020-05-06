#pragma once
#include <core/system/Utils.hpp>
class ColmapParameters
{

public:
	//----------TYPES------------//
	enum class Quality {DEFAULT,LOW,MEDIUM,HIGH,EXTREME};


	//-------CONSTRUCTORS--------//
	ColmapParameters(Quality q = Quality::DEFAULT);


	//--------CONST METHODS---------//
	//Feature extractor 
	uint	siftExtractionImageSize() const;
	bool	siftExtractionEstimateAffineShape() const;
	bool	siftExtractionDomainSizePooling() const;
	uint	siftExtractionMaxNumFeatures() const;

	//Exhaustive matcher
	uint	exhaustiveMatcherExhaustiveMatchingBlockSize() const;

	//Mapper
	uint	mapperMapperDotbaLocalMaxNumIterations() const;
	uint	mapperMapperDotbaGlobalMaxNumIterations() const;
	float	mapperMapperDotbaGlobalImagesRatio() const;
	float	mapperMapperDotbaGlobalPointsRatio() const;
	uint	mapperMapperDotbaGlobalMaxRefinements() const;
	uint	mapperMapperDotbaLocalMaxRefinements() const;

	//Patch match stereo
	int		patchMatchStereoPatchMatchStereoDotMaxImageSize() const;
	uint	patchMatchStereoPatchMatchStereoDotWindowRadius() const;
	uint	patchMatchStereoPatchMatchStereoDotWindowStep() const;
	uint	patchMatchStereoPatchMatchStereoDotNumSamples() const;
	uint	patchMatchStereoPatchMatchStereoDotNumIterations() const;
	bool	patchMatchStereoPatchMatchStereoDotGeomConsistency() const;

	//Stereo fusion
	uint	stereoFusionCheckNumImages() const;
	int		stereoFusionMaxImageSize() const;


	//--------METHODS---------//
	//Feature extractor 
	void	siftExtractionImageSize(uint value);
	void	siftExtractionEstimateAffineShape(bool value);
	void	siftExtractionDomainSizePooling(bool value);
	void	siftExtractionMaxNumFeatures(uint value);

	//Exhaustive matcher
	void	exhaustiveMatcherExhaustiveMatchingBlockSize(uint value);

	//Mapper
	void	mapperMapperDotbaLocalMaxNumIterations(uint value);
	void	mapperMapperDotbaGlobalMaxNumIterations(uint value);
	void	mapperMapperDotbaGlobalImagesRatio(float value);
	void	mapperMapperDotbaGlobalPointsRatio(float value);
	void	mapperMapperDotbaGlobalMaxRefinements(uint value);
	void	mapperMapperDotbaLocalMaxRefinements(uint value);

	//Patch match stereo
	void	patchMatchStereoPatchMatchStereoDotMaxImageSize(int value);
	void	patchMatchStereoPatchMatchStereoDotWindowRadius(uint value);
	void	patchMatchStereoPatchMatchStereoDotWindowStep(uint value);
	void	patchMatchStereoPatchMatchStereoDotNumSamples(uint value);
	void	patchMatchStereoPatchMatchStereoDotNumIterations(uint value);
	void	patchMatchStereoPatchMatchStereoDotGeomConsistency(bool value);

	//Stereo fusion
	void	stereoFusionCheckNumImages(uint value);
	void	stereoFusionMaxImageSize(int value);

	//-------STATIC METHODS------//
	static std::shared_ptr<Quality> stringToQuality(const std::string& sQuality);


private :

	//--------ATTRIBUTES---------//
	//Feature extractor 
	uint	_siftExtraction_ImageSize;
	bool	_siftExtraction_EstimateAffineShape;
	bool	_siftExtraction_DomainSizePooling;
	uint	_siftExtraction_MaxNumFeatures;

	//Exhaustive matcher
	uint	_exhaustiveMatcher_ExhaustiveMatchingBlockSize;

	//Mapper
	uint	_mapper_MapperDotbaLocalMaxNumIterations;
	uint	_mapper_MapperDotbaGlobalMaxNumIterations;
	float	_mapper_MapperDotbaGlobalImagesRatio;
	float	_mapper_MapperDotbaGlobalPointsRatio;
	uint	_mapper_MapperDotbaGlobalMaxRefinements;
	uint	_mapper_MapperDotbaLocalMaxRefinements;

	//Patch match stereo
	int		_patchMatchStereo_PatchMatchStereoDotMaxImageSize;
	uint	_patchMatchStereo_PatchMatchStereoDotWindowRadius;
	uint	_patchMatchStereo_PatchMatchStereoDotWindowStep;
	uint	_patchMatchStereo_PatchMatchStereoDotNumSamples;
	uint	_patchMatchStereo_PatchMatchStereoDotNumIterations;
	bool	_patchMatchStereo_PatchMatchStereoDotGeomConsistency;


	//Stereo fusion
	uint	_stereoFusion_CheckNumImages;
	int		_stereoFusion_MaxImageSize;


	//Feature extractor 
	uint	initSiftExtraction_ImageSize(Quality q);
	bool	initSiftExtraction_EstimateAffineShape(Quality q);
	bool	initSiftExtraction_DomainSizePooling(Quality q);
	uint	initSiftExtraction_MaxNumFeatures(Quality q);

	//Exhaustive matcher
	uint	initExhaustiveMatcher_ExhaustiveMatchingBlockSize(Quality q);

	//Mapper
	uint	initMapper_MapperDotbaLocalMaxNumIterations(Quality q);
	uint	initMapper_MapperDotbaGlobalMaxNumIterations(Quality q);
	float	initMapper_MapperDotbaGlobalImagesRatio(Quality q);
	float	initMapper_MapperDotbaGlobalPointsRatio(Quality q);
	uint	initMapper_MapperDotbaGlobalMaxRefinements(Quality q);
	uint	initMapper_MapperDotbaLocalMaxRefinements(Quality q);

	//Patch match stereo
	int		initMatchMatchStereo_PatchMatchStereoDotMaxImageSize(Quality q);
	uint	initMatchMatchStereo_PatchMatchStereoDotWindowRadius(Quality q);
	uint	initMatchMatchStereo_PatchMatchStereoDotWindowStep(Quality q);
	uint	initMatchMatchStereo_PatchMatchStereoDotNumSamples(Quality q);
	uint	initMatchMatchStereo_PatchMatchStereoDotNumIterations(Quality q);
	bool	initMatchMatchStereo_PatchMatchStereoDotGeomConsistency(Quality q);

	//Stereo fusion
	uint	initStereoFusion_CheckNumImages(Quality q);
	int		initStereoFusion_MaxImageSize(Quality q);

};


