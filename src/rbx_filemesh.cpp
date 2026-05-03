#include "rbx_filemesh.h"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/global_constants.hpp"


#define ERASE_STR_AT_POINT(str, pointChar) str = str.erase(0, dataStr.find(pointChar)+1);
#define SET_FLOAT_TO_STR_DATA(str, floatData, pointChar) floatData = str.substr(0, str.find(pointChar)).to_float();

//kinda ripped from this stackoverflow post:  https://stackoverflow.com/a/37327537
int64_t nthOccurance(const String& str, const String& substr, int64_t nth) {
    int64_t FinalVal = -1;
    int64_t count = 0;
    while (count != nth) {
        FinalVal += 1;
        FinalVal = str.find(substr, FinalVal);
        count++;
    }
    return FinalVal;
}

void RBXFileMesh::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("load_from_file", "file_name"), &RBXFileMesh::load_from_file);
    godot::ClassDB::bind_method(D_METHOD("get_version"), &RBXFileMesh::get_version);

    BIND_ENUM_CONSTANT(VERSION_1)
    BIND_ENUM_CONSTANT(VERSION_1_01)
}

Error RBXFileMesh::load_from_file(const String& p_path) {
    int64_t position;

    ERR_FAIL_COND_V(!FileAccess::file_exists(p_path), ERR_FILE_NOT_FOUND);
    String dataStr = FileAccess::get_file_as_string(p_path);
    ERR_FAIL_COND_V(!(dataStr.begins_with("version 1.00") || dataStr.begins_with("version 1.01")), ERR_INVALID_DATA);
    version = dataStr.begins_with("version 1.01") ? VERSION_1_01 : VERSION_1;
    dataStr = dataStr.erase(0, 12);

    position = dataStr.find("[");
    int64_t faceCount = dataStr.substr(0, position).to_int();
    ERR_FAIL_COND_V(faceCount == 0, ERR_INVALID_DATA);
    dataStr = dataStr.erase(0, position);

    PackedVector3Array vertsPositions;
    PackedVector3Array vertsNormals;
    PackedVector2Array vertsTexCoord;

    for (int64_t i = 0; i < faceCount; i++) {
        PackedVector3Array facePositions;
        PackedVector3Array faceNormals;
        PackedVector2Array faceTexCoords;
        float temp;

        facePositions.resize(3);
        faceNormals.resize(3);
        faceTexCoords.resize(3);

        for (int32_t i = 0; i < 3; i++) {
            int32_t index = i;
            if (index == 1) index = 2;
            else if (index == 2) index = 1;

            ERASE_STR_AT_POINT(dataStr, "[");
            facePositions[index].x = dataStr.substr(0, dataStr.find(",")).to_float();
            ERASE_STR_AT_POINT(dataStr, ",");
            facePositions[index].y = dataStr.substr(0, dataStr.find(",")).to_float();
            ERASE_STR_AT_POINT(dataStr, ",");
            facePositions[index].z = dataStr.substr(0, dataStr.find("]")).to_float();
            ERASE_STR_AT_POINT(dataStr, "[");

            faceNormals[index].x = dataStr.substr(0, dataStr.find(",")).to_float();
            ERASE_STR_AT_POINT(dataStr, ",");
            faceNormals[index].y = dataStr.substr(0, dataStr.find(",")).to_float();
            ERASE_STR_AT_POINT(dataStr, ",");
            faceNormals[index].z = dataStr.substr(0, dataStr.find("]")).to_float();
            ERASE_STR_AT_POINT(dataStr, "[");

            faceTexCoords[index].x = dataStr.substr(0, dataStr.find(",")).to_float();
            ERASE_STR_AT_POINT(dataStr, ",");
            faceTexCoords[index].y = dataStr.substr(0, dataStr.find(",")).to_float();

            if (version == VERSION_1) {
                facePositions[index] *= 0.5;
            }
            faceTexCoords[index].y = 1.0 - faceTexCoords[index].y;
        }

        vertsPositions.append_array(facePositions);
        vertsNormals.append_array(faceNormals);
        vertsTexCoord.append_array(faceTexCoords);

        facePositions.clear();
        faceNormals.clear();
        faceTexCoords.clear();
    }

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