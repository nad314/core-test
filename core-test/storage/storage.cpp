#include <main>
Storage* core::Getter<Storage>::getter = NULL;

void Storage::dispose() {
	cloud.dispose();
	cloudTree.dispose();
	pbvh.dispose();
}

int Storage::load(const char* path) {
	dispose();
	std::string ext = core::Path::getExt(path);
	for (auto& i : ext)
		i = toupper(i);
	if (ext == "CLOUD") {
		if (!cloud.loadRaw(path)) {
			cloud.dispose();
			return 1;
		}
	}
	else if (ext == "OBJ") {
		if (!cloud.loadObj(path)) {
			cloud.dispose();
			return 1;
		}
	}
	else {
		dispose();
		return 1;
	}
	cloud.normalize();
	cloudTree.build(cloud);
	pbvh.build(cloudTree);
	pbvh.setRadius(pbvh.estimateRadius()*0.8f);
	cloudTree.dispose();
	return 0;
}
