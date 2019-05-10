#include "DebugInterfaceUtils.h"

SelectionViewData::SelectionViewData( void ) : 
	numImgs(0), imgsActivated(std::vector<bool>(0,false)), numSelected(0), numImgsToSelect(2) 
{}

SelectionViewData::SelectionViewData( int _numImgs, int _numImgsToSelect ) 
	: numImgs(_numImgs), imgsActivated(std::vector<bool>(_numImgs,false)), numSelected(_numImgs), numImgsToSelect(std::min(_numImgsToSelect,_numImgs)) 
{
	selectedViewMapping.resize(numImgs); 
	selectedViewInverseMapping.resize(numImgs); 
	initMappings();
}

void SelectionViewData::reset( void ) 
{  
	imgsActivated = std::vector<bool>(numImgs,false); numSelected = 0; initMappings(); 
}

//default case, all images are selected, both mappings are identity
void SelectionViewData::initMappings( void )
{ 
	std::vector<int> selectedImgs(numImgs);
	for(int i=0; i<numImgs; ++i){
		selectedImgs.at(i) = i;
	}
	initMappings( selectedImgs );
}

//case when only one image is selected
void SelectionViewData::initMappings( int imId )
{
	std::vector<int> selectedImgs(1,imId);
	initMappings(selectedImgs);
}

//general case
void SelectionViewData::initMappings( std::vector<int> & selectedImgs )
{
	selectedViewMapping.resize( selectedImgs.size() );
	selectedViewInverseMapping.resize(numImgs,-1);
	for(int id=0; id<(int)selectedImgs.size(); ++id){
		int im = selectedImgs.at(id);
		int imClamp = std::max( 0, std::min(numImgs-1,im) );
		selectedViewMapping.at( id ) = imClamp;
		selectedViewInverseMapping.at( imClamp ) = id;
	}
}

void DrawingTool::setImageToDrawOn( Img _img ) {
	img = _img; 
}

const std::vector<sibr::Vector2i> & DrawingTool::getPosPainted(void) const {
	return posPainted; 
}

void DrawingTool::resetPosPainted(void){
	posPainted.clear(); 
}

void DrawingTool::updatePixel( sibr::ImageRGB & img , const sibr::Vector2i & pos, const sibr::Vector3ub & color , float alpha )
{
	DrawingUtils::updatePixel(img,pos,color,alpha);
	posPainted.push_back(pos);
}

void DrawingTool::pixel(const sibr::Vector2i & pos, const sibr::Vector3ub & color, float alpha)
{
	DrawingUtils::pixel(*img, pos, color, alpha);
}

void DrawingTool::patch( const sibr::Vector2i & position , const sibr::Vector3ub & color, int radius, int thickness )
{
	DrawingUtils::patch(*img,position,color,radius,thickness);
}

void DrawingTool::line(const sibr::Vector2i & pA, const sibr::Vector2i & pB, const sibr::Vector3ub & color, int thickness)
{
	DrawingUtils::line(*img, pA, pB, color, thickness);
}

void DrawingTool::rectangle( const sibr::Vector2i & cornerA, const sibr::Vector2i & cornerB, const sibr::Vector3ub & color, int thickness )
{
	DrawingUtils::rectangle(*img,cornerA,cornerB,color,thickness);
}

void DrawingTool::rectangleFilled( const sibr::Vector2i & cornerA, const sibr::Vector2i & cornerB, const sibr::Vector3ub & color)
{
	DrawingUtils::rectangleFilled(*img,cornerA,cornerB,color);
}

void DrawingTool::plusSign( const sibr::Vector2i & center, const sibr::Vector3ub & color, int radius, int thickness )
{
	DrawingUtils::plusSign(*img,center,color,radius,thickness);
}

void DrawingTool::crossSign( const sibr::Vector2i & center, const sibr::Vector3ub & color, int radius, int thickness )
{
	DrawingUtils::crossSign(*img,center,color,radius,thickness);
}

void DrawingTool::diamond( const sibr::Vector2i & center, const sibr::Vector3ub & color, int radius )
{
	DrawingUtils::diamond(*img,center,color,radius);
}

void DrawingTool::ellipse( const sibr::Vector2i & center, const sibr::Vector2i & axesLength, float angle, const sibr::Vector3ub & color, int thickness )
{
	DrawingUtils::ellipse(*img,center,axesLength,angle,color,thickness);
}

ZoomData::ZoomData(void) :
	prevImg(Img(new sibr::ImageRGB())), zoomActive(false), isSelecting(false), firstCornerSelected(false) 
{}

void ZoomData::getSelection( Img & img)
{
	if( ! zoomActive ){
		prevImg = Img( new sibr::ImageRGB( img->clone() ) );
	}
	isSelecting = true;
	firstCornerSelected = false;
}

void ZoomData::reset( Img & img )
{
	zoomActive = false;
	topLeft = sibr::Vector2i(0,0);
	boxSize = sibr::Vector2i( img->w(), img->h() );	
	ratios = sibr::Vector2f(1.0f,1.0f);
}

sibr::Vector2i ZoomData::zoomToGlobal( sibr::Vector2i & pos)
{
	sibr::Vector2f ratio( pos.cast<float>().array()/sibr::Vector2f(prevImg->w(),prevImg->h()).array() );
	return (topLeft + (ratio.array()*boxSize.cast<float>().array()).matrix().cast<int>());
}

sibr::Vector2i ZoomData::globalToZoom( sibr::Vector2i & pos)
{
	sibr::Vector2f ratio( (pos-topLeft).cast<float>().array()/boxSize.cast<float>().array() );
	return  ( ratio.array()*sibr::Vector2f(prevImg->w(),prevImg->h()).array() ).matrix().cast<int>();
}