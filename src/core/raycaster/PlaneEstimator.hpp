#ifndef __SIBR_PLANEESTIMATOR_HPP__
#define __SIBR_PLANEESTIMATOR_HPP__

#include <core/raycaster/Config.hpp>

#include <core/system/Utils.hpp>
#include <core/system/Vector.hpp>
#include <core/graphics/Image.hpp>
#include <core/system/Array2d.hpp>
#include <core/graphics/Mesh.hpp>
#include <core/graphics/Window.hpp>

class SIBR_RAYCASTER_EXPORT PlaneEstimator
{
protected:
	Eigen::MatrixXf _remainPoints3D;
	Eigen::MatrixXf _remainNormals3D;
	std::vector<sibr::Vector3u> _Triangles;
	
	//std::vector<sibr::Vector3i> _remainImPos;
	bool _planeComputed;

public:
	PlaneEstimator();
	PlaneEstimator(const std::vector<sibr::Vector3f> & vertices, bool excludeBB=false);

	// Computes the plane using RANSAC algorithm
	void computePlanes(const int numPlane,const float delta,const int numTry);
	// estimate the best plane in the remainPoints3D using ransac algorithm with numTry.
	sibr::Vector4f estimatePlane(const float delta,const int numTry, Eigen::MatrixXi & bestMask, int & vote, std::pair<Eigen::MatrixXf,sibr::Vector3f> & bestCovMean);
	// choose randomly 3 points and compute the plane
	sibr::Vector4f plane3Pts(); 
	// given a plane and a threshold delta this function return the num of point that fit the plane in the remaining points and also the associated mask.
	std::pair<int, float> votePlane(const sibr::Vector4f plane, const float delta, Eigen::MatrixXi & mask, float normalDot=0.98);
	//
	void displayPCAndPlane(sibr::Window::Ptr _window);

	//Estimate ground plane
	sibr::Vector4f estimateGroundPlane(sibr::Vector3f roughUp);
	//Estimate zenith from cameras array
	static sibr::Vector3f estimateMedianVec(const std::vector<sibr::Vector3f> & ups);

	static sibr::Mesh getMeshPlane(sibr::Vector4f plane , sibr::Vector3f center, float radius);

	std::vector<sibr::Vector3f> _Points;
	// planes are represented as Vector4f(n.x,n.y,n.z,d)
	int _numPoints3D;
	std::vector<sibr::Vector4f> _planes;
	std::vector<std::vector<sibr::Vector3f>> _points;
	std::vector<sibr::Vector3f> _centers;

	std::vector<int> _votes;
	std::vector<std::pair<Eigen::MatrixXf,sibr::Vector3f>> _covMeans;
	
	~PlaneEstimator(void);
};

#endif
