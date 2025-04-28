import serial
import time
import subprocess
import tkinter as tk
from PIL import Image, ImageTk
import cv2
from threading import Thread
import numpy as np
import random
import sys
from PIL import Image, ImageTk, ImageSequence
from tkinter import font as tkfont
import os

arduino = serial.Serial("/dev/ttyACM0", 9600, timeout=1)
start_time = time.time()
end_time = time.time()
timer_active = False
gif_frames = []
gif_index = 0
state = "WWWWWWWWWWWWWWWWOOOOOOOOOOOOOOOOGGGGGGGGGGGGGGGGRRRRRRRRRRRRRRRRBBBBBBBBBBBBBBBBYYYYYYYYYYYYYYYY"
solution = []
pause = False
cap_left = None
cap_right = None
scanning = True
all_moves = ["R", "R2", "R'", "r", "r2", "r'", "Rw", "Rw2", "Rw'",
            "L", "L2", "L'", "l", "l2", "l'", "Lw", "Lw2", "Lw'",
            "F", "F2", "F'", "f", "f2", "f'", "Fw", "Fw2", "Fw'",
            "B", "B2", "B'", "b", "b2", "b'", "Bw", "Bw2", "Bw'",
            "U", "U2", "U'", "u", "u2", "u'", "Uw", "Uw2", "Uw'",
            "D", "D2", "D'", "d", "d2", "d'", "Dw", "Dw2", "Dw'", ]


def send_command(command):
    global pause
    if pause == True:
        while (pause == True):
            time.sleep(1)
    arduino.write(f"{command}\n".encode())
    print(f"Sent command: {command}")

    while True:
        line = ""
        line = arduino.readline().decode().strip()
        if line == command:
            print(f"Confirmed: {line}")
            break
        elif line != '':
            print(f"Ignored: {line}")

def calibration():
    time.sleep(2.0)
    send_command("c")
    send_command("00")
    status_text.set(f"Status: Idle")

def calibration_with_cube():
    time.sleep(2.0)
    send_command("C")
    send_command("00")
    status_text.set(f"Status: Idle")

def set_speed_1():
    send_command("S1")
    status_text.set(f"Status: Idle")

def set_speed_2():
    send_command("S2")
    status_text.set(f"Status: Idle")

def set_speed_3():
    send_command("S3")
    status_text.set(f"Status: Idle")

def scramble():
    scramble = random.choices(all_moves, k=20)
    for move in scramble:
        move_text.set(f"Applying random move: {move}")
        send_command(move)
        time.sleep(0.5)
    send_command("00")
    status_text.set(f"Status: Idle")

def find_solution():
    global solution
    solution = []
    process = subprocess.Popen(["./solver", state],  cwd="/home/mattpidden/Documents/3Year/DIS/code/solver", stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=1 )
    for line in process.stdout:
        line = line.strip()
        if line:
            solution.append(line)
    process.wait()
    process.stdout.close()
    process.stderr.close()
    print(f"Solution length: {len(solution)}")
    move_text.set(f"Found solution in {len(solution)} moves")
    status_text.set(f"Status: Ready to solve")

def toggle_cameras():
    global scanning
    scanning = not scanning
    status_text.set(f"Status: Idle")

def scan():
    global timer_active, start_time, end_time, solution, state, scanning
    status_text.set("Status: Scanning")
    send_command("00")
    scanning = True
    process = subprocess.Popen(
        ["python", "auto_scan.py"],
        cwd="/home/mattpidden/Documents/3Year/DIS/code/scanner",
        stdin=subprocess.PIPE,
        start_new_session=True
    )
    time.sleep(5)
    process.stdin.write(b'\n')
    process.stdin.flush()
    process.wait()
    with open("../scanner/output.txt", "r") as f:
        state = f.read().strip()
    if (state == "Invalid"):
        print("Invalid scan")
        timer_active = False
        end_time = time.time()
        status_text.set(f"Status: Invalid Scan")
        return
    print(state)
    status_text.set("Status: Idle")
    move_text.set(f"Valid cube state: {state}")
    scanning = False

def pause():
    global pause
    pause = not pause
    if pause:
        status_text.set(f"Status: Paused")
    else:
        status_text.set(f"Status: Idle")

def stop():
    sys.exit()
    status_text.set(f"Status: Idle")

def solve():
    global timer_active, start_time, end_time, solution

    time.sleep(1.0)
    send_command("N")
    time.sleep(1.0)
    send_command("N")
    time.sleep(1.0)
    send_command("N")
    time.sleep(1.0)
    send_command("N")
    time.sleep(1.0)

    send_command("T")
    start_time = time.time()
    timer_active = True

    for i, move in enumerate(solution):
        move_text.set(f"Applying move {i+1}/{len(solution)}: {move}")
        print(f"Move {i+1}/{len(solution)}")
        send_command(move)
        time.sleep(0.01)

    timer_active = False
    end_time = time.time()
    send_command("T")
    move_text.set("")
    status_text.set(f"Status: Solved")
    time.sleep(1.0)
    send_command("00")

def full_solve():
    global timer_active, start_time, end_time, solution, state, scanning
    send_command("00")
    scanning = True

    time.sleep(1.0)
    process = subprocess.Popen(
        ["python", "manual_scan.py"],
        cwd="/home/mattpidden/Documents/3Year/DIS/code/scanner",
        stdin=subprocess.PIPE,
        start_new_session=True
    )

    solving_process = subprocess.Popen(
        ["./solver", "-s"],
        cwd="/home/mattpidden/Documents/3Year/DIS/code/solver",
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE, 
        universal_newlines=True, 
        bufsize=1
    )

    time.sleep(2.0)
    send_command("N")
    time.sleep(1.0)
    send_command("N")
    time.sleep(1.0)
    send_command("N")
    time.sleep(1.0)
    send_command("N")
    time.sleep(1.0)

    status_text.set(f"Status: Solving")
    send_command("T")
    start_time = time.time()
    timer_active = True
    process.stdin.write(b'\n')
    process.stdin.flush()
    process.wait()
    with open("../scanner/output.txt", "r") as f:
        state = f.read().strip()
    if (state == "Invalid"):
        print("Invalid scan")
        timer_active = False
        end_time = time.time()
        send_command("T")
        status_text.set(f"Status: Invalid Scan")
        return
    print(state)
    move_text.set(f"Valid cube state: {state}")
    scanning = False

    solution = []
    i = 0
    solving_process.stdin.write(f'{state}\n')
    solving_process.stdin.flush()
    for line in solving_process.stdout:
        line = line.strip()
        if line:
            i = i + 1
            solution.append(line)
            move_text.set(f"Applying move {i+1}: {line}")
            print(f"Move {i+1}/{len(solution)+1}")
            send_command(line)
            time.sleep(0.01)
    solving_process.wait()
    solving_process.stdout.close()
    solving_process.stderr.close()

    timer_active = False
    end_time = time.time()
    send_command("T")

    print(f"Solution length: {len(solution)}")
    move_text.set(f"Found solution in {len(solution)} moves")
    status_text.set(f"Status: Solved")
    time.sleep(1.0)
    send_command("S1")
    send_command("00")


# USER INTERFACE STARTS HERE
root = tk.Tk()
root.title("The Revenger Control Dashboard")
root.geometry("2024x1400")
custom_font = tkfont.Font(family="SevenSegment", size=60)
custom_font_small = tkfont.Font(family="SevenSegment", size=18)
img = Image.open("robot.png")
img = img.resize((1000, 642))
robot_img = ImageTk.PhotoImage(img)
logo = Image.open("logo.png")
logo = logo.resize((750, 175))
logo_img = ImageTk.PhotoImage(logo)

main_frame = tk.Frame(root, bg='#141414')
main_frame.pack(fill='both', expand=True)

left_cam_label = tk.Label(main_frame)
left_cam_label.place(x=0, y=0, width=960, height=712)
left_cam_text = tk.StringVar(value=f"Bottom Left Camera: Live")
left_cam_text_label = tk.Label(main_frame, textvariable=left_cam_text, font=custom_font_small, bg='#0a0a0a', fg='#ad0000')
left_cam_text_label.place(x=120.0, y = 0.0, anchor='nw')

right_cam_label = tk.Label(main_frame)
right_cam_label.place(relx=1.0, y=0, width=960, height=712, anchor='ne')
right_cam_text = tk.StringVar(value=f"Top Right Camera: Live")
right_cam_text_label = tk.Label(main_frame, textvariable=right_cam_text, font=custom_font_small, bg='#0a0a0a', fg='#ad0000')
right_cam_text_label.place(x=1760, y = 0.0, anchor='ne')

logo_label = tk.Label(main_frame, image=logo_img, bg='#0a0a0a')
logo_label.place(relx=0.5, rely=0.0, anchor='n')


status_text = tk.StringVar(value=f"Status: Idle")
info_frame = tk.Frame(main_frame, bg='#0a0a0a')
info_frame.place(relx=0.5, y=185, anchor='n')
time_label = tk.Label(info_frame, text="00:00.00", font=custom_font, bg='#0a0a0a', fg='#ad0000')
time_label.pack()
status_label = tk.Label(info_frame, textvariable=status_text, font=custom_font_small, bg='#0a0a0a', fg='#ad0000')
status_label.pack()

move_text = tk.StringVar(value=f"")
move_label = tk.Label(main_frame, textvariable=move_text, font=custom_font_small, bg='#141414', fg='#ad0000')
move_label.place(relx=0.5, rely = 0.75, anchor='n')

button_grid_frame = tk.Frame(main_frame, bg='#141414')
button_grid_frame.place(relx=0.5, rely=0.8, anchor='n')

button_info = [
    ("Calibrate w/ Cube", calibration_with_cube),
    ("Set Speed 2", set_speed_2),
    ("Scramble", scramble),
    ("Find Solution", find_solution),
    ("Toggle Cameras", toggle_cameras),
    ("Set Speed 1", set_speed_1),
    ("Set Speed 3", set_speed_3),
    ("Scan", scan),
    ("Solve", solve),
    ("Full Solve", full_solve)
]

def wrapped_command(label, func):
    def wrapper():
        print(f"Button pressed: {label}")
        status_text.set(f"Status: {label}")
        Thread(target=func, args=(), daemon=True).start()
    return wrapper


for i, (label, command) in enumerate(button_info):
    row, col = divmod(i, 5)
    btn = tk.Button(button_grid_frame, text=label, font=custom_font_small, bg='#0a0a0a', fg='#ad0000', activebackground='#333333', activeforeground='#ad0000', width=15, height=2, command=wrapped_command(label, command))
    btn.grid(row=row, column=col, padx=10, pady=5)



def update_camera(cam_index, index, label, name, text_label):
    global scanning, cap_left, cap_right, gif_frames, gif_index

    if gif_frames == []:
        gif = Image.open("nosignal.gif")
        gif_frames = [ImageTk.PhotoImage(frame.copy().resize((960, 712))) for frame in ImageSequence.Iterator(gif)]

    if scanning:
        text_label.set(f"{name} Camera: No Signal")
        if index == 0 and cap_left is not None and cap_left.isOpened():
            cap_left.release()
            cap_left = None
        if index == 1 and cap_right is not None and cap_right.isOpened():
            cap_right.release()
            cap_right = None
        if gif_frames:
            label.imgtk = gif_frames[gif_index]
            label.configure(image=gif_frames[gif_index])
            gif_index = (gif_index + 1) % len(gif_frames)
    else:
        text_label.set(f"{name} Camera: Live")
        if index == 0:
            if cap_left is None or not cap_left.isOpened():
                cap_left = cv2.VideoCapture(cam_index)
            ret, frame = cap_left.read()
        else:
            if cap_right is None or not cap_right.isOpened():
                cap_right = cv2.VideoCapture(cam_index)
            ret, frame = cap_right.read()

        if ret:
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            frame = cv2.resize(frame, (960, 712))
            img = Image.fromarray(frame)
            imgtk = ImageTk.PhotoImage(image=img)
            label.imgtk = imgtk
            label.configure(image=imgtk)

    root.after(50, update_camera, cam_index, index, label, name, text_label)


update_camera(2, 0, left_cam_label, "Bottom Left", left_cam_text)
update_camera(4, 1, right_cam_label, "Top Right", right_cam_text)

def update_timer():
    global timer_active, start_time, end_time
    if timer_active:
        end_time = time.time()
    elapsed = end_time - start_time
    secs = int(elapsed)
    mins = int(secs / 60)
    rem_secds = secs - (mins * 60)
    ms = int((elapsed - secs) * 1000)
    time_label.config(text=f"{mins:02}:{rem_secds:02}.{ms:02}")
    root.after(50, update_timer)

update_timer()

root.mainloop()