#ifndef __SIBR_GRAPHICS_DEBUG_INTERFACE_UTILS_HPP__
# define __SIBR_GRAPHICS_DEBUG_INTERFACE_UTILS_HPP__

#include <core/graphics/DrawingUtils.h>

typedef std::shared_ptr<sibr::ImageRGB> Img;

struct LocalPos
{
	LocalPos(void) :  isDefined(false){}
	LocalPos( const sibr::Vector2i & _pos, int _imgId ) : pos(_pos), imgId(_imgId), isDefined(true) {}
	static bool compare( const LocalPos & posA, const LocalPos & posB) {
		if( posA.imgId != posB.imgId ){ 
			return posA.imgId <posB.imgId; 
		} else {
			return posA.pos < posB.pos;
		}
	}

	sibr::Vector2i pos;		//pixel position in image space
	int imgId;				//image id (from dataset ordering)
	bool isDefined;
};

struct MouseActivity
{
	MouseActivity( void ) : isActivated(false), changedStatus(false) {} 
	bool isActivated;
	bool changedStatus;
};

struct SIBR_GRAPHICS_EXPORT ZoomData
{
	ZoomData(void);
	
	void getSelection( Img & img);
	void reset( Img & img );
	sibr::Vector2i zoomToGlobal( sibr::Vector2i & pos);
	sibr::Vector2i globalToZoom( sibr::Vector2i & pos);

	Img prevImg;
	sibr::Vector2f ratios;

	// zoom box proprieties in full res space
	sibr::Vector2i topLeft; 
	sibr::Vector2i tempTopLeft; 
	sibr::Vector2i boxSize;			

	bool isSelecting;
	bool firstCornerSelected;
	bool zoomActive;

	static int selectionTreshold;
};

struct SIBR_GRAPHICS_EXPORT SelectionViewData
{
	std::vector<int> selectedViewMapping;			//local display id to global img id
	std::vector<int> selectedViewInverseMapping;	//global img id to local display id
	std::vector<bool> imgsActivated;				//listed by global img id
	int numImgsToSelect;
	int numSelected;
	int numImgs;

	SelectionViewData( void );
	SelectionViewData( int _numImgs, int _numImgsToSelect = 2 );
	
	void reset( void );

	//default case, all images are selected, both mappings are identity
	void initMappings( void );

	//case when only one image is selected
	void initMappings( int imId );

	//general case
	void initMappings( std::vector<int> & selectedImgs );
};

/**
* \ingroup sibr_graphics
*/
class SIBR_GRAPHICS_EXPORT DrawingTool : public DrawingUtils
{
private :	
	Img img;
	std::vector<sibr::Vector2i> posPainted;

public :
	void setImageToDrawOn( Img _img );

	const std::vector<sibr::Vector2i> & getPosPainted(void) const;
	void resetPosPainted(void);

	void updatePixel( sibr::ImageRGB & img , const sibr::Vector2i & pos, const sibr::Vector3ub & color , float alpha = 0 );

	void pixel(const sibr::Vector2i & pos, const sibr::Vector3ub & color, float alpha = 0);

	void patch( const sibr::Vector2i & position , const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int radius=5, int thickness=1 );

	void line( const sibr::Vector2i & pA, const sibr::Vector2i & pB, const sibr::Vector3ub & color = sibr::Vector3ub(255, 0, 0), int thickness = 1);

	void rectangle( const sibr::Vector2i & cornerA, const sibr::Vector2i & cornerB, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int thickness=1 );
	
	void rectangleFilled( const sibr::Vector2i & cornerA, const sibr::Vector2i & cornerB, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0) );
	
	void plusSign( const sibr::Vector2i & center, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int radius=5, int thickness=1 );

	void crossSign( const sibr::Vector2i & center, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int radius=5, int thickness=2 );

	void diamond( const sibr::Vector2i & center, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int radius=5 );

	void ellipse( const sibr::Vector2i & center, const sibr::Vector2i & axesLength, float angle, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int thickness=1 );
};


#endif // __SIBR_GRAPHICS_DEBUG_INTERFACE_UTILS_HPP__