#pragma

#include <string>
#include <vector>

namespace rt 
{
	class rtPipeline {
	public:
		rtPipeline(const std::string& vertFilepath, const std::string& fragFilepath);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);

	};
}