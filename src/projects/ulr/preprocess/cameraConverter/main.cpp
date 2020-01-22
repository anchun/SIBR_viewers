#include "core/system/CommandLineArgs.hpp"
#include "core/assets/InputCamera.hpp"

using namespace sibr;

struct CameraConverterArgs : virtual AppArgs {
	RequiredArg<std::string> input = { "input", "input camera file" };
	RequiredArg<std::string> output = { "output", "output camera file" };
	RequiredArg<std::string> transfo = { "transfo", "matrix file" };
	Arg<bool> inverse = {"inverse", "reverse the transformation"};
	
};

bool load(const std::string& filename, std::vector<Camera> & cams){
	
	sibr::ByteStream stream;
	if(!stream.load(filename)) {
		return false;
	}

	int32 num = 0;
	stream >> num;
	while (num > 0)
	{
		Camera cam;
		stream >> cam;
		cams.emplace_back(std::move(cam));
		--num;
	}
	return stream;
}

void save(const std::string& filename, const std::vector<Camera> & cams)
{
	sibr::ByteStream stream;
	const int32 num = (int32)cams.size();
	stream << num;
	for (const Camera& cam : cams)
		stream << cam;

	stream.saveToFile(filename);
}

/** The camera converter is expecting a transformation contained in a txt file as values separated by spaces/newlines. Typically, the console output of CloudCompare.
 * If you have saved the inverse transformation, use the --inverse flag.
 * Supports only .path binary files for now.
 * */
int main(int ac, char** av) {

	// Parse Command-line Args
	sibr::CommandLineArgs::parseMainArgs(ac, av);
	const CameraConverterArgs args;

	// Load the transformation.
	std::ifstream transFile(args.transfo.get());
	sibr::Matrix4f transf = sibr::Matrix4f::Identity();
	if(!transFile.is_open()) {
		SIBR_ERR << "Unable to find transformation file." << std::endl;
		return EXIT_FAILURE;
	}
	for(int i = 0; i < 16; ++i) {
		float f;
		transFile >> f;
		transf(i) = f;
	}
	transFile.close();
	transf.transposeInPlace();
	if (args.inverse) {
		transf = transf.inverse().eval();
	}

	SIBR_LOG << "Transformation matrix is: " << std::endl << transf << std::endl;

	// Load cameras.
	std::vector<Camera> cams;
	if(!load(args.input, cams)) {
		SIBR_ERR << "Unable to find cameras file." << std::endl;
		return EXIT_FAILURE;
	}
	SIBR_LOG << "Loaded " << cams.size() << " cameras." << std::endl;

	// Apply trasnformation to each camera keypoints.
	for(auto & cam : cams) {
		sibr::Vector3f pos = cam.position();
		sibr::Vector3f center = cam.position() + cam.dir();
		sibr::Vector3f up = cam.position() + cam.up();
		pos = (transf * pos.homogeneous()).xyz();
		center = (transf * center.homogeneous()).xyz();
		up = (transf * up.homogeneous()).xyz();
		cam.setLookAt(pos, center, (up-pos).normalized());
	}

	// Save the new file.
	save(args.output, cams);
	SIBR_LOG << "Saved transformed cameras to \"" << args.output.get() << "\"." << std::endl;
		
	return EXIT_SUCCESS;
}

