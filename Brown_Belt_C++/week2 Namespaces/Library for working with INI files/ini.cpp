#include "ini.h"

namespace Ini {
	Section& Document::AddSection(string name) {
		sections.insert({ name,Section() });
		return sections[name];
	}

	const Section& Document::GetSection(const string& name) const {
		return sections.at(name);
	}

	size_t Document::SectionCount() const {
		return sections.size();
	}

	Document Load(istream& input) {
		std::string in = "";
		Document doc;
		Section* section = nullptr;
		while (std::getline(input, in)) {
			if (in.size() > 0 && in[0] == '[' && in[in.size() - 1] == ']') {
				section = &(doc.AddSection(string(++in.begin(), --in.end())));
			}
			else {
				size_t pos = in.find('=');
				if (pos != in.npos && section != nullptr) {
					(*section)[in.substr(0, pos)] = in.substr(pos + 1, in.size());
				}
			}
		}
		return doc;
	}
}