#ifndef DEBUGINTERFACE_H
#define DEBUGINTERFACE_H

#include <core/graphics/DebugInterfaceUtils.h>

#include <functional>

typedef void (*CallBackFunction)(int event, int x, int y, int flags, void* userdata);


/* put all viewing modes here, even for derived interface classes - needed because enums cant be inherited ... */
/* actually, one might use SUBMODE for inherited interface class */
enum ViewingMode { ALL, SINGLE, SELECTION, SELECTED, ZOOM,  SUBMODE };

struct InterfaceState
{
	InterfaceState() : forceClickOut(false){}
	ViewingMode mode;
	int event;
	int flags;
	LocalPos localPos;
	bool forceClickOut;
};

/**
* \ingroup sibr_graphics
*/
class SIBR_GRAPHICS_EXPORT DebugInterface
{

protected :
	
	void clearImgChanges(void);

	virtual void inputPoll( int key );
	virtual void printHelp( void );

	DrawingTool draw;
	MouseActivity mouseActivity;
	ViewingMode viewingMode;
	SelectionViewData selectionData;
	ZoomData zoomData;
	InterfaceState previousState;
	
	std::vector<Img> imgsRescaled;
	Img fullResImg;
	Img interfaceImg;
	Img interfaceImgActive;
	std::vector<sibr::ImageRGB::Ptr> inputImgsPtrs;

	std::string interfaceName;

	LocalPos previousLPos;

	sibr::Vector2i inputDimInterface;
	sibr::Vector2i dimInterface;
	sibr::Vector2i dimImage;
	sibr::Vector2i dimImgRescaled;
	sibr::Vector2i dimImgsFullRes;
	sibr::Vector2i dimGrid;

	int numImages;
	int whichKey;
	int current_img;

	bool keychanged;

	void computeFullResImgs( std::vector<sibr::ImageRGB::Ptr> & imgs );
	void computeInterfaceImgFromFullRes( const  sibr::Vector2i & dimInterface );

	void setInterfaceImg(  std::vector<sibr::ImageRGB::Ptr> & imgs, const  sibr::Vector2i & dimInterface  );
	//Img concatImgs(  std::vector<sibr::ImageRGB*> & imgs );

	sibr::Vector2i squareGrid( int n);

	//get the screen pixel location
	sibr::Vector2i toScreenPos( const LocalPos & lpos );

	//get the screen pixel location
	sibr::Vector2i toScreenPosFromFloat( int imgId, const sibr::Vector2f & pos );

	//LocalPos toLocal( sibr::Vector2i & gpos);

	//get the img id and the image pixel location of an input screen position
	LocalPos toImgPix( const sibr::Vector2i & gpos);

	//get the position in the concatenation of full res imgs
	sibr::Vector2i toFullResFromScreen( const sibr::Vector2i & screenPos );

	//get the position in screen from position in full res
	sibr::Vector2i toScreenFromFullRes( const sibr::Vector2i & fullResPos );

	//get the position in screen from position in full res
	sibr::Vector2f toScreenFromFullResFloat( const sibr::Vector2f & fullResPos );

	sibr::Vector2i toFullResFromLPos( const LocalPos & lpos );
	sibr::Vector2f toFullResFloatFromLPos(int imId, const sibr::Vector2f & pos );

	//return the pixel center position in [-1,1]x[-1,1] space
	sibr::Vector2f getPixelCenterPositionFloat( const sibr::ImageRGB & img, const sibr::Vector2i & pos );

	LocalPos toLocalRescaled( const sibr::Vector2i & gpos );

	int toScreenImgIdFromGlobalImgId( int globalImgId );
	int toGlobalImgIdFromScreenImgId( int screenImgId );

	virtual void updateSelection( LocalPos & lpos );
	void printSelectionStatus( void );

	bool zoomSelection( sibr::Vector2i & gPos );
	void zoomCallBack( sibr::Vector2i & gPos );

	virtual void refreshScreen();

	bool isVisibleFromScreen( sibr::ImageRGB & img , sibr::Vector2i & pos );

	//void printPixel( sibr::ImageRGB & img , sibr::Vector2i & pos, sibr::Vector3ub & color , int alpha = 0, bool keepChanges = false );

	void outLinePixel( const LocalPos & lpos , const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), bool forceFilling = false, int thickness=1 );
	void outLineImage( int globalImgId, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int thickness=1 );

	void drawPixel( const sibr::Vector2i & fullResPos , bool filled , const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int thickness=1);
	void drawPixel(const sibr::Vector2f & fullResPos, const sibr::Vector3ub & color = sibr::Vector3ub(255, 0, 0), int thickness = 1);

	void drawEllipse( const sibr::Vector2i & center, const sibr::Vector2f & firstAxis, const sibr::Vector2f & secondAxis, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int thickness=1  );

	void clampPos( sibr::ImageRGB & img, std::vector<sibr::Vector2i> & inPositions );
	void clampPos( sibr::ImageRGB & img , sibr::Vector2i & position );
	bool isImgIdValid( int imgid );

	static float interpolate( float val, float y0, float x0, float y1, float x1 );
	static float base( float val );


	virtual Img & whatToZoomIn( void );

	void selectionInputPoll(void);
	virtual void preInputPoll(void) { }
	void printPixelInfo(LocalPos & lPos, sibr::Vector3ub color=sibr::Vector3ub(255,0,0) );
	void updateInterfaceImage(Img & newImage);

	void updateMouseState(int event);

	static void staticCallBackFunction(int event, int x, int y, int flags, void* userdata);

	virtual void callBackFunction(int event, LocalPos & lPos );
	virtual sibr::Vector3ub filterPos(LocalPos & lPos, sibr::Vector3ub posColor, float scaleFactor);

public:
	DebugInterface(void) : viewingMode(ViewingMode::ALL), keychanged(false){}

	void display( std::vector<sibr::ImageRGB*> & imgs , int waitKeyDelay = 0 , ViewingMode viewingMode = ViewingMode::ALL, sibr::Vector2i dimInterface = DebugInterface::baseDimension ,
		std::string interface_name = "debug",  CallBackFunction func = staticCallBackFunction );

	void display( std::vector<sibr::ImageRGB32F*> & imgs, int waitKeyDelay = 0, ViewingMode viewingMode = ViewingMode::ALL, sibr::Vector2i dimInterface = DebugInterface::baseDimension,
		std::string interface_name = "debug",  CallBackFunction func = staticCallBackFunction );

	void display(std::vector<std::shared_ptr<sibr::ImageRGB32F> > & imgs, int waitKeyDelay = 0, ViewingMode viewingMode = ViewingMode::ALL, sibr::Vector2i dimInterface = DebugInterface::baseDimension,
		std::string interface_name = "debug", CallBackFunction func = staticCallBackFunction);

	void display( std::vector<Img> & imgs, int waitKeyDelay = 0, ViewingMode viewingMode = ViewingMode::ALL, sibr::Vector2i dimInterface = DebugInterface::baseDimension,
		std::string interface_name = "debug",  CallBackFunction func = staticCallBackFunction );

	
	static sibr::Vector3ub jetColor(float gray);
	static sibr::Vector3ub jetColor2(float gray);
	static sibr::Vector3f jetColor3(float gray);

	static sibr::Vector2i baseDimension;

	~DebugInterface(void) {}

};

#endif // DEBUGINTERFACE_H