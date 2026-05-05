#include "rbx_filemesh.h"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"


//Macros to clean code a bit
#define ERASE_STR_AT_POS(str, pos) str = str.erase(0, pos);
#define ERASE_STR_AT_CHAR(str, pointChar, offset) ERASE_STR_AT_POS(str, str.find(pointChar) + offset)

#define READ_VERT_FLOAT(str, floatData, charToDelete)  \
    floatData = str.substr(0, str.find(",")).to_float(); \
    ERASE_STR_AT_CHAR(str, charToDelete, 1)

void RBXFileMesh::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("load_from_file", "file_name"), &RBXFileMesh::load_from_file);
    godot::ClassDB::bind_method(D_METHOD("get_version"), &RBXFileMesh::get_version);

    BIND_ENUM_CONSTANT(VERSION_1)
    BIND_ENUM_CONSTANT(VERSION_1_01)
}

Error RBXFileMesh::load_from_file(const String& p_path) {
    PackedVector3Array vertsPositions;
    PackedVector3Array vertsNormals;
    PackedVector2Array vertsTexCoord;

    ERR_FAIL_COND_V(!FileAccess::file_exists(p_path), ERR_FILE_NOT_FOUND);

    Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ);

    version = RBXFileMeshUtils::getVersionFromFile(file);

    ERR_FAIL_COND_V(version == VERSION_NONE, ERR_INVALID_DATA);

    Error loadError = OK;

    switch (version) {
        case VERSION_1:
        case VERSION_1_01:
            loadError = RBXFileMeshUtils::load_from_v1_file(file, vertsPositions, vertsNormals, vertsTexCoord, version == VERSION_1);
            break;
        case VERSION_NONE: //Not Possible but prevents warning
            break;
    }

    if (loadError != OK) return loadError;

    Array surface_array;
    surface_array.resize(ARRAY_MAX);
    surface_array[ARRAY_VERTEX] = vertsPositions;
    surface_array[ARRAY_NORMAL] = vertsNormals;
    surface_array[ARRAY_TEX_UV] = vertsTexCoord;
    add_surface_from_arrays(PRIMITIVE_TRIANGLES, surface_array);
    return OK;
}

Variant ResourceFormatRBXFileMesh::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
    ERR_FAIL_COND_V_MSG(!FileAccess::file_exists(p_path), Ref<RBXFileMesh>(), "Hi");
    Ref<RBXFileMesh> mesh = memnew(RBXFileMesh);

    mesh->load_from_file(p_path);
    return mesh;
}

PackedStringArray ResourceFormatRBXFileMesh::_get_recognized_extensions() const {
    PackedStringArray exts;
    exts.push_back("mesh");
    return exts;
}

bool ResourceFormatRBXFileMesh::_handles_type(const StringName &p_type) const {
    return ClassDB::is_parent_class(p_type, "RBXFileMesh");
}

String ResourceFormatRBXFileMesh::_get_resource_type(const String& p_path) const {
    return "RBXFileMesh";
}

RBXFileMesh::MeshVersion RBXFileMeshUtils::getVersionFromFile(Ref<FileAccess> p_file) {
    String versionStr = p_file->get_line();

    if (!versionStr.begins_with("version ")) {
        return RBXFileMesh::VERSION_NONE;
    }
    ERASE_STR_AT_POS(versionStr, 8);

    double version = versionStr.substr(0, 4).to_float();

    if (version == 1.00) return RBXFileMesh::VERSION_1;
    else if (version == 1.01) return RBXFileMesh::VERSION_1_01;
    else return RBXFileMesh::VERSION_NONE;
}

void RBXFileMeshUtils::readV1VertData(String& p_file_data, Vector3* p_pos_dist, Vector3* p_normal_dist,
    Vector2* p_tex_coord_dist, bool scaleVerts) {

    ERASE_STR_AT_CHAR(p_file_data, "[", 1);
    READ_VERT_FLOAT(p_file_data, p_pos_dist->x, ",");
    READ_VERT_FLOAT(p_file_data, p_pos_dist->y, ",");
    READ_VERT_FLOAT(p_file_data, p_pos_dist->z, "]");

    if (scaleVerts) *p_pos_dist *= 0.5f;

    ERASE_STR_AT_CHAR(p_file_data, "[", 1);
    READ_VERT_FLOAT(p_file_data, p_normal_dist->x, ",");
    READ_VERT_FLOAT(p_file_data, p_normal_dist->y, ",");
    READ_VERT_FLOAT(p_file_data, p_normal_dist->z, "]");

    ERASE_STR_AT_CHAR(p_file_data, "[", 1);
    READ_VERT_FLOAT(p_file_data, p_tex_coord_dist->x, ",");
    READ_VERT_FLOAT(p_file_data, p_tex_coord_dist->y, "]");

    p_tex_coord_dist->y = 1.0f - p_tex_coord_dist->y;
}

Error RBXFileMeshUtils::load_from_v1_file(Ref<FileAccess> p_file, PackedVector3Array& p_pos_arr_dist, PackedVector3Array& p_normal_arr_dist,
    PackedVector2Array& p_tex_coord_arr_dist, bool scaleVerts) {
    int64_t faceCount = p_file->get_line().to_int();

    ERR_FAIL_COND_V(faceCount == 0, ERR_INVALID_DATA);
    String dataStr = p_file->get_as_text();
    ERR_FAIL_COND_V(dataStr.count("[") / 9 != faceCount, ERR_INVALID_DATA);

    ERASE_STR_AT_CHAR(dataStr, "[", 0);

    for (int64_t i = 0; i < faceCount; i++) {
        PackedVector3Array facePositions;
        PackedVector3Array faceNormals;
        PackedVector2Array faceTexCoords;

        facePositions.resize(3);
        faceNormals.resize(3);
        faceTexCoords.resize(3);

        for (int32_t i = 0; i < 3; i++) {
            int32_t index = i;
            if (index == 1) index = 2;
            else if (index == 2) index = 1;

            RBXFileMeshUtils::readV1VertData(dataStr, &facePositions[index],
                &faceNormals[index], &faceTexCoords[index], scaleVerts);
        }

        p_pos_arr_dist.append_array(facePositions);
        p_normal_arr_dist.append_array(faceNormals);
        p_tex_coord_arr_dist.append_array(faceTexCoords);
    }

    return OK;
}