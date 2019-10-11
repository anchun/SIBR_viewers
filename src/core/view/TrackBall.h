#ifndef SIBR_TRACKBALL_H
#define SIBR_TRACKBALL_H

#include <memory>
#include <fstream>

#include "Config.hpp"
#include "core/graphics/Shader.hpp"
#include "core/assets/InputCamera.hpp"
#include "ICameraHandler.hpp"

namespace sibr {

	class Viewport;
	class Mesh;
	class Input;
	class Raycaster;

	enum class TrackBallState { IDLE, TRANSLATION_PLANE, TRANSLATION_Z, ROTATION_SPHERE, ROTATION_ROLL };
	
	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT TrackBall : public ICameraHandler
	{
	public:
		TrackBall( bool verbose = false );

		bool load( std::string & filePath ,  const Viewport & viewport);
		void save( std::string & filePath ) const ;

		void fromCamera( const InputCamera & cam , const Viewport & viewport , const float & radius = 100.0f );
		bool fromMesh( const Mesh & mesh, const Viewport & viewport );

		bool fromBoundingBox(const Eigen::AlignedBox<float, 3>& box, const Viewport & viewport);

		void update( const Input & input , const Viewport & viewport, std::shared_ptr<Raycaster> raycaster = std::shared_ptr<Raycaster>());
		void updateAspectWithViewport(const Viewport & viewport);

		/// Legacy method, will be removed soon. Use onRender(viewport) instead.
		void draw( const Viewport & viewport );

		bool						isActive;
		bool						drawThis;

		bool						initialized() const { return hasBeenInitialized; }

		/// ICameraHandler interface
		virtual void update(const sibr::Input & input, const float deltaTime, const Viewport & viewport) override;

		virtual const InputCamera & getCamera(void) const override;

		virtual void onRender(const sibr::Viewport & viewport) override;

	private:
		Vector3f mapToSphere( const Vector2i & pos2D, const Viewport & viewport ) const;
		Vector3f mapTo3Dplane( const Vector2i & pos2D ) const;

		void updateZnearZFar( const Input & input );

		void updateBallCenter( const Input & input,  std::shared_ptr<Raycaster> raycaster );
		void updateRadius( const Input & input );
		void updateRotationSphere( const Input & input , const Viewport & viewport );
		void updateRotationRoll( const Input & input , const Viewport & viewport );

		void updateTranslationPlane( const Input & input , const Viewport & viewport, std::shared_ptr<Raycaster> raycaster = std::shared_ptr<Raycaster>() );
		void updateTranslationZ( const Input & input , const Viewport & viewport );

		void updateFromKeyboard(const Input & input);

		bool isInTrackBall2dRegion( const Vector2i & pos2D, const Viewport & viewport ) const;

		bool areClockWise( const Vector2f & a, const Vector2f & b, const Vector2f & c ) const;

		void printMessage( const std::string & msg ) const;

		void saveVectorInFile( std::ofstream & s , const Vector3f & v ) const ;
		void initTrackBallShader( void );

		void setCameraAttributes( const Viewport & viewport );

		void updateTrackBallCameraSize(const Viewport & viewport);

		void updateTrackBallStatus( const Input & input, const Viewport & viewport );
		void updateTrackBallCamera( const Input & input, const Viewport & viewport ,  std::shared_ptr<Raycaster> raycaster = std::shared_ptr<Raycaster>() );


		InputCamera					fixedCamera;	
		InputCamera					tempCamera;

		Vector3f					fixedCenter;
		Vector3f					tempCenter;

		Vector2i					lastPoint2D;
		Vector2i					currentPoint2D;

		Eigen::Hyperplane<float,3>	trackballPlane;

		TrackBallState				state;

		bool						hasBeenInitialized;
		bool						verbose;

		float						zoom=1.0f;//zoom factor used for ortho cams
		//members used for interaction drawing
		std::shared_ptr<Mesh>		quadMesh;
		GLShader					trackBallShader;
		GLParameter					ratioTrackBall2Dgpu;
		GLParameter					trackBallStateGPU;
		bool						shadersCompiled;
		static float				ratioTrackBall2D;

	};

}
#endif // SIBR_TRACKBALL_H