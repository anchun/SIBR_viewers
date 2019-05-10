
#include "core/graphics/Shader.hpp"
#include "core/graphics/RenderUtility.hpp"
#include "core/graphics/Window.hpp"

#define SIBR_WRITESHADER(src) "#version 420 core\n" #src


//#define RenderUtility::camStubDrawSize() 0.10f



namespace sibr
{

	float RenderUtility::_camStubSize;

	static const std::vector<float>&	getCameraStubVertices( void )
	{
		static std::vector<float> _vBuffer;

		if (_vBuffer.empty()) {
			_vBuffer.resize(3*5);

			_vBuffer[3*0+0]= 1*RenderUtility::camStubDrawSize(); _vBuffer[3*0+1]= 1*RenderUtility::camStubDrawSize(); _vBuffer[3*0+2]=-3*RenderUtility::camStubDrawSize();
			_vBuffer[3*1+0]=-1*RenderUtility::camStubDrawSize(); _vBuffer[3*1+1]= 1*RenderUtility::camStubDrawSize(); _vBuffer[3*1+2]=-3*RenderUtility::camStubDrawSize();
			_vBuffer[3*2+0]=-1*RenderUtility::camStubDrawSize(); _vBuffer[3*2+1]=-1*RenderUtility::camStubDrawSize(); _vBuffer[3*2+2]=-3*RenderUtility::camStubDrawSize();
			_vBuffer[3*3+0]= 1*RenderUtility::camStubDrawSize(); _vBuffer[3*3+1]=-1*RenderUtility::camStubDrawSize(); _vBuffer[3*3+2]=-3*RenderUtility::camStubDrawSize();
			_vBuffer[3*4+0]= 0*RenderUtility::camStubDrawSize(); _vBuffer[3*4+1]= 0*RenderUtility::camStubDrawSize(); _vBuffer[3*4+2]= 0*RenderUtility::camStubDrawSize();
		}
		return _vBuffer;
	}
	static const std::vector<uint>&		getCameraStubIndices( void )
	{
		static std::vector<uint>	_iBuffer;

		if (_iBuffer.empty())
		{
			_iBuffer.resize(3*6);

			_iBuffer[3*0+0]=0; _iBuffer[3*0+1]=1; _iBuffer[3*0+2]=4;
			_iBuffer[3*1+0]=1; _iBuffer[3*1+1]=2; _iBuffer[3*1+2]=4;
			_iBuffer[3*2+0]=2; _iBuffer[3*2+1]=4; _iBuffer[3*2+2]=3;
			_iBuffer[3*3+0]=0; _iBuffer[3*3+1]=4; _iBuffer[3*3+2]=3;
			_iBuffer[3*4+0]=0; _iBuffer[3*4+1]=1; _iBuffer[3*4+2]=3;
			_iBuffer[3*5+0]=1; _iBuffer[3*5+1]=2; _iBuffer[3*5+2]=3;
		}
		return _iBuffer;
	}


	void RenderUtility::sendVertsTexToGPU(GLuint vertTexVBO, GLfloat vert[], GLfloat tcoord[], int svert, int stcoord) {
		glBindBuffer(GL_ARRAY_BUFFER, vertTexVBO);
		glBufferData(GL_ARRAY_BUFFER, svert+stcoord, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, svert, vert);
		glBufferSubData(GL_ARRAY_BUFFER, svert, stcoord, tcoord);
	}


	/*static*/ void		RenderUtility::renderScreenQuad( bool reverse, GLfloat tex_coor[] )
	{
		static GLfloat vert[] = { -1,-1,0,  1,-1,0,  1,1,0,  -1,1,0 };
		static GLfloat tcoord[8];
		static GLuint indexVBO, VAO, vertTexVBO;

		static bool firstTime = true;
		
		if(reverse)
		{
			GLfloat tmp[] = { 0,1,  0,0,  1,0,  1,1 };
			if( tex_coor )
				std::memcpy(tmp, tex_coor, sizeof tmp );

			std::memcpy(tcoord, tmp, sizeof tcoord);
			if( !firstTime )  // re-transfer to GPUs
				sendVertsTexToGPU(vertTexVBO, vert, tcoord, sizeof(vert), sizeof(tcoord));
		}
		else
		{
			GLfloat tmp[] = { 0,0,  1,0,  1,1,  0,1 };
			if( tex_coor )
				std::memcpy(tmp, tex_coor, sizeof tmp );	

			std::memcpy(tcoord, tmp, sizeof tcoord);
			if( !firstTime )  // re-transfer to GPU
				sendVertsTexToGPU(vertTexVBO, vert, tcoord, sizeof(vert), sizeof(tcoord));
		}

		static GLuint  ind[] = { 0,1,2,  0,2,3 };

		if( firstTime ) {
			firstTime = false;

			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);

			glGenBuffers(1, &indexVBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind), ind, GL_STATIC_DRAW);

			glGenBuffers(1, &vertTexVBO);
			sendVertsTexToGPU(vertTexVBO, vert, tcoord, sizeof(vert), sizeof(tcoord));
		}

		glBindVertexArray(VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertTexVBO);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vert)	);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}

	void		RenderUtility::renderScreenQuad()
	{
		static GLfloat Fvert[] = { -1,-1,0,  1,-1,0,  1,1,0,  -1,1,0 };
		static GLfloat Ftcoord[] = { 0, 0, 1, 0, 1, 1, 0, 1 };
		static GLuint  Find[] = { 0,1,2,  0,2,3 };
		static GLuint FindexVBO, FVAO, FvertTexVBO;
		//static bool FfirstTime = true;
		static int lastContextId = -1;

		//std::cout << lastContextId << " " << Window::contextId << std::endl;
		if (lastContextId != Window::contextId) {
			lastContextId = Window::contextId;
			//FfirstTime = false;

			glGenVertexArrays(1, &FVAO);
			glBindVertexArray(FVAO);

			glGenBuffers(1, &FindexVBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FindexVBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Find), Find, GL_STATIC_DRAW);

			glGenBuffers(1, &FvertTexVBO);
			sendVertsTexToGPU(FvertTexVBO, Fvert, Ftcoord, sizeof(Fvert), sizeof(Ftcoord));

			glBindBuffer(GL_ARRAY_BUFFER, FvertTexVBO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(Fvert));

			glBindVertexArray(0);
		}

		
		glBindVertexArray(FVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FindexVBO);
		
		const GLboolean cullingWasEnabled = glIsEnabled(GL_CULL_FACE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

		if (!cullingWasEnabled) {
			glDisable(GL_CULL_FACE);
		}

		glBindVertexArray(0);
	}


	/*static*/ Mesh		RenderUtility::createCameraStub( void )
	{
		if (_camStubSize == 0.0f) {
			_camStubSize = 0.1f;
		}
		Mesh m;
		m.vertices( getCameraStubVertices() );
		m.triangles( getCameraStubIndices() );
		return m;
	}

	/*static*/ Mesh		RenderUtility::createScreenQuad( void )
	{
		Mesh::Vertices v;
		v.emplace_back(-1.0f, -1.0f, 0.0f);
		v.emplace_back( 1.0f, -1.0f, 0.0f);
		v.emplace_back( 1.0f,  1.0f, 0.0f);
		v.emplace_back(-1.0f,  1.0f, 0.0f);
		Mesh::UVs tc;
		tc.emplace_back(0.0f,0.0f);
		tc.emplace_back(1.0f,0.0f);
		tc.emplace_back(1.0f,1.0f);
		tc.emplace_back(0.0f,1.0f);
		Mesh::Triangles t;
		t.emplace_back(0);
		t.emplace_back(1);
		t.emplace_back(2);
		t.emplace_back(0);
		t.emplace_back(2);
		t.emplace_back(3);


		Mesh m;
		m.vertices(v);
		m.texCoords(tc);
		m.triangles(t);
		return m;
	}

	/*static*/ Mesh		RenderUtility::createAxisGizmo( void ){
		const float arrowShift = 0.2f;
		const float arrowSpread = 0.1f;

		Mesh::Vertices v;
		// Axis X
		v.emplace_back(-1.0f, 0.0f, 0.0f);
		v.emplace_back( 1.0f, 0.0f, 0.0f);
		// Arrow X
		v.emplace_back( 1.0f - arrowShift, -arrowSpread, 0.0f);
		v.emplace_back( 1.0f - arrowShift, 0.0f,		 -arrowSpread);
		v.emplace_back( 1.0f - arrowShift, arrowSpread,	 0.0f);
		v.emplace_back( 1.0f - arrowShift, 0.0f,		 arrowSpread);
		// Axis Y
		v.emplace_back( 0.0f, -1.0f, 0.0f);
		v.emplace_back( 0.0f,  1.0f, 0.0f);
		// Arrow Y
		v.emplace_back(-arrowSpread, 1.0f - arrowShift, 0.0f);
		v.emplace_back( 0.0f,		 1.0f - arrowShift, -arrowSpread);
		v.emplace_back( arrowSpread, 1.0f - arrowShift, 0.0f);
		v.emplace_back( 0.0f,		 1.0f - arrowShift, arrowSpread);
		// Axis Z
		v.emplace_back( 0.0f,  0.0f, -1.0f);
		v.emplace_back( 0.0f,  0.0f, 1.0f);
		// Arrow Z
		v.emplace_back(-arrowSpread, 0.0f,		   1.0f - arrowShift);
		v.emplace_back( 0.0f,		 -arrowSpread, 1.0f - arrowShift);
		v.emplace_back( arrowSpread, 0.0f,		   1.0f - arrowShift);
		v.emplace_back( 0.0f,		 arrowSpread,  1.0f - arrowShift);

		// Letter X
		v.emplace_back( 1.0f + arrowShift - arrowSpread, -arrowSpread, 0.0f);
		v.emplace_back( 1.0f + arrowShift + arrowSpread, arrowSpread,	0.0f);
		v.emplace_back( 1.0f + arrowShift - arrowSpread, arrowSpread,	 0.0f);
		v.emplace_back( 1.0f + arrowShift + arrowSpread, -arrowSpread, 0.0f);
		// Letter Y
		v.emplace_back( 0.0f,		 1.0f + arrowShift - arrowSpread, 0.0f);
		v.emplace_back( 0.0f,		 1.0f + arrowShift,				  0.0f);
		v.emplace_back(-arrowSpread, 1.0f + arrowShift + arrowSpread, 0.0f);
		v.emplace_back( arrowSpread, 1.0f + arrowShift + arrowSpread, 0.0f);
		// Letter Z
		v.emplace_back( 0.0f, -arrowSpread, 1.0f + arrowShift - arrowSpread);
		v.emplace_back( 0.0f, -arrowSpread, 1.0f + arrowShift + arrowSpread);
		v.emplace_back( 0.0f,  arrowSpread, 1.0f + arrowShift - arrowSpread);
		v.emplace_back( 0.0f,  arrowSpread, 1.0f + arrowShift + arrowSpread);

		Mesh::Colors c;
		// Colors X
		c.emplace_back(1.0f,0.0f,0.0f); c.emplace_back(1.0f,0.0f,0.0f); c.emplace_back(1.0f,0.0f,0.0f);
		c.emplace_back(1.0f,0.0f,0.0f); c.emplace_back(1.0f,0.0f,0.0f); c.emplace_back(1.0f,0.0f,0.0f);
		// Colors Y
		c.emplace_back(0.0f,1.0f,0.0f); c.emplace_back(0.0f,1.0f,0.0f); c.emplace_back(0.0f,1.0f,0.0f); 
		c.emplace_back(0.0f,1.0f,0.0f);c.emplace_back(0.0f,1.0f,0.0f); c.emplace_back(0.0f,1.0f,0.0f);
		// Colors Z
		c.emplace_back(0.0f,0.0f,1.0f); c.emplace_back(0.0f,0.0f,1.0f); c.emplace_back(0.0f,0.0f,1.0f);
		c.emplace_back(0.0f,0.0f,1.0f); c.emplace_back(0.0f,0.0f,1.0f); c.emplace_back(0.0f,0.0f,1.0f);
		// Colors Letter X
		c.emplace_back(1.0f,0.0f,0.0f); c.emplace_back(1.0f,0.0f,0.0f);
		c.emplace_back(1.0f,0.0f,0.0f); c.emplace_back(1.0f,0.0f,0.0f);
		// Colors Letter Y
		c.emplace_back(0.0f,1.0f,0.0f); c.emplace_back(0.0f,1.0f,0.0f);
		c.emplace_back(0.0f,1.0f,0.0f); c.emplace_back(0.0f,1.0f,0.0f);
		// Colors Letter Z
		c.emplace_back(0.0f,0.0f,1.0f); c.emplace_back(0.0f,0.0f,1.0f);
		c.emplace_back(0.0f,0.0f,1.0f); c.emplace_back(0.0f,0.0f,1.0f);
		
		Mesh::Triangles t;
		// Axis X
		t.emplace_back(0,1,0);
		// Arrow X
		t.emplace_back(1,2,3); t.emplace_back(1,3,4); t.emplace_back(1,4,5);
		t.emplace_back(1,5,2); t.emplace_back(2,3,4); t.emplace_back(2,3,5);
		// Axis Y
		t.emplace_back(6,7,6);
		// Arrow Y
		t.emplace_back(7,8,9); t.emplace_back(7,9,10); t.emplace_back(7,10,11);
		t.emplace_back(7,11,8); t.emplace_back(8,9,10); t.emplace_back(8,9,11);
		// Axis Z
		t.emplace_back(12,13,12);
		// Arrow Z
		t.emplace_back(13,14,15); t.emplace_back(13,15,16); t.emplace_back(13,16,17);
		t.emplace_back(13,17,14); t.emplace_back(14,15,16); t.emplace_back(14,15,17);

		// Letter X
		t.emplace_back(18,19,18); t.emplace_back(20,21,20);
		//Letter Y
		t.emplace_back(22,23,22); t.emplace_back(24,23,24); t.emplace_back(25,23,25);
		//Letter Z
		t.emplace_back(26,28,26); t.emplace_back(26,29,26); t.emplace_back(27,29,27);

		Mesh m;
		m.vertices(v);
		m.colors(c);
		m.triangles(t);
		return m;
	}


	/*static*/ void		RenderUtility::useDefaultVAO( void )
	{
		static GLuint gDefaultVAO = 0;

		if (!gDefaultVAO)
			glGenVertexArrays(1, &gDefaultVAO);

		glBindVertexArray(gDefaultVAO);
	}


} // namespace sibr
