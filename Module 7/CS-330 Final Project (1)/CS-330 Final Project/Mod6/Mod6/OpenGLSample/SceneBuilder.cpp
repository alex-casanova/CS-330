#include "SceneBuilder.h"
#include "ShapeBuilder.h"
#include "Mesh.h"

using namespace std;

void SceneBuilder::UBuildScene(vector<GLMesh>& scene)
{
	/// <summary>
	/// BUILDING A SHAPE:
	/// To build a shape, follow the following standard:
	///		GLMesh <glmesh_name>
	///		<glmesh_name>.p = {
	///		red float, green float, blue float, alpha float,
	///		x scale float, y scale float, z scale float,
	///		x rotation degrees float, 1.0f, 0.0f, 0.0f,
	///		y rotation degrees float, 0.0f, 1.0f, 0.0f,
	///		z rotation degrees float, 0.0f, 0.0f, 1.0f,
	///		x translate float, y translate float, z translate float,
	///		x texture scale float, y texture scale float
	///		};
	///		<glmesh_name>.texFilename = "textures/[filename.filetype]";
	///		<glmesh_name>.length = float;
	///		<glmesh_name>.height = float;
	///		<glmesh_name>.radius = float;
	///		<glmesh_name>.number_of_sides = float;
	///		ShapeBuilder::<ShapeBuilderMethod>(<glmesh_name>);
	///		scene.push_back(<glmesh_name>);
	/// </summary>

	GLMesh table;
		table.p = {
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
				5.0f, 0.05f, 2.0f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				0.0f, -2.0f, -1.0f,
			//	x texture scale float, y texture scale float
				2.5, 2.5
		};
		table.texFilename = "textures/table.jpg";
		table.length = 2.0f;
		table.height = 0.05f;
		ShapeBuilder::UBuildCube(table);
		scene.push_back(table);

	GLMesh keyboard;
		keyboard.p = {
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
				2.0f, 0.05f, 1.0f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				0.0f, -1.95f, -0.65f,
			//	x texture scale float, y texture scale float
				3.5, 1.8
		};
		keyboard.texFilename = "textures/keyboard.jpg";
		keyboard.length = 2.0f;
		keyboard.height = 0.00005f;
		ShapeBuilder::UBuildCube(keyboard);
		scene.push_back(keyboard);

	GLMesh mousepad;
		mousepad.p = {
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
				3.5f, 0.025f, 1.5f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				0.0f, -1.96f, -0.8f,
			//	x texture scale float, y texture scale float
				3.5, 1.8
		};
		mousepad.texFilename = "textures/mousepad.jpg";
		mousepad.length = 3.0f;
		mousepad.height = 0.6f;
		ShapeBuilder::UBuildCube(mousepad);
		scene.push_back(mousepad);

	GLMesh cup;
		cup.p = {
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
				1.0f, 1.0f, 1.0f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				-1.8f, -1.99f, -1.65f,
			//	x texture scale float, y texture scale float
				3.5f, 1.8f
		};
		cup.texFilename = "textures/cup.jpg";
		cup.length = 2.0f;
		cup.height = 0.6f;
		cup.radius = 0.25f;
		cup.number_of_sides = 50.0f;
		cup.innerRadius = 0.23;
		ShapeBuilder::UBuildHollowCylinder(cup);
		scene.push_back(cup);

		GLMesh cupHandle;
		cupHandle.p = {
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
				0.8f, 0.1f, 0.8f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
				90.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				-1.95f, -1.25f, -1.2f,
			//	x texture scale float, y texture scale float
				3.5f, 1.8f
		};
		cupHandle.texFilename = "textures/cup.jpg";
		cupHandle.length = 2.0f;
		cupHandle.height = 0.5f;
		cupHandle.radius = 0.25f;
		cupHandle.number_of_sides = 50.0f;
		cupHandle.innerRadius = 0.21;
		ShapeBuilder::UBuildHollowCylinder(cupHandle);
		scene.push_back(cupHandle);




	GLMesh coffee;
		coffee.p = {
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
				0.9f, 1.0f, 0.9f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				-1.75f, -1.98f, -1.6f,
			//	x texture scale float, y texture scale float
				3.5f, 1.8f
		};
		coffee.texFilename = "textures/coffee.png";
		coffee.length = 2.0f;
		coffee.height = 0.55f;
		coffee.radius = 0.275f;
		coffee.number_of_sides = 20.0f;
		ShapeBuilder::UBuildCylinder(coffee);
		scene.push_back(coffee);

	GLMesh mouse;
		mouse.p = {
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
				0.15f, 0.025f, 0.2f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				1.3f, -1.9f, -0.65f,
			//	x texture scale float, y texture scale float
				3.5f, 1.8f
		};
		mouse.texFilename = "textures/mouse.jpg";
		mouse.length = 2.0f;
		mouse.height = 1.6f;
		mouse.radius = 1.25f;
		mouse.number_of_sides = 50.0f;
		ShapeBuilder::UBuildCylinder(mouse);
		scene.push_back(mouse);

	GLMesh monitor;
		monitor.p = { 
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
					4.0f, 1.5f,0.025f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,		
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				0.0f, -1.76f, -1.8f,
			//	x texture scale float, y texture scale float
				2.0, 2.0
								
		};
		monitor.texFilename = "textures/monitor.jpg";
		monitor.length = 2.0f;
		monitor.height = 1.6f;
		ShapeBuilder::UBuildCube(monitor);
		scene.push_back(monitor);
		
	GLMesh monitorStand;
		monitorStand.p = {
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
				0.5f, 0.2f,0.025f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				0.0f, -1.96f, -1.8f,
			//	x texture scale float, y texture scale float
				3.5, 1.8

		};
		monitorStand.texFilename = "textures/monitor.jpg";
		monitorStand.length = 2.0f;
		monitorStand.height = 1.6f;
		ShapeBuilder::UBuildCube(monitorStand);
		scene.push_back(monitorStand);

	GLMesh wall;
		wall.p = {
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
				20.0f, 10.0f, 20.0f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				0.0f, -5.0f, 1.0f,
			//	x texture scale float, y texture scale float
				20.0f, 10.0f
		};
		wall.texFilename = "textures/wall.jpg";
		wall.length = 1.0f;
		wall.height = 1.0f;
		ShapeBuilder::UBuildCube(wall);
		scene.push_back(wall);

	GLMesh charger;
		charger.p = {
			//	red float, green float, blue float, alpha float,
				1.0f, 1.0f, 1.0f, 1.0f,
			// x = left/right, y = up/down, z = in/out,
			// x scale float, y scale float, z scale float,
				0.2f, 0.2, 0.2f,
			//	x rotation degrees float, 1.0f, 0.0f, 0.0f,
					45.0f, 1.0f, 0.0f, 0.0f,
			//	y rotation degrees float, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
			//	z rotation degrees float, 0.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 0.0f, 1.0f,
			//	x translate float, y translate float, z translate float,
				-0.5f, -1.74f, -1.5f,
			//	x texture scale float, y texture scale float
			 	1.0f, 1.0f
		};
		charger.texFilename = "textures/charger.jpg";
		charger.length = 2.0f;
		charger.height = 1.6f;
		charger.radius = 1.25f;
		charger.number_of_sides = 50.0f;
		ShapeBuilder::UBuildCircle(charger);
		scene.push_back(charger);
}

