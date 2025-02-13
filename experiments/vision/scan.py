import cv2
import numpy as np
import math



# Finds intersection of two lines
def line_intersection(line1_start, line1_end, line2_start, line2_end):
    x1, y1 = line1_start
    x2, y2 = line1_end
    x3, y3 = line2_start
    x4, y4 = line2_end
    denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
    intersect_x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denominator
    intersect_y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / denominator
    return (intersect_x, intersect_y)

# Finds the average gradient of two lines
def mid_gradient(line1_start, line1_end, line2_start, line2_end):
    x1, y1 = line1_start
    x2, y2 = line1_end
    x3, y3 = line2_start
    x4, y4 = line2_end
    gradient1 = (y2 - y1) / (x2 - x1) if x2 != x1 else float('inf')
    gradient2 = (y4 - y3) / (x4 - x3) if x4 != x3 else float('inf')
    
    # Step 2: Calculate the angle between the two lines using the formula
    if gradient1 == float('inf') and gradient2 == float('inf'):
        return float('inf')  # Both lines are vertical, they have the same angle
    elif gradient1 == float('inf'):
        angle_between = math.atan(gradient2)
    elif gradient2 == float('inf'):
        angle_between = math.atan(gradient1)
    else:
        angle_between = math.atan(abs((gradient2 - gradient1) / (1 + gradient1 * gradient2)))

    # Step 3: Halve the angle
    half_angle = angle_between / 2
    
    # Step 4: Calculate the angle of each line
    if gradient1 != float('inf'):
        angle1 = math.atan(gradient1)
    else:
        angle1 = math.pi / 2  # For vertical lines

    if gradient2 != float('inf'):
        angle2 = math.atan(gradient2)
    else:
        angle2 = math.pi / 2  # For vertical lines

    # Step 5: Detect if the lines have different polarities (one positive, one negative)
    if (gradient1 > 0 and gradient2 < 0) or (gradient1 < 0 and gradient2 > 0):
        # Lines have different polarities, so we need to adjust how we bisect
        bisect_angle = angle1 + half_angle if angle1 > angle2 else angle1 - half_angle
    else:
        # Same polarity, just add the half angle
        bisect_angle = angle1 + half_angle if angle1 < angle2 else angle1 - half_angle

    # Step 6: Calculate the gradient for the bisecting line
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
    COLORS = {
        "R": (0, 0, 255),
        "Y": (0, 255, 255),
        "G": (0, 255, 0),
        "B": (255, 0, 0),
        "O": (0, 165, 255),
        "W": (255, 255, 255),
    }
    closest_color = None
    closest_name = None
    min_distance = float("inf")
    for name, color in COLORS.items():
        distance = np.linalg.norm(np.array(bgr) - np.array(color))
        if distance < min_distance:
            min_distance = distance
            closest_color = color
            closest_name = name
    return closest_name, closest_color

def init_scan():
    #Adjust these values to line up with cube
    cubeTopCentre = (314, 115)
    cubeTopLeft = (187, 184)
    cubeTopRight = (443, 182)
    cubeMiddle = (321, 282)
    cubeBottomCentre = (317, 379)
    cubeBottomLeft = (212, 296)
    cubeBottomRight = (415, 298)

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
        edge1 = [face[0], face[1]] # Edge 1 and 3 are oppsite
        edge2 = [face[1], face[2]] # Edge 2 and 4 are oppsite
        edge3 = [face[2], face[3]] # Edge 1 and 3 are oppsite
        edge4 = [face[3], face[0]] # Edge 2 and 4 are oppsite
        face_quarters = splitFace(face)
        for quarter in face_quarters:
            face_sixteenths = splitFace(quarter)
            for sixteenths in face_sixteenths:
                faclet_map = {"0": 0, "1": 1, "2": 4, "3": 5, "4": 2, "5": 3, "6": 6, "7": 7, "8": 8, "9": 9, "10": 12, "11": 13, "12": 10, "13": 11, "14": 14, "15": 15}
                face_faclects[i][faclet_map[str(count)]] = sixteenths
                count += 1

    cap = cv2.VideoCapture(2)

    return cap, face_faclects

def capture_and_scan_frame(cap, face_faclects, show_colours = False):
    ret, frame = cap.read()
    
    if not ret:
        return
    
    height, width, _ = frame.shape
    center_x, center_y = width // 2, height // 2

    scan_results = []
    for face in face_faclects:
        face_colors = []
        for faclet in face:
            mask = np.zeros(frame.shape[:2], dtype=np.uint8)
            pts = np.array([faclet], dtype=np.int32)
            cv2.fillPoly(mask, pts, 255)
            mean_color = cv2.mean(frame, mask=mask)[:3]  
            name, closest_color = find_closest_color(mean_color)
            if show_colours:
                cv2.fillPoly(frame, pts, closest_color)
            
            face_colors.append(name)
            for i in range(len(faclet)):
                corner1 = faclet[i]
                corner2 = faclet[(i + 1) % len(faclet)]
                cv2.line(frame, (int(corner1[0]), int(corner1[1])), (int(corner2[0]), int(corner2[1])), (0, 0, 0), 2)
        scan_results.append(face_colors)

    cv2.imshow('Webcam with Cube Grid', frame)
    cv2.waitKey(1)
    return scan_results

def finish_scan(cap):
    cap.release()
    cv2.destroyAllWindows()
