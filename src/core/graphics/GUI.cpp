
#include "core/graphics/Window.hpp"
#include "core/graphics/GUI.hpp"
#include "core/graphics/Mesh.hpp"

namespace sibr
{
	
	bool		showImGuiWindow(const std::string& windowTitle, const IRenderTarget& rt, ImGuiWindowFlags flags, Viewport & viewport, const bool invalidTexture, const bool updateLayout )
	{
		bool isWindowFocused = false;
		// If we are asked to, we need to update the viewport at launch.
		if (updateLayout) {
			ImGui::SetNextWindowPos(ImVec2(viewport.finalLeft(), viewport.finalTop()));
			ImGui::SetNextWindowSize(ImVec2(viewport.finalWidth(), viewport.finalHeight()));
		}

		if (::ImGui::Begin(windowTitle.c_str(), NULL, flags))
		{
			// Get the current cursor position (where your window is)
			ImVec2 pos = /*ImGui::GetItemRectMin() + */::ImGui::GetCursorScreenPos();
			Vector2f offset, size;
			Vector2i availRegionSize(::ImGui::GetContentRegionAvail().x, ::ImGui::GetContentRegionAvail().y);
			
			fitImageToDisplayRegion(viewport.finalSize(), availRegionSize, offset, size);
			
			size = size.cwiseMax( sibr::Vector2f( 1.0f,1.0f) );

			//std::cout << windowTitle << " : " <<  rtSize << availRegionSize << offset << " / " << size << std::endl;
				
			pos.x += offset.x();
			pos.y += offset.y();

			
			ImGui::SetCursorPos(ImVec2(offset.x(), ImGui::GetTitleBarHeight()+offset.y()));
			ImGui::InvisibleButton((windowTitle + "--TEXTURE-INVISIBLE_BUTTON").c_str(), ImVec2(size.x(), size.y()));
			if (!invalidTexture) {
				::ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)(rt.texture()),
					pos, ImVec2(pos.x + size.x(), pos.y + size.y()),
					ImVec2(0, 1), ImVec2(1, 0));
			}
			
			isWindowFocused = ImGui::IsWindowFocused();

			viewport = Viewport(pos.x, pos.y, pos.x+size.x(), pos.y+size.y());

			//std::cout << "RT is " << rt.w() << "," << rt.h() << ", view is " << size.x() << "," << size.y() << std::endl;
			// Hand back the inputs to sibr.
			if (ImGui::IsItemHovered()) {
				ImGui::CaptureKeyboardFromApp(false);
				ImGui::CaptureMouseFromApp(false);
			}
		}
		::ImGui::End();

		return isWindowFocused;
	}

	Mesh::Ptr generateMeshForText(const std::string & text, unsigned int & separationIndex){
		// Technically we don't care if we already are in the middle of a ImGui frame.
		// as long as we clear the draw list. ImGui will detect the empty draw lists and cull them.
		ImGui::PushID(1234);
		ImGui::SetNextWindowPos(ImVec2(0,0));
		ImGui::Begin(text.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
		ImGui::SetWindowFontScale(ImGui::GetIO().FontGlobalScale);

		ImGui::Text(text.c_str());
		// Get back the draw list.
		ImDrawList * drawlist = ImGui::GetWindowDrawList();
		const int vertCount = drawlist->VtxBuffer.Size;
		const int indexCount = drawlist->IdxBuffer.Size;
		// We generate one mesh from the draw list.
		std::vector<sibr::Vector3f> vertices(vertCount);
		std::vector<sibr::Vector2f> uvs(vertCount);
		std::vector<sibr::Vector3f> colors(vertCount);
		std::vector<sibr::Vector3u> faces(indexCount / 3);

		sibr::Vector3f centroid(0.0f, 0.0f, 0.0f);
		for (int k = 0; k < vertCount; ++k) {
			const auto & vtx = drawlist->VtxBuffer[k];
			vertices[k][0] = (vtx.pos.x)*2.0f;
			vertices[k][1] = -vtx.pos.y*2.0f;
			uvs[k][0] = vtx.uv.x; uvs[k][1] = vtx.uv.y;
			ImVec4 col = ImGui::ColorConvertU32ToFloat4(vtx.col);
			colors[k][0] = col.x; colors[k][1] = col.y;
			colors[k][2] = col.z; vertices[k][2] = col.w;
			centroid += vertices[k];
		}
		for (int k = 0; k < indexCount; k += 3) {
			faces[k / 3][0] = (unsigned int)drawlist->IdxBuffer[k];
			faces[k / 3][1] = (unsigned int)drawlist->IdxBuffer[k + 1];
			faces[k / 3][2] = (unsigned int)drawlist->IdxBuffer[k + 2];
		}
		// Center the mesh?
		centroid /= float(vertices.size());
		for (int k = 0; k < vertices.size(); ++k) {
			vertices[k] -= centroid;
		}
		Mesh::Ptr mesh = std::make_shared<Mesh>();
		mesh->vertices(vertices);
		mesh->colors(colors);
		mesh->texCoords(uvs);
		mesh->triangles(faces);
		// Store the separation idnex between the background and the text foreground.
		separationIndex = drawlist->CmdBuffer[0].ElemCount;
		
		// Finish the window, then clear the draw list.
		ImGui::End();
		ImGui::PopID();
		drawlist->Clear();
		return mesh;
	}


	void 			fitImageToDisplayRegion(const Vector2f & imgSize, const Vector2i & regionSize, Vector2f& offset, Vector2f& size)
	{
		
		Vector2f ratios = imgSize.cwiseQuotient(regionSize.cast<float>());
		if (ratios.x() < ratios.y())
		{
			float aspect = imgSize.x() / imgSize.y();
			size.y() = float(regionSize.y());
			size.x() = size.y() * aspect;
		}
		else
		{
			float aspect = imgSize.y() / imgSize.x();
			size.x() = float(regionSize.x());
			size.y() = size.x() * aspect;
		}
		offset = regionSize.cast<float>() / 2 - size / 2;
	}
	


	sibr::Vector2f ZoomData::topLeft()		const { return center - diagonal; }
	sibr::Vector2f ZoomData::bottomRight()	const { return center + diagonal; }

	sibr::Vector2f ZoomData::uvFromBoxPos(const sibr::Vector2f& pos) const
	{
		return topLeft() + 2.0f*diagonal.cwiseProduct(pos);
	}

	ZoomData ZoomData::scaled(const sibr::Vector2f& size) const 
	{
		ZoomData out;
		out.center = center.cwiseProduct(size);
		out.diagonal = diagonal.cwiseProduct(size);
		return out;
	}

	void ZoomInterraction::updateZoom(const sibr::Vector2f& canvasSize)
	{
		const auto & d = callBackData;
		if (d.ctrlPressed) {
			return;
		}

		sibr::Vector2f posF = zoomData.uvFromBoxPos(d.positionRatio);

		//ImGui::Text(std::string(std::to_string(zoomData.center[0]) + " " + std::to_string(zoomData.center[1])).c_str());
		//ImGui::Text(std::string(std::to_string(zoomData.diagonal[0]) + " " + std::to_string(zoomData.diagonal[1])).c_str());

		if (d.isHoovered && d.isClickedRight && !zoomData.underMofidication) {
			zoomData.underMofidication = true;
			zoomData.tmpTopLeft = posF;
			zoomData.firstClickPixel = d.mousePos;
		}
		if (d.isHoovered && zoomData.underMofidication) {
			zoomData.tmpBottonRight = posF;
			zoomData.secondClickPixel = d.mousePos;
		}

		if (zoomData.underMofidication) {
			//ImGui::Text(std::string(std::to_string(zoomData.tmpTopLeft[0]) + " " + std::to_string(zoomData.tmpTopLeft[1])).c_str());
			//ImGui::Text(std::string(std::to_string(zoomData.tmpBottonRight[0]) + " " + std::to_string(zoomData.tmpBottonRight[1])).c_str());

			ImGui::GetWindowDrawList()->AddRect(
				ImVec2(zoomData.firstClickPixel[0], zoomData.firstClickPixel[1]),
				ImVec2(zoomData.secondClickPixel[0], zoomData.secondClickPixel[1]),
				IM_COL32(255, 0, 0, 255), 0, 0, 2
			);
		}

		if (d.isReleasedRight && zoomData.underMofidication) {
			zoomData.underMofidication = false;
			if ((zoomData.tmpBottonRight - zoomData.tmpTopLeft).cwiseProduct(canvasSize).cwiseAbs().minCoeff() > 10) {
				zoomData.center = 0.5f*(zoomData.tmpBottonRight + zoomData.tmpTopLeft);
				zoomData.diagonal = 0.5f*(zoomData.tmpBottonRight - zoomData.tmpTopLeft).cwiseAbs();
				auto scaledBox = zoomData.scaled(canvasSize);
				float target_ratio = canvasSize[0] / canvasSize[1];
				float current_ratio = scaledBox.diagonal[0] / scaledBox.diagonal[1];
				if (current_ratio > target_ratio) {
					scaledBox.diagonal.y() = scaledBox.diagonal.x() / target_ratio;
				} else {
					scaledBox.diagonal.x() = scaledBox.diagonal.y() * target_ratio;
				}
				zoomData.diagonal = scaledBox.diagonal.cwiseQuotient(canvasSize);
			}
		}

		if (d.isHoovered && d.scroll != 0) {
			zoomData.diagonal = zoomData.diagonal.cwiseProduct(pow(1.15f, -d.scroll)*sibr::Vector2f(1, 1));
		}

		

		zoomData.diagonal = zoomData.diagonal.cwiseMin(sibr::Vector2f(0.5, 0.5));
		using Box = Eigen::AlignedBox2f;
		using Corner = Box::CornerType;

		Box target(sibr::Vector2f(0, 0), sibr::Vector2f(1, 1));
		Box current(zoomData.topLeft(), zoomData.bottomRight());
		
		if (!target.contains(current)) {
			Box inside = current;
			inside.clamp(target);
			for (int c = 0; c < 4; ++c) {
				Corner cType = (Corner)c;
				if ( (current.corner(cType)-inside.corner(cType)).isZero() ) {			
					Corner opposite = (Corner)(3 - c);
					zoomData.center += (inside.corner(opposite) - current.corner(opposite));
					break;
				}
			}
		}

	}


	void SegmentSelection::update(const CallBackData & callback, const sibr::Vector2i & size, const ZoomData & zoom)
	{
		sibr::Vector2i pos = zoom.scaled(size.cast<float>()).uvFromBoxPos(callback.positionRatio).cast<int>();

		if (callback.isHoovered && callback.isClickedRight && callback.ctrlPressed && (!first || valid)) {
			firstPosScreen = callback.mousePos.cast<int>();
			firstPosIm = pos.cast<int>();
			secondPosScreen = firstPosScreen;
			first = true;
		} else if (callback.isHoovered && first) {
			secondPosScreen = callback.mousePos.cast<int>();
			secondPosIm = pos.cast<int>();

			if (callback.isClickedRight) {
				first = false;
				valid = true;
				computeRasterizedLine();
			}
		}
	}

	void SegmentSelection::computeRasterizedLine()
	{
		if (!valid) {
			return;
		}

		sibr::Vector2i diff = secondPosIm - firstPosIm;
		int l = diff.cwiseAbs().maxCoeff();
		rasterizedLine.resize(l + 1);
		for (int i = 0; i <= l; ++i) {
			rasterizedLine[i] = (firstPosIm.cast<float>() + (i / (float)l)*diff.cast<float>()).cast<int>();
		}

		std::cout << firstPosIm << " " << secondPosIm << std::endl;
		std::cout << rasterizedLine.front() << " " << rasterizedLine.back() << std::endl;
		//for (const auto & p : rasterizedLine) {
		//	std::cout << p << " ";
		//}
		std::cout << std::endl;
	}

	void DisplayImageGui(
		GLuint texture,
		const sibr::Vector2i & displaySize,
		const sibr::Vector2f& uv0,
		const sibr::Vector2f& uv1 
	) {
		ImGui::Image((void*)(intptr_t)(texture), ImVec2(float(displaySize[0]), float(displaySize[1])), ImVec2(uv0[0], uv0[1]), ImVec2(uv1[0], uv1[1]));
	}

	void ImageWithCallback(
		GLuint texture,
		const sibr::Vector2i & displaySize,
		CallBackData & callbackDataOut,
		const sibr::Vector2f & uv0,
		const sibr::Vector2f & uv1
	) {
		CallBackData & data = callbackDataOut;

		data.itemPos = toSIBR<float>(ImGui::GetCursorScreenPos());
		DisplayImageGui(texture, displaySize, uv0, uv1);

		data.itemSize = toSIBR<float>(ImGui::GetItemRectSize());
		data.isHoovered = ImGui::IsItemHovered();
		data.isClickedLeft = ImGui::IsMouseClicked(0);
		data.isReleasedLeft = ImGui::IsMouseReleased(0);
		data.isClickedRight = ImGui::IsItemClicked(1);
		data.isReleasedRight = ImGui::IsMouseReleased(1);
		data.ctrlPressed = ImGui::GetIO().KeyCtrl;
		data.scroll = ImGui::GetIO().MouseWheel;

		if (data.isHoovered) {
			data.mousePos = toSIBR<float>(ImGui::GetIO().MousePos);
			data.positionRatio = (data.mousePos - data.itemPos).cwiseQuotient(data.itemSize);
		}
	}

} // namespace sibr


namespace ImGui {

	const float GetTitleBarHeight() { return GetTextLineHeight() + GetStyle().FramePadding.y * 2.0f; }

	void DrawTextWindow(const char * title, const float x, const float y, const char * text) {
		ImGui::SetNextWindowPos(ImVec2(x, y));
		if (ImGui::Begin(title, NULL, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Text("%s", text);
		}
		ImGui::End();
	}

	void PushScaledItemWidth(float item_width)
	{
		ImGui::PushItemWidth(ImGui::GetIO().FontGlobalScale * item_width);
	}

	bool TabButton(const char * label, bool highlight, const ImVec2 & size)
	{
		if (highlight) {
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
		}
		bool b = ImGui::Button(label, size);
		if (highlight) {
			ImGui::PopStyleColor(2);
		}
		return b;
	}

}
