import random
import numpy as np

class RubiksCube4x4:
    def __init__(self, method="solved", moves=None, colours=None):
        self.faces = ["U", "D", "F", "B", "L", "R"]
        self.colours = ["Y", "W", "G", "B", "R", "O"]
        
        if method == "solved":
            self.cube = self._create_solved_cube()
        elif method == "random":
            self.cube = self._create_solved_cube()
            self._apply_moves(self._generate_random_moves())
        elif method == "moves" and moves:
            self.cube = self._create_solved_cube()
            self._apply_moves(moves)
        elif method == "colours" and colours:
            self.cube = self._create_cube_from_colours(colours)
        else:
            raise ValueError("Invalid initialization method.")

    def _create_solved_cube(self):
        return {face: [[colour] * 4 for _ in range(4)] for face, colour in zip(self.faces, self.colours)}

    def _create_cube_from_colours(self, colours):
        if len(colours) != 6 or any(len(face) != 16 for face in colours):
            raise ValueError("colours must be a list of 6 faces with 16 elements each.")
        return {face: [colours[idx][i:i + 4] for i in range(0, 16, 4)] for idx, face in enumerate(self.faces)}

    def _generate_random_moves(self, count=20):
        moves = ["U", "D", "R", "L", "F", "B", "U'", "D'", "R'", "L'", "F'", "B'", "U2", "D2", "R2", "L2", "F2", "B2"]
        return [random.choice(moves) for _ in range(count)]
    
    def _update_face(self, face, faclets):
        self.cube[face] = faclets

    def _apply_moves(self, moves):
        for move in moves:
            self._apply_move(move)

    def _apply_move(self, move):
        # Handle basic moves (U, D, R, L, F, B)
        if move in ["U", "D", "R", "L", "F", "B"]:
            self._rotate_face(move)
        elif move in ["x", "y", "z"]:
            self._rotate_cube(move)
        elif move.endswith("2'"):  # Counterclockwise moves
            self._rotate_face(move[0], clockwise=False)
            self._rotate_face(move[0], clockwise=False)
        elif move.endswith("2"):  # Double moves
            self._rotate_face(move[0])
            self._rotate_face(move[0])
        elif move.endswith("'"):  # Double moves
            self._rotate_face(move[0], clockwise=False)

    def _rotate_face(self, face, clockwise=True):
        # Rotate the face itself
        face_matrix = np.array(self.cube[face])
        if clockwise:
            self.cube[face] = np.rot90(face_matrix, -1).tolist()
        else:
            self.cube[face] = np.rot90(face_matrix, 1).tolist()

        # Rotate the edges (adjacent rows/columns)
        # Define the edge relationships for each face
        adjacent_faces = {
            "U": [("F", 0), ("L", 0), ("B", 0), ("R", 0)],
            "D": [("F", 3), ("R", 3), ("B", 3), ("L", 3)],
            "F": [("U", 0), ("R", ":", 3), ("D", 0), ("L", ":", 0)],
            "B": [("U", 3), ("L", ":", 3), ("D", 3), ("R", ":", 0)],
            "L": [("U", ":", 0), ("F", ":", 0), ("D", ":", 0), ("B", ":", 3)],
            "R": [("U", ":", 3), ("B", ":", 0), ("D", ":", 3), ("F", ":", 3)],
        }


        edges = adjacent_faces[face]
        if not clockwise:
            edges.reverse()

        temp = self._get_edge(edges[-1])
        for i in range(len(edges) - 1, 0, -1):
            self._set_edge(edges[i], self._get_edge(edges[i - 1]))
        self._set_edge(edges[0], temp)

    def _rotate_cube(self, move):
        if move == "x":  # Rotate around x-axis
            # Front to Down, Down to Back, Back to Up, Up to Front
            temp = self.cube["F"]
            self.cube["F"] = self.cube["D"]
            self.cube["D"] = np.rot90(self.cube["B"], 2).tolist()
            self.cube["B"] = np.rot90(self.cube["U"], 2).tolist()
            self.cube["U"] = temp
            # Rotate L and R faces
            self.cube["L"] = np.rot90(self.cube["L"], -1).tolist()
            self.cube["R"] = np.rot90(self.cube["R"], 1).tolist()

        elif move == "y":  # Rotate around y-axis
            # Front to left, Right to front, Back to right, Left to back
            temp = self.cube["F"]
            self.cube["F"] = self.cube["R"]
            self.cube["R"] = self.cube["B"]
            self.cube["B"] = self.cube["L"]
            self.cube["L"] = temp
            # Rotate U and D faces
            self.cube["U"] = np.rot90(self.cube["U"], -1).tolist()
            self.cube["D"] = np.rot90(self.cube["D"], 1).tolist()

        elif move == "z":  # Rotate around z-axis
            # Up to right, Left to up, Down to left, Right to down
            temp = self.cube["U"]
            self.cube["U"] = self.cube["L"]
            self.cube["L"] = self.cube["D"]
            self.cube["D"] = self.cube["R"]
            self.cube["R"] = temp
            # Rotate F and B faces
            self.cube["F"] = np.rot90(self.cube["F"], -1).tolist()
            self.cube["B"] = np.rot90(self.cube["B"], 1).tolist()



    def _get_edge(self, edge):
        face, index, col = edge if len(edge) == 3 else (*edge, None)
        if col is not None:  # Column index
            return [row[col] for row in self.cube[face]]
        else:  # Row index
            return self.cube[face][index]

    def _set_edge(self, edge, values):
        face, index, col = edge if len(edge) == 3 else (*edge, None)
        if col is not None:  # Column index
            for row, value in zip(self.cube[face], values):
                row[col] = value
        else:  # Row index
            self.cube[face][index] = values

    def display(self):
        colours_to_emojis_map = {"W": "⬜", "Y": "🟨", "G": "🟩", "B": "🟦", "O": "🟧", "R": "🟥"}
        face_order = ["U", "L", "F", "R", "B", "D"] 

        face_emoji = {
            face: [[colours_to_emojis_map[color] for color in row] for row in self.cube[face]]
            for face in face_order
        }

        print("  U    ")
        print("L F R B")
        print("  D    ")

        for i in range(4):
            print(f"        {''.join(face_emoji['U'][i])}")
        for i in range(4):
            print(f"{''.join(face_emoji['L'][i])}{''.join(face_emoji['F'][i])}{''.join(face_emoji['R'][i])}{''.join(face_emoji['B'][i])}")
        for i in range(4):
            print(f"        {''.join(face_emoji['D'][i])}")