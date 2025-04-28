import cv2
import json
import os
import subprocess
import time
import numpy as np

def nothing(x):
    pass

def force_manual_exposure(index):
    subprocess.run(["v4l2-ctl", "-d", f"/dev/video{index}", "-c", "auto_exposure=1"])
    time.sleep(0.5)

def setup_camera(index):
    force_manual_exposure(index)

    cap = cv2.VideoCapture(index)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
    return cap

def apply_settings(cap, settings):
    cap.set(cv2.CAP_PROP_EXPOSURE, settings.get('mapped_exposure', 200))
    cap.set(cv2.CAP_PROP_GAIN, settings.get('gain', 0))
    cap.set(cv2.CAP_PROP_BRIGHTNESS, settings.get('brightness', 128))
    cap.set(cv2.CAP_PROP_CONTRAST, settings.get('contrast', 128))
    cap.set(cv2.CAP_PROP_SATURATION, settings.get('saturation', 128))
    cap.set(cv2.CAP_PROP_HUE, settings.get('hue', 0))

def main(camera_index):
    cap = setup_camera(camera_index)

    cv2.namedWindow('Camera Control', cv2.WINDOW_NORMAL)
    cv2.resizeWindow('Camera Control', 800, 600)

    cv2.createTrackbar('Exposure', 'Camera Control', 100, 300, nothing)  
    cv2.createTrackbar('Gain', 'Camera Control', 0, 100, nothing)
    cv2.createTrackbar('Brightness', 'Camera Control', 0, 255, nothing)
    cv2.createTrackbar('Contrast', 'Camera Control', 0, 255, nothing)
    cv2.createTrackbar('Saturation', 'Camera Control', 0, 255, nothing)
    cv2.createTrackbar('Hue', 'Camera Control', 0, 179, nothing)

    settings_path = f'camera_settings/camera_{camera_index}.json'
    if os.path.exists(settings_path):
        with open(settings_path, 'r') as f:
            settings = json.load(f)
            print(f"Loaded settings for camera {camera_index}: {settings}")

            cv2.setTrackbarPos('Exposure', 'Camera Control', settings.get('exposure_slider', 100))
            cv2.setTrackbarPos('Gain', 'Camera Control', settings.get('gain', 0))
            cv2.setTrackbarPos('Brightness', 'Camera Control', settings.get('brightness', 128))
            cv2.setTrackbarPos('Contrast', 'Camera Control', settings.get('contrast', 128))
            cv2.setTrackbarPos('Saturation', 'Camera Control', settings.get('saturation', 128))
            cv2.setTrackbarPos('Hue', 'Camera Control', settings.get('hue', 0))

            apply_settings(cap, settings)

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        exposure_slider = cv2.getTrackbarPos('Exposure', 'Camera Control')
        gain = cv2.getTrackbarPos('Gain', 'Camera Control')
        brightness = cv2.getTrackbarPos('Brightness', 'Camera Control')
        contrast = cv2.getTrackbarPos('Contrast', 'Camera Control')
        saturation = cv2.getTrackbarPos('Saturation', 'Camera Control')
        hue = cv2.getTrackbarPos('Hue', 'Camera Control')

        real_exposure = int(exposure_slider * 2)  

        cap.set(cv2.CAP_PROP_EXPOSURE, real_exposure)
        cap.set(cv2.CAP_PROP_GAIN, gain)
        cap.set(cv2.CAP_PROP_BRIGHTNESS, brightness)
        cap.set(cv2.CAP_PROP_CONTRAST, contrast)
        cap.set(cv2.CAP_PROP_SATURATION, saturation)
        cap.set(cv2.CAP_PROP_HUE, hue)

        cv2.imshow('Camera Control', frame)

        key = cv2.waitKey(1) & 0xFF
        if key == 27:  
            break
        elif key == ord('s'):
            settings = {
                'exposure_slider': exposure_slider,
                'mapped_exposure': real_exposure,
                'gain': gain,
                'brightness': brightness,
                'contrast': contrast,
                'saturation': saturation,
                'hue': hue
            }
            os.makedirs('camera_settings', exist_ok=True)
            with open(settings_path, 'w') as f:
                json.dump(settings, f, indent=4)
            print(f"Settings saved for camera {camera_index}")

    cap.release()
    cv2.destroyAllWindows()


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
    main(lenovo_indexes[0])
    main(lenovo_indexes[2])
