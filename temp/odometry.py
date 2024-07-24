import cv2
import numpy as np
import requests
import time
from urllib.request import urlopen  # Import urlopen for fetching images

# Function to detect and match features
def get_matches(img1, img2):
    # Create ORB detector with 1000 keypoints
    orb = cv2.ORB_create(500)
    
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
url = 'http://192.168.0.106/cam-lo.jpg'  # Replace with your camera URL
cm_to_pixels = 100  # 1 cm = 10 pixels

response = urlopen(url)
buffer = bytes()
previous_frame = None
trajectory = np.zeros((400, 400, 3), dtype=np.uint8)
current_pos = np.array([0, 0], dtype=np.float32)
current_orientation = np.eye(3)  # Initial orientation (identity matrix)
last_capture_time = time.time()

def update_trajectory_window(current_pos, trajectory, origin):
    max_x, max_y = int(origin[0] + current_pos[0]), int(origin[1] - current_pos[1])
    height, width = trajectory.shape[:2]
    
    if max_x >= width or max_y >= height or max_x < 0 or max_y < 0:
        new_width = max(max_x + 100, width)
        new_height = max(max_y + 100, height)
        new_trajectory = np.zeros((new_height, new_width, 3), dtype=np.uint8)
        new_trajectory[:height, :width] = trajectory
        return new_trajectory, (new_width // 2, new_height // 2)
    
    return trajectory, origin

trajectory, origin = update_trajectory_window(current_pos, trajectory, (200, 200))  # Initial origin

while True:
    imgResp = urlopen(url)
    imgNp = np.array(bytearray(imgResp.read()), dtype=np.uint8)
    frame = cv2.imdecode(imgNp, -1)

    if frame is not None:
        frame = cv2.flip(frame, 0)
        
        current_time = time.time()
        if current_time - last_capture_time >= 0:  # 200 ms interval
            last_capture_time = current_time

            if previous_frame is not None:
                kp1, kp2, matches = get_matches(previous_frame, frame)
                
                if kp1 is not None and kp2 is not None and matches is not None:
                    R, t = estimate_motion(kp1, kp2, matches)
                    
                    if R is not None and t is not None:
                        # Update the orientation
                        current_orientation = current_orientation @ R
                        
                        # Transform translation vector t using current orientation
                        translation = current_orientation @ t
                        dx = translation[0][0] * cm_to_pixels
                        dz = translation[2][0] * cm_to_pixels
                        current_pos += np.array([dx, dz])
                        
                        # Update trajectory window size if necessary
                        trajectory, origin = update_trajectory_window(current_pos, trajectory, origin)
                        
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

cv2.destroyAllWindows()

