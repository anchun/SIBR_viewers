

#include "core/assets/ActiveImageFile.hpp"
#include "core/assets/InputCamera.hpp"
#include "boost/algorithm/string.hpp"
#include <map>

#define SIBR_INPUTCAMERA_BINARYFILE_VERSION 10
#define IBRVIEW_TOPVIEW_SAVEVERSION "version002"

namespace sibr
{
	InputCamera::InputCamera(float f, float k1, float k2, int w, int h, int id) :
		_focal(f), _k1(k1), _k2(k2), _w(w), _h(h), _id(id), _active(true), _name("")
	{
		// Update fov and aspect ratio.
		float fov = 2.0f * atan(0.5f*h / f);
		float aspect = float(w) / float(h);

		Camera::aspect(aspect);
		Camera::fovy(fov);

		_id = id;
	}


	InputCamera::InputCamera(int id, int w, int h, sibr::Matrix4f m, bool active) :
		_active(active)
	{
		Vector3f t;
		float r[9];

		for (int i = 0; i < 9; i++)  r[i] = m(3 + i);
		for (int i = 0; i < 3; i++)  t[i] = m(12 + i);

		_w = w;
		_h = h;

		_focal = m(0);
		_k1 = m(1);
		_k2 = m(2);

		float fov = 2.0f * atan(0.5f*h / m(0));
		float aspect = float(w) / float(h);

		Eigen::Matrix3f		matRotation;
		matRotation <<
			r[0], r[1], r[2],
			r[3], r[4], r[5],
			r[6], r[7], r[8]
			;

		Camera::aspect(aspect);
		Camera::fovy(fov);

		// http://www.cs.cornell.edu/~snavely/bundler/bundler-v0.4-manual.html#S6
		// Do pos = -R' * t
		matRotation.transposeInPlace();
		Camera::position(-matRotation * t);

		Camera::rotation(Quaternionf(matRotation));

		_id = id;
		_name = "";
	}


	// ------------------------------------------------------------------------


	void InputCamera::size(uint w, uint h) { _w = w; _h = h; }
	uint InputCamera::w(void)  const { return _w; }
	uint InputCamera::h(void)  const { return _h; }
	bool InputCamera::isActive(void)  const { return _active; }

	/* compatibility for preprocess (depth) */


	Vector3f InputCamera::projectScreen(const Vector3f& pt) const {
		Vector3f proj_pt = project(pt);
		Vector3f screen_pt((proj_pt[0] + 1.f)*_w / 2.0f, (1.f - proj_pt[1])*_h / 2.0f, proj_pt[2] * 0.5f + 0.5f);

		return screen_pt;
	}

	float InputCamera::focal() const { return _focal; };
	float InputCamera::k1() const { return _k1; };
	float InputCamera::k2() const { return _k2; };

	std::vector<InputCamera> InputCamera::load(const std::string& datasetPath, float zNear, float zFar, const std::string & bundleName, const std::string & listName)
	{
		const std::string bundlerFile = datasetPath + "/cameras/" + bundleName;
		const std::string listFile = datasetPath + "/images/" + listName;
		const std::string clipFile = datasetPath + "/clipping_planes.txt";
		SIBR_LOG << "Loading input cameras." << std::endl;

		struct Z {
			Z() {}
			Z(float f, float n) : far(f), near(n) {}
			float far;
			float near;
		};

		std::vector<Z> nearsFars;

		float z;
		{ // Load znear & zfar for unprojecting depth samples
			SIBR_LOG << "Loading clipping planes from " << clipFile << std::endl;

			std::ifstream zfile(clipFile);
			// During preprocessing clipping planes are not yet defined
			// the preprocess utility "depth" defines this
			// SIBR_ASSERT(zfile.is_open());
			if (zfile.is_open()) {
				int num_z_values = 0;
				while (zfile >> z) {
					if (num_z_values % 2 == 0) {
						nearsFars.push_back(Z());
						nearsFars[nearsFars.size() - 1].near = z;
					}
					else {
						nearsFars[nearsFars.size() - 1].far = z;
					}
					++num_z_values;
				}

				if (num_z_values > 0 && num_z_values % 2 != 0) {
					nearsFars.resize(nearsFars.size() - 1);
				}

				if (nearsFars.size() == 0) {
					SIBR_WRG << " Could not extract at leat 2 clipping planes from '" << clipFile << "' ." << std::endl;
				}
			}
			else {
				SIBR_WRG << "Cannot open '" << clipFile << "' (not clipping plane loaded)." << std::endl;
			}

			//default values for all cameras
			if (nearsFars.size() == 0) {
				nearsFars.push_back(Z(zFar, zNear));
			}
		}

		// check bundler file
		std::ifstream bundle_file(bundlerFile);
		if (!bundle_file.is_open()) {
			SIBR_ERR << "Unable to load bundle file at path \"" << bundlerFile << "\"." << std::endl;
		}

		//check list of images
		std::ifstream list_images(listFile);
		if (!list_images.is_open()) {
			SIBR_ERR << "Unable to load list_images file at path \"" << listFile << "\"." << std::endl;
		}

		// read number of images
		std::string line;
		getline(bundle_file, line);	// ignore first line - contains version
		int numImages = 0;
		bundle_file >> numImages;	// read first value (number of images)
		getline(bundle_file, line);	// ignore the rest of the line

		// Read all filenames
		struct ImgInfos
		{
			std::string name;
			int id;
			int w, h;
		};
		std::vector<ImgInfos>	imgInfos;
		{
			ImgInfos				infos;
			while (true)
			{
				list_images >> infos.name;
				if (infos.name.empty()) break;
				list_images >> infos.w >> infos.h;
				infos.name.erase(infos.name.find_last_of("."), std::string::npos);
				infos.id = atoi(infos.name.c_str());
				imgInfos.push_back(infos);
				infos.name.clear();	// why? clear doesn't reclaim memory
			}
		}

		std::vector<InputCamera> cameras(numImages);
		//  Parse bundle.out file for camera calibration parameters
		for (int i = 0, infosId = 0, currentIdZnearZfar = 0; i < numImages && infosId < imgInfos.size(); i++) {
			const ImgInfos& infos = imgInfos[infosId];
			//if (i != infos.id)
			//	continue;

			Matrix4f m; // bundler params

			bundle_file >> m(0) >> m(1) >> m(2) >> m(3) >> m(4);
			bundle_file >> m(5) >> m(6) >> m(7) >> m(8) >> m(9);
			bundle_file >> m(10) >> m(11) >> m(12) >> m(13) >> m(14);

			//
			cameras[infosId] = InputCamera(infosId, infos.w, infos.h, m, true);
			// get rid of suffix of image file name
			cameras[infosId].name(infos.name);

			const Z & current_Z = nearsFars[currentIdZnearZfar];
			cameras[infosId].znear(current_Z.near); cameras[infosId].zfar(current_Z.far);

			++infosId;
			currentIdZnearZfar = std::min(currentIdZnearZfar + 1, (int)nearsFars.size() - 1);
		}


		// Load active images
		ActiveImageFile activeImageFile;
		activeImageFile.setNumImages((int)cameras.size());
		// load active image file and set (in)active
		if (activeImageFile.load(datasetPath + "/active_images.txt", false)) {
			for (int i = 0; i < numImages; i++) {
				if (!activeImageFile.active()[i])
					cameras[i].setActive(false);
			}
		}

		// Load excluded images
		ActiveImageFile excludeImageFile;
		excludeImageFile.setNumImages((int)cameras.size());
		// load exclude image file and set *in*active
		if (excludeImageFile.load(datasetPath + "/exclude_images.txt", false)) {
			for (int i = 0; i < numImages; i++) {
				// Attn (GD): invert the meaning of active for exclude:
				// only file numbers explicitly in exclude_images are set
				// to active, and these are the only ones we set to *inactive*
				// should really create a separate class or have a flag "invert"
				if (excludeImageFile.active()[i])
					cameras[i].setActive(false);
			}
		}
		return cameras;
	}

	std::vector<InputCamera> InputCamera::loadNVM(const std::string& nvmPath, float zNear, float zFar, std::vector<sibr::Vector2u> wh)
	{

		std::ifstream in(nvmPath);
		std::vector<InputCamera> cameras;

		if (in.is_open())
		{
			int rotation_parameter_num = 4;
			bool format_r9t = false;
			std::string token;
			if (in.peek() == 'N')
			{
				in >> token; //file header
				if (strstr(token.c_str(), "R9T"))
				{
					rotation_parameter_num = 9;    //rotation as 3x3 matrix
					format_r9t = true;
				}
			}

			int ncam = 0, npoint = 0, nproj = 0;
			// read # of cameras
			in >> ncam;  if (ncam <= 1) return std::vector<InputCamera>();

			//read the camera parameters

			std::function<Eigen::Matrix3f(const double[9])> matrix = [](const double q[9])
			{

				Eigen::Matrix3f m;
				double qq = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
				double qw, qx, qy, qz;
				if (qq > 0)
				{
					qw = q[0] / qq;
					qx = q[1] / qq;
					qy = q[2] / qq;
					qz = q[3] / qq;
				}
				else
				{
					qw = 1;
					qx = qy = qz = 0;
				}
				m(0, 0) = float(qw*qw + qx * qx - qz * qz - qy * qy);
				m(0, 1) = float(2 * qx*qy - 2 * qz*qw);
				m(0, 2) = float(2 * qy*qw + 2 * qz*qx);
				m(1, 0) = float(2 * qx*qy + 2 * qw*qz);
				m(1, 1) = float(qy*qy + qw * qw - qz * qz - qx * qx);
				m(1, 2) = float(2 * qz*qy - 2 * qx*qw);
				m(2, 0) = float(2 * qx*qz - 2 * qy*qw);
				m(2, 1) = float(2 * qy*qz + 2 * qw*qx);
				m(2, 2) = float(qz*qz + qw * qw - qy * qy - qx * qx);

				return m;
			};

			for (int i = 0; i < ncam; ++i)
			{
				double f, q[9], c[3], d[2];
				in >> token >> f;
				for (int j = 0; j < rotation_parameter_num; ++j) in >> q[j];
				in >> c[0] >> c[1] >> c[2] >> d[0] >> d[1];

				int wIm = 1, hIm = 1;
				if (ncam == wh.size()) {
					wIm = wh[i].x();
					hIm = wh[i].y();
				}
				//camera_data[i].SetFocalLength(f);
				cameras.push_back(InputCamera((float)f, (float)d[0], (float)d[1], wIm, hIm, i));

				float fov = 2.0f * atan(0.5f*hIm / (float)f);
				float aspect = float(wIm) / float(hIm);
				cameras[i].aspect(aspect);
				cameras[i].fovy(fov);

				//translation
				Vector3f posCam((float)c[0], (float)c[1], (float)c[2]);

				if (format_r9t)
				{

					std::cout << " WARNING THIS PART OF THE CODE WAS NEVER TESTED. IT IS SUPPOSED NOT TO WORK PROPERLY" << std::endl;
					Eigen::Matrix3f		matRotation;
					matRotation <<
						float(q[0]), float(q[1]), float(q[2]),
						float(q[3]), float(q[4]), float(q[5]),
						float(q[6]), float(q[7]), float(q[8])
						;
					matRotation.transposeInPlace();


					cameras[i].position(posCam);
					cameras[i].rotation(Quaternionf(matRotation));

				}
				else
				{

					Eigen::Matrix3f converter;
					converter <<
						1, 0, 0,
						0, -1, 0,
						0, 0, -1;
					//older format for compability
					Quaternionf quat((float)q[0], (float)q[1], (float)q[2], (float)q[3]);
					Eigen::Matrix3f	matRotation = converter.transpose()*quat.toRotationMatrix();
					matRotation.transposeInPlace();

					cameras[i].position(posCam);
					cameras[i].rotation(Quaternionf(matRotation));

				}
				//camera_data[i].SetNormalizedMeasurementDistortion(d[0]);
				cameras[i].name(token);
			}
			std::cout << ncam << " cameras; " << npoint << " 3D points; " << nproj << " projections\n";
		}
		else {
			SIBR_WRG << "Cannot open '" << nvmPath << std::endl;
		}

		return cameras;
	}

	std::vector<InputCamera> InputCamera::loadLookat(const std::string& lookatPath, const std::vector<sibr::Vector2u>& wh)
	{

		std::ifstream in(lookatPath);
		std::vector<InputCamera> cameras;

		if (in.is_open())
		{
			int i = 0;
			for (std::string line; std::getline(in, line); i++)
			{
				int w = 1920, h = 1280;
				if (wh.size() > 0) {
					int whI = std::min(i, (int)wh.size() - 1);
					w = wh[whI].x();
					h = wh[whI].y();
				}

				std::string camName = line.substr(0, line.find(" "));
				size_t originPos = line.find("-D origin=") + 10;
				size_t targetPos = line.find("-D target=") + 10;
				size_t upPos = line.find("-D up=") + 6;
				size_t fovPos = line.find("-D fov=") + 7;
				size_t clipPos = line.find("-D clip=") + 8;
				size_t endPos = line.size();

				std::string originStr = line.substr(originPos, targetPos - originPos - 11);
				std::string targetStr = line.substr(targetPos, upPos - targetPos - 7);
				std::string upStr = line.substr(upPos, fovPos - upPos - 8);
				std::string fovStr = line.substr(fovPos, clipPos - fovPos - 9);
				std::string clipStr = line.substr(clipPos, endPos - clipPos);

				std::vector<std::string> vecVal;
				boost::split(vecVal, originStr, [](char c) {return c == ','; });
				Vector3f Eye(std::strtof(vecVal[0].c_str(), 0), std::strtof(vecVal[1].c_str(), 0), std::strtof(vecVal[2].c_str(), 0));
				boost::split(vecVal, targetStr, [](char c) {return c == ','; });
				Vector3f At(std::strtof(vecVal[0].c_str(), 0), std::strtof(vecVal[1].c_str(), 0), std::strtof(vecVal[2].c_str(), 0));

				boost::split(vecVal, upStr, [](char c) {return c == ','; });
				Vector3f Up(std::strtof(vecVal[0].c_str(), 0), std::strtof(vecVal[1].c_str(), 0), std::strtof(vecVal[2].c_str(), 0));

				float fov = std::strtof(fovStr.c_str(), 0);

				boost::split(vecVal, clipStr, [](char c) {return c == ','; });
				Vector2f clip(std::strtof(vecVal[0].c_str(), 0), std::strtof(vecVal[1].c_str(), 0));

				Vector3f zAxis((Eye - At).normalized());
				Vector3f xAxis((Up.cross(zAxis)).normalized());
				Vector3f yAxis(zAxis.cross(xAxis));

				Vector3f transl(-Eye.dot(xAxis), -Eye.dot(yAxis), -Eye.dot(zAxis));

				Eigen::Matrix3f rotation;
				rotation << xAxis, yAxis, zAxis;
				rotation.transposeInPlace();

				Eigen::Matrix4f mLook;
				mLook.setZero();
				mLook.block<3, 3>(0, 0) = rotation;
				mLook.block<3, 1>(0, 3) = transl;
				mLook(3, 3) = 1;

				float fovRad = fov * M_PI / 180;
				float sibr_focal = 0.5*w / tan(fovRad / 2); // Blender plugin register horizontal fov ! Using w here

				Eigen::Matrix4f r(mLook);
				/*float m[15] = {
					sibr_focal,0,0,
					r(0,0),r(0,1),r(0,2),
					r(1,0),r(1,1),r(1,2),
					r(2,0),r(2,1),r(2,2),
					r(0,3),r(1,3),r(2,3)
				};
				*/
				Eigen::Matrix4f m;
				m(0) = sibr_focal;  m(1) = 0; m(2) = 0;
				m(3) = r(0, 0); m(4) = r(0, 1); m(5) = r(0, 2);
				m(6) = r(1, 0); m(7) = r(1, 1); m(8) = r(1, 2);
				m(9) = r(2, 0); m(10) = r(2, 1); m(11) = r(2, 2);
				m(12) = r(0, 3); m(13) = r(1, 3); m(14) = r(2, 3);


				bool isActive = true;

				cameras.push_back(InputCamera((int)cameras.size(), w, h, m, isActive));

				cameras[i].znear(clip.x());
				cameras[i].zfar(clip.y());
				cameras[i].name(camName);
			}

		}
		else {
			SIBR_WRG << "Cannot open '" << lookatPath << std::endl;
		}

		return cameras;

	}

	std::vector<InputCamera> InputCamera::loadColmap(const std::string & colmapSparsePath, const float zNear, const float zFar)
	{
		const std::string camerasListing = colmapSparsePath + "/cameras.txt";
		const std::string imagesListing = colmapSparsePath + "/images.txt";


		std::ifstream camerasFile(camerasListing);
		std::ifstream imagesFile(imagesListing);
		if (!camerasFile.is_open()) {
			SIBR_ERR << "Unable to load camera colmap file" << std::endl;
		}
		if (!imagesFile.is_open()) {
			SIBR_WRG << "Unable to load images colmap file" << std::endl;
		}

		std::vector<sibr::InputCamera> cameras;

		std::string line;

		struct CameraParametersColmap {
			size_t id;
			size_t width;
			size_t height;
			float  fx;
			float  fy;
			float  dx;
			float  dy;
		};

		std::map<size_t, CameraParametersColmap> cameraParameters;

		while (std::getline(camerasFile, line)) {
			if (line.empty() || line[0] == '#') {
				continue;
			}

			std::vector<std::string> tokens = sibr::split(line, ' ');
			if (tokens.size() < 8) {
				SIBR_WRG << "Unknown line." << std::endl;
				continue;
			}
			if (tokens[1] != "PINHOLE" && tokens[1] != "OPENCV") {
				SIBR_WRG << "Unknown camera type." << std::endl;
				continue;
			}

			CameraParametersColmap params;
			params.id = std::stol(tokens[0]);
			params.width = std::stol(tokens[2]);
			params.height = std::stol(tokens[3]);
			params.fx = std::stof(tokens[4]);
			params.fy = std::stof(tokens[5]);
			params.dx = std::stof(tokens[6]);
			params.dy = std::stof(tokens[7]);

			cameraParameters[params.id] = params;

		}

		// Now load the individual images and their extrinsic parameters
		sibr::Matrix3f converter;
		converter << 1, 0, 0,
			0, -1, 0,
			0, 0, -1;

		int camid = 0;
		while (std::getline(imagesFile, line)) {
			if (line.empty() || line[0] == '#') {
				continue;
			}
			std::vector<std::string> tokens = sibr::split(line, ' ');
			if (tokens.size() < 10) {
				SIBR_WRG << "Unknown line." << std::endl;
				continue;
			}
			size_t      id = std::stol(tokens[8]);
			float       qw = std::stof(tokens[1]);
			float       qx = std::stof(tokens[2]);
			float       qy = std::stof(tokens[3]);
			float       qz = std::stof(tokens[4]);
			float       tx = std::stof(tokens[5]);
			float       ty = std::stof(tokens[6]);
			float       tz = std::stof(tokens[7]);

			std::string imageName = tokens[9];
			const std::string::size_type dotPos = imageName.find_last_of(".");
			imageName = imageName.substr(0, dotPos);

			if (cameraParameters.find(id) == cameraParameters.end())
			{
				SIBR_ERR << "Could not find intrinsics for image: "
					<< tokens[9] << std::endl;
			}
			const CameraParametersColmap & camParams = cameraParameters[id];

			const sibr::Quaternionf quat(qw, qx, qy, qz);
			const sibr::Matrix3f orientation = converter.transpose() * quat.toRotationMatrix().transpose() * converter;
			sibr::Vector3f position(tx, ty, tz);
			position = -(orientation * converter.transpose() * position);

			sibr::InputCamera camera(camParams.fy, 0.0f, 0.0f, camParams.width, camParams.height, camid);
			camera.name(imageName);
			camera.position(position);
			camera.rotation(sibr::Quaternionf(orientation));
			camera.znear(zNear);
			camera.zfar(zFar);
			cameras.push_back(camera);

			++camid;
			// Skip the observations.
			std::getline(imagesFile, line);
		}


		return cameras;
	}

	Vector3f			InputCamera::unprojectImgSpaceInvertY(const sibr::Vector2i & pixelPos, const float & depth) const
	{
		sibr::Vector2f pos2dGL(2.0f*((pixelPos.cast<float>() + sibr::Vector2f(0.5, 0.5)).cwiseQuotient(sibr::Vector2f(w(), h()))) - sibr::Vector2f(1, 1));  //to [-1,1]
		pos2dGL.y() = -pos2dGL.y();
		return unproject(sibr::Vector3f(pos2dGL.x(), pos2dGL.y(), depth));
	}

	Vector3f			InputCamera::projectImgSpaceInvertY(const Vector3f& point3d) const
	{
		sibr::Vector3f pos2dGL = project(point3d);
		pos2dGL.y() = -pos2dGL.y();
		sibr::Vector2f pos2dImg = (0.5f* (pos2dGL.xy() + sibr::Vector2f(1, 1))).cwiseProduct(sibr::Vector2f(w(), h()));
		return sibr::Vector3f(pos2dImg.x(), pos2dImg.y(), pos2dGL.z());
	}

	bool				InputCamera::loadFromBinary(const std::string& filename)
	{
		ByteStream	bytes;

		if (bytes.load(filename))
		{
			uint8	version;
			float	focal;
			float	k1;
			float	k2;
			uint16	w;
			uint16	h;
			Vector3f	pos;
			Quaternionf	rot;
			float		fov;
			float		aspect;
			float		znear;
			float		zfar;

			bytes
				>> version;

			if (version != SIBR_INPUTCAMERA_BINARYFILE_VERSION)
			{
				// Maybe the file format has been updated, or your binary file is not about InputCamera...
				SIBR_ERR << "incorrect file format (version number does not correspond)." << std::endl;

				return false;
			}

			bytes
				>> focal >> k1 >> k2 >> w >> h
				>> pos.x() >> pos.y() >> pos.z()
				>> rot.w() >> rot.x() >> rot.y() >> rot.z()
				>> fov >> aspect >> znear >> zfar
				;

			_focal = focal;
			_k1 = k1;
			_k2 = k2;
			_w = (uint)w;
			_h = (uint)h;
			Camera::position(pos);
			Camera::rotation(rot);
			Camera::fovy(fov);
			Camera::aspect(aspect);
			Camera::znear(znear);
			Camera::zfar(zfar);

			return true;
		}
		else
		{
			SIBR_WRG << "cannot open file '" << filename << "'." << std::endl;
		}
		return false;
	}

	void				InputCamera::saveToBinary(const std::string& filename) const
	{
		ByteStream	bytes;

		uint8	version = SIBR_INPUTCAMERA_BINARYFILE_VERSION;
		float	focal = _focal;
		float	k1 = _k1;
		float	k2 = _k2;
		uint16	w = (uint16)_w;
		uint16	h = (uint16)_h;
		Vector3f	pos = position();
		Quaternionf	rot = rotation();
		float		fov = _fov;
		float		aspect = _aspect;
		float		znear = _znear;
		float		zfar = _zfar;

		bytes
			<< version
			<< focal << k1 << k2 << w << h
			<< pos.x() << pos.y() << pos.z()
			<< rot.w() << rot.x() << rot.y() << rot.z()
			<< fov << aspect << znear << zfar
			;

		bytes.saveToFile(filename);
	}

	void InputCamera::readFromFile(std::istream& infile)
	{
		std::string version;
		infile >> version;
		if (version != IBRVIEW_TOPVIEW_SAVEVERSION)
		{
			SIBR_WRG << "Sorry but your TopView camera configuration "
				"is too old (we added new features since!)" << std::endl;
			return;
		}

		Vector3f v;
		infile >> v.x() >> v.y() >> v.z();
		Quaternionf q;
		infile >> q.x() >> q.y() >> q.z() >> q.w();
		set(v, q);
	}

	void InputCamera::writeToFile(std::ostream& outfile) const
	{
		outfile << IBRVIEW_TOPVIEW_SAVEVERSION "\n";
		Vector3f v = transform().position();
		Quaternionf q = transform().rotation();
		outfile << " " << v.x() << " " << v.y() << " " << v.z() << " ";
		outfile << q.x() << " " << q.y() << " " << q.z() << " " << q.w();
	}

	std::string InputCamera::toBundleString(bool negativeZ) const {

		std::stringstream ss;
		ss << std::setprecision(16);
		float focal = 0.5f*h() / tan(fovy() / 2.0f); // We cannot set the focal but we need to compute it

		Eigen::Matrix3f r = transform().rotation().toRotationMatrix();
		sibr::Vector3f t = -transform().rotation().toRotationMatrix().transpose()*position();

		ss << focal << " " << k1() << " " << k2() << "\n"; // The focal is set to zero in the loading module we use fov=2.0f * atan( 0.5f*h / focal) here
		if (!negativeZ) {
			ss << r(0) << " " << r(1) << " " << r(2) << "\n";
			ss << r(3) << " " << r(4) << " " << r(5) << "\n";
			ss << r(6) << " " << r(7) << " " << r(8) << "\n";
			ss << t(0) << " " << t(1) << " " << t(2) << "\n";
		}
		else {
			ss << r(0) << " " << -r(2) << " " << r(1) << "\n";
			ss << r(3) << " " << -r(5) << " " << r(4) << "\n";
			ss << r(6) << " " << -r(8) << " " << r(7) << "\n";
			ss << t(0) << " " << t(1) << " " << t(2) << "\n";
		}

		return ss.str();
	}

	std::vector<sibr::Vector2i> InputCamera::getImageCorners() const
	{
		return { {0,0}, {_w - 1, 0}, {_w - 1,_h - 1}, {0, _h - 1} };
	}

	void InputCamera::saveAsBundle(const std::vector<sibr::InputCamera> & cams, const std::string & fileName, bool negativeZ, const bool exportImages) {

		std::ofstream outputBundleCam;
		outputBundleCam.open(fileName);
		outputBundleCam << "# Bundle file v0.3" << std::endl;
		outputBundleCam << cams.size() << " " << 0 << std::endl;

		for (int c = 0; c < cams.size(); c++) {
			auto & camIm = cams[c];
			outputBundleCam << camIm.toBundleString(negativeZ);
		}

		outputBundleCam.close();

		// Export the images list and empty images (useful for fribr).
		if (exportImages) {
			std::ofstream outList;
			const std::string listpath = fileName + "/../list_images.txt";
			const std::string imagesDir = fileName + "/../visualize/";
			sibr::makeDirectory(imagesDir);

			outList.open(listpath);
			if (outList.good()) {
				for (int i = 0; i < cams.size(); ++i) {
					const sibr::InputCamera & cam = cams[i];
					const std::string imageName = cam.name().empty() ? sibr::intToString<8>(i) + ".jpg" : cam.name();
					outList << "visualize/" << imageName << " " << cam.w() << " " << cam.h() << std::endl;
					cv::Mat3b dummy(cam.h(), cam.w());
					cv::imwrite(imagesDir + imageName, dummy);
				}
				outList.close();
			} else {
				SIBR_WRG << "Unable to export images list to path \"" << listpath << "\"." << std::endl;
			}
		}
	}
} // namespace sibr
