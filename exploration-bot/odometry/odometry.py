'''import cv2
import numpy as np
import requests

base_url = 'http://192.168.151.209:81'

response = requests.get(base_url+"/stream", stream=True)
buffer = bytes()

for chunk in response.iter_content(chunk_size=1024):
    buffer += chunk
    a = buffer.find(b'\xff\xd8')
    b = buffer.find(b'\xff\xd9')

    if a != -1 and b != -1:
        jpg = buffer[a:b+2]
        buffer = buffer[b+2:]

        # Check if jpg is not empty
        if jpg:
            # Convert the byte array to a numpy array
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            
            # Check if frame is not None (imdecode fails)
            if frame is not None:
                frame = cv2.flip(frame, 0)
                
                # Display the image
                cv2.imshow('IP Camera Stream', frame)

                key = cv2.waitKey(1)
                if key == ord('q'):
                    exit(0)
            else:
                print("Failed to decode frame.")
        else:
            print("Received empty image data.")

cv2.destroyAllWindows()'''










'''import cv2
import numpy as np
import requests

base_url = 'http://192.168.151.209:81'

# Parameters for Lucas-Kanade optical flow
lk_params = dict(winSize=(21, 21),
                 maxLevel=3,
                 criteria=(cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 30, 0.01))

# Initialize variables
old_frame = None
old_gray = None
p0 = None
trajectory = np.zeros((600, 600, 3), dtype=np.uint8)  # Window to show trajectory
position = np.array([300, 300], dtype=np.float32)     # Starting point of the trajectory

response = requests.get(base_url + "/stream", stream=True)
buffer = bytes()

for chunk in response.iter_content(chunk_size=1024):
    buffer += chunk
    a = buffer.find(b'\xff\xd8')
    b = buffer.find(b'\xff\xd9')

    if a != -1 and b != -1:
        jpg = buffer[a:b + 2]
        buffer = buffer[b + 2:]

        if jpg:
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            if frame is not None:
                frame = cv2.flip(frame, 0)
                frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

                if old_gray is not None:
                    # Calculate optical flow
                    p1, st, err = cv2.calcOpticalFlowPyrLK(old_gray, frame_gray, p0, None, **lk_params)
                    
                    # Ensure p1 and st are not None
                    if p1 is not None and st is not None:
                        # Select good points
                        good_new = p1[st == 1]
                        good_old = p0[st == 1]

                        if len(good_new) > 0 and len(good_old) > 0:
                            # Estimate the motion
                            dx, dy = np.mean(good_new - good_old, axis=0)

                            # Update the position
                            position += np.array([dx, dy], dtype=np.float32)

                            # Draw the trajectory
                            cv2.circle(trajectory, tuple(position.astype(int)), 1, (0, 255, 0), 2)

                            # Draw the tracks on the current frame
                            for i, (new, old) in enumerate(zip(good_new, good_old)):
                                a, b = new.ravel()
                                c, d = old.ravel()
                                frame = cv2.line(frame, (int(a), int(b)), (int(c), int(d)), (0, 255, 0), 2)
                                frame = cv2.circle(frame, (int(a), int(b)), 5, (0, 0, 255), -1)

                        # Update the previous frame and previous points
                        old_gray = frame_gray.copy()
                        p0 = good_new.reshape(-1, 1, 2)
                    else:
                        # Handle the case where optical flow calculation fails
                        old_gray = frame_gray
                        p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7))
                else:
                    old_gray = frame_gray
                    p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7))

                # Display the image
                cv2.imshow('IP Camera Stream', frame)
                cv2.imshow('Trajectory', trajectory)

                key = cv2.waitKey(1)
                if key == ord('q'):
                    break
            else:
                print("Failed to decode frame.")
        else:
            print("Received empty image data.")

cv2.destroyAllWindows()'''








'''import cv2
import numpy as np
import requests

base_url = 'http://192.168.151.209:81'

# Parameters for Lucas-Kanade optical flow
lk_params = dict(winSize=(21, 21),
                 maxLevel=3,
                 criteria=(cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 30, 0.01))

# Initialize variables
old_frame = None
old_gray = None
p0 = None
trajectory = np.zeros((600, 600, 3), dtype=np.uint8)  # Window to show trajectory
position = np.array([300, 300], dtype=np.float32)     # Starting point of the trajectory

response = requests.get(base_url + "/stream", stream=True)
buffer = bytes()

while True:
    buffer += response.raw.read(1024)
    a = buffer.find(b'\xff\xd8')
    b = buffer.find(b'\xff\xd9')

    if a != -1 and b != -1:
        jpg = buffer[a:b + 2]
        buffer = buffer[b + 2:]

        if jpg:
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            if frame is not None:
                frame = cv2.flip(frame, 0)
                frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

                if old_gray is not None:
                    # Calculate optical flow
                    p1, st, err = cv2.calcOpticalFlowPyrLK(old_gray, frame_gray, p0, None, **lk_params)
                    
                    # Ensure p1 and st are not None
                    if p1 is not None and st is not None:
                        # Select good points
                        good_new = p1[st == 1]
                        good_old = p0[st == 1]

                        if len(good_new) > 0 and len(good_old) > 0:
                            # Estimate the motion
                            dx, dy = np.mean(good_new - good_old, axis=0)

                            # Update the position
                            position += np.array([dx, dy], dtype=np.float32)

                            # Draw the trajectory
                            cv2.circle(trajectory, tuple(position.astype(int)), 1, (0, 255, 0), 2)

                            # Draw the tracks on the current frame (optional for visualization)
                            for i, (new, old) in enumerate(zip(good_new, good_old)):
                                a, b = new.ravel()
                                c, d = old.ravel()
                                frame = cv2.line(frame, (int(a), int(b)), (int(c), int(d)), (0, 255, 0), 2)
                                frame = cv2.circle(frame, (int(a), int(b)), 5, (0, 0, 255), -1)

                        # Update the previous frame and previous points
                        old_gray = frame_gray.copy()
                        p0 = good_new.reshape(-1, 1, 2)
                    else:
                        # Handle the case where optical flow calculation fails
                        old_gray = frame_gray
                        p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7))
                else:
                    old_gray = frame_gray
                    p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7))

                # Resize trajectory to match frame height
                resized_trajectory = cv2.resize(trajectory, (frame.shape[1], frame.shape[0]))

                # Display the image with trajectory
                combined_display = np.hstack((frame, resized_trajectory))
                cv2.imshow('IP Camera Stream with Trajectory', combined_display)

                key = cv2.waitKey(1)
                if key == ord('q'):
                    break
            else:
                print("Failed to decode frame.")
        else:
            print("Received empty image data.")

cv2.destroyAllWindows()'''






#correct for x and y axis
'''import cv2
import numpy as np
import requests
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

base_url = 'http://192.168.151.209:81'

# Parameters for Lucas-Kanade optical flow
lk_params = dict(winSize=(21, 21),
                 maxLevel=3,
                 criteria=(cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 30, 0.01))

# Initialize variables
old_frame = None
old_gray = None
p0 = None
trajectory = np.zeros((600, 600, 3), dtype=np.uint8)  # Window to show trajectory
position = np.array([300, 300], dtype=np.float32)     # Starting point of the trajectory
path = []  # List to store path coordinates

response = requests.get(base_url + "/stream", stream=True)
buffer = bytes()

# Matplotlib setup for path plotting
fig, ax = plt.subplots(figsize=(8, 6))
line, = ax.plot([], [], marker='o', color='b')
ax.invert_yaxis()  # Invert y-axis to match image coordinate system
ax.set_title('Path of Movement')
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.grid(True)

def update_plot(frame):
    global path
    if path:
        path_arr = np.array(path)
        line.set_data(path_arr[:, 0], path_arr[:, 1])
        ax.relim()
        ax.autoscale_view()

# Animation update every second
ani = FuncAnimation(fig, update_plot, interval=1000)

while True:
    buffer += response.raw.read(1024)
    a = buffer.find(b'\xff\xd8')
    b = buffer.find(b'\xff\xd9')

    if a != -1 and b != -1:
        jpg = buffer[a:b + 2]
        buffer = buffer[b + 2:]

        if jpg:
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            if frame is not None:
                frame = cv2.flip(frame, 0)
                frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

                if old_gray is not None:
                    # Calculate optical flow
                    p1, st, err = cv2.calcOpticalFlowPyrLK(old_gray, frame_gray, p0, None, **lk_params)
                    
                    # Ensure p1 and st are not None
                    if p1 is not None and st is not None:
                        # Select good points
                        good_new = p1[st == 1]
                        good_old = p0[st == 1]

                        if len(good_new) > 0 and len(good_old) > 0:
                            # Estimate the motion
                            dx, dy = np.mean(good_new - good_old, axis=0)

                            # Update the position
                            position += np.array([dx, dy], dtype=np.float32)
                            path.append(position.copy())  # Store current position in path

                            # Draw the trajectory
                            cv2.circle(trajectory, tuple(position.astype(int)), 1, (0, 255, 0), 2)

                            # Draw the tracks on the current frame (optional for visualization)
                            for i, (new, old) in enumerate(zip(good_new, good_old)):
                                a, b = new.ravel()
                                c, d = old.ravel()
                                frame = cv2.line(frame, (int(a), int(b)), (int(c), int(d)), (0, 255, 0), 2)
                                frame = cv2.circle(frame, (int(a), int(b)), 5, (0, 0, 255), -1)

                        # Update the previous frame and previous points
                        old_gray = frame_gray.copy()
                        p0 = good_new.reshape(-1, 1, 2)
                    else:
                        # Handle the case where optical flow calculation fails
                        old_gray = frame_gray
                        p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7))
                else:
                    old_gray = frame_gray
                    p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7))

                # Display the image with trajectory
                cv2.imshow('IP Camera Stream with Trajectory', np.hstack((frame, cv2.resize(trajectory, (frame.shape[1], frame.shape[0])))))

                key = cv2.waitKey(1)
                if key == ord('q'):
                    break
            else:
                print("Failed to decode frame.")
        else:
            print("Received empty image data.")

cv2.destroyAllWindows()
plt.show()'''







#z axis movement
'''import cv2
import numpy as np
import requests
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

base_url = 'http://192.168.151.209:81'

# Parameters for Lucas-Kanade optical flow
lk_params = dict(winSize=(21, 21),
                 maxLevel=3,
                 criteria=(cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 30, 0.01))

# Initialize variables
old_frame = None
old_gray = None
p0 = None
trajectory = np.zeros((600, 600, 3), dtype=np.uint8)  # Window to show trajectory
position = np.array([300, 300], dtype=np.float32)     # Starting point of the trajectory
path = []  # List to store path coordinates
z_movement = []  # List to store z-axis movement

response = requests.get(base_url + "/stream", stream=True)
buffer = bytes()

# Matplotlib setup for z-axis movement plotting
fig, ax = plt.subplots(figsize=(8, 6))
line, = ax.plot([], [], marker='o', color='b')
ax.set_title('Z-axis Movement')
ax.set_xlabel('Time')
ax.set_ylabel('Z-axis Displacement')
ax.grid(True)

def update_plot(frame):
    global z_movement
    if z_movement:
        line.set_data(range(len(z_movement)), z_movement)
        ax.relim()
        ax.autoscale_view()

# Animation update every second
ani = FuncAnimation(fig, update_plot, interval=1000)

while True:
    buffer += response.raw.read(1024)
    a = buffer.find(b'\xff\xd8')
    b = buffer.find(b'\xff\xd9')

    if a != -1 and b != -1:
        jpg = buffer[a:b + 2]
        buffer = buffer[b + 2:]

        if jpg:
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            if frame is not None:
                frame = cv2.flip(frame, 0)
                frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

                if old_gray is not None:
                    # Calculate optical flow
                    p1, st, err = cv2.calcOpticalFlowPyrLK(old_gray, frame_gray, p0, None, **lk_params)
                    
                    # Ensure p1 and st are not None
                    if p1 is not None and st is not None:
                        # Select good points
                        good_new = p1[st == 1]
                        good_old = p0[st == 1]

                        if len(good_new) > 0 and len(good_old) > 0:
                            # Estimate the motion along z-axis (forward movement in image plane)
                            dz = np.mean(good_new[:, 1] - good_old[:, 1])

                            # Update the z-axis movement
                            z_movement.append(dz)

                            # Update the position
                            position[1] += dz  # Update y coordinate based on forward movement

                            # Draw the trajectory
                            cv2.circle(trajectory, tuple(position.astype(int)), 1, (0, 255, 0), 2)

                            # Draw the tracks on the current frame (optional for visualization)
                            for i, (new, old) in enumerate(zip(good_new, good_old)):
                                a, b = new.ravel()
                                c, d = old.ravel()
                                frame = cv2.line(frame, (int(a), int(b)), (int(c), int(d)), (0, 255, 0), 2)
                                frame = cv2.circle(frame, (int(a), int(b)), 5, (0, 0, 255), -1)

                        # Update the previous frame and previous points
                        old_gray = frame_gray.copy()
                        p0 = good_new.reshape(-1, 1, 2)
                    else:
                        # Handle the case where optical flow calculation fails
                        old_gray = frame_gray
                        p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7))
                else:
                    old_gray = frame_gray
                    p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7))

                # Display the image with trajectory
                cv2.imshow('IP Camera Stream with Trajectory', np.hstack((frame, cv2.resize(trajectory, (frame.shape[1], frame.shape[0])))))

                key = cv2.waitKey(1)
                if key == ord('q'):
                    break
            else:
                print("Failed to decode frame.")
        else:
            print("Received empty image data.")

cv2.destroyAllWindows()
plt.show()'''




#for x and z axis
'''import cv2
import numpy as np
import requests
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

base_url = 'http://192.168.151.209:81'

# Parameters for Lucas-Kanade optical flow
lk_params = dict(winSize=(21, 21),
                 maxLevel=3,
                 criteria=(cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 30, 0.01))

# Initialize variables
old_frame = None
old_gray = None
p0 = None
trajectory = np.zeros((600, 600, 3), dtype=np.uint8)  # Window to show trajectory
position = np.array([300, 300], dtype=np.float32)     # Starting point of the trajectory
path = []  # List to store path coordinates
xz_movement = []  # List to store x and z axis movements

response = requests.get(base_url + "/stream", stream=True)
buffer = bytes()

# Matplotlib setup for x and z-axis movement plotting
fig, ax = plt.subplots(2, 1, figsize=(10, 8))
line_x, = ax[0].plot([], [], marker='o', color='b')
line_z, = ax[1].plot([], [], marker='o', color='g')
ax[0].set_title('X-axis Movement')
ax[0].set_xlabel('Time')
ax[0].set_ylabel('X-axis Displacement')
ax[0].grid(True)
ax[1].set_title('Z-axis Movement')
ax[1].set_xlabel('Time')
ax[1].set_ylabel('Z-axis Displacement')
ax[1].grid(True)

def update_plot(frame):
    global xz_movement
    if xz_movement:
        line_x.set_data(range(len(xz_movement)), [item[0] for item in xz_movement])
        line_z.set_data(range(len(xz_movement)), [item[1] for item in xz_movement])
        for axis in ax:
            axis.relim()
            axis.autoscale_view()

# Animation update every second
ani = FuncAnimation(fig, update_plot, interval=1000)

while True:
    buffer += response.raw.read(1024)
    a = buffer.find(b'\xff\xd8')
    b = buffer.find(b'\xff\xd9')

    if a != -1 and b != -1:
        jpg = buffer[a:b + 2]
        buffer = buffer[b + 2:]

        if jpg:
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            if frame is not None:
                frame = cv2.flip(frame, 0)
                frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

                if old_gray is not None:
                    # Calculate optical flow
                    p1, st, err = cv2.calcOpticalFlowPyrLK(old_gray, frame_gray, p0, None, **lk_params)
                    
                    # Ensure p1 and st are not None
                    if p1 is not None and st is not None:
                        # Select good points
                        good_new = p1[st == 1]
                        good_old = p0[st == 1]

                        if len(good_new) > 0 and len(good_old) > 0:
                            # Estimate the motion along x and z axes
                            dx = np.mean(good_new[:, 0] - good_old[:, 0])
                            dz = np.mean(good_new[:, 1] - good_old[:, 1])

                            # Update the x and z axis movements
                            xz_movement.append([dx, dz])

                            # Update the position
                            position[0] += dx  # Update x coordinate based on horizontal movement
                            position[1] += dz  # Update y coordinate based on forward movement (z-axis)

                            # Draw the trajectory
                            cv2.circle(trajectory, tuple(position.astype(int)), 1, (0, 255, 0), 2)

                            # Draw the tracks on the current frame (optional for visualization)
                            for i, (new, old) in enumerate(zip(good_new, good_old)):
                                a, b = new.ravel()
                                c, d = old.ravel()
                                frame = cv2.line(frame, (int(a), int(b)), (int(c), int(d)), (0, 255, 0), 2)
                                frame = cv2.circle(frame, (int(a), int(b)), 5, (0, 0, 255), -1)

                        # Update the previous frame and previous points
                        old_gray = frame_gray.copy()
                        p0 = good_new.reshape(-1, 1, 2)
                    else:
                        # Handle the case where optical flow calculation fails
                        old_gray = frame_gray
                        p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7))
                else:
                    old_gray = frame_gray
                    p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7))

                # Display the image with trajectory
                cv2.imshow('IP Camera Stream', frame)
                cv2.imshow('Trajectory', trajectory)

                key = cv2.waitKey(1)
                if key == ord('q'):
                    break
            else:
                print("Failed to decode frame.")
        else:
            print("Received empty image data.")

cv2.destroyAllWindows()
plt.show()'''






import cv2
import numpy as np
import requests

# Function to detect and match features
def get_matches(img1, img2):
    # Create ORB detector with 1000 keypoints
    orb = cv2.ORB_create(1000)
    
    # Detect and compute the descriptors with ORB
    kp1, des1 = orb.detectAndCompute(img1, None)
    kp2, des2 = orb.detectAndCompute(img2, None)
    
    # Check if descriptors are None
    if des1 is None or des2 is None:
        return None, None, None
    
    # Create BFMatcher object
    bf = cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=True)
    
    # Match descriptors
    matches = bf.match(des1, des2)
    
    # Sort them in the order of their distance
    matches = sorted(matches, key=lambda x: x.distance)
    
    return kp1, kp2, matches

# Function to estimate motion
def estimate_motion(kp1, kp2, matches):
    if len(matches) < 8:  # Minimum number of matches required
        return None, None
    
    src_pts = np.float32([kp1[m.queryIdx].pt for m in matches]).reshape(-1, 1, 2)
    dst_pts = np.float32([kp2[m.trainIdx].pt for m in matches]).reshape(-1, 1, 2)
    
    # Find essential matrix
    E, mask = cv2.findEssentialMat(src_pts, dst_pts, method=cv2.RANSAC, prob=0.999, threshold=1.0)
    
    # Recover pose
    _, R, t, mask = cv2.recoverPose(E, src_pts, dst_pts)
    
    return R, t

# Initialize the stream
base_url = 'http://192.168.4.23:81'
response = requests.get(base_url + "/stream", stream=True)
buffer = bytes()
previous_frame = None
trajectory = np.zeros((800, 800, 3), dtype=np.uint8)
origin = (400, 400)
current_pos = np.array([0, 0], dtype=np.float32)
current_orientation = np.eye(3)  # Initial orientation (identity matrix)

for chunk in response.iter_content(chunk_size=3078):
    buffer += chunk
    a = buffer.find(b'\xff\xd8')
    b = buffer.find(b'\xff\xd9')

    if a != -1 and b != -1:
        jpg = buffer[a:b + 2]
        buffer = buffer[b + 2:]
        
        if jpg:
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            
            if frame is not None:
                frame = cv2.flip(frame, 0)
                
                if previous_frame is not None:
                    kp1, kp2, matches = get_matches(previous_frame, frame)
                    
                    if kp1 is not None and kp2 is not None and matches is not None:
                        R, t = estimate_motion(kp1, kp2, matches)
                        
                        if R is not None and t is not None:
                            # Update the orientation
                            current_orientation = current_orientation @ R
                            
                            # Transform translation vector t using current orientation
                            translation = current_orientation @ t
                            dx = translation[0][0]
                            dz = translation[2][0]
                            current_pos += np.array([dx, dz])
                            
                            # Update trajectory
                            cv2.circle(trajectory, (int(origin[0] + current_pos[0]), int(origin[1] - current_pos[1])), 1, (0, 255, 0), 2)
                
                cv2.imshow('IP Camera Stream', frame)
                cv2.imshow('Trajectory', trajectory)
                
                previous_frame = frame

                key = cv2.waitKey(1)
                if key == ord('q'):
                    break
            else:
                print("Failed to decode frame.")
        else:
            print("Received empty image data.")

cv2.destroyAllWindows()
