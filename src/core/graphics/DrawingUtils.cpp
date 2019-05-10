#include "DrawingUtils.h"

void DrawingUtils::pixel(sibr::ImageRGB & img, const sibr::Vector2i & pos, const sibr::Vector3ub & color, float alpha)
{
	if (!img.isInRange(pos.x(), pos.y())) { return; }
	updatePixel(img, pos, color, alpha);
}

void DrawingUtils::updatePixel(sibr::ImageRGB & img, const sibr::Vector2i & pos, const sibr::Vector3ub & color, float alpha)
{
	img.pixel(pos.x(), pos.y()) = (alpha == 0 ? color :
		(alpha*img.pixel(pos.x(), pos.y()).cast<float>() + (1.0f - alpha)*color.cast<float>()).cast<unsigned char>());
}

void DrawingUtils::rectangle(sibr::ImageRGB & img, const sibr::Vector2i & cornerA, const sibr::Vector2i & cornerB, const sibr::Vector3ub & color, int thickness)
{
	sibr::Vector2i tl(cornerA.cwiseMin(cornerB));
	sibr::Vector2i br(cornerA.cwiseMax(cornerB));

	if (thickness == 1) {
		if (tl == br) {
			pixel(img, br, color);
		}
		else {
			for (int i = tl.x(); i < br.x(); ++i) { pixel(img, sibr::Vector2i(i, tl.y()), color); }
			for (int j = tl.y(); j < br.y(); ++j) { pixel(img, sibr::Vector2i(br.x(), j), color); }
			for (int i = br.x(); i > tl.x(); --i) { pixel(img, sibr::Vector2i(i, br.y()), color); }
			for (int j = br.y(); j > tl.y(); --j) { pixel(img, sibr::Vector2i(tl.x(), j), color); }
		}
	}
	else {
		for (int t = 2; t <= thickness; ++t) {
			rectangle(img, tl, br, color, 1);
			tl += sibr::Vector2i(-1, -1);
			br += sibr::Vector2i(1, 1);
		}
	}
}

void DrawingUtils::patch(sibr::ImageRGB & img, const sibr::Vector2i & position, const sibr::Vector3ub & color, int radius, int thickness)
{
	int x = position.x();
	int y = position.y();

	for (int t = 0; t <= thickness; ++t) {
		int r = radius + t;
		sibr::Vector2i tl(x - r, y - r);
		sibr::Vector2i br(x + r, y + r);
		rectangle(img, tl, br, color);
	}
}

void DrawingUtils::line(sibr::ImageRGB & img, const sibr::Vector2i & pA, const sibr::Vector2i & pB, const sibr::Vector3ub & color, int thickness)
{
	int radius = thickness - 1;
	sibr::Vector2i shift(radius, radius);

	sibr::Vector2f deltas((pB - pA).cast<float>());
	sibr::Vector2f dirs = deltas.unaryExpr([](float f) { return f >= 0 ? 1 : -1; });
	deltas = deltas.cwiseAbs();

	float delta = std::abs(deltas.y() / deltas.x());
	float error = deltas.x() - deltas.y();
	
	sibr::Vector2i p = pA;

	bool isDone  = (p == pB);
	
	while (true) {
		if (thickness == 1) {
			pixel(img, p, color);
		}
		else {	
			rectangleFilled(img, p - shift, p + shift, color);
		}
		if (isDone) { break; }

		float error2 = 2.0f*error;
		if (error2 >= -deltas.y()) {
			error -= deltas.y();
			p.x() += int(dirs.x());
		}
		if (error2 < deltas.x()) {
			error += deltas.x();
			p.y() += int(dirs.y());
		}
		isDone = (p == pB);
	}
}

void DrawingUtils::rectangleFilled( sibr::ImageRGB & img, const sibr::Vector2i & cornerA, const sibr::Vector2i & cornerB, const sibr::Vector3ub & color )
{
	int radius = ((cornerB - cornerA).cwiseAbs().minCoeff()+1)/2;
	sibr::Vector2i shift(1,1);
	sibr::Vector2i tl ( cornerA.cwiseMin(cornerB) );
	sibr::Vector2i br ( cornerA.cwiseMax(cornerB) );

	for(int t=0; t<=radius; ++t){
		rectangle(img,tl,br,color);
		tl += shift;
		br -= shift;
	}
}

void DrawingUtils::plusSign( sibr::ImageRGB & img, const sibr::Vector2i & center, const sibr::Vector3ub & color , int radius, int thickness )
{
	pixel(img,center,color);
	for( int t=-thickness+1; t<thickness; ++t){
		sibr::Vector2i left    ( center + sibr::Vector2i( -radius, t ) );
		sibr::Vector2i top     ( center + sibr::Vector2i( t, +radius ) );
		sibr::Vector2i right   ( center + sibr::Vector2i( +radius, t ) );
		sibr::Vector2i bottom  ( center + sibr::Vector2i( t, -radius ) );
		for( int u=0; u<radius-std::abs(t)+1; ++u){
			pixel(img,left  ,color);
			pixel(img,top   ,color);
			pixel(img,right ,color);
			pixel(img,bottom,color);
			++left.x();
			--top.y();
			--right.x();
			++bottom.y();
		}
	}
}

void DrawingUtils::crossSign( sibr::ImageRGB & img, const sibr::Vector2i & center, const sibr::Vector3ub & color, int radius, int thickness )
{	
	pixel(img,center,color);
	for( int t=-thickness+1; t<thickness; ++t){
		bool p = ( t > 0);
		sibr::Vector2i topLeft		( center + sibr::Vector2i( (p?+t:0) -radius, (p?0:+t) +radius ) );
		sibr::Vector2i topRight     ( center + sibr::Vector2i( (p?-t:0) +radius, (p?0:+t) +radius ) );
		sibr::Vector2i bottomLeft   ( center + sibr::Vector2i( (p?+t:0) -radius, (p?0:-t) -radius ) );
		sibr::Vector2i bottomRight  ( center + sibr::Vector2i( (p?-t:0) +radius, (p?0:-t) -radius ) );
		for( int u=0; u<radius-std::abs(t)+1; ++u){
			pixel(img,topLeft    ,color);
			pixel(img,topRight   ,color);
			pixel(img,bottomLeft ,color);
			pixel(img,bottomRight,color);
			topLeft     += sibr::Vector2i(+1,-1);
			topRight    += sibr::Vector2i(-1,-1);
			bottomLeft  += sibr::Vector2i(+1,+1);
			bottomRight += sibr::Vector2i(-1,+1);
		}
	}
}

void DrawingUtils::diamond( sibr::ImageRGB & img, const sibr::Vector2i & center, const sibr::Vector3ub & color, int radius )
{
	pixel(img,center,color);
	for( int r=1; r<=radius; ++r){
		sibr::Vector2i top	  ( center + sibr::Vector2i(0,+r) );
		sibr::Vector2i right  ( center + sibr::Vector2i(+r,0) );
		sibr::Vector2i bottom ( center + sibr::Vector2i(0,-r) );
		sibr::Vector2i left   ( center + sibr::Vector2i(-r,0) );	
		
		for( int t=1; t<=r; ++t){
			pixel(img,top   ,color);
			pixel(img,left  ,color);
			pixel(img,right ,color);
			pixel(img,bottom,color);
			top    += sibr::Vector2i(+1,-1);
			right  += sibr::Vector2i(-1,-1);
			bottom += sibr::Vector2i(-1,+1);
			left   += sibr::Vector2i(+1,+1);		
		}
	}
}

void DrawingUtils::ellipse( sibr::ImageRGB & img, const sibr::Vector2i & center, const sibr::Vector2i & axesLength, float angle, const sibr::Vector3ub & color, int thickness )
{
	/*
	cv::Size size( (int)axesLength.x(), (int)axesLength.y() );
	cv::Point cvCenter( center.x(), center.y() );
	cv::Scalar cvColor(color.x(),color.y(),color.z());
	cv::ellipse( img.toOpenCVnonConst(), cvCenter,size, angle, 0, 360, cvColor, thickness );
	*/

	//approximate formula for ellipse length
	int tMax = 4*axesLength.sum();

	Eigen::Matrix2f rotation;
	rotation << std::cos(angle), -std::sin(angle), std::sin(angle), std::cos(angle);
	
	for( int t=0; t<tMax; ++t){
		float t_angle = 2.0f*(float)M_PI*t/(float)tMax;
		sibr::Vector2f pos( std::cos(t_angle)*(float)(axesLength.x()), std::sin(t_angle)*(float)(axesLength.y()) );
		sibr::Vector2i posInt(  center + (rotation*pos).unaryExpr([](float f){ return std::floor(f+0.5f); }).cast<int>() );
		diamond(img,posInt,color,thickness);
	}	
		
	/*
	for( int t=0; t<tMax; ++t){
		float t_angle = 2.0f*(float)M_PI*t/(float)tMax;
		for( int r= -thickness; r<=thickness; ++r){		
			sibr::Vector2f pos( std::cos(t_angle)*(float)(axesLength.x()+r), std::sin(t_angle)*(float)(axesLength.y()+r) );
			pixel(img,center + (rotation*pos).unaryExpr([](float f){ return std::floor(f+0.5f); }).cast<int>(),color);
		}
	}
	*/

}
