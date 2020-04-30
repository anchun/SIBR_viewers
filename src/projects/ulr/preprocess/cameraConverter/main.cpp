#include "core/system/CommandLineArgs.hpp"
#include "core/assets/InputCamera.hpp"

using namespace sibr;

/* Camera converter args. */
struct CameraConverterArgs : virtual AppArgs {
	RequiredArg<std::string> input = { "input", "input camera file" };
	RequiredArg<std::string> output = { "output", "output camera file" };
	Arg<std::string> transfo = { "transfo",  "", "matrix file" };
	Arg<sibr::Vector2u> inputRes = {"ires", {1920, 1080}, "input camera resolution (not required for all formats)"};
	Arg<sibr::Vector2u> outputRes = { "ores", {1920, 1080}, "output camera resolution (not required for all formats)" };
	Arg<bool> inverse = {"inverse", "reverse the transformation"};
	Arg<bool> bundleImageList = { "images_list", "for a bundle output, output list_images.txt" };
	Arg<bool> bundleImageFiles = { "images_files", "for a bundle output, output empty images in a 'visualize' subdirectory" };
};

/* SIBR binary path loader helper.
 * \param filename the .path binary file
 * \param cams will be populated with the loaded cameras
 * \param res the image resolution (for aspect ratio)
 * \return the loading status
 */
bool load(const std::string& filename, std::vector<InputCamera::Ptr> & cams, const sibr::Vector2u & res){
	sibr::ByteStream stream;
	if(!stream.load(filename)) {
		return false;
	}
	int32 num = 0;
	stream >> num;
	while (num > 0) {
		Camera cam;
		stream >> cam;
		cams.push_back(std::make_shared<InputCamera>(cam, res[0], res[1]));
		--num;
	}
	return stream;
}

/** SIBR binary path saver helper.
 * \param filename the .path output file
 * \param cams the cameras to save
 */
void save(const std::string& filename, const std::vector<InputCamera::Ptr> & cams){
	sibr::ByteStream stream;
	const int32 num = int32(cams.size());
	stream << num;
	for (const InputCamera::Ptr& cam : cams) {
		Camera subcam(*cam);
		stream << subcam;
	}
	stream.saveToFile(filename);
}

/** The camera converter is a utility to convert a camera path from a file format to another, with additional options.
 * You can specify a 4x4 transformation matrix to apply, stored in a txt file (values separated by spaces/newlines, f.i. the output of CloudCompare alignment).
 * If you want to apply the inverse transformation, use the --inverse flag.
 * Supported inputs: path (*), lookat (*), bundle, nvm (*), colmap
 * Supported outputs: path, bundle(*)
 * (*): requires an input/output resolution (often only for the aspect ratio).
 * */
int main(int ac, char** av) {

	// Parse Command-line Args
	sibr::CommandLineArgs::parseMainArgs(ac, av);
	const CameraConverterArgs args;

	// Load cameras.
	std::vector<InputCamera::Ptr> cams;
	const std::string ext = sibr::getExtension(args.input);
	if(ext == "path") {
		load(args.input, cams, args.inputRes);
	} else if(ext == "lookat") {
		cams = InputCamera::loadLookat(args.input, { args.inputRes });
	} else if (ext == "out") {
		cams = InputCamera::loadBundle(args.input);
	} else if (ext == "nvm") {
		cams = InputCamera::loadNVM(args.input, 0.01f, 1000.0f, {args.inputRes});
	} else if (sibr::directoryExists(args.input)) {
		// If we got a directory, assume colmap sparse.
		cams = InputCamera::loadColmap(args.input);
	} else {
		SIBR_ERR << "Unsupported path file extension: " << ext << "." << std::endl;
		return EXIT_FAILURE;
	}
	SIBR_LOG << "Loaded " << cams.size() << " cameras." << std::endl;


	// Load the transformation.
	std::ifstream transFile(args.transfo.get());
	sibr::Matrix4f transf = sibr::Matrix4f::Identity();
	if (transFile.is_open()) {
		for (int i = 0; i < 16; ++i) {
			float f;
			transFile >> f;
			transf(i) = f;
		}
		transFile.close();
	}
	transf.transposeInPlace();
	if (args.inverse) {
		transf = transf.inverse().eval();
	}

	// Apply transformation to each camera keypoints, if it's not identity.
	if(!transf.isIdentity()) {
		SIBR_LOG << "Applying transformation: " << std::endl << transf << std::endl;
		for (auto & cam : cams) {
			sibr::Vector3f pos = cam->position();
			sibr::Vector3f center = cam->position() + cam->dir();
			sibr::Vector3f up = cam->position() + cam->up();
			pos = (transf * pos.homogeneous()).xyz();
			center = (transf * center.homogeneous()).xyz();
			up = (transf * up.homogeneous()).xyz();
			cam->setLookAt(pos, center, (up - pos).normalized());
		}
	}

	// Save cameras.
	const std::string outExt = sibr::getExtension(args.output);
	if (outExt == "path") {
		save(args.output, cams);
	} else if (outExt == "out") {
		std::vector<InputCamera::Ptr> outCams;
		for(const auto & cam : cams) {
			const int outH = int(args.outputRes.get()[1]);
			const int outW = int(std::round(cam->aspect() * float(outH)));
			outCams.push_back(std::make_shared<InputCamera>(*cam, outW, outH));
		}
		sibr::InputCamera::saveAsBundle(outCams, args.output, args.bundleImageList, args.bundleImageFiles);
	} else {
		SIBR_ERR << "Unsupported output file extension: " << outExt << "." << std::endl;
		return EXIT_FAILURE;
	}
	SIBR_LOG << "Saved transformed cameras to \"" << args.output.get() << "\"." << std::endl;
		
	return EXIT_SUCCESS;
}

