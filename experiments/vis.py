import open3d as o3d
import numpy as np
from cube import RubiksCube4x4
import time

def create_cubelet(center, face_colors):
    geometries = []

    face_directions = {
        "U": [0, 1, 0],  # Up
        "D": [0, -1, 0],  # Down
        "F": [0, 0, 1],  # Front
        "B": [0, 0, -1],  # Back
        "L": [-1, 0, 0],  # Left
        "R": [1, 0, 0],  # Right
    }

    for face, normal in face_directions.items():
        if face in face_colors:
            color = face_colors[face]

            # Create the base cubelet
            cubelet = o3d.geometry.TriangleMesh.create_box(width=1.0, height=1.0, depth=1.0)

            if face == "F" or face == "B":
                cubelet.translate(np.array(center) - np.array([0.5, 0.5, 0.5]))
                facelet = o3d.geometry.TriangleMesh.create_box(width=0.9, height=0.9, depth=0.01)
                facelet.translate(np.array(center) + 0.5 * np.array(normal) - np.array([0.45, 0.45, 0.005]))
            elif face == "U" or face == "D":
                cubelet.translate(np.array(center) - np.array([0.5, 0.5, 0.5]))
                facelet = o3d.geometry.TriangleMesh.create_box(width=0.9, height=0.01, depth=0.9)
                facelet.translate(np.array(center) + 0.5 * np.array(normal) - np.array([0.45, 0.005, 0.45]))
            elif face == "L" or face == "R":
                cubelet.translate(np.array(center) - np.array([0.5, 0.5, 0.5]))
                facelet = o3d.geometry.TriangleMesh.create_box(width=0.01, height=0.9, depth=0.9)
                facelet.translate(np.array(center) + 0.5 * np.array(normal) - np.array([0.005, 0.45, 0.45]))
            
            cubelet.paint_uniform_color([0.2, 0.2, 0.2])  # Base color for cubelet
            geometries.append(cubelet)
            facelet.paint_uniform_color(color)
            geometries.append(facelet)

    return geometries


def calculate_position(face, row, col):
    offset = 1.5  # Center of the cube offset
    face_positions = {
        "U": (col - offset, offset, offset - row),
        "D": (col - offset, -offset, offset - row),
        "F": (col - offset, offset - row, offset),
        "B": (offset - col, offset - row , -offset),
        "L": (-offset, offset - row, offset - col),
        "R": (offset, offset - row, col - offset),
    }
    return face_positions[face]


def visualize_rubiks_cube(cube):
    # Define color mapping
    color_map = {
        "W": [1, 1, 1],  # White
        "Y": [1, 1, 0],  # Yellow
        "G": [0, 1, 0],  # Green
        "B": [0, 0, 1],  # Blue
        "O": [1, 0.5, 0],  # Orange
        "R": [1, 0, 0],  # Red
    }

    vis = o3d.visualization.Visualizer()
    vis.create_window(window_name='TopRight', width=500, height=500, left=0, top=0)
    ctr = vis.get_view_control()
    parameters = o3d.io.read_pinhole_camera_parameters("CameraTopRight.json")
    ctr.convert_from_pinhole_camera_parameters(parameters, True)

    vis2 = o3d.visualization.Visualizer()
    vis2.create_window(window_name='BottomLeft', width=500, height=500, left=0, top=550)
    ctr2 = vis2.get_view_control()
    parameters2 = o3d.io.read_pinhole_camera_parameters("CameraBottomLeft.json")
    ctr2.convert_from_pinhole_camera_parameters(parameters2, True)

    try:
        while True:
            # Clear existing geometries
            vis.clear_geometries()
            vis2.clear_geometries()

            # Create a list to hold all cubelet geometries
            geometries = []

            # Iterate through each face and add its cubelets
            for face, face_colors in cube.cube.items():
                for row_idx, row in enumerate(face_colors):
                    for col_idx, color in enumerate(row):
                        # Calculate the cubelet position
                        x, y, z = calculate_position(face, row_idx, col_idx)
                        # Get face colors for the cubelet
                        facelet_colors = {face: color_map[color]}
                        # Create the cubelet and add its geometries
                        geometries.extend(create_cubelet([x, y, z], facelet_colors))

            # Add updated geometries
            for geo in geometries:
                vis.add_geometry(geo)
                vis2.add_geometry(geo)

            # Restore camera parameters to maintain view
            ctr.convert_from_pinhole_camera_parameters(parameters, True)
            ctr2.convert_from_pinhole_camera_parameters(parameters2, True)

            vis.poll_events()
            vis.update_renderer()
            vis2.poll_events()
            vis2.update_renderer()

            # Refresh every 0.5 seconds
            time.sleep(0.5)
    finally:
        vis.destroy_window()
        vis2.destroy_window()







        
   