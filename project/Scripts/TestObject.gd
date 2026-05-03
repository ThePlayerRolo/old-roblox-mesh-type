extends MeshInstance3D

@onready var swordMesh := load("res://Models/sword.mesh")
var swordMaterial : StandardMaterial3D = preload("res://Materials/SwordMaterial.tres")

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var mesh_data := swordMesh
	mesh_data.surface_set_material(0, swordMaterial)
	mesh = mesh_data


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	pass
