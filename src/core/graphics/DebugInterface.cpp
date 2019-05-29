#include "DebugInterface.h"
#include <algorithm>
#include <thread>

sibr::Vector2i DebugInterface::baseDimension = sibr::Vector2i(1600,1200); 
int ZoomData::selectionTreshold = 5;

void DebugInterface::printHelp( void )
{
	std::cout << "[DebugInterface] help : "<< std::endl;
	std::cout << "\t  ESC            :  quit"<< std::endl;
	std::cout << "\t  w              :  to print the help again "<< std::endl;
	std::cout << "\t  [ or ]         :  make the window bigger/smaller"<< std::endl;
	std::cout << "\t z               :  to zoom, press once then draw a rectangle to zoom on. Then press twice to quit zoom "<< std::endl;
	std::cout << "\t s               :  selection mode, select a subset of view to display by clicking on them, use + or - to increase/decrease the number of views "<< std::endl;
	std::cout << "\t number key >= 1 :  switch to single view mode. Press 0 to return to all views display "<< std::endl;
	std::cout << "\t  + or -         :  switch to next/previous view in single view mode"<< std::endl;
}

void DebugInterface::display( std::vector<sibr::ImageRGB::Ptr> & imgs, int waitKeyDelay , ViewingMode _viewingMode, sibr::Vector2i dim_interface, std::string interface_name, CallBackFunction func)
{
	std::cout<<"[DebugInterface] interface <" << interfaceName << "> created" << std::endl;
	printHelp();
	interfaceName = interface_name;
	inputDimInterface = dim_interface;
	/// \todo TODO ; change this
	dimInterface = inputDimInterface;
	inputImgsPtrs = imgs;
	viewingMode = _viewingMode;
	selectionData = SelectionViewData((int)inputImgsPtrs.size());
	
	previousState.mode = viewingMode;
	previousState.event = 0;

	cv::namedWindow(interfaceName);
	//updateInterfaceImage( concatImgs( imgs ) );
	setInterfaceImg(inputImgsPtrs, dimInterface);

	cv::setMouseCallback(interfaceName, func, this);


	preInputPoll();
	do {
		whichKey = cvWaitKey(waitKeyDelay);

		//std::this_thread::sleep_for( std::chrono::milliseconds(50) );

		//std::cout << " cv " << std::flush;
		//std::cout << "[" << interfaceImg->w() << "," << interfaceImg->h() << "] " << std::flush;

		inputPoll(whichKey);

	} while( whichKey != 27 ); //ascii for ESC

	cvDestroyWindow(interfaceName.c_str());
	std::cout<<"[DebugInterface] interface <"<< interfaceName << "> deleted" << std::endl;
}

void DebugInterface::display( std::vector<sibr::ImageRGB32F*> & imgs, int waitKeyDelay , ViewingMode _viewingMode, sibr::Vector2i dim_interface, std::string interface_name, CallBackFunction func)
{
	std::vector<Img> imgsConverted(imgs.size());
	std::vector<sibr::ImageRGB*> imgsPtr;
	int id = 0;
	for(auto & imgCvt : imgsConverted){
		//imgCvt = Img(new sibr::ImageRGB( (imgs.at(id))->cast<sibr::ImageRGB>().clone() ));
		imgCvt = std::make_shared<sibr::ImageRGB>(imgs.at(id)->cast<sibr::ImageRGB>().clone());
		imgsPtr.push_back(imgCvt.get());
		++id;
	}
	//std::cout<<"[DebugInterface] conversion done" << std::endl;

	display(imgsPtr,waitKeyDelay,_viewingMode,dim_interface,interface_name,func);
}

void DebugInterface::display( std::vector<sibr::ImageRGB *> & imgs, int waitKeyDelay , ViewingMode _viewingMode, sibr::Vector2i dim_interface, std::string interface_name,  CallBackFunction func )
{
	std::vector<sibr::ImageRGB::Ptr> imgsPtr;
	for( auto & img : imgs ){
		imgsPtr.push_back(img);
	}
	display(imgsPtr,waitKeyDelay,_viewingMode,dim_interface,interface_name,func);
}


void DebugInterface::display(std::vector<std::shared_ptr<sibr::ImageRGB32F> > & imgs, int waitKeyDelay, ViewingMode viewingMode, sibr::Vector2i dimInterface,
	std::string interface_name, CallBackFunction func)
{
	std::vector<sibr::ImageRGB32F*> imgsPtr;
	for (auto & img : imgs) {
		imgsPtr.push_back(img.get());
	}
	display(imgsPtr, waitKeyDelay, viewingMode, dimInterface, interface_name, func);
}

void DebugInterface::setInterfaceImg(  std::vector<sibr::ImageRGB::Ptr> & imgs , const  sibr::Vector2i & dimInterface )
{
	inputImgsPtrs = imgs;
	computeFullResImgs( inputImgsPtrs );
	computeInterfaceImgFromFullRes(dimInterface);
}

void DebugInterface::computeInterfaceImgFromFullRes( const  sibr::Vector2i & newDimInterface )
{
	double ratio = dimImage.x()/dimImage.y();
	float scaleFactorX = newDimInterface.x()/(float)(dimImage.x()*dimGrid.x());
	float scaleFactorY = newDimInterface.y()/(float)(dimImage.y()*dimGrid.y());
	
	float scaleFactor = std::min(scaleFactorX, scaleFactorY);

	//dimImgRescaled = sibr::Vector2i( (int)std::floor(dimImage.x()*scaleFactor), (int)std::floor(dimImage.y()*scaleFactor));
	dimImgRescaled = sibr::Vector2i( (int)std::floor(dimImage.x()*scaleFactor), (int)std::floor(dimImage.y()*scaleFactor));
	dimInterface = (dimGrid.array() * dimImgRescaled.array()).matrix();

	//std::cout << dimImage.transpose() << " " << dimImgRescaled.transpose() << " " << dimInterface.transpose() << std::endl;

	imgsRescaled = std::vector<Img>( inputImgsPtrs.size());
	int imId = 0;
	for( auto & img : imgsRescaled ){
		//show( *inputImgsPtrs.at(imId) );
		img = Img( new sibr::ImageRGB( inputImgsPtrs.at(imId)->resized( dimImgRescaled.x(),dimImgRescaled.y(), (scaleFactor >  1.0f ? cv::INTER_NEAREST : cv::INTER_LINEAR) ) ) );
		++imId;
	}

	Img newInterfaceImg = 	Img( new sibr::ImageRGB( dimInterface.x(),  dimInterface.y() ) );

	for(int y=0; y<dimInterface.y(); y++){
		for(int x=0; x<dimInterface.x(); ++x){	
			LocalPos lpos(toLocalRescaled(sibr::Vector2i(x,y)));

			if( lpos.isDefined ) {
				newInterfaceImg(x,y) = filterPos(lpos, imgsRescaled.at(lpos.imgId)(lpos.pos.x(), lpos.pos.y()), scaleFactor);
			} else {
				//checkers texture
				unsigned char c = (unsigned char) std::floor(255* (0.75+0.25*((x/10+y/10)%2) ) );
				newInterfaceImg(x,y) = sibr::Vector3ub(c,c,c);
			}
			
		}		
	}

	draw.resetPosPainted();
	updateInterfaceImage( newInterfaceImg );	

}

void DebugInterface::computeFullResImgs( std::vector<sibr::ImageRGB::Ptr> & imgs )
{
	//numImages = (int)imgs.size();

	numImages = selectionData.numSelected;

	std::cout << " numImages : " << numImages << std::endl;
	if( numImages == 0 ) { return ;}

	dimGrid = squareGrid(numImages);

	std::cout << " dim grid : " << dimGrid << std::endl;

	dimImage = sibr::Vector2i(imgs.at(0)->w(),imgs.at(0)->h());
	dimImgsFullRes = (dimGrid.array()*dimImage.array());

	fullResImg = Img( new sibr::ImageRGB( dimImgsFullRes.x(),  dimImgsFullRes.y() ) );
#pragma omp parallel for
	for( int it=0; it<numImages; it++){
	//for( int imId=0; imId<numImages; imId++){
		//sibr::ImageRGB & img = *imgs.at(imId);
		sibr::ImageRGB & img = *imgs.at(selectionData.selectedViewMapping.at(it));
		int grid_x = it % dimGrid.x();
		int grid_y = it / dimGrid.x();
		sibr::Vector2i topLeft( img.w()*grid_x, img.h()*grid_y );

		for(int j=0; j<(int)img.h(); ++j){
			for(int i=0; i<(int)img.w(); ++i){
				fullResImg(topLeft.x()+i,topLeft.y()+j) = img(i,j);
			}
		}
	}
	zoomData.reset(fullResImg);

}

/*
Img DebugInterface::concatImgs(  std::vector<sibr::ImageRGB*> & imgs )
{
	numImages = (int)imgs.size();
	dimInterface = inputDimInterface;

	if( numImages == 0 ) {
		return Img(new sibr::ImageRGB(0,0) );
	}

	dimGrid = squareGrid(numImages);

	std::cout << " dim grid : " << dimGrid << std::endl;

	dimImage = sibr::Vector2i(imgs.at(0)->w(),imgs.at(0)->h());

	double ratio = imgs.at(0)->w()/(double)imgs.at(0)->h();
	float scaleFactorX = dimInterface.x()/(float)(imgs.at(0)->w()*dimGrid.x());
	float scaleFactorY = dimInterface.y()/(float)(imgs.at(0)->h()*dimGrid.y());
	
	float scaleFactor = std::min(scaleFactorX, scaleFactorY);

	dimImgRescaled = sibr::Vector2i( (int)std::floor(imgs.at(0)->w()*scaleFactor), (int)std::floor(imgs.at(0)->h()*scaleFactor));
	dimInterface = (dimGrid.array() * dimImgRescaled.array()).matrix();
	
	dimImgsFullRes = (dimGrid.array()*dimImage.array());

	
	//std::cout << dimInterface.transpose() << std::endl;
	//std::cout << dimImage.transpose() << std::endl;
	//std::cout << dimImgRescaled.transpose() << std::endl;
	//std::cout << dimGrid.transpose() << std::endl;
	

	imgsRescaled = std::vector<Img>( imgs.size());
	int imId = 0;
	for( auto & img : imgsRescaled ){
		img = Img( new sibr::ImageRGB( imgs.at(imId)->resized( dimImgRescaled.x(),dimImgRescaled.y(), (scaleFactor >  1.0f ? cv::INTER_NEAREST : cv::INTER_LINEAR) ) ) );
		++imId;
	}

	Img output( new sibr::ImageRGB( dimInterface.x(),  dimInterface.y() ) );

	fullResImg = Img( new sibr::ImageRGB( dimImgsFullRes.x(),  dimImgsFullRes.y() ) );
#pragma omp parallel for
	for( int imId=0; imId<numImages; imId++){
		sibr::ImageRGB & img = *imgs.at(imId);
		int grid_x = imId % dimGrid.x();
		int grid_y = imId / dimGrid.x();
		sibr::Vector2i topLeft( img.w()*grid_x, img.h()*grid_y );

		for(int j=0; j<(int)img.h(); ++j){
			for(int i=0; i<(int)img.w(); ++i){
				fullResImg(topLeft.x()+i,topLeft.y()+j) = img(i,j);
			}
		}
	}
	zoomData.reset(fullResImg);

	for(int y=0; y<dimInterface.y(); y++){
		for(int x=0; x<dimInterface.x(); ++x){	
			LocalPos lpos(toLocalRescaled(sibr::Vector2i(x,y)));

			if( lpos.isDefined ) {
				output(x,y) = imgsRescaled.at(lpos.imgId)(lpos.pos.x(), lpos.pos.y());
			} else {
				unsigned char c = (unsigned char) std::floor(255* (0.75+0.25*((x/10+y/10)%2) ) );
				output(x,y) = sibr::Vector3ub(c,c,c);
			}
			
		}		
	}
	return output;
} 
*/

void DebugInterface::inputPoll( int key )
{
	//std::cout << " key DI : " << key << " mode : " <<  viewingMode << std::endl;
	keychanged = true;

	if( key == 'w' ) { printHelp(); }

	if( key == (int)'z' ){		
		if( viewingMode != ZOOM ){
			std::cout << " zoom in " << std::endl;
			clearImgChanges();
			zoomData.getSelection(interfaceImgActive);
			viewingMode = ZOOM;
		} else {
			std::cout << " zoom out " << std::endl;
			updateInterfaceImage( zoomData.prevImg );		
			zoomData.reset( fullResImg );

			viewingMode = previousState.mode;	
			previousState.forceClickOut = true;
			mouseActivity.isActivated = true;
			callBackFunction(previousState.event,previousState.localPos);
			mouseActivity.isActivated = false;
		}
	} 

	if( key == '[' ){
		dimInterface = (0.9*dimInterface.cast<double>()).cast<int>();
		computeInterfaceImgFromFullRes( dimInterface );
	}
		if( key == ']' ){
		dimInterface = (1.1*dimInterface.cast<double>()).cast<int>();
		computeInterfaceImgFromFullRes( dimInterface );
	}

	 // + or -
	if( key == 43 || key == (int)45 ){
		int diff = (44-key);
		if( viewingMode == SINGLE ) { /* if single image view move to next or previous image */
			current_img = std::max(0,std::min((int)inputImgsPtrs.size()-1, current_img + diff));
			key = (int)'1' + current_img;
		} else if ( viewingMode == SELECTION ) { /* if selection, more images to chose */
			selectionData.numImgsToSelect = std::max(2,std::min((int)inputImgsPtrs.size()-1, selectionData.numImgsToSelect + diff));
			std::cout << "  number of images to select : " << selectionData.numImgsToSelect << std::endl;
		}
	}

	/* display only one image */
	/* disabled for now */
	/*
	if( key >= (int)'1' && key <= (int)'9' ){
		int key_int = key - (int)'1';
		if( key_int<(int)inputImgsPtrs.size()){
			//updateInterfaceImage( concatImgs( std::vector<sibr::ImageRGB*>(1,inputImgsPtrs.at(key_int)) ) );
			setInterfaceImg( std::vector<sibr::ImageRGB*>(1,inputImgsPtrs.at(key_int)), dimInterface );
			selectionData.initMappings(key_int);
			current_img = key_int;
			std::cout << " current image : " << current_img << std::endl;
			viewingMode = SINGLE;
		}
	}
	*/

	/* to select a subset of images to display */
	/* disabled for now */
	/*
	if( key == (int)'s'  ){
		selectionInputPoll();
	}
	*/

	/*reset to displaying all images*/
	if( key==(int)'0' ){
		selectionData.initMappings();
		setInterfaceImg( inputImgsPtrs, inputDimInterface );
		//updateInterfaceImage( concatImgs( inputImgsPtrs ) );	
		viewingMode = ALL;
		zoomData.zoomActive = false;
	}

}

void DebugInterface::staticCallBackFunction(int event, int x, int y, int flags, void* userdata)
{
	//std::cout << " <CB> " << std::flush;

	DebugInterface* dbi= (DebugInterface*)(userdata);
	sibr::Vector2i screenPos(x,y);
	LocalPos localPos( dbi->toImgPix( screenPos ) );
	dbi->updateMouseState(event);

	if(dbi->mouseActivity.isActivated){ 
		dbi->clearImgChanges(); 
	} 

	dbi->callBackFunction(event,localPos);
	
	if( dbi->mouseActivity.isActivated && dbi->viewingMode !=ZOOM ){
		dbi->previousState.mode = dbi->viewingMode;
		dbi->previousState.event = event;
		dbi->previousState.localPos = localPos;
		
		/*
		if( localPos.isDefined ){
			std::cout << "previous pixel infos : " << std::endl;
			std::cout << "\t image : " << localPos.imgId << std::endl;
			std::cout << "\t pixel : " << localPos.pos.transpose() << std::endl;
		} else {
			std::cout << "no pixel info" << std::endl;
		}
		*/

		if( dbi->previousState.forceClickOut ) {
			dbi->mouseActivity.isActivated = false;
			dbi->mouseActivity.changedStatus = false;
			dbi->previousState.forceClickOut = false;
		}
	}

	//std::cout << " mstate : " << dbi->previousState.mouseWasActivated << std::endl;
	dbi->keychanged = false; 

}

void DebugInterface::callBackFunction(int event, LocalPos & lPos )
{
	
	MouseActivity& m = mouseActivity;
	//sibr::Vector2i screenPos( toScreenPos( lPos ) );


	//std::cout << " lpos : " << lPos.imgId << " , " << lPos.pos << std::endl;
	//std::cout << " fullResPos : " << fullResPos << std::endl;

	switch(viewingMode){

	case SUBMODE : {
		//std::cout<<"s"<<std::endl;
		return;
				   }

	case SELECTION : {
		if(!m.isActivated){ return ; }	
		updateSelection(lPos);
		previousLPos = lPos;
		printSelectionStatus();
		break;
					 }
	case ZOOM : {
		if( !lPos.isDefined ) { return; }
		sibr::Vector2i fullResPos( toFullResFromLPos( lPos ) );
		if( zoomSelection(fullResPos) ) { return ; }
		//zoomCallBack(fullResPos);
		break;
				}

	default : {
		if(!m.isActivated && !keychanged){ return ; }
		printPixelInfo(lPos);
		refreshScreen();
			  }
	}


}

sibr::Vector3ub DebugInterface::filterPos(LocalPos & localPos, sibr::Vector3ub posColor, float scaleFactor) {
	
	return posColor;
}

sibr::Vector2i DebugInterface::squareGrid( int n)
{
	int n_x = (int)std::ceil(std::sqrt(n));
	int n_y = n_x;
	while( n <= n_x*n_y ) { --n_y; }
	++n_y;
	return sibr::Vector2i(n_x,n_y);
}

/*
LocalPos DebugInterface::toLocal( sibr::Vector2i & gpos )
{
	if( zoomData.zoomActive ){
		gpos = zoomData.zoomToGlobal(gpos);
	}
	float ratio_x = gpos.x()/((float)dimImgRescaled.x());
	float ratio_y = gpos.y()/((float)dimImgRescaled.y());
	int local_x = (int)std::floor( (dimImage.x())*(ratio_x - std::floor(ratio_x)) );
	int local_y = (int)std::floor( (dimImage.y())*(ratio_y - std::floor(ratio_y)) );
	int img_id = (int)std::floor(ratio_x) + dimGrid.x()*(int)std::floor(ratio_y);
	sibr::Vector2i localPos(local_x, local_y);
	return ( (0 <= img_id && img_id < numImages && ratio_x<(float)dimGrid.x() ) ? LocalPos(localPos,img_id) : LocalPos() );
}
*/

int DebugInterface::toScreenImgIdFromGlobalImgId( int globalImgId )
{
	return selectionData.selectedViewInverseMapping.at(globalImgId);
}

int DebugInterface::toGlobalImgIdFromScreenImgId( int screenImgId )
{
	if( screenImgId >= 0 && screenImgId<(int)selectionData.selectedViewMapping.size()) {
		return selectionData.selectedViewMapping.at(screenImgId);
	} else {
		return -1;
	}
}

sibr::Vector2i DebugInterface::toFullResFromScreen( const sibr::Vector2i & screenPos )
{
	return zoomData.topLeft.array() + (zoomData.boxSize.array()*screenPos.array())/dimInterface.array();
}

sibr::Vector2i DebugInterface::toScreenFromFullRes( const sibr::Vector2i & fullResPos )
{
	return ( (fullResPos - zoomData.topLeft).array()*dimInterface.array() )/zoomData.boxSize.array();
}

sibr::Vector2f DebugInterface::toScreenFromFullResFloat(const sibr::Vector2f & fullResPos)
{
	return ((fullResPos - zoomData.topLeft.cast<float>()).array()*dimInterface.cast<float>().array()) / zoomData.boxSize.cast<float>().array();
}

LocalPos DebugInterface::toImgPix( const sibr::Vector2i & screenPos )
{
	sibr::Vector2i fullResPos( toFullResFromScreen(screenPos) );
	//std::cout << " fullResPos : " << fullResPos << std::endl;
	sibr::Vector2i imgPos( fullResPos.x() % dimImage.x(), fullResPos.y() % dimImage.y() );
	sibr::Vector2i bins( fullResPos.array() / dimImage.array() );
	int screen_img_id = bins.x() + dimGrid.x()*bins.y();
	//std::cout << " screen_img_id : " << screen_img_id << std::endl;
	int global_img_id = toGlobalImgIdFromScreenImgId( screen_img_id );
	if( (0 <= screen_img_id && screen_img_id < numImages && bins.x()<dimGrid.x() ) && global_img_id>= 0 ){
		return LocalPos(imgPos,global_img_id);
	} else {
		return LocalPos();
	}
}

sibr::Vector2i DebugInterface::toFullResFromLPos( const LocalPos & lpos )
{
	if( !lpos.isDefined ) { std::cout << " [DebugInterface] toFullResFromLPos() warning : undefined local pos cannot be converted to full res pos" << std::endl; }

	const sibr::Vector2i & imgPos = lpos.pos;
	int global_img_id = lpos.imgId;
	int screen_img_id = toScreenImgIdFromGlobalImgId( global_img_id );
	bool isVisible = lpos.isDefined && ( screen_img_id >= 0 );
	sibr::Vector2i bins( screen_img_id % dimGrid.x(), screen_img_id / dimGrid.x() );

	return ( lpos.isDefined ? bins.array()*dimImage.array() + imgPos.array() : sibr::Vector2i() );
}

sibr::Vector2f DebugInterface::toFullResFloatFromLPos(int imId, const sibr::Vector2f & pos)
{
	int screen_img_id = toScreenImgIdFromGlobalImgId(imId);
	sibr::Vector2i bins(screen_img_id % dimGrid.x(), screen_img_id / dimGrid.x());
	return bins.cast<float>().array()*dimImage.cast<float>().array() + pos.array();
}

sibr::Vector2i DebugInterface::toScreenPos( const LocalPos & lpos)
{
	if( !lpos.isDefined ) { std::cout << " [DebugInterface] toGlobal() warning : undefined local pos cannot be converted to global pos" << std::endl; }

	const sibr::Vector2i & imgPos = lpos.pos;
	int global_img_id = lpos.imgId;
	int screen_img_id = toScreenImgIdFromGlobalImgId( global_img_id );
	bool isVisible = lpos.isDefined && ( screen_img_id >= 0 );
	sibr::Vector2i bins( screen_img_id % dimGrid.x(), screen_img_id / dimGrid.x() );
	sibr::Vector2i fullResPos( bins.array()*dimImage.array() + imgPos.array() );

	return ( isVisible ? toScreenFromFullRes( fullResPos ) : sibr::Vector2i());
}

sibr::Vector2i DebugInterface::toScreenPosFromFloat( int imgId, const sibr::Vector2f & pos )
{
	int screen_img_id = toScreenImgIdFromGlobalImgId( imgId );
	bool isVisible = ( screen_img_id >= 0 );
	sibr::Vector2i bins( screen_img_id % dimGrid.x(), screen_img_id / dimGrid.x() );
	sibr::Vector2f fullResPos( (bins.array()*dimImage.array()).cast<float>() + pos.array() );

	sibr::Vector2f screenPosFloat((fullResPos - zoomData.topLeft.cast<float>()).array()*dimInterface.cast<float>().array() /zoomData.boxSize.cast<float>().array() );
	sibr::Vector2i screenPosInt = screenPosFloat.unaryExpr( [] (float f) { return sibr::round(f); } ).cast<int>();

	return ( isVisible ? screenPosInt : sibr::Vector2i(-1, -1));
}

LocalPos DebugInterface::toLocalRescaled( const sibr::Vector2i & gpos)
{
	sibr::Vector2i localPos( gpos.x() % dimImgRescaled.x(), gpos.y() % dimImgRescaled.y() ); 
	sibr::Vector2i bins(gpos.array()/dimImgRescaled.array());
	int img_id = bins.x() + dimGrid.x()*bins.y();
	int global_img_id = toGlobalImgIdFromScreenImgId(img_id);
	return ( (0 <= img_id && img_id < numImages && bins.x()<dimGrid.x() && global_img_id>=0 ) ? LocalPos(localPos,global_img_id) : LocalPos() );
}

sibr::Vector2f DebugInterface::getPixelCenterPositionFloat( const sibr::ImageRGB & img, const sibr::Vector2i & pos )
{
	sibr::Vector2f resolution( sibr::Vector2i(img.w(),img.h()).cast<float>() );
	sibr::Vector2f posCenter( ( pos.cast<float>() + sibr::Vector2f(0.5f,0.5f)).array()/resolution.array() ); //pos center in  [0,1]x[0,1]
	return 2.0f*posCenter-sibr::Vector2f(1,1);
}

void DebugInterface::updateMouseState(int event)
{
	MouseActivity & m = mouseActivity;
	m.changedStatus = false;
	if (event == CV_EVENT_LBUTTONDOWN && ! m.isActivated) { 
		m.isActivated = true;
		m.changedStatus = true;
		//std::cout << m.isActivated << " <> " << m.changedStatus << std::endl;
	}
	if (event == CV_EVENT_LBUTTONUP && m.isActivated) { 
		m.isActivated = false;
		m.changedStatus = true;
		//std::cout << m.isActivated << " <> " << m.changedStatus << std::endl;
	}
	
}

void DebugInterface::drawPixel( const sibr::Vector2i & fullResPos , bool filled, const sibr::Vector3ub & color, int thickness )
{
	sibr::Vector2i pixelSize = zoomData.ratios.cast<int>();
	sibr::Vector2i screenCenter( toScreenFromFullRes( fullResPos ) );
	sibr::Vector2i bottomRight( screenCenter + pixelSize ); 
	if( filled ){
		draw.rectangleFilled(screenCenter,bottomRight,color);
	} else {
		draw.rectangle(screenCenter, bottomRight, color, thickness);
	}
}

void DebugInterface::drawPixel(const sibr::Vector2f & fullResPos, const sibr::Vector3ub & color, int thickness )
{
	sibr::Vector2i pixelSize = zoomData.ratios.cast<int>();
	sibr::Vector2i screenPosition(toScreenFromFullResFloat(fullResPos).cast<int>());
	draw.pixel(screenPosition, color, float(thickness));
}

void DebugInterface::drawEllipse( const sibr::Vector2i & center, const sibr::Vector2f & firstAxis, const sibr::Vector2f & secondAxis, const sibr::Vector3ub & color, int thickness )
{
	float angle = 180.0f*(float)M_1_PI*std::atan2(firstAxis.y(),firstAxis.x());
	sibr::Vector2i resizeCenter = toScreenFromFullRes(center);
	float sizeFirstAxis = ( toScreenFromFullRes(center + firstAxis.cast<int>()).cast<float>() - resizeCenter.cast<float>() ).norm();
	float sizeSecondAxis = ( toScreenFromFullRes(center + secondAxis.cast<int>()).cast<float>() - resizeCenter.cast<float>() ).norm();
	sibr::Vector2i sizes( sizeFirstAxis,sizeSecondAxis);
	draw.ellipse( resizeCenter, sizes,angle,color,thickness);
}

/*
void DebugInterface::drawPatch( sibr::ImageRGB & img , sibr::Vector2i position , sibr::Vector3ub color, int radius, int thickness, bool keepChanges)
{
	 int x = position.x();
	 int y = position.y();
	
	 for( int t=0; t<=thickness; ++t){
		 int r = radius + t;
		 sibr::Vector2i tl( x-r, y-r);
		 sibr::Vector2i br( x+r, y+r);
		 drawRectangle( img, tl,br,color, keepChanges);
	 }
}

void DebugInterface::drawPatchF( sibr::ImageRGB & img , sibr::Vector2i position , sibr::Vector3f color, int radius, int thickness , bool keepChanges )
{
	sibr::Vector3ub colorUB( color.unaryExpr( [] (float f) { return std::floor((float)255.0*f); } ).cast<unsigned char>() );
	drawPatch( img, position, colorUB, radius, thickness,keepChanges);
}

void DebugInterface::drawRectangle(sibr::ImageRGB & img, sibr::Vector2i & ca, sibr::Vector2i & cb, sibr::Vector3ub color, int thickness, bool keepChanges )
{
	sibr::Vector2i tl = ca.cwiseMin(cb);
	sibr::Vector2i br = ca.cwiseMax(cb);

	if( thickness == 0 ){
		for(int i=tl.x(); i<br.x(); ++i) { printPixel( img, sibr::Vector2i(i,tl.y()), color, keepChanges); }
		for(int j=tl.y(); j<br.y(); ++j) { printPixel( img, sibr::Vector2i(br.x(),j), color, keepChanges); }
		for(int i=br.x(); i>tl.x(); --i) { printPixel( img, sibr::Vector2i(i,br.y()), color, keepChanges); }
		for(int j=br.y(); j>tl.y(); --j) { printPixel( img, sibr::Vector2i(tl.x(),j), color, keepChanges); }
	} else {
		for(int t=0; t<=thickness; ++t){
			drawRectangle(img, tl, br, color, 0 , keepChanges);
			tl += sibr::Vector2i(-1,-1);
			br += sibr::Vector2i( 1, 1);
		}
	}
}

void DebugInterface::drawFullRectangle( sibr::ImageRGB & img, sibr::Vector2i & ca, sibr::Vector2i & cb, sibr::Vector3ub color )
{
	int size = (cb-ca).cwiseAbs().minCoeff();
	sibr::Vector2i shift(1,1);
	sibr::Vector2i tl = ca.cwiseMin(cb);
	sibr::Vector2i br = ca.cwiseMax(cb);

	for(int t=0; t<size; ++t){
		drawRectangle(img,tl,br,color);
		tl += shift;
		br -= shift;
	}
}

void DebugInterface::drawCross( sibr::ImageRGB & img, sibr::Vector2i & center, sibr::Vector3ub color, int radius, int thickness )
{
	printPixel(img,center,color);
	for( int t=-thickness+1; t<thickness; ++t){
		sibr::Vector2i left  ( center + sibr::Vector2i( -radius, t ) );
		sibr::Vector2i top   ( center + sibr::Vector2i( t, +radius ) );
		sibr::Vector2i right ( center + sibr::Vector2i( +radius, t ) );
		sibr::Vector2i bott  ( center + sibr::Vector2i( t, -radius ) );
		for( int u=0; u<radius; ++u){
			printPixel(img,left,color);
			printPixel(img,top,color);
			printPixel(img,right,color);
			printPixel(img,bott,color);
			++left.x();
			--top.y();
			--right.x();
			++bott.y();
		}
	}
}
*/
 void DebugInterface::clampPos( sibr::ImageRGB & img, std::vector<sibr::Vector2i> & inPositions )
{
	for(auto & position : inPositions ){
		clampPos(img, position);
	}
}

void DebugInterface::clampPos( sibr::ImageRGB & img , sibr::Vector2i & position )
{
	position.x() = std::max( 0, std::min( (int)img.w()-1, position.x() ) );
	position.y() = std::max( 0, std::min( (int)img.h()-1, position.y() ) );
}


void DebugInterface::clearImgChanges(void)
{
	for( auto & pos : draw.getPosPainted() ){
		interfaceImgActive(pos.x(),pos.y()) = interfaceImg(pos.x(),pos.y());
	}
	draw.resetPosPainted();
}


void DebugInterface::updateInterfaceImage(Img & newImage)
{
	std::cout << "up" << std::endl;

	interfaceImg = newImage;
	interfaceImgActive = Img( new sibr::ImageRGB( interfaceImg->clone() ) );
	draw.setImageToDrawOn( interfaceImgActive );
	refreshScreen();
	//posPainted.clear();

}


bool DebugInterface::isVisibleFromScreen( sibr::ImageRGB & img , sibr::Vector2i & pos )
{
	return pos.x() >= 0 && pos.x() < (int)img.w() && pos.y() >= 0 && pos.y() < (int)img.h();
}

void DebugInterface::outLinePixel( const LocalPos & lPos , const sibr::Vector3ub & color, bool forceFilling, int thickness)
{
	if( !inputImgsPtrs.at(lPos.imgId)->isInRange( lPos.pos.x(), lPos.pos.y() ) ) { return; }

	sibr::Vector2i screenPos(toScreenPos(lPos));

	if( !isVisibleFromScreen(*interfaceImgActive,screenPos) ) { return; }
	float ratio = zoomData.ratios.maxCoeff();
	if( ratio < 5.0f && !forceFilling) {
		draw.crossSign(screenPos, color);
	} else {
		drawPixel(toFullResFromLPos(lPos), forceFilling, color);
	}
}

void DebugInterface::outLineImage( int globalImgId, const sibr::Vector3ub & color, int thickness )
{
	if( globalImgId<0 || globalImgId>= (int)inputImgsPtrs.size() ) { return; }
	sibr::ImageRGB & img = *inputImgsPtrs.at(globalImgId);
	sibr::Vector2i bottomRight(0,0);
	sibr::Vector2i topLeft(img.w()-1,img.h()-1);
	LocalPos tl( topLeft, globalImgId);
	LocalPos br( bottomRight, globalImgId);
	sibr::Vector2i screenTL( toScreenPos(  tl ) );
	sibr::Vector2i screenBR( toScreenPos(  br ) );

	draw.rectangle( screenTL , screenBR, color, thickness);
}

void DebugInterface::printPixelInfo(LocalPos & lPos, sibr::Vector3ub color)
{
	outLinePixel(lPos,color);

	//std::cout << " global pos : " << globalPos.transpose();
	if( lPos.isDefined ){
		std::cout << "clicked pixel infos : " << std::endl;
		std::cout << "\t image : " << lPos.imgId << std::endl;
		std::cout << "\t pixel : " << lPos.pos.transpose() << std::endl;
		std::cout << "\t color : " << inputImgsPtrs.at(lPos.imgId )(lPos.pos.x(),lPos.pos.y()).cast<int>().transpose() << std::endl;
	} else {
		std::cout << "no pixel info" << std::endl;
	}
}

void DebugInterface::updateSelection( LocalPos & lpos )
{
	if((  lpos.isDefined && (!previousLPos.isDefined || ( previousLPos.imgId != lpos.imgId ) ) ) || whichKey == (int)'a' ){
		if( whichKey == (int)'a' ) {
			std::cout << "all" << std::endl;
			selectionData.numImgsToSelect = (int)selectionData.imgsActivated.size();

			for( auto & act : selectionData.imgsActivated ){
				act = true;
			}
			selectionData.numSelected  = (int)selectionData.imgsActivated.size();
		} else {
			int imId = lpos.imgId;

			selectionData.imgsActivated.at(imId) = !selectionData.imgsActivated.at(imId);

			if( selectionData.imgsActivated.at(imId) ){
				//std::cout << "  view " << imId << " selected " << std::endl;
				selectionData.numSelected++;
			} else {
				//std::cout << "  view " << imId << " not selected anymore " << std::endl;
				selectionData.numSelected--;
			}
		}
		if( selectionData.numSelected != selectionData.numImgsToSelect ){ return; }

		std::cout << "td" << std::endl;
		selectionData.selectedViewMapping = std::vector<int>(selectionData.numImgsToSelect);
		selectionData.selectedViewInverseMapping = std::vector<int>(selectionData.numImgs,-1);
		std::vector<sibr::ImageRGB::Ptr> selectedImgsPtrs;

		std::cout << " imgs selected : ";
		int selectionId = 0;
		for(int imId = 0; imId<selectionData.numImgs; imId++ ){
			if(!selectionData.imgsActivated.at(imId)) { continue; }
			selectedImgsPtrs.push_back( inputImgsPtrs.at(imId) );
			std::cout << imId << " ";
			selectionData.selectedViewMapping.at(selectionId) = imId;
			selectionData.selectedViewInverseMapping.at(imId) = selectionId;
			selectionId++;
		}
		std::cout << std::endl;
		setInterfaceImg( inputImgsPtrs, inputDimInterface );
		//updateInterfaceImage( concatImgs( selectedImgsPtrs ) );
		viewingMode = SELECTED;
	}
}

void DebugInterface::printSelectionStatus( void )
{
	if( viewingMode != SELECTION ) { return; }

	sibr::Vector2i origin(0,0);
	sibr::Vector2i shift(10,10);
	sibr::Vector3ub green(0,255,0);
	sibr::Vector3ub red(255,0,0);

	for( int imId=0; imId<(int)selectionData.imgsActivated.size(); imId++){
		sibr::Vector2i position = toScreenPos(LocalPos(origin,imId)) + shift;
		draw.patch( position, (selectionData.imgsActivated.at(imId) ? green : red) );
	}
	refreshScreen();
}

bool DebugInterface::zoomSelection( sibr::Vector2i & fullResPos )
{
	ZoomData & z = zoomData;
	if( viewingMode != ZOOM || !z.isSelecting ){ return false; }

	MouseActivity & m = mouseActivity;

	if( m.isActivated && z.firstCornerSelected ) {
		draw.rectangle(toScreenFromFullRes(z.tempTopLeft)  , toScreenFromFullRes( fullResPos ) );
		refreshScreen();
	} else if ( m.changedStatus ){
		std::cout << " status changed" << std::endl;
		sibr::Vector2i screenPos( toScreenFromFullRes( fullResPos ) );
		sibr::Vector2i screenPosClamp ( screenPos.cwiseMax( sibr::Vector2i(0,0) ).cwiseMin( dimInterface-sibr::Vector2i(1,1) ) );
		sibr::Vector2i gPosFullRes( toFullResFromScreen(screenPosClamp) );
		if( m.isActivated && !z.firstCornerSelected){
			std::cout << " first c " << std::endl;
			z.firstCornerSelected = true;
			z.tempTopLeft = gPosFullRes;

		} else if( !m.isActivated && z.firstCornerSelected) {
			std::cout << " second c try " << std::endl;						
			sibr::Vector2i bottomRight(  z.tempTopLeft.cwiseMax(gPosFullRes) );
			sibr::Vector2i topLeft( z.tempTopLeft.cwiseMin(gPosFullRes) );			

			if( (bottomRight-topLeft).minCoeff() > z.selectionTreshold ){
				std::cout << " good " << std::endl;
				z.topLeft = topLeft;
				z.boxSize = bottomRight - topLeft;
				clearImgChanges();
							
				zoomData.ratios = dimInterface.cast<float>().array()/z.boxSize.array().cast<float>();
				
				std::cout << " RATIOS : " << zoomData.ratios << std::endl;

				cv::Mat subMat( fullResImg->toOpenCVBGR(), cv::Rect( topLeft.x(), topLeft.y(), z.boxSize.x(), z.boxSize.y() ));

				//cv::Mat subMat( whatToZoomIn()->toOpenCVBGR(), cv::Rect( z.topLeft.x(), z.topLeft.y(), z.zSize.x(), z.zSize.y() ));
				cv::Mat fullMat;
				cv::resize(subMat, fullMat, cv::Size(z.prevImg->w(),z.prevImg->h()), 0, 0, ( zoomData.ratios.maxCoeff() > 1 ? cv::INTER_NEAREST : cv::INTER_LINEAR  ) );			
				interfaceImgActive->fromOpenCVBGR(fullMat);
				updateInterfaceImage( interfaceImgActive );

				z.zoomActive = true;
				z.isSelecting = false;
				z.firstCornerSelected = false;

				viewingMode = previousState.mode;
				previousState.forceClickOut = true;
				mouseActivity.isActivated = true;
				//if( previousState.localPos.isDefined ){
					std::cout << " call zoom cb " << std::endl;
					callBackFunction(previousState.event,previousState.localPos);
				//}
				mouseActivity.isActivated = false;
				//mouseActivity.changedStatus = false;
				return true;
			} else {
				std::cout << " too close " << std::endl;
				z.firstCornerSelected = false;
			}
		}
	} 
	return false;
}

Img & DebugInterface::whatToZoomIn( void )
{
	return (zoomData.prevImg);
}

void DebugInterface::zoomCallBack( sibr::Vector2i & gPos )
{
	if( zoomData.zoomActive && mouseActivity.changedStatus ){
		LocalPos lPos( toImgPix(zoomData.zoomToGlobal(gPos)) );
		std::cout << lPos.isDefined << " : ";
		if( lPos.isDefined ){
			std::cout <<   lPos.imgId << ", " << lPos.pos.transpose(); 
		}
		std::cout << std::endl;
	}
}

void DebugInterface::refreshScreen()
{
	//std::cout << "imshow" << std::endl;
	cv::imshow(interfaceName,interfaceImgActive->toOpenCVBGR());
}

float DebugInterface::interpolate( float val, float y0, float x0, float y1, float x1 ) {
     return (val-x0)*(y1-y0)/(x1-x0) + y0;
}

float DebugInterface::base( float val ){
	if ( val <= -0.75 ) return 0;
	else if ( val <= -0.25 ) return interpolate( val, 0.0, -0.75, 1.0, -0.25 );
	else if ( val <= 0.25 ) return 1.0;
	else if ( val <= 0.75 ) return interpolate( val, 1.0, 0.25, 0.0, 0.75 );
	else return 0.0;
}

sibr::Vector3ub DebugInterface::jetColor(float gray)
{
	gray = 2.0f*(gray-0.5f);
	return sibr::Vector3f(base( gray + 0.5f ),base( gray ), base(gray - 0.5f )).unaryExpr( [] (float f) { return std::floor((float)255.0*f); } ).cast<unsigned char>();
}

sibr::Vector3ub DebugInterface::jetColor2(float gray)
{
	return jetColor3(gray).unaryExpr( [] (float f) { return std::floor((float)255.0*f); } ).cast<unsigned char>();
}

sibr::Vector3f DebugInterface::jetColor3(float gray)
{
	if (gray < 0) { return sibr::Vector3f(0, 0, 0); };

	sibr::Vector3f output(1,1,1);
	float g = std::min(1.0f,std::max(0.0f,gray));
	float dg = 0.25f;
	float d = 4.0f;
	if( g<dg ){ output.x() = 0.0f; output.y() = d*g; }
	else if ( g< 2.0f*dg ) { output.x() = 0.0f; output.z() = 1.0f + d*(dg-g); }
	else if ( g< 3.0f*dg ) { output.x() = d*(g-0.5f);  output.z() = 0.0f; }
	else { output.y() = 1.0f + d*(0.75f-g);  output.z() = 0.0f; }

	return output;
}

void DebugInterface::selectionInputPoll(void)
{
	if( viewingMode == SELECTION ){
		std::cout << " selection desactivated " << std::endl;
		//updateInterfaceImage( concatImgs( inputImgsPtrs ) );
		setInterfaceImg( inputImgsPtrs, dimInterface );
		viewingMode = ALL;
	} else if( viewingMode == ALL ) {
		std::cout << " selection activated " << std::endl;
		viewingMode = SELECTION;
		selectionData.reset();
		std::cout << "  number of images to select : " << selectionData.numImgsToSelect << std::endl;
		clearImgChanges();
		printSelectionStatus();
		refreshScreen();
	}
}