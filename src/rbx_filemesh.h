#pragma once

#include "godot_cpp/classes/array_mesh.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/classes/resource_format_loader.hpp"

using namespace godot;

class RBXFileMesh : public ArrayMesh {
    GDCLASS(RBXFileMesh, ArrayMesh)

public:
    enum MeshVersion {
        VERSION_1,
        VERSION_1_01,
    };

private:
    MeshVersion version = VERSION_1;

protected:
	static void _bind_methods();

public:

	RBXFileMesh() = default;
	~RBXFileMesh() override = default;

    Error load_from_file(const String &p_path);

    MeshVersion get_version() { return version; }
};

VARIANT_ENUM_CAST(RBXFileMesh::MeshVersion)

class ResourceFormatRBXFileMesh : public ResourceFormatLoader {
    GDCLASS(ResourceFormatRBXFileMesh, ResourceFormatLoader);
protected:
	static void _bind_methods() {}
public:
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &type) const override;
	virtual String _get_resource_type(const String &p_path) const override;
};

namespace RBXFileMeshUtils {
    /* Reads a Vector3 value from a Version 1 mesh file's data */
    void readV1Vec3(String& p_file_data, Vector3* p_dist);
    /* Reads a Vector3 value from a Version 1 mesh file's data */
    void readV1Vec2(String& p_file_data, Vector2* p_dist);

    /* Reads A Vert from a Version 1 file mesh*/
    void readV1VertData(String& p_file_data, Vector3* p_pos_dist, Vector3* p_normal_dist, Vector2* p_tex_coord_dist, RBXFileMesh::MeshVersion version);
};