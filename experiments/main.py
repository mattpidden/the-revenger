import time
import threading
from cube import RubiksCube4x4
from vis import visualize_rubiks_cube
from scan import init_scan, capture_and_scan_frame, finish_scan
import cv2


rubiks_cube = RubiksCube4x4(method="colours", colours=[
  ["Y", "W", "W", "W", "W", "W", "W", "W", "W", "W", "W", "W", "W", "W", "W", "W"], # TOP
  ["W", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y", "Y"], # BOTTOM
  ["O", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R"], # FRONT
  ["R", "O", "O", "O", "O", "O", "O", "O", "O", "O", "O", "O", "O", "O", "O", "O"], # BACK
  ["B", "G", "G", "G", "G", "G", "G", "G", "G", "G", "G", "G", "G", "G", "G", "G"], # LEFT
  ["G", "B", "B", "B", "B", "B", "B", "B", "B", "B", "B", "B", "B", "B", "B", "B"], # RIGHT
])

thread = threading.Thread(target=visualize_rubiks_cube, args=(rubiks_cube,), daemon=True)
thread.start()

cap, face_faclects = init_scan()

while True:
  scan_results = capture_and_scan_frame(cap, face_faclects)
  print(scan_results)
  faces = ["U", "F", "R"]
  for i, face in enumerate(scan_results):
    result = [face[i:i+4] for i in range(0, 16, 4)]

    #rubiks_cube._update_face(faces[i], result)


finish_scan(cap)