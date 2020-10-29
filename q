[1mdiff --git a/shortSightedNinja/gameLayer.cpp b/shortSightedNinja/gameLayer.cpp[m
[1mindex 6cf27ba..f759b2f 100644[m
[1m--- a/shortSightedNinja/gameLayer.cpp[m
[1m+++ b/shortSightedNinja/gameLayer.cpp[m
[36m@@ -876,44 +876,16 @@[m [mbool gameLogic(float deltaTime)[m
 			}[m
 		}[m
 [m
[31m-	}else[m
[31m-	{[m
[31m-	player.idleTime = 0;[m
[31m-	jumpDelayTime = 0;[m
[31m-	}[m
[31m-[m
[31m-	if (!platform::isFocused())[m
[31m-	{[m
[31m-		player.idleTime = 0;[m
[31m-		jumpDelayTime = 0;[m
[31m-	}[m
[31m-[m
[31m-		renderer2d.currentCamera.follow(player.pos + (player.dimensions / 2.f), deltaTime * 100, 4 * BLOCK_SIZE, renderer2d.windowW, renderer2d.windowH);[m
 [m
[31m-		player.applyGravity(deltaTime);[m
[31m-		player.applyVelocity(deltaTime);[m
[31m-[m
[31m-		player.resolveConstrains(mapData);[m
[31m-[m
[31m-		player.updateMove(deltaTime);[m
[31m-[m
[31m-		player.checkGrounded(mapData, deltaTime);[m
[31m-[m
[31m-		//ilog(player.velocity.x);[m
[31m-[m
[31m-		if(!currentDialog.blockMovement())[m
[32m+[m		[32mif (input::isKeyHeld(input::Buttons::down))[m
 		{[m
[31m-[m
[31m-			if (input::isKeyHeld(input::Buttons::down))[m
[31m-			{[m
[31m-				player.wallGrab = 0;[m
[31m-				player.iceGrab = 0;[m
[31m-			}[m
[31m-			else[m
[31m-			{[m
[31m-				player.checkWall(mapData, input::getMoveDir());[m
[31m-			}[m
[31m-[m
[32m+[m			[32mplayer.wallGrab = 0;[m
[32m+[m			[32mplayer.iceGrab = 0;[m
[32m+[m		[32m}[m
[32m+[m		[32melse[m
[32m+[m		[32m{[m
[32m+[m			[32mplayer.checkWall(mapData, input::getMoveDir());[m
[32m+[m		[32m}[m
 [m
 [m
 #pragma region jump prticle[m
[36m@@ -944,48 +916,76 @@[m [mbool gameLogic(float deltaTime)[m
 		}[m
 [m
 #pragma endregion[m
[31m-		[m
[31m-		[m
[32m+[m
[32m+[m
[32m+[m	[32m}else[m
[32m+[m	[32m{[m
[32m+[m	[32mplayer.idleTime = 0;[m
[32m+[m	[32mjumpDelayTime = 0;[m
[32m+[m	[32m}[m
[32m+[m
[32m+[m	[32mif (!platform::isFocused())[m
[32m+[m	[32m{[m
[32m+[m		[32mplayer.idleTime = 0;[m
[32m+[m		[32mjumpDelayTime = 0;[m
[32m+[m	[32m}[m
[32m+[m
[32m+[m	[32mrenderer2d.currentCamera.follow(player.pos + (player.dimensions / 2.f), deltaTime * 100, 4 * BLOCK_SIZE, renderer2d.windowW, renderer2d.windowH);[m
[32m+[m
[32m+[m	[32mplayer.applyGravity(deltaTime);[m
[32m+[m	[32mplayer.applyVelocity(deltaTime);[m
[32m+[m
[32m+[m	[32mplayer.resolveConstrains(mapData);[m
[32m+[m
[32m+[m	[32mplayer.updateMove(deltaTime);[m
[32m+[m
[32m+[m	[32mplayer.checkGrounded(mapData, deltaTime);[m
[32m+[m
[32m+[m	[32m//ilog(player.velocity.x);[m
[32m+[m
[32m+[m	[32mif(!currentDialog.blockMovement() && !inGameMenu)[m
[32m+[m	[32m{[m
[32m+[m
 [m
 #pragma region inventory[m
 [m
[31m-		if (!platform::isFocused())[m
[31m-		{[m
[31m-			currentArrow = -1;[m
[31m-		}[m
[32m+[m	[32mif (!platform::isFocused())[m
[32m+[m	[32m{[m
[32m+[m		[32mcurrentArrow = -1;[m
[32m+[m	[32m}[m
 [m
[31m-		if (input::isKeyPressedOn(input::Buttons::swapLeft))[m
[31m-		{[m
[31m-			currentArrow--;[m
[31m-		}[m
[31m-		else if (input::isKeyPressedOn(input::Buttons::swapRight))[m
[31m-		{[m
[31m-			currentArrow++;[m
[31m-		}[m
[32m+[m	[32mif (input::isKeyPressedOn(input::Buttons::swapLeft))[m
[32m+[m	[32m{[m
[32m+[m		[32mcurrentArrow--;[m
[32m+[m	[32m}[m
[32m+[m	[32melse if (input::isKeyPressedOn(input::Buttons::swapRight))[m
[32m+[m	[32m{[m
[32m+[m		[32mcurrentArrow++;[m
[32m+[m	[32m}[m
 [m
[31m-		if (currentArrow < -1)[m
[31m-		{[m
[31m-			currentArrow = actualInventorty.size() - 1;[m
[31m-		}[m
[32m+[m	[32mif (currentArrow < -1)[m
[32m+[m	[32m{[m
[32m+[m		[32mcurrentArrow = actualInventorty.size() - 1;[m
[32m+[m	[32m}[m
 [m
[31m-		if (currentArrow >= actualInventorty.size())[m
[31m-		{[m
[31m-			currentArrow = -1;[m
[31m-		}[m
[32m+[m	[32mif (currentArrow >= actualInventorty.size())[m
[32m+[m	[32m{[m
[32m+[m		[32mcurrentArrow = -1;[m
[32m+[m	[32m}[m
 [m
[31m-		if (actualInventorty.size() == 0)[m
[31m-		{[m
[31m-			currentArrow = -1;[m
[31m-		}[m
[32m+[m	[32mif (actualInventorty.size() == 0)[m
[32m+[m	[32m{[m
[32m+[m		[32mcurrentArrow = -1;[m
[32m+[m	[32m}[m
 [m
 #pragma endregion[m
 [m
[31m-		[m
[31m-		}[m
[31m-		else[m
[31m-		{[m
[31m-			currentArrow = -1;[m
[31m-		}[m
[32m+[m[41m	[m
[32m+[m	[32m}[m
[32m+[m	[32melse[m
[32m+[m	[32m{[m
[32m+[m		[32mcurrentArrow = -1;[m
[32m+[m	[32m}[m
 [m
 [m
 #pragma endregion[m
[36m@@ -1000,8 +1000,6 @@[m [mbool gameLogic(float deltaTime)[m
 		}[m
 	}[m
 [m
[31m-	[m
[31m-[m
 	//stencilRenderer2d.currentCamera = renderer2d.currentCamera;[m
 [m
 [m
[1mdiff --git a/shortSightedNinja/opengl2Dlib.cpp b/shortSightedNinja/opengl2Dlib.cpp[m
[1mindex 4fb24fb..76b7974 100644[m
[1m--- a/shortSightedNinja/opengl2Dlib.cpp[m
[1m+++ b/shortSightedNinja/opengl2Dlib.cpp[m
[36m@@ -2,7 +2,6 @@[m
 #include <GL/wglew.h>[m
 #include <fstream>[m
 #include <sstream>[m
[31m-#include "tools.h"[m
 #include <algorithm>[m
 [m
 #undef max[m
[36m@@ -57,14 +56,14 @@[m [mnamespace gl2d[m
 [m
 #pragma endregion[m
 [m
[31m-	static errorFuncType *errorFunc = defaultErrorFunc;[m
[32m+[m	[32mstatic errorFuncType* errorFunc = defaultErrorFunc;[m
 [m
 	void defaultErrorFunc(const char* msg)[m
 	{[m
 [m
 	}[m
 [m
[31m-	errorFuncType *setErrorFuncCallback(errorFuncType *newFunc)[m
[32m+[m	[32merrorFuncType* setErrorFuncCallback(errorFuncType* newFunc)[m
 	{[m
 		auto a = errorFunc;[m
 		errorFunc = newFunc;[m
[36m@@ -86,14 +85,14 @@[m [mnamespace gl2d[m
 		stbtt_aligned_quad fontGetGlyphQuad(const Font font, const char c)[m
 		{[m
 			stbtt_aligned_quad quad = { 0 };[m
[31m-		[m
[31m-			float x=0;[m
[31m-			float y=0;[m
[32m+[m
[32m+[m			[32mfloat x = 0;[m
[32m+[m			[32mfloat y = 0;[m
 [m
 			stbtt_GetPackedQuad(font.packedCharsBuffer,[m
 				font.size.x, font.size.y, c - ' ', &x, &y, &quad, 1);[m
[31m-			[m
[31m-			[m
[32m+[m
[32m+[m
 			return quad;[m
 		}[m
 [m
[36m@@ -191,8 +190,12 @@[m [mnamespace gl2d[m
 		bool WGL_EXT_swap_control_ext;[m
 	}extensions = {};[m
 [m
[32m+[m	[32mbool hasInitialized = 0;[m
 	void init()[m
 	{[m
[32m+[m		[32mif (hasInitialized) { return; }[m
[32m+[m		[32mhasInitialized = true;[m
[32m+[m
 		//int last = 0;[m
 		//glGetIntegerv(GL_NUM_EXTENSIONS, &last);[m
 		//for(int i=0; i<last; i++)[m
[36m@@ -217,7 +220,7 @@[m [mnamespace gl2d[m
 [m
 	bool setVsync(bool b)[m
 	{[m
[31m-		if(extensions.WGL_EXT_swap_control_ext)[m
[32m+[m		[32mif (extensions.WGL_EXT_swap_control_ext)[m
 		{[m
 			wglSwapIntervalEXT(b);[m
 			return true;[m
[36m@@ -254,7 +257,7 @@[m [mnamespace gl2d[m
 	///////////////////// Texture /////////////////////[m
 #pragma region Texture[m
 [m
[31m-	void convertFromRetardedCoordonates(int tSizeX, int tSizeY, int x, int y, int sizeX, int sizeY, int s1, int s2, int s3, int s4, Texture_Coords *outer, Texture_Coords *inner)[m
[32m+[m	[32mvoid convertFromRetardedCoordonates(int tSizeX, int tSizeY, int x, int y, int sizeX, int sizeY, int s1, int s2, int s3, int s4, Texture_Coords* outer, Texture_Coords* inner)[m
 	{[m
 		float newX = (float)tSizeX / (float)x;[m
 		float newY = (float)tSizeY / (float)y;[m
[36m@@ -286,7 +289,7 @@[m [mnamespace gl2d[m
 	///////////////////// Font /////////////////////[m
 #pragma region Font[m
 [m
[31m-	void Font::createFromTTF(const unsigned char * ttf_data, const size_t ttf_data_size)[m
[32m+[m	[32mvoid Font::createFromTTF(const unsigned char* ttf_data, const size_t ttf_data_size)[m
 	{[m
 [m
 		size.x = 2000,[m
[36m@@ -353,7 +356,7 @@[m [mnamespace gl2d[m
 		}[m
 	}[m
 [m
[31m-	void Font::createFromFile(const char * file)[m
[32m+[m	[32mvoid Font::createFromFile(const char* file)[m
 	{[m
 		std::ifstream fileFont(file, std::ios::binary);[m
 [m
[36m@@ -370,7 +373,7 @@[m [mnamespace gl2d[m
 		fileFont.seekg(0, std::ios::end);[m
 		fileSize = (int)fileFont.tellg();[m
 		fileFont.seekg(0, std::ios::beg);[m
[31m-		unsigned char * fileData = new unsigned char[fileSize];[m
[32m+[m		[32munsigned char* fileData = new unsigned char[fileSize];[m
 		fileFont.read((char*)fileData, fileSize);[m
 		fileFont.close();[m
 [m
[36m@@ -883,11 +886,7 @@[m [mnamespace gl2d[m
 		texturePositionsCount = 0;[m
 		spriteTexturesCount = 0;[m
 [m
[31m-		windowW;[m
[31m-		windowH;[m
[31m-[m
[31m-		currentShader = defaultShader;[m
[31m-		currentCamera = defaultCamera;[m
[32m+[m		[32mthis->resetCameraAndShader();[m
 [m
 		glGenVertexArrays(1, &vao);[m
 		glBindVertexArray(vao);[m
[36m@@ -909,7 +908,7 @@[m [mnamespace gl2d[m
 		glBindVertexArray(0);[m
 	}[m
 [m
[31m-	glm::vec4 Renderer2D::toScreen(const glm::vec4 &transform)[m
[32m+[m	[32mglm::vec4 Renderer2D::toScreen(const glm::vec4& transform)[m
 	{[m
 		//We need to flip texture_transforms.y[m
 		const float transformsY = transform.y * -1;[m
[36m@@ -928,7 +927,7 @@[m [mnamespace gl2d[m
 		v3.y += currentCamera.position.y;[m
 		v4.x -= currentCamera.position.x;[m
 		v4.y += currentCamera.position.y;[m
[31m-	[m
[32m+[m
 		//Apply camera zoom[m
 		//if(renderer->currentCamera.zoom != 1)[m
 		{[m
[36m@@ -949,7 +948,7 @@[m [mnamespace gl2d[m
 		return glm::vec4(v1.x, v1.y, v3.x, v3.y);[m
 	}[m
 [m
[31m-	glm::vec2 Renderer2D::getTextSize(const char * text, const Font font,[m
[32m+[m	[32mglm::vec2 Renderer2D::getTextSize(const char* text, const Font font,[m
 		const float size, const float spacing, const float line_space)[m
 	{[m
 		glm::vec2 position = {};[m
[36m@@ -960,7 +959,6 @@[m [mnamespace gl2d[m
 		float linePositionY = position.y;[m
 [m
 		//This is the y position we render at because it advances when we encounter newlines[m
[31m-[m
 		float maxPos = 0;[m
 		float maxPosY = 0;[m
 		float bonusY = 0;[m
[36m@@ -1020,21 +1018,21 @@[m [mnamespace gl2d[m
 		paddY += font.max_height * size + bonusY;[m
 [m
 		return glm::vec2{ paddX, paddY };[m
[31m-		[m
[32m+[m
 	}[m
 [m
[31m-	void Renderer2D::renderText(glm::vec2 position, const char * text, const Font font,[m
[32m+[m	[32mvoid Renderer2D::renderText(glm::vec2 position, const char* text, const Font font,[m
 		const Color4f color, const float size, const float spacing, const float line_space, bool showInCenter,[m
 		const Color4f ShadowColor[m
 		, const Color4f LightColor[m
[31m-		)[m
[32m+[m	[32m)[m
 	{[m
 		const int text_length = (int)strlen(text);[m
 		Rect rectangle;[m
 		rectangle.x = position.x;[m
 		float linePositionY = position.y;[m
 [m
[31m-		if(showInCenter)[m
[32m+[m		[32mif (showInCenter)[m
 		{[m
 			//This is the y position we render at because it advances when we encounter newlines[m
 [m
[36m@@ -1062,7 +1060,7 @@[m [mnamespace gl2d[m
 					(font, '_');[m
 					auto x = quad.x1 - quad.x0;[m
 [m
[31m-					rectangle.x += x * size+ spacing*size;[m
[32m+[m					[32mrectangle.x += x * size + spacing * size;[m
 				}[m
 				else if (text[i] >= ' ' && text[i] <= '~')[m
 				{[m
[36m@@ -1135,13 +1133,13 @@[m [mnamespace gl2d[m
 				//rectangle.y = linePositionY - rectangle.w;[m
 				rectangle.y = linePositionY + quad.y0 * size;[m
 [m
[31m-				glm::vec4 colorData[4] = { color, color, color, color };				[m
[31m-				[m
[31m-				if(ShadowColor.w)[m
[32m+[m				[32mglm::vec4 colorData[4] = { color, color, color, color };[m
[32m+[m
[32m+[m				[32mif (ShadowColor.w)[m
 				{[m
 					glm::vec2 pos = { -5, 3 };[m
 					pos *= size;[m
[31m-					renderRectangle({ rectangle.x + pos.x, rectangle.y + pos.y,  rectangle.z, rectangle.w},[m
[32m+[m					[32mrenderRectangle({ rectangle.x + pos.x, rectangle.y + pos.y,  rectangle.z, rectangle.w },[m
 						ShadowColor, glm::vec2{ 0, 0 }, 0, font.texture,[m
 						glm::vec4{ quad.s0, quad.t0, quad.s1, quad.t1 });[m
 [m
[36m@@ -1149,7 +1147,7 @@[m [mnamespace gl2d[m
 [m
 				renderRectangle(rectangle, colorData, glm::vec2{ 0, 0 }, 0, font.texture, glm::vec4{ quad.s0, quad.t0, quad.s1, quad.t1 });[m
 [m
[31m-				if(LightColor.w)[m
[32m+[m				[32mif (LightColor.w)[m
 				{[m
 					glm::vec2 pos = { -2, 1 };[m
 					pos *= size;[m
[36m@@ -1159,7 +1157,7 @@[m [mnamespace gl2d[m
 [m
 				}[m
 [m
[31m-				[m
[32m+[m
 				rectangle.x += rectangle.z + spacing * size;[m
 			}[m
 		}[m
[36m@@ -1198,7 +1196,7 @@[m [mnamespace gl2d[m
 		return s;[m
 	}[m
 [m
[31m-	void Texture::createFromBuffer(const char * image_data, const int width, const int height)[m
[32m+[m	[32mvoid Texture::createFromBuffer(const char* image_data, const int width, const int height)[m
 	{[m
 		GLuint id = 0;[m
 [m
[36m@@ -1218,9 +1216,9 @@[m [mnamespace gl2d[m
 		this->id = id;[m
 	}[m
 [m
[31m-	void Texture::create1PxSquare(const char *b)[m
[32m+[m	[32mvoid Texture::create1PxSquare(const char* b)[m
 	{[m
[31m-		if(b==nullptr)[m
[32m+[m		[32mif (b == nullptr)[m
 		{[m
 			const unsigned char buff[] =[m
 			{[m
[36m@@ -1231,14 +1229,15 @@[m [mnamespace gl2d[m
 			};[m
 [m
 			createFromBuffer((char*)buff, 1, 1);[m
[31m-		}else[m
[32m+[m		[32m}[m
[32m+[m		[32melse[m
 		{[m
 			createFromBuffer(b, 1, 1);[m
 		}[m
[31m-	[m
[32m+[m
 	}[m
 [m
[31m-	void Texture::createFromFileData(const unsigned char * image_file_data, const size_t image_file_size)[m
[32m+[m	[32mvoid Texture::createFromFileData(const unsigned char* image_file_data, const size_t image_file_size)[m
 	{[m
 		stbi_set_flip_vertically_on_load(true);[m
 [m
[36m@@ -1254,7 +1253,7 @@[m [mnamespace gl2d[m
 		free((void*)decodedImage);[m
 	}[m
 [m
[31m-	void Texture::createFromFileDataWithPixelPadding(const unsigned char * image_file_data, const size_t image_file_size, int blockSize)[m
[32m+[m	[32mvoid Texture::createFromFileDataWithPixelPadding(const unsigned char* image_file_data, const size_t image_file_size, int blockSize)[m
 	{[m
 		stbi_set_flip_vertically_on_load(true);[m
 [m
[36m@@ -1305,20 +1304,20 @@[m [mnamespace gl2d[m
 [m
 				}[m
 			}[m
[31m-			[m
[32m+[m
 		}[m
 		*/[m
[31m-		[m
[32m+[m
 		int newW = width + ((width * 2) / blockSize);[m
 		int newH = height + ((height * 2) / blockSize);[m
 [m
 		auto getOld = [decodedImage, width](int x, int y, int c)->const unsigned char[m
 		{[m
[31m-			return decodedImage[4*(x + (y * width)) + c];[m
[32m+[m			[32mreturn decodedImage[4 * (x + (y * width)) + c];[m
 		};[m
 [m
[31m-		[m
[31m-		unsigned char *newData = new unsigned char[newW * newH * 4];[m
[32m+[m
[32m+[m		[32munsigned char* newData = new unsigned char[newW * newH * 4];[m
 		ZeroMemory(newData, newW * newH * 4);[m
 [m
 		auto getNew = [newData, newW](int x, int y, int c)[m
[36m@@ -1330,36 +1329,37 @@[m [mnamespace gl2d[m
 		int dataCursor = 0;[m
 [m
 		//first copy data[m
[31m-		for(int y=0; y < newH; y++)[m
[32m+[m		[32mfor (int y = 0; y < newH; y++)[m
 		{[m
 			int yNo = 0;[m
[31m-			if((y==0 || y== newH-1[m
[31m-				|| ((y)%(blockSize+2))==0||[m
[31m-				((y+1) % (blockSize + 2)) == 0[m
[32m+[m			[32mif ((y == 0 || y == newH - 1[m
[32m+[m				[32m|| ((y) % (blockSize + 2)) == 0 ||[m
[32m+[m				[32m((y + 1) % (blockSize + 2)) == 0[m
 				))[m
 			{[m
 				yNo = 1;[m
 			}[m
[31m-			[m
[32m+[m
 			for (int x = 0; x < newW; x++)[m
 			{[m
 				if ([m
[31m-					yNo||[m
[31m-					[m
[32m+[m					[32myNo ||[m
[32m+[m
 					(([m
[31m-					x == 0 || x == newW - 1[m
[31m-					|| (x % (blockSize + 2)) == 0 ||[m
[31m-					((x + 1) % (blockSize + 2)) == 0[m
[31m-					) [m
[32m+[m						[32mx == 0 || x == newW - 1[m
[32m+[m						[32m|| (x % (blockSize + 2)) == 0 ||[m
[32m+[m						[32m((x + 1) % (blockSize + 2)) == 0[m
[32m+[m						[32m)[m
 						)[m
[31m-					[m
[32m+[m
 					)[m
 				{[m
 					newData[newDataCursor++] = 0;[m
 					newData[newDataCursor++] = 0;[m
 					newData[newDataCursor++] = 0;[m
 					newData[newDataCursor++] = 0;[m
[31m-				}else[m
[32m+[m				[32m}[m
[32m+[m				[32melse[m
 				{[m
 					newData[newDataCursor++] = decodedImage[dataCursor++];[m
 					newData[newDataCursor++] = decodedImage[dataCursor++];[m
[36m@@ -1368,20 +1368,20 @@[m [mnamespace gl2d[m
 				}[m
 [m
 			}[m
[31m-		[m
[32m+[m
 		}[m
 [m
 		//then add margins[m
[31m-		[m
 [m
[31m-		for(int x =1; x< newW-1; x++)[m
[32m+[m
[32m+[m		[32mfor (int x = 1; x < newW - 1; x++)[m
 		{[m
 			//copy on left[m
[31m-			if (x == 1 || [m
[31m-				(x%(blockSize+2)) == 1[m
[32m+[m			[32mif (x == 1 ||[m
[32m+[m				[32m(x % (blockSize + 2)) == 1[m
 				)[m
 			{[m
[31m-				for(int y=0; y< newH; y++)[m
[32m+[m				[32mfor (int y = 0; y < newH; y++)[m
 				{[m
 					*getNew(x - 1, y, 0) = *getNew(x, y, 0);[m
 					*getNew(x - 1, y, 1) = *getNew(x, y, 1);[m
[36m@@ -1389,22 +1389,23 @@[m [mnamespace gl2d[m
 					*getNew(x - 1, y, 3) = *getNew(x, y, 3);[m
 				}[m
 [m
[31m-			}else //copy on rigght[m
[32m+[m			[32m}[m
[32m+[m			[32melse //copy on rigght[m
 				if (x == newW - 2 ||[m
 					(x % (blockSize + 2)) == blockSize[m
 					)[m
 				{[m
 					for (int y = 0; y < newH; y++)[m
 					{[m
[31m-						*getNew(x+1, y, 0) = *getNew(x, y, 0);[m
[31m-						*getNew(x+1, y, 1) = *getNew(x, y, 1);[m
[31m-						*getNew(x+1, y, 2) = *getNew(x, y, 2);[m
[31m-						*getNew(x+1, y, 3) = *getNew(x, y, 3);[m
[32m+[m						[32m*getNew(x + 1, y, 0) = *getNew(x, y, 0);[m
[32m+[m						[32m*getNew(x + 1, y, 1) = *getNew(x, y, 1);[m
[32m+[m						[32m*getNew(x + 1, y, 2) = *getNew(x, y, 2);[m
[32m+[m						[32m*getNew(x + 1, y, 3) = *getNew(x, y, 3);[m
 					}[m
 				}[m
 		}[m
 [m
[31m-		for(int y=1; y< newH -1; y++)[m
[32m+[m		[32mfor (int y = 1; y < newH - 1; y++)[m
 		{[m
 			if (y == 1 ||[m
 				(y % (blockSize + 2)) == 1[m
[36m@@ -1412,10 +1413,10 @@[m [mnamespace gl2d[m
 			{[m
 				for (int x = 0; x < newW; x++)[m
 				{[m
[31m-					*getNew(x, y-1, 0) = *getNew(x, y, 0);[m
[31m-					*getNew(x, y-1, 1) = *getNew(x, y, 1);[m
[31m-					*getNew(x, y-1, 2) = *getNew(x, y, 2);[m
[31m-					*getNew(x, y-1, 3) = *getNew(x, y, 3);[m
[32m+[m					[32m*getNew(x, y - 1, 0) = *getNew(x, y, 0);[m
[32m+[m					[32m*getNew(x, y - 1, 1) = *getNew(x, y, 1);[m
[32m+[m					[32m*getNew(x, y - 1, 2) = *getNew(x, y, 2);[m
[32m+[m					[32m*getNew(x, y - 1, 3) = *getNew(x, y, 3);[m
 				}[m
 			}[m
 			else[m
[36m@@ -1425,13 +1426,13 @@[m [mnamespace gl2d[m
 				{[m
 					for (int x = 0; x < newW; x++)[m
 					{[m
[31m-						*getNew(x ,y+1, 0) = *getNew(x, y , 0);[m
[31m-						*getNew(x, y+1, 1) = *getNew(x, y , 1);[m
[31m-						*getNew(x, y+1, 2) = *getNew(x, y , 2);[m
[31m-						*getNew(x, y+1, 3) = *getNew(x, y , 3);[m
[32m+[m						[32m*getNew(x, y + 1, 0) = *getNew(x, y, 0);[m
[32m+[m						[32m*getNew(x, y + 1, 1) = *getNew(x, y, 1);[m
[32m+[m						[32m*getNew(x, y + 1, 2) = *getNew(x, y, 2);[m
[32m+[m						[32m*getNew(x, y + 1, 3) = *getNew(x, y, 3);[m
 					}[m
 				}[m
[31m-		[m
[32m+[m
 		}[m
 [m
 [m
[36m@@ -1442,7 +1443,7 @@[m [mnamespace gl2d[m
 		delete[] newData;[m
 	}[m
 [m
[31m-	void Texture::loadFromFile(const char * fileName)[m
[32m+[m	[32mvoid Texture::loadFromFile(const char* fileName)[m
 	{[m
 		std::ifstream file(fileName, std::ios::binary);[m
 [m
[36m@@ -1459,7 +1460,7 @@[m [mnamespace gl2d[m
 		file.seekg(0, std::ios::end);[m
 		fileSize = (int)file.tellg();[m
 		file.seekg(0, std::ios::beg);[m
[31m-		unsigned char * fileData = new unsigned char[fileSize];[m
[32m+[m		[32munsigned char* fileData = new unsigned char[fileSize];[m
 		file.read((char*)fileData, fileSize);[m
 		file.close();[m
 [m
[36m@@ -1469,7 +1470,7 @@[m [mnamespace gl2d[m
 [m
 	}[m
 [m
[31m-	void Texture::loadFromFileWithPixelPadding(const char * fileName, int blockSize)[m
[32m+[m	[32mvoid Texture::loadFromFileWithPixelPadding(const char* fileName, int blockSize)[m
 	{[m
 		std::ifstream file(fileName, std::ios::binary);[m
 [m
[36m@@ -1486,7 +1487,7 @@[m [mnamespace gl2d[m
 