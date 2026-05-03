#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include "rbx_constants.h"
#include "rbx_filemesh.h"

using namespace godot;

static Ref<ResourceFormatRBXFileMesh> resource_loader_rbx_filemesh;

void initialize_gdextension_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_CLASS(RBXFileMesh)
	GDREGISTER_CLASS(ResourceFormatRBXFileMesh)
	//GDREGISTER_CLASS(ResourceFormatRBXFileMesh)

	resource_loader_rbx_filemesh.instantiate();
	ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_rbx_filemesh);

}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_rbx_filemesh);
	resource_loader_rbx_filemesh.unref();
}

extern "C"
{
	// Initialization
	GDExtensionBool GDE_EXPORT RobloxMeshType_Library_Init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
		init_obj.register_initializer(initialize_gdextension_types);
		init_obj.register_terminator(uninitialize_gdextension_types);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}