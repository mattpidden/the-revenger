import cv2
import numpy as np
import math
import time
import json
import os
import subprocess


def line_intersection(line1_start, line1_end, line2_start, line2_end):
    x1, y1 = line1_start
    x2, y2 = line1_end
    x3, y3 = line2_start
    x4, y4 = line2_end
    denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
    intersect_x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denominator
    intersect_y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / denominator
    return (intersect_x, intersect_y)

def mid_gradient(line1_start, line1_end, line2_start, line2_end):
    x1, y1 = line1_start
    x2, y2 = line1_end
    x3, y3 = line2_start
    x4, y4 = line2_end
    gradient1 = (y2 - y1) / (x2 - x1) if x2 != x1 else float('inf')
    gradient2 = (y4 - y3) / (x4 - x3) if x4 != x3 else float('inf')
    
    if gradient1 == float('inf') and gradient2 == float('inf'):
        return float('inf')
    elif gradient1 == float('inf'):
        angle_between = math.atan(gradient2)
    elif gradient2 == float('inf'):
        angle_between = math.atan(gradient1)
    else:
        angle_between = math.atan(abs((gradient2 - gradient1) / (1 + gradient1 * gradient2)))

    half_angle = angle_between / 2
    if gradient1 != float('inf'):
        angle1 = math.atan(gradient1)
    else:
        angle1 = math.pi / 2  
    if gradient2 != float('inf'):
        angle2 = math.atan(gradient2)
    else:
        angle2 = math.pi / 2 
    if (gradient1 > 0 and gradient2 < 0) or (gradient1 < 0 and gradient2 > 0):
        bisect_angle = angle1 + half_angle if angle1 > angle2 else angle1 - half_angle
    else:
        bisect_angle = angle1 + half_angle if angle1 < angle2 else angle1 - half_angle
    bisect_gradient = math.tan(bisect_angle)
    return bisect_gradient

def find_point_on_gradient(point, gradient, line_start, line_end):
    x1, y1 = point
    x2, y2 = line_start
    x3, y3 = line_end
    line_gradient = (y3 - y2) / (x3 - x2) if x3 != x2 else float('inf') 
    if line_gradient == float('inf'):
        x_intersect = x2
        y_intersect = y1 + gradient * (x_intersect - x1)
    else:
        x_intersect = (y2 - y1 + gradient * x1 - line_gradient * x2) / (gradient - line_gradient)
        y_intersect = gradient * (x_intersect - x1) + y1
    return (x_intersect, y_intersect)


def splitFace(vertices):
    A, B, C, D = vertices

    faceMiddle = line_intersection(A, C, B, D)
    ab_cd_mid_gradient = mid_gradient(A, B, C, D)
    bc_da_mid_gradient = mid_gradient(B, C, D, A)

    point1 = find_point_on_gradient(faceMiddle, bc_da_mid_gradient, A, B)
    point2 = find_point_on_gradient(faceMiddle, ab_cd_mid_gradient, B, C)
    point3 = find_point_on_gradient(faceMiddle, bc_da_mid_gradient, C, D)
    point4 = find_point_on_gradient(faceMiddle, ab_cd_mid_gradient, D, A)

    split1 = [A, point1, faceMiddle, point4]
    split2 = [point1, B, point2, faceMiddle]
    split3 = [faceMiddle, point2, C, point3]
    split4 = [point4, faceMiddle, point3, D]

    return [split1, split2, split3, split4]


def find_closest_color(bgr):
    COLOR_RANGES = {
        "R1": [(160, 100, 85), (180, 255, 255)],  
        "R2": [(0, 100, 85), (10, 255, 255)],    
        "Y":  [(11, 100, 75), (33, 255, 255)],    
        "G":  [(34, 100, 60), (89, 255, 255)],    
        "B":  [(90, 100, 60), (125, 255, 255)],   
        "W":  [(0, 0, 65), (180, 75, 255)],   
    }

    bgr_arr = np.uint8([[bgr]])
    hsv_pixel = cv2.cvtColor(bgr_arr, cv2.COLOR_BGR2HSV)[0][0]
    for name, (lower, upper) in COLOR_RANGES.items():
        lower_np = np.array(lower)
        upper_np = np.array(upper)
        hsv_pixel_reshaped = np.uint8([[hsv_pixel]]) 
        mask = cv2.inRange(hsv_pixel_reshaped, lower_np, upper_np)
        if mask[0][0]:
            confidence = 100.0
            return name[0], confidence

    return "O", 0.0


def init_scan(index, cubeTopCentre, cubeTopLeft, cubeTopRight, cubeMiddle, cubeBottomCentre, cubeBottomLeft, cubeBottomRight):
    topFaceNodes = [cubeTopCentre, cubeTopLeft, cubeMiddle, cubeTopRight]
    leftFaceNodes = [cubeTopLeft, cubeBottomLeft, cubeBottomCentre, cubeMiddle]
    rightFaceNodes = [cubeTopRight, cubeBottomRight, cubeBottomCentre, cubeMiddle]
    faces = [topFaceNodes, leftFaceNodes, rightFaceNodes]

    face_faclects = []
    for i in range(3):
        side = []
        for j in range(16):
            rect = []
            for k in range(4):
                coord = (0, 0)
                rect.append(coord)
            side.append(rect)
        face_faclects.append(side)

    for i, face in enumerate(faces):
        count = 0
        face_quarters = splitFace(face)
        for quarter in face_quarters:
            face_sixteenths = splitFace(quarter)
            for sixteenths in face_sixteenths:
                face_faclects[i][count] = sixteenths
                count += 1

    cap = cv2.VideoCapture(index)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

    settings_path = f"camera_settings/camera_{index}.json"
    if os.path.exists(settings_path):
        subprocess.run(["v4l2-ctl", "-d", f"/dev/video{index}", "-c", "auto_exposure=1"])
        time.sleep(0.5)
        with open(settings_path, 'r') as f:
            settings = json.load(f)
            real_exposure = settings.get('mapped_exposure', 200)
            cap.set(cv2.CAP_PROP_EXPOSURE, real_exposure)
            cap.set(cv2.CAP_PROP_GAIN, settings.get('gain', 0))
            cap.set(cv2.CAP_PROP_BRIGHTNESS, settings.get('brightness', 128))
            cap.set(cv2.CAP_PROP_CONTRAST, settings.get('contrast', 128))
            cap.set(cv2.CAP_PROP_SATURATION, settings.get('saturation', 128))
            cap.set(cv2.CAP_PROP_HUE, settings.get('hue', 0))

    return cap, face_faclects

def capture_and_scan_frame(cap, camera_name, errors, face_faclects, user_edits, allow_edits=False):
    BGR_COLORS = {
        "R": (0, 0, 255),
        "Y": (32, 220, 255),
        "G": (0, 255, 0),
        "B": (255, 0, 0),
        "O": (0, 0, 0),
        "W": (255, 255, 255),
    }
    color_order = list(BGR_COLORS.keys())

    ret, base_frame = cap.read()
    if not ret:
        return

    facelet_info = []

    for face_index, face in enumerate(face_faclects):
        colors_for_face = []
        for faclet_index, faclet in enumerate(face):
            mask = np.zeros(base_frame.shape[:2], dtype=np.uint8)
            pts = np.array(faclet, dtype=np.float32)
            centroid = np.mean(pts, axis=0)
            shrink_factor = 0.5
            shrunk_pts = centroid + shrink_factor * (pts - centroid)
            shrunk_pts = shrunk_pts.astype(np.int32).reshape((-1, 1, 2))
            mask = np.zeros(base_frame.shape[:2], dtype=np.uint8)
            cv2.fillPoly(mask, [shrunk_pts], 255)
            mean_color = cv2.mean(base_frame, mask=mask)[:3]
            name, confidence = find_closest_color(mean_color)
            edit_key = (face_index, faclet_index)
            if edit_key in user_edits:
                name = user_edits[edit_key]
            facelet_info.append({
                "face_index": face_index,
                "faclet_index": faclet_index,
                "polygon": faclet,
                "color_name": name,        
                "mean_bgr": mean_color       
            })
            colors_for_face.append(name)

    def draw_all(frame_to_draw):
        frame_copy = frame_to_draw.copy()
        for f in facelet_info:
            pts = np.array(f["polygon"], dtype=np.float32)
            centroid = np.mean(pts, axis=0)
            shrunk_pts = centroid + 0.5 * (pts - centroid)
            shrunk_pts = shrunk_pts.astype(np.int32).reshape((-1, 1, 2))
            pts_draw = pts.astype(np.int32)
            cv2.polylines(frame_copy, [shrunk_pts], isClosed=True, color=(255, 255, 255), thickness=1)
            cv2.polylines(frame_copy, [pts_draw], isClosed=True, color=(0, 0, 0), thickness=2)
            offset_pts_left = np.array([[[x - 450, y + 150] for (x, y) in f["polygon"]]], dtype=np.int32)
            overlay_left = frame_copy.copy()
            cv2.fillPoly(overlay_left, offset_pts_left, f["mean_bgr"])
            cv2.addWeighted(overlay_left, 1.0, frame_copy, 0.0, 0, frame_copy)
            for j in range(len(offset_pts_left[0])):
                corner1 = offset_pts_left[0][j]
                corner2 = offset_pts_left[0][(j + 1) % len(offset_pts_left[0])]
                cv2.line(frame_copy, tuple(corner1), tuple(corner2), (0, 0, 0), 2)
            offset_pts_right = np.array([[[x + 450, y + 150] for (x, y) in f["polygon"]]], dtype=np.int32)
            overlay_right = frame_copy.copy()
            corrected_bgr = BGR_COLORS[f["color_name"]]
            cv2.fillPoly(overlay_right, offset_pts_right, corrected_bgr)
            cv2.addWeighted(overlay_right, 1.0, frame_copy, 0.0, 0, frame_copy)
            for j in range(len(offset_pts_right[0])):
                corner1 = offset_pts_right[0][j]
                corner2 = offset_pts_right[0][(j + 1) % len(offset_pts_right[0])]
                cv2.line(frame_copy, tuple(corner1), tuple(corner2), (0, 0, 0), 2)
            M_right = np.mean(offset_pts_right[0], axis=0).astype(int)
            cv2.putText(frame_copy, f["color_name"], (M_right[0] - 5, M_right[1] + 5), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 0, 0), 1, cv2.LINE_AA)
            cv2.rectangle(frame_copy, (0, 0), (475, 20 + len(errors)*16), (255, 255, 255), -1)
            for i, message in enumerate(errors):
                cv2.putText(frame_copy, message, (20, 20 + i*16), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 0), 1, cv2.LINE_AA)

        return frame_copy

    def mouse_callback(event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            for f in facelet_info:
                pts = np.array(f["polygon"], dtype=np.int32)
                inside = cv2.pointPolygonTest(pts, (x, y), False)
                if inside >= 0:
                    current_idx = color_order.index(f["color_name"])
                    new_idx = (current_idx + 1) % len(color_order)
                    f["color_name"] = color_order[new_idx]
                    user_edits[(f["face_index"], f["faclet_index"])] = f["color_name"]
                    break

    cv2.namedWindow(camera_name, cv2.WINDOW_NORMAL)
    if camera_name == "Bottom Front Camera":
        cv2.resizeWindow(camera_name, 960, 1080)
        cv2.moveWindow(camera_name, 0, 0) 
    elif camera_name == "Top Back Camera":
        cv2.resizeWindow(camera_name, 960, 1080)
        cv2.moveWindow(camera_name, 960, 0) 


    if allow_edits:
        cv2.setMouseCallback(camera_name, mouse_callback)
        while True:
            display_frame = draw_all(base_frame)
            cv2.imshow(camera_name, display_frame)
            key = cv2.waitKey(20)
            if key == 27:
                break
    else:
        display_frame = draw_all(base_frame)
        cv2.imshow(camera_name, display_frame)
        cv2.waitKey(1)

    scan_results = [ [] for _ in range( len(face_faclects) ) ]
    for f in facelet_info:
        scan_results[f["face_index"]].append(f["color_name"])

    return scan_results, user_edits

def check_state_is_valid(input_string: str):
    errors = []
    if len(input_string) != 96:
        errors.append(f"Must be of length 96 chars. (found {len(input_string)})")
    valid_colors = {'W', 'O', 'G', 'R', 'B', 'Y'}
    color_count = {c: 0 for c in valid_colors}
    invalid_chars = 0
    for c in input_string:
        if c not in valid_colors:
            invalid_chars = invalid_chars + 1
        if c != 'E':
            color_count[c] += 1
    if invalid_chars != 0:
        errors.append(f"Could not classify {invalid_chars} facelets.")
    centre_indices = [5,6,9,10,85,86,89,90,37,38,41,42,69,70,73,74,21,22,25,26,53,54,57,58]
    centre_color_count = {c: 0 for c in valid_colors}
    for i in centre_indices:
        if input_string[i] != 'E':
            centre_color_count[input_string[i]] += 1
    for color, count in centre_color_count.items():
        if count != 4:
            errors.append(f"Found {count} '{color}' centre facelets.")
    edge_indices = [1,2,4,8,7,11,13,14,17,18,33,34,49,50,65,66,29,30,45,46,61,62,77,78,81,82,84,88,87,91,93,94,20,24,23,27,36,40,39,43,52,56,55,59,68,72,71,75]
    edge_color_count = {c: 0 for c in valid_colors}
    for i in edge_indices:
        if input_string[i] != 'E':
            edge_color_count[input_string[i]] += 1
    for color, count in edge_color_count.items():
        if count != 8:
            errors.append(f"Found {count} '{color}' edge facelets.")
    corner_indices = [0,3,12,15,16,19,28,31,32,35,44,47,48,51,60,63,64,67,76,79,80,83,92,95]
    corner_color_count = {c: 0 for c in valid_colors}
    for i in corner_indices:
        if input_string[i] != 'E':
            corner_color_count[input_string[i]] += 1
    for color, count in corner_color_count.items():
        if count != 4:
            errors.append(f"Found {count} '{color}' corner facelets.")
    return errors

def capture_clicks(index):
    cap = cv2.VideoCapture(index)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
    clicks = []
    def click_callback(event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN and len(clicks) < 7:
            clicks.append((x, y))
            print(f"Corner {len(clicks)}: ({x}, {y})")
    cv2.namedWindow("Select each corner of the cube")
    cv2.setMouseCallback("Select each corner of the cube", click_callback)

    while (len(clicks) < 7):
        ret, frame = cap.read()
        if not ret:
            break
        for i, (x, y) in enumerate(clicks):
            cv2.circle(frame, (x, y), 5, (0, 255, 0), -1)
            cv2.putText(frame, str(i+1), (x + 5, y - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 1)
        cv2.imshow("Select each corner of the cube", frame)
        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

def finish_scan(cap):
    cap.release()
    cv2.destroyAllWindows()

def stream_scan(index1, index2):
    face_mapBRU = {0: "U", 1: "R", 2: "B"}
    face_mapDLF = {0: "D", 1: "F", 2: "L"}
    results = {}
    U_remap = [15, 14, 11, 10,12, 13, 8, 9, 3, 2, 7, 6, 0, 1, 4, 5]
    R_remap = [0, 3, 12, 15, 1, 2, 13, 14, 4, 7, 8, 11, 5, 6, 9, 10]
    B_remap = [15, 12, 3, 0, 14, 13, 2, 1, 11, 8, 7, 4, 10, 9, 6, 5]
    D_remap = [10, 9, 6, 5, 11, 8, 7, 4, 14, 13, 2, 1, 15, 12, 3, 0]
    L_remap = [5, 6, 9, 10, 4, 7, 8, 11, 1, 2, 13, 14, 0, 3, 12, 15]
    F_remap = [10, 9, 6, 5, 11, 8, 7, 4, 14, 13, 2, 1, 15, 12, 3, 0]
    face_remapsBRU = [U_remap, R_remap, B_remap]
    face_remapsDLF = [D_remap, F_remap, L_remap]
    
    cubeTopCentreBRU = (650, 214)
    cubeTopLeftBRU = (512, 269)
    cubeTopRightBRU = (794, 269)
    cubeMiddleBRU = (646,349)
    cubeBottomCentreBRU = (650, 513)
    cubeBottomLeftBRU = (530, 420)
    cubeBottomRightBRU = (777, 424)
    capBRU, face_faclectsBRU = init_scan(index2, cubeTopCentreBRU, cubeTopLeftBRU, cubeTopRightBRU, cubeMiddleBRU, cubeBottomCentreBRU, cubeBottomLeftBRU, cubeBottomRightBRU)
    
    cubeBottomCentreDLF = (631, 485)
    cubeBottomRightDLF = (772, 429)
    cubeBottomLeftDLF = (514, 423)
    cubeMiddleDLF = (657,349)
    cubeTopCentreDLF = (646, 206)
    cubeTopRightDLF = (751, 289)
    cubeTopLeftDLF = (524, 288)
    capDLF, face_faclectsDLF = init_scan(index1, cubeBottomCentreDLF, cubeBottomRightDLF, cubeBottomLeftDLF, cubeMiddleDLF, cubeTopCentreDLF, cubeTopRightDLF, cubeTopLeftDLF)

    for _ in range(25):
        # warm up cameras but throw away frames
        ret1, frame1 = capBRU.read()
        ret2, frame2 = capDLF.read()

    wait = input("Press enter to start scanning...")

    count = 0
    allow_edits = False
    errors = []
    max_frames = 5  
    user_editsBRU = {}
    user_editsDLF = {}
    try:
        while (count <= max_frames):
            resultsDLF, user_editsDLF = capture_and_scan_frame(capDLF, "Bottom Front Camera", errors, face_faclectsDLF, user_editsDLF, allow_edits)
            if resultsDLF:
                for i, face_colors in enumerate(resultsDLF):
                    ordered = [face_colors[j] for j in face_remapsDLF[i]]
                    results[face_mapDLF[i]] = ''.join(ordered)
            resultsBRU, user_editsBRU = capture_and_scan_frame(capBRU, "Top Back Camera", errors, face_faclectsBRU, user_editsBRU, allow_edits)
            if resultsBRU:
                for i, face_colors in enumerate(resultsBRU):
                    ordered = [face_colors[j] for j in face_remapsBRU[i]]
                    results[face_mapBRU[i]] = ''.join(ordered)
            result_string = f"{results["U"]}{results["L"]}{results["F"]}{results["R"]}{results["B"]}{results["D"]}"
            errors.clear()
            errors = check_state_is_valid(result_string)
            if (len(errors) == 0):
                print(result_string)
                with open("output.txt", "w") as f:
                    f.write(f"{results['U']}{results['L']}{results['F']}{results['R']}{results['B']}{results['D']}")
                return
            else:
                count = count + 1
                if count >= max_frames:
                    print("Invalid")
                    with open("output.txt", "w") as f:
                        f.write(f"Invalid")
                    return
                    
        
    finally:
        finish_scan(capBRU)
        finish_scan(capDLF)


def get_lenovo_camera_indexes():
    camera_indexes = []
    for i in range(10): 
        video_device = f"/dev/video{i}"
        if os.path.exists(video_device):
            try:
                result = subprocess.run(
                    ["v4l2-ctl", "-d", video_device, "--info"],
                    capture_output=True, text=True, check=True
                )
                if "lenovo" in result.stdout.lower():
                    camera_indexes.append(i)
            except subprocess.CalledProcessError:
                continue 

    return camera_indexes



if __name__ == "__main__":
    lenovo_indexes = get_lenovo_camera_indexes()
    #capture_clicks(lenovo_indexes[0])
    #capture_clicks(lenovo_indexes[2])
    stream_scan(lenovo_indexes[0], lenovo_indexes[2])